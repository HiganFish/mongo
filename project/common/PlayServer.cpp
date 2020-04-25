//
// Created by lsmg on 4/21/20.
//

#include <InetAddress.h>
#include <protobuf/ProtobufServer.h>
#include <EventLoop.h>
#include <Logger.h>
#include "play.pb.h"

mongo::net::ProtobufServer* server_internal;

struct GameRoom
{
    int player_nums = 0;
    mongo::net::TcpConnectionPtr players[2];
};

typedef std::map<int, GameRoom> DeskToConnMap;
DeskToConnMap conn_map;

typedef std::map<std::string, int> ConnToDeskMap;
ConnToDeskMap desk_map;

int current_room_id = 0;
int current_room_player_num = 0;

void OnPlayMessage(const mongo::net::TcpConnectionPtr& conn, const mongo::net::MessagePtr& message)
{
    std::shared_ptr<mongo::InputPackage> input = std::static_pointer_cast<mongo::InputPackage>(message);
    if (input == nullptr)
    {
        LOG_ERROR << conn->GetConnectionName() << " Invalid Message";
        return;
    }

    int roomid = desk_map[conn->GetConnectionName()];
    GameRoom* room =  &conn_map[roomid];

    if (room->player_nums == 2)
    {
        LOG_INFO << input->ByteSizeLong() << " " << conn->GetConnectionName() << " " << input->timestamp();

        server_internal->Send((room->players[0]), message.get());
        server_internal->Send((room->players[1]), message.get());
    }
}

void OnCloseCallback(const mongo::net::TcpConnectionPtr& conn)
{

}

void OnNewConnection(const mongo::net::TcpConnectionPtr& conn)
{
    if (current_room_player_num == 2)
    {
        current_room_id++;
        current_room_player_num = 0;
    }
    desk_map[conn->GetConnectionName()] = current_room_id;

    conn_map[current_room_id].player_nums++;
    conn_map[current_room_id].players[current_room_player_num++] = conn;

    LOG_INFO << conn->GetConnectionName() << " roomid: " << current_room_id << " current players: " << current_room_player_num;
}

int main()
{
    mongo::net::EventLoop loop;
    mongo::net::InetAddress addr(8010);
    mongo::net::ProtobufServer server(&loop, "play-server", addr);
    server_internal = &server;

    server.RegistCallback(mongo::InputPackage::descriptor(), OnPlayMessage);
    server.SetCloseCallback(OnCloseCallback);
    server.SetNewConnectionCallback(OnNewConnection);

    server.SetThreadNum(2);
    server.Start();
    loop.Loop();
}
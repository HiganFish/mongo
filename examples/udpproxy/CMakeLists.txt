add_executable(udpproxy main.cpp Codec.h Codec.cpp)
target_link_libraries(udpproxy mongo_net)
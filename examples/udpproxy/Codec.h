//
// Created by lsmg on 5/6/20.
//

#ifndef _CODEC_H_
#define _CODEC_H_


#include <Buffer.h>
#include <cstdint>
#include <cstring>
struct Response
{
	const char* uuid;
	uint16_t port;
	uint32_t check;
	uint8_t addr_len;
	uint32_t data_len;
	const char* addr;
	const char* data;
};

class Codec
{
public:
	Codec();
	~Codec();

	bool Decode(Response* response, mongo::net::Buffer* buffer);
private:
	const size_t MIN_DATA_SIZE = 47;
};

#endif //_CODEC_H_

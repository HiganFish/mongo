//
// Created by lsmg on 5/6/20.
//
#include <Logger.h>
#include "Codec.h"
Codec::Codec()
{

}
Codec::~Codec()
{

}
bool Codec::Decode(Response* response, mongo::net::Buffer* buffer)
{
	if (buffer->ReadableBytes() < MIN_DATA_SIZE)
	{
		LOG_WARN << "the package size: " << buffer->ReadableBytes()  << " is to small!";
		return false;
	}

	response->uuid = buffer->ReadBegin();
	buffer->AddReadIndex(37);

	response->port = buffer->PeekUint16();
	buffer->AddReadIndex(2);

	response->check = buffer->PeekUint32();
	buffer->AddReadIndex(4);

	response->addr_len = buffer->PeekUint8();
	buffer->AddReadIndex(1);

	response->data_len = buffer->PeekUint32();
	buffer->AddReadIndex(4);

	response->addr = buffer->ReadBegin();
	response->data = buffer->ReadBegin() + response->addr_len;
	return true;
}

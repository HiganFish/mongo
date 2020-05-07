//
// Created by lsmg on 4/10/20.
//

#ifndef MONGO_SRC_NET_ENDIAN_H
#define MONGO_SRC_NET_ENDIAN_H
#include <cstdint>
#include <endian.h>
namespace mongo
{

namespace sockets
{
inline uint32_t NetworkToHost32(uint32_t num)
{
    return be32toh(num);
}

inline uint32_t HostToNetwork32(uint32_t num)
{
    return htobe32(num);
}


inline uint16_t NetworkToHost16(uint16_t num)
{
	return be16toh(num);
}

inline uint16_t HostToNetwork16(uint16_t num)
{
	return htobe16(num);
}
}
}


#endif //MONGO_SRC_NET_ENDIAN_H

//
// Created by lsmg on 4/9/20.
//

#include <sys/time.h>
#include "mongo/base/Timestamp.h"

using namespace mongo;

const int Timestamp::US_PER_SECOND;


Timestamp Timestamp::Now()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);

    return Timestamp(tv.tv_sec * US_PER_SECOND + tv.tv_usec);
}
Timestamp& Timestamp::operator-(Timestamp& stamp)
{
	us_since_create = us_since_create - stamp.us_since_create;

	return *this;
}

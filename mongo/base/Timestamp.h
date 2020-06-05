//
// Created by lsmg on 4/9/20.
//

#ifndef MONGO_SRC_BASE_TIMESTAMP_H
#define MONGO_SRC_BASE_TIMESTAMP_H

#include <cstdint>
#include <string>

namespace mongo
{
class Timestamp
{
public:
    Timestamp():
		create_msec_(0) {}

    explicit Timestamp(int64_t us):
		create_msec_(us) {}

    static Timestamp Now();

    const static int US_PER_SECOND = 1000 * 1000;

    int64_t GetCreateTimeAsUsec() const
    { return create_msec_; }

    std::string ToUsec()
    { return std::to_string(create_msec_); }

    std::string ToSecMsec()
    { return std::to_string(create_msec_ / US_PER_SECOND) + ":" +
			 std::to_string(create_msec_ / 1000 % 1000); }

    std::string ToSecMsecUsec()
    {
        return std::to_string(create_msec_ / US_PER_SECOND) + ":" +
			   std::to_string(create_msec_ / 1000 % 1000) + ":" +
			   std::to_string(create_msec_ % 1000);
    }

    Timestamp& operator-(Timestamp& stamp);

	int64_t GetSec()
	{ return create_msec_ / US_PER_SECOND; }

private:
    int64_t create_msec_;
};
}
#endif //MONGO_SRC_BASE_TIMESTAMP_H

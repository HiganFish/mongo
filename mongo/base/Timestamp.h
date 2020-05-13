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
        us_since_create(0) {}

    explicit Timestamp(int64_t us):
        us_since_create(us) {}

    static Timestamp Now();

    const static int US_PER_SECOND = 1000 * 1000;

    int64_t GetUsSinceCreate() const
    { return us_since_create; }

    std::string ToUsec()
    { return std::to_string(us_since_create); }

    std::string ToSecMsec()
    { return std::to_string(us_since_create / US_PER_SECOND) + ":" +
    std::to_string(us_since_create / 1000 % 1000); }

    std::string ToSecMsecUsec()
    {
        return std::to_string(us_since_create / US_PER_SECOND) + ":" +
            std::to_string(us_since_create / 1000 % 1000) + ":" +
            std::to_string(us_since_create % 1000);
    }

private:
    int64_t us_since_create;
};
}
#endif //MONGO_SRC_BASE_TIMESTAMP_H

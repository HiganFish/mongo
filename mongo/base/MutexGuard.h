//
// Created by lsmg on 4/8/20.
//

#ifndef MONGO_SRC_BASE_MUTEXGUARD_H
#define MONGO_SRC_BASE_MUTEXGUARD_H

#include "mongo/base/Mutex.h"
#include "mongo/base/noncopyable.h"

namespace mongo
{
class MutexGuard : noncopyable
{
public:
    explicit MutexGuard(Mutex& mutex);
    ~MutexGuard();
private:
    Mutex& mutex_;
};

}
#endif //MONGO_SRC_BASE_MUTEXGUARD_H

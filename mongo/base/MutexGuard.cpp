//
// Created by lsmg on 4/8/20.
//

#include "mongo/base/MutexGuard.h"

mongo::MutexGuard::MutexGuard(mongo::Mutex& mutex):
mutex_(mutex)
{
    mutex_.Lock();
}
mongo::MutexGuard::~MutexGuard()
{
    mutex_.UnLock();
}

//
// Created by lsmg on 5/23/20.
//

#ifndef _DAEMON_H_
#define _DAEMON_H_

#include <unistd.h>
#include <csignal>
#include <cstdlib>
#include "noncopyable.h"

namespace mongo
{

class Daemon : noncopyable
{
public:

	static void InitDaemon();

private:

	static void ForkAndExitParentProcess();
};
}

#endif //_DAEMON_H_

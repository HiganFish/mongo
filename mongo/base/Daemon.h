//
// Created by lsmg on 5/23/20.
//

#ifndef _DAEMON_H_
#define _DAEMON_H_

#include <unistd.h>
#include <csignal>
#include <cstdlib>
#include "mongo/base/noncopyable.h"

namespace mongo
{

/**
 * 守护进程
 */
class Daemon : noncopyable
{
public:

	/*
	 * 将程序初始化为守护进程
	 */
	static void InitDaemon();

private:

	static void ForkAndExitParentProcess();
};
}

#endif //_DAEMON_H_

//
// Created by lsmg on 5/23/20.
//

#include <sys/param.h>
#include <sys/stat.h>
#include "mongo/base/Logger.h"
#include "mongo/base/Daemon.h"

using namespace mongo;
void Daemon::InitDaemon()
{
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	signal(SIGHUP, SIG_IGN);


	ForkAndExitParentProcess();

	// create new group

	/*
	 * 子进程从父进程继承了：SessionID、进程组ID和打开的终端。子进程如果要脱离这些，代码中可通过调用setsid来实现。
	 * ，而命令行或脚本中可以通过使用命令setsid来运行程序实现。setsid帮助一个进程脱离从父进程继承而来的已打开的终端、隶属进程组和隶属的会话。
	 *
	 *  第一子进程成为新的会话组长和进程组长
	 * */
	setsid();

	// 第二子进程不再是会话组长
	ForkAndExitParentProcess();

	// close parent fd
	for (int i = 0; i < NOFILE; ++i)
	{
		close(i);
	}

	// change work dir
	chdir("/");

	// clear parent file mask
	umask(0);

	signal(SIGCHLD, SIG_IGN);

	LOG_INFO << "Init Daemon success";
}
void Daemon::ForkAndExitParentProcess()
{
	pid_t pid = fork();
	if (pid > 0)
	{
		exit(0);
	}
	else if (pid < 0)
	{
		exit(-1);
	}
}

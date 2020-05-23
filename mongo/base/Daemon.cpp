//
// Created by lsmg on 5/23/20.
//

#include <sys/param.h>
#include <sys/stat.h>
#include "Logger.h"
#include "Daemon.h"

using namespace mongo;
void Daemon::InitDaemon()
{
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	signal(SIGHUP, SIG_IGN);

	ForkAndExitParentProcess();

	// create new group
	setsid();

	ForkAndExitParentProcess();

	// close parent fd
	for (int i = 0; i < NOFILE; ++i)
	{
		close(i);
	}

	// change work dir
	chdir("/");

	// change file mask
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

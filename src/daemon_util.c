#include "daemon_util.h"
void init_daemon(const char *_current_directory)
{
	pid_t _pid,_sid;
	_pid=fork();
	if(_pid<0)
	{
		exit(EXIT_FAILURE);
	}
	if(_pid>0)
	{
		exit(EXIT_SUCCESS);
	}
	umask(0);
	_sid=setsid();
	if(_sid<0)
	{
		exit(EXIT_FAILURE);
	}
	if(chdir(_current_directory)<0)
	{
		exit(EXIT_FAILURE);
	}
	close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
	openlog(LOG_NAME,LOG_CONS|LOG_PID,LOG_DAEMON);
}

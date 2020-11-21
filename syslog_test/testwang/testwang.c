#include <syslog.h>

int main(int argc, char * argv[])
{
	openlog("testsyslog", LOG_CONS|LOG_PID, LOG_LOCAL0);
	syslog(LOG_LOCAL0|LOG_INFO, "test mesage wanghaitaoLOG_INFO \n");
	syslog(LOG_LOCAL0|LOG_DEBUG, "test mesage wanghaitaoLOG_DEBUG \n");
	closelog();
	
	return 0;
	
}
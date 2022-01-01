#define _GNU_SOURCE
#include <stdio.h>
#include <sched.h>
#include <string.h>
int main()
{
	int ret;

	struct sched_param sd;
    cpu_set_t set;

	sd.sched_priority=50;   //设置实时优先级50
	ret = sched_setscheduler(0,SCHED_RR,&sd);
	if(ret != 0)
	{
		perror("sched_setscheduler ret error"); //确保有root权限！！！--20211027
		return -1;
	}
	CPU_ZERO(&set);
	CPU_SET(0,&set);   
	ret = sched_setaffinity(0,sizeof(cpu_set_t),&set);  //设置CPU亲和力
	if(ret != 0)
	{
		perror("sched_setaffinity ret error"); //确保有root权限！！！
		return -1;
	}

	printf("this is SCHED_RR\n");
	while(1);
	return 0;
}
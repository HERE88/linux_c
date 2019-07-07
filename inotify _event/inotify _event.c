#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <malloc.h>

#include <sys/stat.h>

#include <semaphore.h>
#include <termios.h>
#include <pthread.h>

#include <unistd.h>
#include <sys/types.h>


#include <sys/socket.h>
#include <sys/un.h>

#include <dirent.h>


#include <linux/netlink.h>
#include <arpa/inet.h>
#include <ctype.h>

#include <math.h>
#include <sys/inotify.h>


int watch_inotify_event(int fd)
{
	char event_buf[512];
	int  read_len;
	int event_pos = 0;
	int event_size = 0;
	struct inotify_event *event;

	read_len = read(fd, event_buf, sizeof(event_buf));  //blocked IO
	if(read_len < (int)sizeof(struct inotify_event))
	{
		printf("couldn't get event\n");
		return -1;

	}

	//it is possible to return more than one inotify_event
	while(read_len >= (int)sizeof(struct inotify_event))
	{
		event = (struct inotify_event *)(event_buf + event_pos);
		if(event->len)
		{
			if(event->mask & IN_CREATE)
			{
				printf("create file:%s \n", event->name);
			}
			else
			{
				printf("delete file:%s \n", event->name);
			}
		}

		event_size = sizeof(struct inotify_event) + event->len;
		read_len -= event_size;
		event_pos +=  event_size;
	}



	return 0;
}

int main(int argc, char * argv[])
{
	int ify_fd;
	int wd;

	if(argc != 2)
	{
		printf("Enter file name\n");
		return -1;
	}

	ify_fd = inotify_init();
	if(ify_fd < 0)
	{
		perror("inotify_init error");
		return -1;
	}

	wd = inotify_add_watch(ify_fd, argv[1], IN_CREATE | IN_DELETE);
	watch_inotify_event(ify_fd);

	if(inotify_rm_watch(ify_fd, wd) == -1)
	{
		printf("inotify_rm_watch error\n");
		return -1;
	}

	close(ify_fd);
	return 0;
}
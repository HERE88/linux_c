#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    char buf[1024];
    int fd, n;

    fd = open("hello.file", O_CREAT | O_RDWR , S_IRUSR | S_IWUSR);
    if(fd < 0)
    {
        perror("open error");
        return -1;
    }

    if(dup2(fd, STDIN_FILENO) < 0)
    {
        perror("dup2 error");
        return -1;
    }
    close(fd); //here could close the fd

    char c;
    while((c = getchar()) != EOF) //注意此处getchar是一个标准I/O函数，其标准输入对应于stdin，这里说明stdin与STDIN_FILENO具有一致性。
    {
        printf("%c", c);
    }

    return 0;
}
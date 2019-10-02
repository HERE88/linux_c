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

#define NUM_S 1000000
#define UINT16 unsigned short

#define MIN_RAN  20      //ms
#define MAX_RAN  20000
//#define EX_VALUE   2895  //this Mathematics Expectation depeneds just on MIN_RAN & MAX_RAN.
#define MAX_C    500   //here c just relates to b which is in Y=a*(b^X) while MIN_RAN & MAX_RAN known.

//get value X~U(0, MAX_C)
int get_rand_value(int fd, UINT16 *u16_ptr)
{
    int ret =0;
    UINT16 u16_tmp = 0;

    ret = read(fd, &u16_tmp, sizeof(UINT16));
    if(ret < 0)
    {
        printf("read error, errno:%d, fd:%d\n", errno, fd);
        return -1;
    }
    *u16_ptr = u16_tmp % MAX_C;
    return 0;
}

int main()
{
    int i;
    unsigned short ran_value;
    int fd_ran;
    double d_ran_value = 0.0;

    unsigned long sum = 0;
    double d_sum = 0.0;


    double exp_para_a = 0.0;
    double exp_para_b = 0.0;



    fd_ran = open("/dev/urandom", O_RDONLY);
    if(fd_ran < 0)
    {
        perror("open /dev/urandom error");
        return 0;
    }

    //compute parameters of the Exponent function: Y=a*(b^X)
    exp_para_a = MIN_RAN;
    exp_para_b = pow((MAX_RAN/MIN_RAN)*1.0, 1.0/MAX_C);  //get b in Y=a*(b^X)

    for(i = 0; i < NUM_S; i++)
    {
        if(get_rand_value(fd_ran, &ran_value) != 0)
        {
            exit(0);
        }
        //printf("ran_value = [%d]\n", (int)ran_value);
        sum += ran_value;

        d_ran_value = exp_para_a * pow(exp_para_b, ran_value * 1.0);  //Y=a*(b^X)

        //printf("d_ran_value = [%f]\n", d_ran_value);
        d_sum += d_ran_value;
    }

    printf("AVG111 = [%lu]\n", sum/NUM_S);
    printf("AVG112 = [%f]\n", d_sum/NUM_S);


    close(fd_ran);


    return 0;

}
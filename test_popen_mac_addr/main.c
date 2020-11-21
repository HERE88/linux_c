#define MAIN_C

/*---------------------------- include head file -----------------------------*/

#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

#include <sys/system_properties.h>

#define LOG_TAG                                 "test_popen_mac"
#include <utils/Log.h>
#include <cutils/uevent.h>

#include "main.h"




/*---------------------- external variable declaration -----------------------*/



/*----------------- external function prototype declaration ------------------*/



/*----------------------- file-local macro definition ------------------------*/


/*----------------- file-local constant and type definition ------------------*/





/*---------------- file-local function prototype declaration -----------------*/




/*--------------------- file-local variables definition ----------------------*/




/*--------------------------- function definition ----------------------------*/





int main( int argc, char *argv[] ) {
    FILE * fp_mac;
    char line[128];
    char cmd[256];

    memset(line, 0x0, sizeof(line));
    memset(cmd, 0x0, sizeof(cmd));

    fp_mac = popen("cat /sys/class/net/lmi40/address", "r");
    while(fgets(line, sizeof(line), fp_mac) != NULL)
    {
        if(line[strlen(line) - 1] == '\n')
        {
            line[strlen(line) - 1] = 0;
        }
        printf("result:[%s],len:[%d]\n", line, strlen(line));
    }

    snprintf(cmd,sizeof(cmd),"AT^DSONUSN=%s,\"%s\"\r","A020001", line);
    printf("cmd::[%s],len:[%d]\n", cmd, strlen(cmd));

    pclose(fp_mac);

    return 0;
}

#undef MAIN_C

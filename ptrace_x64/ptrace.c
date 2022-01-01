#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

//#include <linux/user.h>
//#include <sys/user.h>
#include <sys/reg.h>


#include <stdio.h>

int main()
{   
    pid_t child;
    long orig_eax;
    
    child = fork();
    
    if(child == 0) {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execl("/bin/ls", "ls", "-l", NULL);
    }
    else {
        wait(NULL);
        //orig_eax = ptrace(PTRACE_PEEKUSER,child, 4 * ORIG_EAX,NULL);
        orig_eax = ptrace(PTRACE_PEEKUSER,child, 8 * ORIG_RAX,NULL);
        printf("The child made a ""system call %ld\n", orig_eax);
        ptrace(PTRACE_CONT, child, NULL, NULL);
    }
    return 0;
}


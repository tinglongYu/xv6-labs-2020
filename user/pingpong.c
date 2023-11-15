#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define STDERR_FILENO 2

#define READEND 0
#define WRITEEND 1

typedef int pid_t;

int
main(){
    //save parent process and child process' fd
    int pfd[2];
    int cfd[2];

    //buffer used to save message
    char buf[10];

    pid_t pid;

    pipe(pfd);
    pipe(cfd);

    if((pid=fork())<0){
        fprintf(STDERR_FILENO,"fork error\n");
        exit(1);
    }
    //child process
    else if(pid==0){
        close(pfd[WRITEEND]);
        close(cfd[READEND]);
        read(pfd[READEND], buf, 4);
        printf("%d: received %s\n", getpid(), buf);
        write(cfd[WRITEEND], "pong", 4);
        close(cfd[WRITEEND]);
    }
    //parent process
    else{
        close(pfd[READEND]);
        close(cfd[WRITEEND]);
        write(pfd[WRITEEND], "ping", 4);
        close(pfd[WRITEEND]);
        read(cfd[READEND], buf, 4);
        printf("%d: received %s\n", getpid(), buf);
    }
    exit(0);
}

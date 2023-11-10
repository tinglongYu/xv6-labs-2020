#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define STDERR_FILENO 2

#define READEND 0
#define WRITEEND 1

typedef int pid_t;

int
main(){
    int numbers[35],fd[2];
    int index=0;
    pid_t pid;
    for (int i = 2; i <=35 ; i++)
    {
        numbers[index++]=i;
    }
    while(index>0){
        pipe(fd);
        if((pid=fork())<0){
            fprintf(STDERR_FILENO,"fork erroe\n");
            exit(0);
        }
        else if(pid>0){
            close(fd[READEND]);
            for(int i=0;i<index;i++){
                write(fd[WRITEEND],&numbers[i],sizeof(numbers[i]));
            }
            close(fd[WRITEEND]);
            wait((int*)0);
            exit(0);
        }
        else{
            close(fd[WRITEEND]);
            int prime=0;
            int temp=0;
            index=-1;
            while(read(fd[READEND],&temp,sizeof(temp))!=0){
                if(index<0)prime=temp,index++;
                else{
                    if(temp%prime!=0)numbers[index++]=temp;
                }
            }
            printf("prime %d\n",prime);
            close(fd[READEND]);
        }
    }
    exit(0);
}


#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define STDERR_FILENO 2

#define READEND 0
#define WRITEEND 1

typedef int pid_t;

int main()
{
    int numbers[35], fd[2];
  	//存储需要筛选的数字数量
    int index = 0;
    pid_t pid;
  	//存储待筛选的数字
    for (int i = 2; i <= 35; i++)
    {
        numbers[index++] = i;
    }
    while (index > 0)
    {
        pipe(fd);
        if ((pid = fork()) < 0)
        {
            fprintf(STDERR_FILENO, "fork erroe\n");
            exit(0);
        }
        else if (pid > 0)
        {
            close(fd[READEND]);
          	//将待筛选的数字写入管道
            for (int i = 0; i < index; i++)
            {
                write(fd[WRITEEND], &numbers[i], sizeof(numbers[i]));
            }
            close(fd[WRITEEND]);
          	//等待子进程结束
          	//这样的用法表明父进程对子进程的退出状态不感兴趣，只是等待子进程执行完毕。
          	//通常，如果不需要获取子进程的退出状态，可以将 wait 的参数设置为 NULL 或 (int*)0。
            wait((int *)0);
            exit(0);
        }
        else
        {
            close(fd[WRITEEND]);
            int prime = 0;//这一轮找出的素数，用这个素数去筛
            int temp = 0;//用于存储每一个管道读取的数
            index = -1;
          	//不断从管道读取
            while (read(fd[READEND], &temp, sizeof(temp)) != 0)
            {
              	//每一个进程处理时，读取的第一个数就是新找到的素数
                if (index < 0)
                    prime = temp, index++;
                //筛
                else
                {
                    if (temp % prime != 0)
                        numbers[index++] = temp;
                }
            }
            printf("prime %d\n", prime);
            close(fd[READEND]);
        }
    }
    exit(0);
}

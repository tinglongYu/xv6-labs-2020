#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

#define STDIN_FILENO 0
#define MAXLINE 1024

int main(int argc, char *argv[])
{
    char line[MAXLINE];//存储从标准输入读取的行
    char* params[MAXARG];//存储命及其参数
    int n;//标准输入读取字符串长度
    int args_index = 0;
    int i;

    char* cmd = argv[1];
    for (i = 1; i < argc; i++) params[args_index++] = argv[i];

    while ((n = read(STDIN_FILENO, line, MAXLINE)) > 0)
    {
        if (fork() == 0) //创建子进程执行一个命令
        {
            char *arg = (char*) malloc(sizeof(line));
            int index = 0;
            for (i = 0; i < n; i++)
            {
                //使用空格和换行符作为参数的分隔符
                if (line[i] == ' ' || line[i] == '\n')
                {
                    arg[index] = '\0';
                    params[args_index++] = arg;
                    index = 0;
                    //重新分配内存，否则上一个参数会被覆盖
                    arg = (char*) malloc(sizeof(line));
                }
                else arg[index++] = line[i];
            }
            arg[index] = '\0';
            params[args_index] = '\0';
            exec(cmd, params);
        }
        //主进程等待子进程结束
        else wait((int*)0);
    }
    exit(0);
}
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

#define STDERR_FILENO 2
#define O_RDONLY 0

//获取路径中最右边的文件名
char* get_name(char * path)
{
    char *p;
    
    for (p = path + strlen(path); p >= path && *p != '/'; p--);
    p++;
    return p;
}

void find(char* path, char *name)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;
    
    if ((fd = open(path, O_RDONLY)) < 0)
    {
        fprintf(STDERR_FILENO, "find open %s error\n", path);
        exit(1);
    }

    if (fstat(fd, &st) < 0)
    {
        fprintf(STDERR_FILENO, "fstat %s error\n", path);
        close(fd);
        exit(1);
    }

    switch (st.type)
    {
    //如果是文件则检查是否是要查询的
    case T_FILE: 
        if (strcmp(get_name(path), name) == 0) printf("%s\n", path);
        break;
    //如果是问价夹则递归查找
    case T_DIR: 
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf))
        {
            fprintf(STDERR_FILENO, "find: path too long\n");
            break;
        }

        // add '/'
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';
        
        while (read(fd, &de, sizeof(de)) == sizeof(de))
        {
            // 空文件夹
            if (de.inum == 0) continue;

            // 文件夹名加入路径
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            
            // 不能递归进入.和..文件夹，否则会无限递归
            if (!strcmp(de.name, ".") || !strcmp(de.name, "..")) continue;
            
            find(buf, name);
        }
        break;
    }
    close(fd);
}


int main(int argc, char *argv[])
{
    if (argc != 3) 
    {
        fprintf(STDERR_FILENO, "usage: find <path> <name>\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}
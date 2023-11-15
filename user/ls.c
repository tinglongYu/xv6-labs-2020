#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

//输出指定路径下的文件和文件夹，会递归的遍历子文件夹中的文件和文件夹
char*
fmtname(char *path)
{
  //存储格式化后的文件或目录名称,这里限定返回的文件名长度为DIRSIZ+1
  //这里有个关键的点，声明为static数组，在返回值以后，非静态的数组在栈区的生命周期结束，会被直接清空
  //而static数组存在于内存空间的data段，能够一直驻留在内存中，不受影响，因此返回buf指针，也能读取buf中的值
  static char buf[DIRSIZ+1];
  //用于查找路径中的文件名或目录名
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    //由于p原本就指向传入参数的某个位置，所以返回p也可以访问到文件名
    return p;
  //将文件名或目录名拷贝到‘buf’中
  memmove(buf, p, strlen(p));
  //使用空格填充‘buf’中剩余的空间
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

void
ls(char *path)
{
  //buf存储路径信息，用于构建子文件或子目录的完整路径，p用于定位当前路径的末尾
  //buf保持父目录路径，在末尾添加子目录或文件名称
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  //系统调用打开文件，0代表的是O_RDONLY只读的形式
  //打开成功会返回文件描述符，打开失败返回<0的数
  if((fd = open(path, 0)) < 0){
    fprintf(2, "ls: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  //如果类型是文件，直接格式化后输出
  case T_FILE:
    printf("%s %d %d %l\n", fmtname(path), st.type, st.ino, st.size);
    break;
  //如果是目录
  case T_DIR:
    //path是当前目录，1代表‘/’，DIRSIZ表示文件或目录名的最大长度，1表示字符串末尾的空字符（因为文件名后面还有其他内容，要隔开）
    //如果拼接后超过了buf大小，说明缓冲区不足以存储完整的路径
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("ls: path too long\n");
      break;
    }
    //路径拷贝到缓冲区
    strcpy(buf, path);
    p = buf+strlen(buf);
    //路径末尾加‘/’
    //对p的操作也是对buf的操作
    *p++ = '/';
    //遍历目录中的文件和子目录
    //文件夹中存在很多个目录项，每次从fd中读取一个dirent
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      //代表此文件夹无文件
      if(de.inum == 0)
        continue;
      //将文件名赋值给p
      memmove(p, de.name, DIRSIZ);
      //0对应的ascii码就是'\0'，表示一个字符串的结尾
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf("ls: cannot stat %s\n", buf);
        continue;
      }
      printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  int i;

  if(argc < 2){
    ls(".");
    exit(0);
  }
  for(i=1; i<argc; i++)
    ls(argv[i]);
  exit(0);
}

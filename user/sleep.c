#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define STDERR_FILENO 2

int main(int argc, char* argv[]){
    if (argc != 2)
    {
        fprintf(STDERR_FILENO, "usage: sleep <number>\n");
        exit(1);
    }
    int number = atoi(argv[1]);
    sleep(number);
    exit(0);
}
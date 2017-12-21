#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

void signalHandler(int sig){}

#define MAX_BUF 1024

int main(int argc, char *argv[])
{
    int fd;
    char * myfifo = "fifo.ftc";
    char buf[MAX_BUF];
    /* create the FIFO (named pipe) */
    int fid=mkfifo(myfifo, S_IRUSR|S_IWUSR );
    
    pid_t pid = fork();
    if(pid < 0)
    {
        perror("fork failed\n");
        exit(EXIT_FAILURE);
    }
    if(pid == 0)
    {
        printf("1st child\n");
    }
    else if(pid > 0)
    {
        pid_t pid2 = fork();
        if(pid2 > 0)
        {
            printf("parent\n");
        }
        else if(pid2 == 0)
        {
            printf("2nd child\n");
        }

    }
    unlink(myfifo);
}
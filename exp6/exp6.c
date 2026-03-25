#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "pipe.h"

void client(int, int);
void server(int, int);

int main(int argc, char **argv)
{
    int fd1[2], fd2[2];
    pid_t childpid;

    // Create pipes
    if (pipe(fd1) < 0) {
        perror("pipe creation error");
        exit(1);
    }

    if (pipe(fd2) < 0) {
        perror("pipe creation error");
        exit(1);
    }

    // Fork process
    if ((childpid = fork()) < 0) {
        perror("fork error");
        exit(1);
    }
    else if (childpid == 0) {  // Child (server)
        close(fd2[0]); // close read end of pipe2
        close(fd1[1]); // close write end of pipe1
        server(fd1[0], fd2[1]);
        exit(0);
    }
    else { // Parent (client)
        close(fd1[0]);
        close(fd2[1]);
        client(fd2[0], fd1[1]);

        if (waitpid(childpid, NULL, 0) < 0) {
            perror("waitpid error");
            exit(1);
        }
        exit(0);
    }
}

void client(int readfd, int writefd)
{
    int length;
    ssize_t n;
    struct message mesg;

    printf("Give the name of the file:\n");
    fgets(mesg.message_data, MAXMESSAGEDATA, stdin);

    length = strlen(mesg.message_data);
    if (mesg.message_data[length - 1] == '\n')
        length--;

    mesg.message_length = length;
    mesg.message_type = 1;

    // Send request to server
    write(writefd, &mesg, MESGHDRSIZE + mesg.message_length);

    // Receive response
    while (1) {
        if ((n = read(readfd, &mesg, MESGHDRSIZE)) == -1) {
            perror("read error");
            exit(1);
        }

        if (n != MESGHDRSIZE) {
            fprintf(stderr, "header size mismatch\n");
            exit(1);
        }

        length = mesg.message_length;
        if (length == 0)
            break;

        n = read(readfd, mesg.message_data, length);
        write(STDOUT_FILENO, mesg.message_data, n);
    }
}

void server(int readfd, int writefd)
{
    FILE *fp;
    ssize_t n;
    struct message mesg;
    size_t length;

    mesg.message_type = 1;

    // Read request
    n = read(readfd, &mesg, MESGHDRSIZE);
    if (n != MESGHDRSIZE) {
        fprintf(stderr, "header size mismatch\n");
        exit(1);
    }

    length = mesg.message_length;
    n = read(readfd, mesg.message_data, length);
    mesg.message_data[n] = '\0';

    // Open file
    if ((fp = fopen(mesg.message_data, "r")) == NULL) {
        snprintf(mesg.message_data + n,
                 sizeof(mesg.message_data) - n,
                 ": can't open\n");

        mesg.message_length = strlen(mesg.message_data);
        write(writefd, &mesg, MESGHDRSIZE + mesg.message_length);
    }
    else {
        while (fgets(mesg.message_data, MAXMESSAGEDATA, fp) != NULL) {
            mesg.message_length = strlen(mesg.message_data);
            write(writefd, &mesg, MESGHDRSIZE + mesg.message_length);
        }
        fclose(fp);
    }

    // End message
    mesg.message_length = 0;
    write(writefd, &mesg, MESGHDRSIZE + mesg.message_length);
}

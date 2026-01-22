#include<stdio.h>      // printf(), perror(), fgets()
#include <stdlib.h>     // exit()
#include <sys/types.h>  // pid_t
#include <unistd.h>     // pipe(), fork(), read(), write(), close()
#include <string.h>     // strlen()
#include <sys/wait.h>   // waitpid()
#include "pipe.h"

/* Function declarations */
void client(int, int);
void server(int, int);

int main(int argc, char **argv)
{
    int fd1[2], fd2[2];     // Two pipes
    pid_t childpid;         // Process ID

    /* Create first pipe (client → server) */
    if (pipe(fd1) < 0) {
        perror("pipe creation error");
        exit(1);
    }

    /* Create second pipe (server → client) */
    if (pipe(fd2) < 0) {
        perror("pipe creation error");
        exit(1);
    }

    /* Create child process */
    if ((childpid = fork()) < 0) {
        perror("fork error");
        exit(1);
    }

    /* ================= CHILD PROCESS (SERVER) ================= */
    else if (childpid == 0) {

        /* Close unused pipe ends */
        close(fd2[0]);  // Server does not read from fd2
        close(fd1[1]);  // Server does not write to fd1

        /* Execute server logic */
        server(fd1[0], fd2[1]);
        exit(0);
    }

    /* ================= PARENT PROCESS (CLIENT) ================= */
    else {

        /* Close unused pipe ends */
        close(fd1[0]);  // Client does not read from fd1
        close(fd2[1]);  // Client does not write to fd2

        /* Execute client logic */
        client(fd2[0], fd1[1]);

        /* Wait for child to finish */
        if (waitpid(childpid, NULL, 0) < 0) {
            perror("waitpid error");
            exit(1);
        }
        exit(0);
    }
}
void client(int readfd, int writefd)
{
    int length;             // Length of message data
    ssize_t n;              // Number of bytes read/written
    struct message mesg;    // Message structure

    /* Ask user for file name */
    printf("Give the name of the file\n");

    /* Read file name from user */
    fgets(mesg.message_data, MAXMESSAGEDATA, stdin);

    /* Calculate length */
    length = strlen(mesg.message_data);

    /* Remove newline character */
    if (mesg.message_data[length - 1] == '\n')
        length--;

    /* Fill message header */
    mesg.message_length = length;
    mesg.message_type = 1;

    /* Send file name to server */
    write(writefd, &mesg, MESGHDRSIZE + mesg.message_length);

    /* Read server response */
    while (1) {

        /* Read message header */
        n = read(readfd, &mesg, MESGHDRSIZE);
        if (n == -1) {
            perror("read error");
            exit(1);
        }

        /* Validate header size */
        if (n != MESGHDRSIZE) {
            fprintf(stderr, "header size not same\n");
            exit(1);
        }

        /* End of file indicator */
        length = mesg.message_length;
        if (length == 0)
            break;

        /* Read actual data */
        n = read(readfd, mesg.message_data, length);

        /* Output data to screen */
        write(STDOUT_FILENO, mesg.message_data, n);
    }
}
void server(int readfd, int writefd)
{
    FILE *fp;               // File pointer
    ssize_t n;              // Bytes read
    struct message mesg;    // Message structure
    size_t length;          // Data length

    mesg.message_type = 1;

    /* Read message header */
    n = read(readfd, &mesg, MESGHDRSIZE);
    if (n != MESGHDRSIZE) {
        fprintf(stderr, "header size not same\n");
        exit(1);
    }

    /* Read filename */
    length = mesg.message_length;
    n = read(readfd, mesg.message_data, length);
    mesg.message_data[n] = '\0';

    /* Open file */
    if ((fp = fopen(mesg.message_data, "r")) == NULL) {

        /* File open error message */
        snprintf(mesg.message_data + n,
                 sizeof(mesg.message_data) - n,
                 ": cant open\n");

        mesg.message_length = strlen(mesg.message_data);
        write(writefd, &mesg, MESGHDRSIZE + mesg.message_length);
    }
    else {

        /* Send file contents line by line */
        while (fgets(mesg.message_data, MAXMESSAGEDATA, fp) != NULL) {
            mesg.message_length = strlen(mesg.message_data);
            mesg.message_type = 1;
            write(writefd, &mesg, MESGHDRSIZE + mesg.message_length);
        }

        fclose(fp);
    }

    /* Send end-of-file message */
    mesg.message_length = 0;
    write(writefd, &mesg, MESGHDRSIZE);
}
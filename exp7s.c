#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    struct sockaddr_in server, client;
    int sd, connfd;
    socklen_t client_len;
    char buffer[100];
    int i, len, temp;

    // 1. Create socket
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0)
    {
        perror("Socket failed");
        exit(1);
    }

    // 2. Initialize server address
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[1]));
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    // 3. Bind socket
    if (bind(sd, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("Bind failed");
        exit(1);
    }

    // 4. Listen
    listen(sd, 5);
    printf("Server waiting for connection...\n");

    // 5. Accept connection
    client_len = sizeof(client);
    connfd = accept(sd, (struct sockaddr *)&client, &client_len);
    if (connfd < 0)
    {
        perror("Accept failed");
        exit(1);
    }

    // 6. Read string from client
    read(connfd, buffer, sizeof(buffer));
    printf("Received from client: %s\n", buffer);

    // 7. Reverse string
    len = strlen(buffer);
    for (i = 0; i < len / 2; i++)
    {
        temp = buffer[i];
        buffer[i] = buffer[len - 1 - i];
        buffer[len - 1 - i] = temp;
    }

    // 8. Send reversed string
    write(connfd, buffer, sizeof(buffer));

    // 9. Close sockets
    close(connfd);
    close(sd);

    return 0;
}

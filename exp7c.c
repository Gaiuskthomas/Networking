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
    struct sockaddr_in server;
    int sd;
    char buffer[200];

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
    server.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &server.sin_addr);

    // 3. Connect to server
    if (connect(sd, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("Connection failed");
        exit(1);
    }

    // 4. Read string from user
    printf("Enter a string: ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strlen(buffer) - 1] = '\0';

    // 5. Send string to server
    write(sd, buffer, sizeof(buffer));

    // 6. Read reversed string
    read(sd, buffer, sizeof(buffer));

    // 7. Display output
    printf("Reversed string: %s\n", buffer);

    // 8. Close socket
    close(sd);

    return 0;
}

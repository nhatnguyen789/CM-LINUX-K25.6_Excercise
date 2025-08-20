#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/un.h>

#define SOCKET_PATH "/tmp/my_unix_socket.sock" //must be identical to the server path

int main()
{
    int client_fd;
    struct sockaddr_un server_addr;
    char buffer[1024] = {0};
    const char *message = "Hello from Unix Stream client";

    //-----STEP 1: CREATE SOCKET ENDPOINT-----
    if((client_fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
        perror("Sock creation failed");
        exit(EXIT_FAILURE);
    }

    // Clear the server address struct cache
    memset(&server_addr, 0, sizeof(struct sockaddr_un));

    //-----STEP 2: FILL IN THE SERVER ADDRESS INFORMATION (file path)-----
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);
    
    //-----STEP 3: CONNECT TO SERVER-----
    if(connect(client_fd, (const struct sockaddr *)&server_addr, sizeof(struct sockaddr_un)) < 0)
    {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server.\n");

    //-----STEP 4,5,6: COMMUNICATE AND CLOSE CONNECTION (Similar to TCP)-----
    send(client_fd, message, strlen(message), 0);
    printf("Message sent.\n");

    read(client_fd, buffer, 1024);
    printf("Server response: %s\n", buffer);

    return 0;
}
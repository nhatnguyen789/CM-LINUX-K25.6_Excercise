#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/un.h>  // Main library for Unix domain sockets

//File path will be used as socket address
//Usually located in /tmp
#define SOCKET_PATH "/tmp/my_unix_socket.sock"

int main()
{
    int server_fd, new_socket;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buffer[1024] = {0};
    const char *reply_message = "Connected to unix stream server";

    //---IMPORTANT STEP: CLEAN UP OLD SOCKET FILE---
    // Delete the socket file if it still exists from the previous run to avoid 
    // "Address Already in use" error
    unlink(SOCKET_PATH);

    //---STEP 1: CREATE SOCKET ENDPOINT---
    // AF_UNIX: Use Unix address family (filesystem-based)
    // SOCK_STREAM: Reliable, stream-based communication
    if((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Clear the address struct cache
    memset(&server_addr, 0, sizeof(struct sockaddr_un));

    //---STEP 2: ASSIGN ADDRESS (file path) TO SOCKET---
    server_addr.sun_family = AF_UNIX;

    // Copy file path into address structure
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    if(bind(server_fd, (const struct sockaddr*)&server_addr, sizeof(struct sockaddr_un)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    
    //---STEP 3: LISTENING FOR INCOMING CONNECTIONS---
    if(listen(server_fd, 3) < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server Unix Stream is waiting for connection at '%s'...\n", SOCKET_PATH);

    //---STEP 4: ACCEPT CONNECTION FROM CLIENT---
    if((new_socket = accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_len)) < 0)
    {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    printf("Connection accepted\n");

    //---STEP 5,6,7: COMMUNICATION AND CONNECTION CLOSING (SIMILAR TO TCP)---
    read(new_socket, buffer, 1024);
    printf("Client say: %s\n", buffer);

    send(new_socket, reply_message, strlen(reply_message), 0);
    printf("Sent feedback.\n");

    close(new_socket);
    close(server_fd);

    //---Final Cleanup---
    unlink(SOCKET_PATH);

    return 0;
}

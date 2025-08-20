#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/un.h>


#define SERVER_SOCKET_PATH "/tmp/my_unix_dgram.sock"      // Server address
#define CLIENT_SOCKET_PATH "/tmp/my_unix_dgram_client.sock" // Client specific address
#define BUFFER_SIZE 1024

int main()
{
    int client_fd;
    struct sockaddr_un server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    const char *message = "Hello from UNIX Datagram Client";
    
    //---CLEAN UP OLD CLIENT SOCKET FILE---
    unlink(CLIENT_SOCKET_PATH);

    //---STEP 1: CREATE SOCKET ENDPOINT---
    if((client_fd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // --- Step 2: Assign the address (bind) to the Client itself ---
    // This is necessary so that the Server knows where to send the reply.
    memset(&client_addr, 0, sizeof(struct sockaddr_un));
    client_addr.sun_family = AF_UNIX;
    strncpy(client_addr.sun_path, CLIENT_SOCKET_PATH, sizeof(client_addr.sun_path) - 1);

    if(bind(client_fd, (const struct sockaddr*)&client_addr, sizeof(struct sockaddr_un)) < 0)
    {
        perror("Client bind failed");
        exit(EXIT_FAILURE);
    }

    //---STEP 3: FILL IN THE SERVER ADDRESS INFORMATION AND SEND DATA---
    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SERVER_SOCKET_PATH, sizeof(server_addr.sun_path));

    sendto(client_fd, message, strlen(message), 0, (const struct sockaddr *)&server_addr, sizeof(struct sockaddr_un));
    printf("Sent packet to server.\n");

    //---STEP 4: WAIT FOR RESPONSE
    int n = recvfrom(client_fd, buffer, BUFFER_SIZE, 0, NULL, NULL);
    if(n < 0)
    {
        perror("recvfrom failed");
        exit(EXIT_FAILURE);
    }
    buffer[n] = '\0';
    printf("Server response: %s\n", buffer);

    //---STEP 5: CLOSE AND CLEAN UP---
    close(client_fd);
    unlink(CLIENT_SOCKET_PATH);

    return 0;
}
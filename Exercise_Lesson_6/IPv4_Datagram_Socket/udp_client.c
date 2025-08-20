#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"   //Server Port
#define BUFFER_SIZE 1024    // Server IP address

int main()
{
    int client_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    const char *message = "Hello from UDP client!";

    //-----STEP 1: CREATE SOCKET ENDPOINT-----
    if((client_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Socket creation failed!\n");
        exit(EXIT_FAILURE);
    }

    // Clear the cache of the server address struct
    memset(&server_addr, 0, sizeof(server_addr));

    //-----STEP 2: FILL IN THE SERVER ADDRESS INFORMATION-----
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if(inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0)
    {
        perror("Invalid address");
        exit(EXIT_FAILURE);
    }

    //-----STEP 3: SEND DATA TO SERVER-----
    //No need to connect, just send directly with server address
    sendto(client_fd, message, strlen(message), 0, (const struct sockaddr*)&server_addr, sizeof(server_addr));
    printf("Sent packet to server\n");


    //-----STEP 4: WAITING FOR RESPONSE FROM SERVER-----
    socklen_t server_addr_len = sizeof(server_addr);
    int n = recvfrom(client_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&server_addr, &server_addr_len);
    if(n < 0)
    {
        perror("recvfrom failed");
        exit(EXIT_FAILURE);
    }

    buffer[n] = '\0';
    printf("Server respond: %s\n", buffer);

    //-----STEP 5: CLOSE SOCKET-----
    close(client_fd);

    return 0;
}


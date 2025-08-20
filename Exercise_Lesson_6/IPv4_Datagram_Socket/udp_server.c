#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main()
{
    int server_fd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    socklen_t client_addr_len = sizeof(client_addr);
    const char *reply_message = "The packet has been received by the server";

    //-----STEP 1: CREATE SOCKET ENDPOINT-----
    // AF_INET: Use IPv4 address family 
    // SOCK_DGRAM: Use Datagram(UDP) socket type, no connection required
    if((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Socket creation failed!");
        exit(EXIT_FAILURE);
    }

    // Clear the cache of address structs 
    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr,0, sizeof(client_addr));

    //-----STEP 2: ASSIGN ADDRESS (IP + PORT) TO SOCKET-----
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // Listen on every machine IP address
    server_addr.sin_port = htons(PORT);

    if(bind(server_fd, (const struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    printf("UDP server waiting for message on PORT: %d...\n", PORT);

    //-----STEP 3: WAIT FOR DATA FROM CLIENT-----
    // recvfrom(): is a blocking function, it will wait here until it receives a packet
    // it also fills the client's address information into the client_addr variable
    int n = recvfrom(server_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client_addr, &client_addr_len);
    if(n < 0)
    {
        perror("recvfrom failed");
        exit(EXIT_FAILURE);
    }

    buffer[n] = '\0';   // Make sure the string ends with null

    // Print received information
    printf("Receive client packet: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    printf("Content: %s\n", buffer);

    //-----STEP 5: SEND REPONSE BACK TO CLIENT-----
    // Use the address the client just received to send a response packet
    sendto(server_fd, reply_message, strlen(reply_message), 0, (const struct sockaddr *)&client_addr, client_addr_len);
    printf("Send feedback to client.\n");

    //-----STEP 5: CLOSE SOCKET-----
    close(server_fd);

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_MESSAGE_LENGTH 1024

int connectToRemoteServer(const char* server_addr, int server_port) {
    int client_socket;
    struct sockaddr_in server_address;
    char message[MAX_MESSAGE_LENGTH];

    // Create a socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Error: Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Prepare the server address structure
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(server_addr);
    server_address.sin_port = htons(server_port);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("CError: onnection failed");
        exit(EXIT_FAILURE);
    }

    printf("Successfully connected to the server.\n");

    while (1) {
        // Get a message from the user
        printf("> ");
        fgets(message, sizeof(message), stdin);

        // Remove the trailing newline character
        message[strcspn(message, "\n")] = '\0';

        // Send the message to the server
        if (send(client_socket, message, strlen(message), 0) == -1) {
            perror("Error: Message sending failed");
            exit(EXIT_FAILURE);
        }

        // Receive a response from the server
        memset(message, 0, sizeof(message));
        ssize_t bytes_received = recv(client_socket, message, sizeof(message), 0);
        if (bytes_received == -1) {
            perror("Error: Response receiving failed");
            exit(EXIT_FAILURE);
        }
        if (bytes_received == 0) {
            printf("Server disconnected.\n");
            break;
        }

        printf("Received from server: %s\n", message);
    }

    // Close the socket
    close(client_socket);

    return 0;
}
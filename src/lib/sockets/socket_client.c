#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_MESSAGE_LENGTH 1000

int sendAndReceive(const char* server_addr, int server_port, char* message) {
    int client_socket;
    struct sockaddr_in server_address;

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
        perror("Error: failed connecting to remote server.");
        exit(EXIT_FAILURE);
    }

    if (send(client_socket, message, MAX_MESSAGE_LENGTH, 0) == -1) {
        perror("Error: failed sending message to remote server.");
        exit(EXIT_FAILURE);
    }

    memset(message, 0, sizeof(*message) * MAX_MESSAGE_LENGTH);
    ssize_t bytes_received = recv(client_socket, message, MAX_MESSAGE_LENGTH, 0);
    if (bytes_received == -1) {
        perror("Error: failed receiving response from remote server.");
        exit(EXIT_FAILURE);
    }
    if (bytes_received == 0) {
        perror("Error: server disconnected.");
        exit(EXIT_FAILURE);
    }

    // Close the socket
    close(client_socket);

    return 0;
}
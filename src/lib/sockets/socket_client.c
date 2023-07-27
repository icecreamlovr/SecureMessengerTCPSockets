#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_MESSAGE_LENGTH 1000

void simpleSendAndReceive(int client_socket, char* message) {
    // send(client_socket, message, MAX_MESSAGE_LENGTH, 0)
    if (send(client_socket, message, strlen(message) + 1, 0) == -1) {
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
}

// + 2 buffer, 1 for send and 1 for receive
// + rename this to sendAndReceiveBasic
// + wrap inside another function, which can encrypt msg, and decrypt response
// + free, or memset, outside of the function call
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
        perror("[DEBUG] Failed connecting to remote server.");
        return 0;
    }

    simpleSendAndReceive(client_socket, message);

    close(client_socket);
    return 1;
}
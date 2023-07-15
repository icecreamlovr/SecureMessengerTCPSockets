#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include "util.h"

#define MAX_MESSAGE_LENGTH 1024
#define SERVER_PORT 12345
#define MAX_LISTEN_QUEUE 10

void* handleClient(void* arg) {
    int client_socket = *((int*)arg);
    char message[MAX_MESSAGE_LENGTH];

    while (1) {
        // Receive a message from the client
        memset(message, 0, sizeof(message));
        ssize_t bytes_received = recv(client_socket, message, sizeof(message), 0);
        if (bytes_received == -1) {
            printf("[Client %d] Message receiving failed\n", client_socket);
            perror("Error: Message receiving failed");
            exit(EXIT_FAILURE);
        } else if (bytes_received == 0) {
            printf("[Client %d] Client disconnected.\n", client_socket);
            break;
        }

        printf("[Client %d] Received from client: %s\n", client_socket, message);

        // Send a response back to the client
        if (send(client_socket, message, strlen(message), 0) == -1) {
            printf("[Client %d] Response sending failed\n", client_socket);
            perror("Error: Response sending failed");
            exit(EXIT_FAILURE);
        }
    }

    // Close the client socket
    close(client_socket);

    pthread_exit(NULL);
}

int main() {
    printf("%d + %d = %d\n", 3, 5, Sum(3, 5));

    // Create a socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error: Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to a specific address and port
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(SERVER_PORT);

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Error: Socket binding failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, MAX_LISTEN_QUEUE) == -1) {
        perror("Error: Socket listening failed");
        exit(EXIT_FAILURE);
    }

    printf("Server started. Waiting for clients to connect...\n");

    while (1) {
        struct sockaddr_in client_address;
        socklen_t client_address_length = sizeof(client_address);

        // Blocking call to accept a client connection
        int client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_length);
        if (client_socket == -1) {
            perror("Error: Accept client connection failed");
            exit(EXIT_FAILURE);
        }

        printf("[Client %d] Client connected.\n", client_socket);

        // Create a new thread to handle this client.
        // The main thread will go back to wait for new client connection
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handleClient, &client_socket) != 0) {
            perror("Error: Thread creation failed");
            exit(EXIT_FAILURE);
        }

        // Detach the thread (to allow it to run independently)
        if (pthread_detach(thread_id) != 0) {
            perror("Error: Thread detachment failed");
            exit(EXIT_FAILURE);
        }
    }

    // Close the server socket
    close(server_socket);

    return 0;
}

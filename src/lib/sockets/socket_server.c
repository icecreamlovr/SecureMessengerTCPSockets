#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_MESSAGE_LENGTH 1000
#define MAX_LISTEN_QUEUE 10

// Simple listener that prints the message and sends a response.
void simpleMessageListener(int client_socket, int length, const char* message) {
    // Print the received message from client
    printf("[Client %d]: (%d bytes) %s\n", client_socket, length, message);
    printf("[DEBUG][Client %d]: Received %d bytes: <start>", client_socket, length);
    for (int i = 0; i < length; i++) {
        printf("%02X", message[i]);
    }
    printf("<end>\n");

    // Send a response back to the client
    if (send(client_socket, message, length, 0) == -1) {
        fprintf(stderr, "Error: [Client %d] Response sending failed.\n", client_socket);
        exit(EXIT_FAILURE);
    }
}

void* handleIncomingConnection(void* arg) {
    int client_socket = *((int*)arg);
    char message[MAX_MESSAGE_LENGTH];

    while (1) {
        // Receive a message from the client
        memset(message, 0, MAX_MESSAGE_LENGTH);
        ssize_t bytes_received = recv(client_socket, message, MAX_MESSAGE_LENGTH, 0);
        if (bytes_received == -1) {
            fprintf(stderr, "Error: [Client %d] Message receiving failed.\n", client_socket);
            exit(EXIT_FAILURE);
        } else if (bytes_received == 0) {
            printf("[DEBUG][Client %d] Client disconnected.\n", client_socket);
            break;
        }

        simpleMessageListener(client_socket, bytes_received, message);
    }

    // Close the client socket
    close(client_socket);

    pthread_exit(NULL);
}

void* startSocketServer(void* arg) {
    int server_port = *(int*)arg;
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
    server_address.sin_port = htons(server_port);

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Error: Socket binding failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, MAX_LISTEN_QUEUE) == -1) {
        perror("Error: Socket listening failed");
        exit(EXIT_FAILURE);
    }

    printf("[DEBUG] Server started on: %d.\n", server_port);

    while (1) {
        struct sockaddr_in client_address;
        socklen_t client_address_length = sizeof(client_address);

        // Blocking call to accept a client connection
        int client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_length);
        if (client_socket == -1) {
            perror("Error: Accept client connection failed");
            exit(EXIT_FAILURE);
        }

        printf("[DEBUG][Client %d] Client connected.\n", client_socket);

        // Create a new thread to handle this client.
        // The main thread will go back to wait for new client connection
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handleIncomingConnection, &client_socket) != 0) {
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
}

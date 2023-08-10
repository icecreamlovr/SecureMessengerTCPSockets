#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include "socket_server.h"
#include "../crypto/rsa_encryption.h"
#include "../crypto/rsa_store.h"

#define MAX_MESSAGE_LENGTH 1000
#define MAX_LISTEN_QUEUE 10

extern int debug_mode;

void encryptedMessageListener(char* local_keypair_dir, char* server_privkey_filename, const char* other_pubkey_dir, int client_socket, int length, unsigned char* message) {
    RSA* decrypt_privkey = readPrivateKeyFromFile(local_keypair_dir, server_privkey_filename);
    char *decrypted_msg = NULL;

    // Decrypt the received message with the private key
    if (!decryptRSA(decrypt_privkey, (unsigned char*)message, length, &decrypted_msg)) {
        fprintf(stderr, "Error: [Client %d]: Decryption failed\n", client_socket);
        // exit(EXIT_FAILURE);
        return;
    }

    printf("[Received from Client %d]: %s\n", client_socket, decrypted_msg);
}

// Simple listener that prints the message and sends a response.
void simpleMessageListener(int client_socket, int length, const char* message) {
    // Print the received message from client
    printf("[Received from client %d]: %s\n", client_socket, message);

    // Send a response back to the client
    if (send(client_socket, message, length, 0) == -1) {
        fprintf(stderr, "Error: [Client %d] Response sending failed.\n", client_socket);
        // exit(EXIT_FAILURE);
        return;
    }
}

struct HandleIncomingConnectionArgs {
    int client_socket;
    char* server_pubkey_filename;
    char* server_privkey_filename;
    char* local_keypair_dir;
    char* other_pubkey_dir;
};

void* handleIncomingConnection(void* arg) {
    struct HandleIncomingConnectionArgs* args = (struct HandleIncomingConnectionArgs*)arg;
    int client_socket = args->client_socket;
    char* server_pubkey_filename = args->server_pubkey_filename;
    char* server_privkey_filename = args->server_privkey_filename;
    char* local_keypair_dir = args->local_keypair_dir;
    char* other_pubkey_dir = args->other_pubkey_dir;

    unsigned char message[MAX_MESSAGE_LENGTH];

    while (1) {

        // Receive a message from the client
        memset(message, 0, MAX_MESSAGE_LENGTH);
        ssize_t bytes_received = recv(client_socket, message, MAX_MESSAGE_LENGTH, 0);
        if (bytes_received == -1) {
            fprintf(stderr, "Error: [Client %d] Message receiving failed.\n", client_socket);
            // exit(EXIT_FAILURE);
            return NULL;
        } else if (bytes_received == 0) {
//            printf("[DEBUG][Client %d] Client disconnected.\n", client_socket);
            break;
        }
        if (debug_mode) {
            printf("[DEBUG][Client %d]: Received %d bytes: <start>", client_socket, (int)bytes_received);
            for (int i = 0; i < (int)bytes_received; i++) {
                printf("%02X", message[i]);
            }
            printf("<end>\n");
        }

        // simpleMessageListener(client_socket, bytes_received, message);
        // RSA* server_privkey = readPrivateKeyFromFile(local_keypair_dir, server_privkey_filename);
        encryptedMessageListener(local_keypair_dir, server_privkey_filename, other_pubkey_dir, client_socket, bytes_received, message);
    }

    // Close the client socket
    close(client_socket);

    pthread_exit(NULL);
}

void* startSocketServer(void* arg) {
    struct StartSocketServerArgs* args = (struct StartSocketServerArgs*)arg;
    int server_port = args->server_port;
    char* server_pubkey_filename = args->server_pubkey_filename;
    char* server_privkey_filename = args->server_privkey_filename;
    char* local_keypair_dir = args->local_keypair_dir;
    char* other_pubkey_dir = args->other_pubkey_dir;

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

    printf("[INFO] Server started on: %d.\n", server_port);

    while (1) {
        struct sockaddr_in client_address;
        socklen_t client_address_length = sizeof(client_address);

        // Blocking call to accept a client connection
        int client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_length);
        if (client_socket == -1) {
            perror("Error: Accept client connection failed");
            close(server_socket);
            exit(EXIT_FAILURE);
        }

//        printf("[DEBUG][Client %d] Client connected.\n", client_socket);

        // Create a new thread to handle this client.
        // The main thread will go back to wait for new client connection
        struct HandleIncomingConnectionArgs connThreadArgs;
        connThreadArgs.client_socket = client_socket;
        connThreadArgs.server_pubkey_filename = server_pubkey_filename;
        connThreadArgs.server_privkey_filename = server_privkey_filename;
        connThreadArgs.local_keypair_dir = local_keypair_dir;
        connThreadArgs.other_pubkey_dir = other_pubkey_dir;
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handleIncomingConnection, (void*)&connThreadArgs) != 0) {
            perror("Error: Thread creation failed");
            close(server_socket);
            exit(EXIT_FAILURE);
        }

        // Detach the thread (to allow it to run independently)
        if (pthread_detach(thread_id) != 0) {
            perror("Error: Thread detachment failed");
            close(server_socket);
            exit(EXIT_FAILURE);
        }
    }

    // Close the server socket
    close(server_socket);
}

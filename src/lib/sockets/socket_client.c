#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include "../crypto/rsa_encryption.h"
#include "../crypto/rsa_store.h"

#define MAX_MESSAGE_LENGTH 1000

extern int debug_mode;

int sendAndReceive(const char* server_addr, int server_port, unsigned char* message, int send_len, int* recv_len) {
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

    // send(client_socket, message, MAX_MESSAGE_LENGTH, 0)
    if (send(client_socket, message, send_len, 0) == -1) {
        perror("Error: failed sending message to remote server.");
        exit(EXIT_FAILURE);
    }

    memset(message, 0, sizeof(*message) * MAX_MESSAGE_LENGTH);
    sleep(1);
    printf("Message delivered.\n");

    close(client_socket);
    return 1;
}

int encryptedSendAndReceive(char* base_dir, char* other_pubkey_dir, char* pubkey_file, char* privkey_file, const char* server_addr, int server_port, char* message) {
    RSA* encrypt_pubkey = readPublicKeyFromFile(other_pubkey_dir, pubkey_file);

    unsigned char *encrypted_msg = NULL;
    size_t encrypted_msg_len;

    // Encrypt the sending message with the public key
    if (!encryptRSA(encrypt_pubkey, message, &encrypted_msg, &encrypted_msg_len)) {
        fprintf(stderr, "Error: Encryption failed\n");
        exit(EXIT_FAILURE);
    }
    if (debug_mode) {
        printf("[DEBUG] Sending encrypted message (%d): ", (int)encrypted_msg_len);
        for (size_t i = 0; i < encrypted_msg_len; i++) {
            printf("%02X", encrypted_msg[i]);
        }
        printf("\n");
    }

    // Send through sockets
    int recv_len = 0;
    int send_result = sendAndReceive(server_addr, server_port, encrypted_msg, encrypted_msg_len, &recv_len);
    if (send_result != 1) {
        free(encrypted_msg);
        return send_result;
    }
    free(encrypted_msg);
    return 1;
}

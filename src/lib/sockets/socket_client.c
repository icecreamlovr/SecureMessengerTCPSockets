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
    ssize_t bytes_received = recv(client_socket, message, MAX_MESSAGE_LENGTH, 0);
    if (bytes_received == -1) {
        perror("Error: failed receiving response from remote server.");
        exit(EXIT_FAILURE);
    }
    if (bytes_received == 0) {
        perror("Error: server disconnected.");
        exit(EXIT_FAILURE);
    }
    *recv_len = (int)bytes_received;


    printf("[DEBUG]: Received %d bytes: <start>", (int)bytes_received);
    for (int i = 0; i < bytes_received; i++) {
        printf("%02X", message[i]);
    }
    printf("<end>\n");

    close(client_socket);
    return 1;
}

int encryptedSendAndReceive(char* base_dir, char* other_pubkey_dir, char* pubkey_file, char* privkey_file, const char* server_addr, int server_port, char* message) {
    RSA* encrypt_pubkey = readPublicKeyFromFile(other_pubkey_dir, pubkey_file);
    printf("[DEBUG] encrypt key (begin)\n");
    PEM_write_RSAPublicKey(stdout, encrypt_pubkey);
    printf("[DEBUG] encrypt key (end)\n");

    unsigned char *encrypted_msg = NULL;
    size_t encrypted_msg_len;

    // Encrypt the sending message with the public key
    if (!encryptRSA(encrypt_pubkey, message, &encrypted_msg, &encrypted_msg_len)) {
        fprintf(stderr, "Error: Encryption failed\n");
        exit(EXIT_FAILURE);
    }
    printf("[DEBUG] Sending encrypted message (%d): ", (int)encrypted_msg_len);
    for (size_t i = 0; i < encrypted_msg_len; i++) {
        printf("%02X", encrypted_msg[i]);
    }
    printf("\n");

    // Debug: verify we can decrypt the message
    // char recipient_dir[20];
    // snprintf(recipient_dir, sizeof(recipient_dir), "user_%d", server_port);
    // char* recipient_privkey_filename = getRsaPrivateKeyFileName(server_addr, server_port);
    // RSA* recipient_privkey = readPrivateKeyFromFile(recipient_dir, recipient_privkey_filename);
    // if (!decryptRSA(recipient_privkey, encrypted_msg, encrypted_msg_len, &decrypted_msg)) {
    //     fprintf(stderr, "Error: Verify decryption failed\n");
    //     free(encrypted_msg);
    //     return 1;
    // }
    // printf("[DEBUG] verify - key: %s/%s. result: %s\n", recipient_dir, recipient_privkey_filename, decrypted_msg);
    // free(decrypted_msg);

    // Send through sockets
    int recv_len = 0;
    int send_result = sendAndReceive(server_addr, server_port, encrypted_msg, encrypted_msg_len, &recv_len);
    if (send_result != 1) {
        return send_result;
    }
    printf("[DEBUG] Receiving encrypted message (%d): ", (int)recv_len);
    for (size_t i = 0; i < recv_len; i++) {
        printf("%02X", encrypted_msg[i]);
    }
    printf("\n");

    RSA* decrypt_privkey = readPrivateKeyFromFile(base_dir, privkey_file);
    // printf("[DEBUG] Decrypt key for response1 (begin)\n");
    // PEM_write_RSAPrivateKey(stdout, decrypt_privkey, NULL, NULL, 0, NULL, NULL);
    // printf("[DEBUG] Decrypt key for response (end)\n");

    // Decrypt the received message with the private key
    char *decrypted_msg = NULL;
    if (!decryptRSA(decrypt_privkey, encrypted_msg, recv_len, &decrypted_msg)) {
        fprintf(stderr, "[DEBUG] Decryption failed\n");
        free(encrypted_msg);
        // exit(EXIT_FAILURE);
        return 0;
    }
    printf("[DEBUG] Decryption finished\n");
    printf("[DEBUG] Decrypted message: %s\n", decrypted_msg);

    free(encrypted_msg);
    free(decrypted_msg);
    return 1;
}

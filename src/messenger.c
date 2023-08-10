#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include "lib/cli/cli_flags.h"
#include "lib/crypto/rsa_store.h"
#include "lib/crypto/rsa_encryption.h"
#include "lib/sockets/socket_server.h"
#include "lib/sockets/socket_client.h"

#define MAX_INTERACTIVE_CLI_LENGTH 2000
#define MAX_MESSAGE_LENGTH 1000

int debug_mode = 0;

void printUsage() {
    printf("Type 'message <IP> <PORT> <MESSAGE>' to send message to an user on specific IP and port.\n");
    printf("Type 'keygen' to generate or regenerate RSA key pairs.\n");
    printf("Type 'test' to run encryption and decryption on a simple message to verify the key pairs.\n");
    printf("Type 'debug on/off' to toggle debug mode to on/off.\n");
    printf("Type 'exit' to quit.\n");
}

int main(int argc, char *argv[]) {
    debug_mode = getDebugFromCliFlags(argc, argv, 0);
    if (debug_mode) {
        printf("[DEBUG] Debug mode is on\n");
    }

    const char* host_ip = "127.0.0.1";
    int host_port = getListeningPortFromCliFlags(argc, argv);
    printf("[INFO] Listening port value: %d\n", host_port);


    // Create base folder if it doesn't already exist. Use user_<port> as default name of the folder if unspecified.
    const char* OTHER_PUBKEY_DIR = "public";
    char* base_file_directory;
    char default_base_file_directory[20];
    snprintf(default_base_file_directory, sizeof(default_base_file_directory), "user_%d", host_port);
    base_file_directory = getFileDirectoryFromCliFlags(argc, argv, default_base_file_directory);
    int created = createRsaDirectoryIfNotExist(base_file_directory);
    if (created == 0) {
        printf("[INFO] Directory %s already exists\n", base_file_directory);
    } else {
        printf("[INFO] Created directory %s\n", base_file_directory);
    }
    int created2 = createRsaDirectoryIfNotExist(OTHER_PUBKEY_DIR);
    if (created2 == 0) {
        printf("[INFO] Directory %s already exists\n", OTHER_PUBKEY_DIR);
    } else {
        printf("[INFO] Created directory %s\n", OTHER_PUBKEY_DIR);
    }

    // Format file names for the host public key and private key.
    char* host_pubkey_filename = getRsaPublicKeyFileName(host_ip, host_port);
    char* host_privkey_filename = getRsaPrivateKeyFileName(host_ip, host_port);
    printf("[INFO] Host key pair file names: %s, %s\n", host_pubkey_filename, host_privkey_filename);
    // RSA* server_pubkey = readPublicKeyFromFile(base_file_directory, host_pubkey_filename);
    // RSA* server_privkey = readPrivateKeyFromFile(base_file_directory, host_privkey_filename);

    // Start socket server.
    // The server is started in a separate thread, so that the main thread can go back to wait for user inputs.
    struct StartSocketServerArgs serverThreadArgs;
    serverThreadArgs.server_port = host_port;
    serverThreadArgs.server_pubkey_filename = host_pubkey_filename;
    serverThreadArgs.server_privkey_filename = host_privkey_filename;
    serverThreadArgs.local_keypair_dir = (char*)base_file_directory;
    serverThreadArgs.other_pubkey_dir = (char*)OTHER_PUBKEY_DIR;
    pthread_t thread_id;
    if (pthread_create(&thread_id, NULL, startSocketServer, (void*)&serverThreadArgs) != 0) {
        perror("Error: Server thread creation failed");
        exit(EXIT_FAILURE);
    }
    if (pthread_detach(thread_id) != 0) {
        perror("Error: Thread detachment failed");
        exit(EXIT_FAILURE);
    }

    sleep(1);

    printUsage();

    // Handle user inputs in a loop.
    // Every time, prompt for user input, match it with one of the predefined directives, and execute.
    char user_input[MAX_INTERACTIVE_CLI_LENGTH];
    while (1) {
        // Prompt for user input.
        // printf("> ");
        // Remove the trailing newline character from input.
        memset(user_input, 0, MAX_INTERACTIVE_CLI_LENGTH);
        fgets(user_input, sizeof(user_input), stdin);
        user_input[strcspn(user_input, "\n")] = '\0';
        int success = 0;

        if (strcmp(user_input, "exit") == 0) {
            // User wants to exit.
            printf("Exiting...\n");
            break;
        } else if (strncmp(user_input, "message ", 8) == 0) {
            // User wants to message another user.
            // Example: message 127.0.0.1 12345 hi!!!
            // Example: message 127.0.0.1 12347 hello!!!
            char recipient_ip[100];
            int recipient_port;
            char message[MAX_MESSAGE_LENGTH];
            if (sscanf(user_input, "message %99s %d %[^\n]", recipient_ip, &recipient_port, message) == 3) {
                if (recipient_port == host_port) {
                    success = 0;
                    printf("[WARN] Please do not send message to yourself.\n");
                } else {
    //                printf("[DEBUG] From user %s(%d), %s:%d\n", message, (int)strlen(message), recipient_ip, recipient_port);
                    char* recipient_pubkey_file = getRsaPublicKeyFileName(recipient_ip, recipient_port);
                    int result = encryptedSendAndReceive(base_file_directory, OTHER_PUBKEY_DIR, recipient_pubkey_file, host_privkey_filename, recipient_ip, recipient_port, message);
                    if (result == 1) {
                        // printf("[DEBUG] received from server (%d): %s\n", recv_len, message);
                        // printf("[DEBUG] received from server (%d): %s\n", (int)strlen(message), message);
                        success = 1;
                    } else {
                        printf("[WARN] Failed to send message to %s:%d.\n", recipient_ip, recipient_port);
                    }
                    free(recipient_pubkey_file);
                }
            } else {
                printf("[WARN] Invalid input. Please use the format 'message <IP> <PORT> <MESSAGE>'.\n");
            }
        } else if (strncmp(user_input, "keygen", 6) == 0) {
            // User wants to generate / regenerate RSA key pairs.
            success = 1;
            generateKeyPairsAndSaveAsPem(base_file_directory, OTHER_PUBKEY_DIR, host_pubkey_filename, host_privkey_filename);
        } else if (strncmp(user_input, "test", 4) == 0) {
            // User wants to verify the generated key paris.
            success = 1;
            testEncryptionDecryption(
                "hello this is test", base_file_directory, host_pubkey_filename, host_privkey_filename);
        } else if (strncmp(user_input, "debug on", 8) == 0) {
            // User wants to turn on debug mode
            success = 1;
            debug_mode = 1;
        } else if (strncmp(user_input, "debug off", 9) == 0) {
            // User wants to turn on debug mode
            success = 1;
            debug_mode = 0;
        } else {
            printf("[WARN] Unrecognized input. Please try a different command.\n");
        }

//        if (success == 0) {
//
////            printUsage();
//        }
    }

    return 0;
}

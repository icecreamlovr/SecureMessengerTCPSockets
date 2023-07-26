#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "lib/cli/cli_flags.h"
#include "lib/sockets/socket_server.h"
#include "lib/sockets/socket_client.h"

#define MAX_INTERACTIVE_CLI_LENGTH 2000
#define MAX_MESSAGE_LENGTH 1000

int main(int argc, char *argv[]) {
    const char* host_ip = "127.0.0.1";
    int host_port = getListeningPortFromCliFlags(argc, argv);
    printf("[DEBUG] Listening port value: %d\n", host_port);

    // Use user_<port> as default name of the folder if unspecified.
    char default_base_file_directory[10];
    snprintf(default_base_file_directory, sizeof(default_base_file_directory), "user_%d", host_port);
    char* base_file_directory = getFileDirectoryFromCliFlags(argc, argv, default_base_file_directory);
    printf("[DEBUG] File directory value: %s\n", base_file_directory);

    // Start socket server.
    // The server is started in a separate thread, so that the main thread can go back to wait for user inputs.
    pthread_t thread_id;
    if (pthread_create(&thread_id, NULL, startSocketServer, (void*)&host_port) != 0) {
        perror("Error: Server thread creation failed");
        exit(EXIT_FAILURE);
    }
    if (pthread_detach(thread_id) != 0) {
        perror("Error: Thread detachment failed");
        exit(EXIT_FAILURE);
    }

    sleep(1);

    // Handle user inputs.
    char user_input[MAX_INTERACTIVE_CLI_LENGTH];
    printf("Type 'message <IP> <PORT> <MESSAGE>' send message to an user on specific IP and port.\n");
    printf("Type 'exit' to quit.\n");

    while (1) {
        // Prompt for user input.
        // Remove the trailing newline character from input.
        // printf("> ");
        fgets(user_input, sizeof(user_input), stdin);
        user_input[strcspn(user_input, "\n")] = '\0';

        if (strcmp(user_input, "exit") == 0) {
            // User wants to exit.
            printf("Exiting...\n");
            break;
        } else if (strncmp(user_input, "message ", 8) == 0) {
            // User wants to message another user.
            char recipient_ip[100];
            int recipient_port;
            char message[MAX_MESSAGE_LENGTH];
            if (sscanf(user_input, "message %99s %d %[^\n]", recipient_ip, &recipient_port, message) == 3) {\
                printf("[DEBUG] sending %s\n", message);
                sendAndReceive(recipient_ip, recipient_port, message);
                printf("[DEBUG] received from server: %s\n", message);
            } else {
                printf("Invalid message input. Please use the format 'message <IP> <PORT> <MESSAGE>'.\n");
            }
            continue;
        }
        printf("Unrecognized input.\n");
    }

    return 0;
}

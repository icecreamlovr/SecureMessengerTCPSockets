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
    int port = getListeningPortFromCliFlags(argc, argv);
    printf("[DEBUG] Listening port value: %d\n", port);

    // Start socket server.
    // The server is started in a separate thread, so that the main thread can go back to wait for user inputs.
    pthread_t thread_id;
    if (pthread_create(&thread_id, NULL, startSocketServer, (void*)&port) != 0) {
        perror("Error: Server thread creation failed");
        exit(EXIT_FAILURE);
    }
    if (pthread_detach(thread_id) != 0) {
        perror("Error: Thread detachment failed");
        exit(EXIT_FAILURE);
    }

    sleep(1);

    // Handle user inputs.
    char userInput[MAX_INTERACTIVE_CLI_LENGTH];
    printf("Type 'message <IP> <PORT> <MESSAGE>' send message to an user on specific IP and port.\n");
    printf("Type 'exit' to quit.\n");

    while (1) {
        // Prompt for user input.
        // Remove the trailing newline character from input.
        // printf("> ");
        fgets(userInput, sizeof(userInput), stdin);
        userInput[strcspn(userInput, "\n")] = '\0';

        if (strcmp(userInput, "exit") == 0) {
            // User wants to exit.
            printf("Exiting...\n");
            break;
        } else if (strncmp(userInput, "message ", 8) == 0) {
            // User wants to message another user.
            char recipientIp[100];
            int recipientPort;
            char message[MAX_MESSAGE_LENGTH];
            if (sscanf(userInput, "message %99s %d %[^\n]", recipientIp, &recipientPort, message) == 3) {\
                printf("[DEBUG] sending %s\n", message);
                sendAndReceive(recipientIp, recipientPort, message);
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "lib/sockets/socket_server.h"
#include "lib/sockets/socket_client.h"

#define MAX_INTERACTIVE_CLI_LENGTH 2000
#define MAX_MESSAGE_LENGTH 1000

int getCliListeningPort(int argc, char *argv[]) {
    int port = -1;

    // Check if the "listen-port" flag is provided
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--listen-port") == 0 || strcmp(argv[i], "-p") == 0) {
            // Check if there's a value provided after the flag
            if (i + 1 < argc) {
                // Print the value of the port flag
                port = atoi(argv[i + 1]);
                if (port <= 1024) {
                    fprintf(stderr, "Please specify a listening port that is larger than 1024. Got: %d\n", port);
                    exit(EXIT_FAILURE);
                }
            } else {
                fprintf(stderr, "Listening port flag provided without a value.\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    if (port == -1) {
        // If the "listen-port" flag is not found, print a message
        fprintf(stderr, "Listening port flag not provided.\n");
        exit(EXIT_FAILURE);
    }
    return port;
}

int main(int argc, char *argv[]) {
    int port = getCliListeningPort(argc, argv);
    printf("Listening port value: %d\n", port);

    // Start socket server.
    // The server is started in a separate thread, so that the main thread can go back to wait for user inputs).
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
    printf("Interactive Command Line Interface\n");
    printf("Type 'message <IP> <PORT> <MESSAGE>' send message to an user on specific IP and port.\n");
    printf("Type 'exit' to quit.\n");

    while (1) {
        // Prompt for user input.
        // Remove the trailing newline character from input.
//        printf("> ");
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
                printf("[debug] sending %s\n", message);
                sendAndReceive(recipientIp, recipientPort, message);
                printf("[debug] received from server: %s\n", message);
            } else {
                printf("Invalid message input. Please use the format 'message <IP> <PORT> <MESSAGE>'.\n");
            }
            continue;
        }
        printf("Unrecognized input.\n");
    }

    return 0;

    printf("Main thread going to sleep...\n");
    sleep(1000);
}

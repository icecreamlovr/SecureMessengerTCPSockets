#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "lib/socket_server.h"
#include "lib/socket_client.h"

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
        // Testing client.
        connectToRemoteServer("127.0.0.1", 12345);
        exit(EXIT_FAILURE);
    }
    return port;
}

int main(int argc, char *argv[]) {
    int port = getCliListeningPort(argc, argv);
    printf("Listening port value: %d\n", port);

    // Create a thread to start socket server.
    // The main thread will go back to wait for CLI inputs.
    pthread_t thread_id;
    if (pthread_create(&thread_id, NULL, startSocketServer, (void*)&port) != 0) {
        perror("Error: Server thread creation failed");
        exit(EXIT_FAILURE);
    }
    // Detach the thread (to allow it to run independently)
    if (pthread_detach(thread_id) != 0) {
        perror("Error: Thread detachment failed");
        exit(EXIT_FAILURE);
    }

    printf("Main thread going to sleep...\n");
    sleep(1000);
}

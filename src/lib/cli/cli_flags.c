#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int getListeningPortFromCliFlags(int argc, char *argv[]) {
    int port = -1;

    // Check if the "listen-port" flag is provided
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--listen-port") == 0 || strcmp(argv[i], "-p") == 0) {
            // Check if there's a value provided after the flag
            if (i + 1 < argc) {
                // Print the value of the port flag
                port = atoi(argv[i + 1]);
            } else {
                fprintf(stderr, "Error: Listening port flag provided without a value.\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    if (port == -1) {
        // If the "listen-port" flag is not found, print a message
        fprintf(stderr, "Error: Listening port flag not provided.\n");
        exit(EXIT_FAILURE);
    } else if (port <= 1024) {
        fprintf(stderr, "Error: Please specify a listening port larger than 1024. Got: %d\n", port);
        exit(EXIT_FAILURE);
    } else if (port >= 65536) {
        fprintf(stderr, "Error: Please specify a listening port smaller than 65536. Got: %d\n", port);
        exit(EXIT_FAILURE);
    }

    return port;
}

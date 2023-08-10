#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Get value of "--listen-port" or "-p" flag. The value needs to be between 1025 and 65535.*/
int getListeningPortFromCliFlags(int argc, char* argv[]) {
    int port = -1;

    // Check if the "listen-port" flag is provided
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--listen-port") == 0 || strcmp(argv[i], "-p") == 0) {
            // Check if there's a value provided after the flag
            if (i + 1 < argc) {
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

/* Get value of "--file-directory" or "-f" flag. If unspecified, use the default value.*/
char* getFileDirectoryFromCliFlags(int argc, char* argv[], const char* default_value) {
    char* file_directory = NULL;

    // Check if the "file-directory" flag is provided
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--file-directory") == 0 || strcmp(argv[i], "-f") == 0) {
            // Check if there's a value provided after the flag
            if (i + 1 < argc) {
                // Allocate memory for the string.
                file_directory = (char*)malloc(strlen(argv[i + 1]) + 1);
                strcpy(file_directory, argv[i + 1]);
            } else {
                fprintf(stderr, "Error: file directory flag provided without a value.\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    // If --file-directory is not specified, use default value instead.
    if (file_directory == NULL) {
        file_directory = (char*)malloc(strlen(default_value) + 1);
        strcpy(file_directory, default_value);
    }

    return file_directory;
}

int getDebugFromCliFlags(int argc, char* argv[], int default_value) {
    // Check if the "debug" flag is provided
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--debug") == 0 || strcmp(argv[i], "-d") == 0) {
            return 1;
        }
    }
    return default_value;
}

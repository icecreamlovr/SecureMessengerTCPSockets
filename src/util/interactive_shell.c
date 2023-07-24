#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_MESSAGE_LENGTH 256

pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

void print_message(const char* message) {
    pthread_mutex_lock(&print_mutex);

    // Move the cursor to the beginning of the line and clear the line
    printf("\r\x1B[K");
    printf("%s\n", message);

    // Move the cursor back to the prompt area
    printf("\r>> ");
    fflush(stdout);

    pthread_mutex_unlock(&print_mutex);
}

void* message_thread(void* arg) {
    while (1) {
        // Simulate some work by sleeping for a short time
        // Replace this with your actual logic for handling messages from other threads.
        // For example, you could use message queues or shared variables.
        sleep(10); // Sleep for 10 s

        // Generate a random message for demonstration purposes
        char message[MAX_MESSAGE_LENGTH];
        snprintf(message, MAX_MESSAGE_LENGTH, "Message from another thread: %d", rand() % 100);

        // Print the message in the message area
        print_message(message);
    }

    return NULL;
}

int main() {
    pthread_t message_tid;
    int ret;

    printf("Interactive Command Line Interface\n");
    printf("Type 'exit' to quit.\n");

    // Create the message thread
    ret = pthread_create(&message_tid, NULL, message_thread, NULL);
    if (ret != 0) {
        perror("pthread_create");
        return 1;
    }

    // Wait for the message thread to finish (in this example, we don't join the thread)

    // Input handling loop (same as before)
    char message[MAX_MESSAGE_LENGTH];

    while (1) {
        printf("\r>> ");
        fflush(stdout);

        if (fgets(message, MAX_MESSAGE_LENGTH, stdin) == NULL) {
            print_message("Error reading input.");
            break;
        }

        message[strcspn(message, "\n")] = '\0';

        if (strcmp(message, "exit") == 0) {
            print_message("Exiting the program.");
            break;
        }

        print_message(message);
    }

    return 0;
}
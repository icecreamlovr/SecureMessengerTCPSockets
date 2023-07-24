#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <ncurses.h>
#include <unistd.h>

/*
 * To compile:
 * $ gcc -o interactive_shell_ncurses.o ./src/util/interactive_shell_ncurses.c -lncurses
 */

#define MAX_MESSAGE_LENGTH 256

pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

void print_message(const char* message) {
    pthread_mutex_lock(&print_mutex);
    mvprintw(LINES - 1, 0, "\r\x1B[K"); // Move cursor to the bottom and clear the line
    printw("%s\n", message);
    refresh(); // Update the screen
    move(LINES - 1, 3); // Move cursor back to the prompt area
    pthread_mutex_unlock(&print_mutex);
}

void* message_thread(void* arg) {
    while (1) {
        // Simulate some work by sleeping for a short time
        // Replace this with your actual logic for handling messages from other threads.
        // For example, you could use message queues or shared variables.
        usleep(500000); // Sleep for 500 ms

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

    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    // Create the message thread
    ret = pthread_create(&message_tid, NULL, message_thread, NULL);
    if (ret != 0) {
        perror("pthread_create");
        endwin(); // End ncurses mode
        return 1;
    }

    // Input handling loop (same as before)
    char message[MAX_MESSAGE_LENGTH];

    while (1) {
        mvprintw(LINES - 1, 0, "\r\x1B[K"); // Move cursor to the bottom and clear the line
        printw(">> ");
        refresh(); // Update the screen

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

    // Clean up ncurses
    endwin();

    return 0;
}
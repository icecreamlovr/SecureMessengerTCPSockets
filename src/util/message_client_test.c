#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 12345

int main() {
    int client_socket;
    struct sockaddr_in server_address;
    // unsigned char data[] = {
    //     0x5C, 0xFF, 0xC1, 0xDF, 0x4A, 0x5B, 0x03, 0x18,
    //     0x95, 0xA2, 0x5C, 0xFF, 0x5C, 0xFF, 0x5C, 0xFF,
    //     0x5C, 0xFF, 0x5C, 0xFF, 0x5C, 0xFF, 0x5C, 0xFF
    // }; // 24-byte hexadecimal value
    unsigned char data[1000];
    data[0] = 0x5C;
    data[1] = 0xFF;
    data[2] = 0xC1;
    data[3] = 0xDF;
    int send_len = 4;


    // Create a socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Prepare the server address structure
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
    server_address.sin_port = htons(SERVER_PORT);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    // Send the raw bytes over the TCP socket
    if (send(client_socket, data, send_len, 0) == -1) {
        perror("Data sending failed");
        exit(EXIT_FAILURE);
    }

    printf("Data sent: 0x");
    for (int i = 0; i < send_len; i++) {
        printf("%02X", data[i]);
    }
    printf("\n");

    // Close the socket
    close(client_socket);

    return 0;
}
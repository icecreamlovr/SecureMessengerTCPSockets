#ifndef SOCKET_CLIENT_H_INCLUDED
#define SOCKET_CLIENT_H_INCLUDED
/* ^^ these are the include guards */

/* Prototypes for the functions */
int sendAndReceive(const char* server_addr, int server_port, char* message, int send_len, int* recv_len);

int encryptedSendAndReceive(char* base_dir, char* other_pubkey_dir, char* pubkey_file, char* privkey_file, const char* server_addr, int server_port, char* message);

#endif

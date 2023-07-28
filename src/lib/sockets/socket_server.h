#ifndef SOCKET_SERVER_H_INCLUDED
#define SOCKET_SERVER_H_INCLUDED
/* ^^ these are the include guards */

// Struct that will be used to pass multiple parameters to startSocketServer() in pthread_create
struct StartSocketServerArgs {
    int server_port;
    char* server_pubkey_filename;
    char* server_privkey_filename;
    char* local_keypair_dir;
    char* other_pubkey_dir;
};

/* Prototypes for the functions */
void* startSocketServer(void* arg);

#endif

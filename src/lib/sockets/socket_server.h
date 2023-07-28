#ifndef SOCKET_SERVER_H_INCLUDED
#define SOCKET_SERVER_H_INCLUDED
/* ^^ these are the include guards */

// Struct that will be used to pass multiple parameters to startSocketServer() in pthread_create
struct StartSocketServerArgs {
    int server_port;
    RSA* server_privkey;
    RSA* server_pubkey;
    char* other_pubkey_dir;
};

/* Prototypes for the functions */
void* startSocketServer(void* arg);

#endif

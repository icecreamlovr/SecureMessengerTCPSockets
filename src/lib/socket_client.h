#ifndef SOCKET_CLIENT_H_INCLUDED
#define SOCKET_CLIENT_H_INCLUDED
/* ^^ these are the include guards */

/* Prototypes for the functions */
int sendAndReceive(const char* server_addr, int server_port, char* message);

#endif
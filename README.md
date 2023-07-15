# Secure Messenger based on TCP Sockets

A secure messaging system with CLI interface. The system is implemented in C and based on TCP sockets. It consists of a centralized server (server/message_server.c) and many clients (client/message_client.c). The system employs End-to-End encryption between any clients for max level of security.

### To run the program:

Start one instance of server:
```bash
$ gcc src/server/message_server.c -o server.o
$ ./server.o
```

Start two instances of client:
```bash
# On another computer, or another terminal window on localhost
$ gcc src/client/message_client.c -o client.o
$ ./client.o

# On a third computer, or a third terminal window on localhost
$ gcc src/client/client.c -o client.o
$ ./client.o
```

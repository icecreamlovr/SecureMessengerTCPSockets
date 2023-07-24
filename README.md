# Secure Messenger based on TCP Sockets

A secure messaging system with CLI interface. The system is implemented in C and based on TCP sockets. It consists of a centralized server (server/message_server.c) and many clients (client/message_client.c). The system employs End-to-End encryption between any clients for max level of security.

### Install dependencies:

1. Install MacPorts from https://github.com/macports/macports-base/releases/tag/v2.8.1

2. Run self-update:

```bash
### (open a new terminal to make sure `port` is in $PATH)
$ sudo port selfupdate
```

3. Install OpenSSL using MacPorts:

```bash
$ sudo /opt/local/bin/port install openssl
```

### Run the program:

1. 

1. Compile and start one instance of server:
```bash
$ gcc -o messenger.o ./src/messenger.c ./src/lib/socket_server.c ./src/lib/socket_client.c
$ ./messenger.o -p 12345
```

2. Start one instances of client:
```bash
# On another computer, or another terminal window on localhost
$ ./messenger.o
(Send messages)
```

### Generate RSA key pairs

```bash
$ gcc -o rsa_keygen.o ./src/util/rsa_keygen.c -I/opt/local/include -L/opt/local/lib -lssl -lcrypto
(ignore warnings)
$ ./rsa_keygen.o
(this will create private_key.pem and public_key.pem)
$ cat private_key.pem
$ cat public_key.pem
```

### Run sample RSA encryption decryption

```bash
$ gcc -o rsa_encrypt_decrypt.o ./src/util/rsa_encrypt_decrypt.c -I/opt/local/include -L/opt/local/lib -lssl -lcrypto
(ignore warnings)
$ ./rsa_encrypt_decrypt.o
```
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

1. Compile and start one instance of messenger:
```bash
$ gcc -o messenger.o ./src/messenger.c ./src/lib/sockets/socket_server.c ./src/lib/sockets/socket_client.c ./src/lib/cli/cli_flags.c ./src/lib/crypto/rsa_store.c ./src/lib/crypto/rsa_encryption.c -I/opt/local/include -L/opt/local/lib -lssl -lcrypto
# (ignore warnings)
$ ./messenger.o -p 12345
# This will create a local folder user_12345 if it doesn't already exist.
# The folder will be used to store user's private key and public key.
# The folder name can be changed by specifying "-f <folder_name>"
```

2. Start another instances of messenger in a different shell:
```bash
# On another computer, or another terminal window on localhost
$ ./messenger.o  -p 12346
# This will also create a local folder user_12346 if it doesn't already exist.
```

3. Send message in one of the messenger terminal.

### Generate RSA key pairs

```bash
$ gcc -o rsa_keygen.o ./src/util/rsa_keygen.c -I/opt/local/include -L/opt/local/lib -lssl -lcrypto
# (ignore warnings)
$ ./rsa_keygen.o
# (this will create private_key.pem and public_key.pem)
$ cat private_key.pem
$ cat public_key.pem
```

### Run sample RSA encryption decryption

```bash
$ gcc -o rsa_encrypt_decrypt.o ./src/util/rsa_encrypt_decrypt.c -I/opt/local/include -L/opt/local/lib -lssl -lcrypto
(ignore warnings)
$ ./rsa_encrypt_decrypt.o
```
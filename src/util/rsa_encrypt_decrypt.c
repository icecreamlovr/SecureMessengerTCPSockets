#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>

int encryptRSA(RSA* pubKey, const char *message, unsigned char **encryptedMessage, size_t *encryptedMessageLen) {
    // Encrypt the message with the public key
    unsigned char* encrypted = (unsigned char*)malloc(RSA_size(pubKey));
    int encryptedLen =
        RSA_public_encrypt(strlen(message), (unsigned char *)message, encrypted, pubKey, RSA_PKCS1_PADDING);
    if (encryptedLen == -1) {
        fprintf(stderr, "Failed to encrypt message\n");
        RSA_free(pubKey);
        free(encrypted);
        return 0;
    }

    RSA_free(pubKey);
    *encryptedMessage = encrypted;
    *encryptedMessageLen = encryptedLen;
    return 1;
}

int decryptRSA(RSA* privKey, const unsigned char *encryptedMessage, size_t encryptedMessageLen, char **decryptedMessage) {
    // Decrypt the encrypted message with the private key
    unsigned char* decrypted = (unsigned char *)malloc(encryptedMessageLen);
    int decryptedLen =
        RSA_private_decrypt(encryptedMessageLen, encryptedMessage, decrypted, privKey, RSA_PKCS1_PADDING);
    if (decryptedLen == -1) {
        fprintf(stderr, "Failed to decrypt message\n");
        RSA_free(privKey);
        free(decrypted);
        return 0;
    }

    RSA_free(privKey);
    *decryptedMessage = (char *)malloc(decryptedLen + 1);
    memcpy(*decryptedMessage, decrypted, decryptedLen);
    (*decryptedMessage)[decryptedLen] = '\0';
    free(decrypted);
    return 1;
}

RSA *readPublicKeyFromFile(const char *publicKeyFile)
{
    RSA *rsaKey = NULL;
    FILE *pubKeyFile = NULL;

    // Read the public key from PEM file
    pubKeyFile = fopen(publicKeyFile, "r");
    if (pubKeyFile == NULL)
    {
        fprintf(stderr, "Failed to open public key file: %s\n", publicKeyFile);
        return NULL;
    }
    rsaKey = PEM_read_RSAPublicKey(pubKeyFile, NULL, NULL, NULL);
    if (rsaKey == NULL)
    {
        fprintf(stderr, "Failed to read public key from file\n");
        ERR_print_errors_fp(stderr);
        fclose(pubKeyFile);
        return NULL;
    }
    fclose(pubKeyFile);

    return rsaKey;
}
// Read the private key from PEM file as openssl RSA* struct.
RSA* readPrivateKeyFromFile(const char* privateKeyFile) {
    FILE* privKeyFile = fopen(privateKeyFile, "r");
    if (privKeyFile == NULL) {
        fprintf(stderr, "Failed to open private key file\n");
        return 0;
    }

    RSA* rsaKey = PEM_read_RSAPrivateKey(privKeyFile, NULL, NULL, NULL);
    if (rsaKey == NULL) {
        fprintf(stderr, "Failed to read private key from file\n");
        ERR_print_errors_fp(stderr);
        fclose(privKeyFile);
        return 0;
    }

    fclose(privKeyFile);
    return rsaKey;
}

int main() {
    const char *message = "Hello, RSA encryption and decryption!";
    unsigned char *encryptedMessage = NULL;
    size_t encryptedMessageLen;
    char *decryptedMessage = NULL;

    RSA* pubKey = readPublicKeyFromFile("public_key.pem");
    // Encrypt the message with the public key
    if (!encryptRSA(pubKey, message, &encryptedMessage, &encryptedMessageLen)) {
        fprintf(stderr, "Encryption failed\n");
        return 1;
    }

    printf("Encrypted message: ");
    for (size_t i = 0; i < encryptedMessageLen; i++) {
        printf("%02X", encryptedMessage[i]);
    }
    printf("\n");

    RSA* privKey = readPrivateKeyFromFile("private_key.pem");
    // Decrypt the encrypted message with the private key
    if (!decryptRSA(privKey, encryptedMessage, encryptedMessageLen, &decryptedMessage)) {
        fprintf(stderr, "Decryption failed\n");
        free(encryptedMessage);
        return 1;
    }

    printf("Decrypted message: %s\n", decryptedMessage);

    free(encryptedMessage);
    free(decryptedMessage);

    return 0;
}
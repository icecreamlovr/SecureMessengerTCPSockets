#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include "rsa_store.h"

int encryptRSA(RSA* pub_key, const char *message, unsigned char **encrypted_msg, size_t *encrypted_msg_len) {
    // Encrypt the message with the public key
    unsigned char* encrypted = (unsigned char*)malloc(RSA_size(pub_key));
    int encrypted_len =
        RSA_public_encrypt(strlen(message), (unsigned char *)message, encrypted, pub_key, RSA_PKCS1_PADDING);
    if (encrypted_len == -1) {
        fprintf(stderr, "Error: Failed to encrypt message\n");
        RSA_free(pub_key);
        free(encrypted);
        return 0;
    }

    RSA_free(pub_key);
    *encrypted_msg = encrypted;
    *encrypted_msg_len = encrypted_len;
    return 1;
}

int decryptRSA(RSA* priv_key, const unsigned char *encrypted_msg, size_t encrypted_msg_len, char **decrypted_msg) {
    // Decrypt the encrypted message with the private key
    unsigned char* decrypted = (unsigned char *)malloc(encrypted_msg_len);
    int decrypted_len =
        RSA_private_decrypt(encrypted_msg_len, encrypted_msg, decrypted, priv_key, RSA_PKCS1_PADDING);
    if (decrypted_len == -1) {
        fprintf(stderr, "Error: Failed to decrypt message\n");
        RSA_free(priv_key);
        free(decrypted);
        return 0;
    }

    RSA_free(priv_key);
    *decrypted_msg = (char *)malloc(decrypted_len + 1);
    memcpy(*decrypted_msg, decrypted, decrypted_len);
    (*decrypted_msg)[decrypted_len] = '\0';
    free(decrypted);
    return 1;
}

int testEncryptionDecryption(
  const char* message, const char* base_dir, const char* pubkey_file_name, const char* privkey_file_name) {
    unsigned char *encrypted_msg = NULL;
    size_t encrypted_msg_len;
    char *decrypted_msg = NULL;

    RSA* pub_key = readPublicKeyFromFile(base_dir, pubkey_file_name);
    // Encrypt the message with the public key
    if (!encryptRSA(pub_key, message, &encrypted_msg, &encrypted_msg_len)) {
        fprintf(stderr, "Error: Encryption failed\n");
        return 1;
    }

    printf("[DEBUG] Encrypted message: ");
    for (size_t i = 0; i < encrypted_msg_len; i++) {
        printf("%02X", encrypted_msg[i]);
    }
    printf("\n");

    RSA* priv_key = readPrivateKeyFromFile(base_dir, privkey_file_name);
    // Decrypt the encrypted message with the private key
    if (!decryptRSA(priv_key, encrypted_msg, encrypted_msg_len, &decrypted_msg)) {
        fprintf(stderr, "Error: Decryption failed\n");
        free(encrypted_msg);
        return 1;
    }

    printf("[DEBUG] Decrypted message: %s\n", decrypted_msg);

    free(encrypted_msg);
    free(decrypted_msg);

    return 0;
}
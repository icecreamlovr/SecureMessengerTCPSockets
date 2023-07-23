#include <stdio.h>
#include <stdlib.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>

RSA* generateRSAKey(int keyLength) {
    RSA* rsaKey = NULL;
    BIGNUM* e = NULL;

    // Create a new RSA key
    rsaKey = RSA_new();
    if (rsaKey == NULL) {
        fprintf(stderr, "Failed to create RSA key structure\n");
        return NULL;
    }

    // Set the public exponent
    e = BN_new();
    if (e == NULL) {
        fprintf(stderr, "Failed to create BIGNUM for public exponent\n");
        RSA_free(rsaKey);
        return NULL;
    }
    if (BN_set_word(e, RSA_F4) != 1) {
        fprintf(stderr, "Failed to set public exponent value\n");
        BN_free(e);
        RSA_free(rsaKey);
        return NULL;
    }

    // Generate the key pair
    if (RSA_generate_key_ex(rsaKey, keyLength, e, NULL) != 1) {
        fprintf(stderr, "Failed to generate RSA key pair\n");
        BN_free(e);
        RSA_free(rsaKey);
        return NULL;
    }

    BN_free(e);
    return rsaKey;
}

int main() {
    RSA* rsaKeyPair = NULL;

    // Generate RSA key pair with 2048-bit key length
    rsaKeyPair = generateRSAKey(2048);
    if (rsaKeyPair == NULL) {
        fprintf(stderr, "Failed to generate RSA key pair\n");
        return 1;
    }

    // Print the public key in PEM format
    FILE* pubKeyFile = fopen("public_key.pem", "w");
    if (pubKeyFile == NULL) {
        fprintf(stderr, "Failed to open public key file\n");
        RSA_free(rsaKeyPair);
        return 1;
    }
    if (PEM_write_RSAPublicKey(pubKeyFile, rsaKeyPair) != 1) {
        fprintf(stderr, "Failed to write public key to file\n");
        RSA_free(rsaKeyPair);
        fclose(pubKeyFile);
        return 1;
    }
    fclose(pubKeyFile);

    // Print the private key in PEM format
    FILE* privKeyFile = fopen("private_key.pem", "w");
    if (privKeyFile == NULL) {
        fprintf(stderr, "Failed to open private key file\n");
        RSA_free(rsaKeyPair);
        return 1;
    }
    if (PEM_write_RSAPrivateKey(privKeyFile, rsaKeyPair, NULL, NULL, 0, NULL, NULL) != 1) {
        fprintf(stderr, "Failed to write private key to file\n");
        RSA_free(rsaKeyPair);
        fclose(privKeyFile);
        return 1;
    }
    fclose(privKeyFile);

    RSA_free(rsaKeyPair);

    printf("RSA key pair generated and saved to 'public_key.pem' and 'private_key.pem'\n");

    return 0;
}
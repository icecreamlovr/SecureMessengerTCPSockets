#include <stdio.h>
#include <stdlib.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>

int main() {
    RSA* rsaKeyPair = RSA_generate_key(2048, RSA_F4, NULL, NULL);
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
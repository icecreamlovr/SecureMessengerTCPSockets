#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>

// Create a directory to store RSA key pairs. If it already exists, do nothing and return 0.
int createRsaDirectoryIfNotExist(const char* directory_name) {
    if (mkdir(directory_name, 0755) != 0) {
        // Did not create directory
        return 0;
    }
    return 1;
}

char* concatIpPortAndSuffix(const char* ip_address, int port, const char* suffix) {
    int ip_length = strlen(ip_address);
    int port_length = snprintf(NULL, 0, "%d", port);
    int suffix_length = strlen(suffix);
    int total_len = ip_length + 1 + port_length + suffix_length + 1; // Final +1 for null-terminator

    char* result_str = (char*)malloc(total_len);
    snprintf(result_str, total_len, "%s_%d%s", ip_address, port, suffix);
    // Sanitize the IP part and change all dots to "_".
    for (int i = 0; i < ip_length; i++) {
        if (result_str[i] == '.') {
            result_str[i] = '_';
        }
    }
    return result_str;
}

// Given an IP and port, return string "<IP>_<PORT>_public_key.pem". The dots in IP will be transformed to "_".
char* getRsaPublicKeyFileName(const char* host_ip, int host_port) {
    return concatIpPortAndSuffix(host_ip, host_port, "_public_key.pem");
}

// Given an IP and port, return string "<IP>_<PORT>_private_key.pem". The dots in IP will be transformed to "_".
char* getRsaPrivateKeyFileName(const char* host_ip, int host_port) {
    return concatIpPortAndSuffix(host_ip, host_port, "_private_key.pem");
}

// Generate RSA public key and private key, and store them to target files in PEM format.
void generateKeyPairsAndSaveAsPem(
  const char* base_dir, const char* public_dir, const char* pubkey_file_name, const char* privkey_file_name) {
    RSA* rsa_key_pair = RSA_generate_key(2048, RSA_F4, NULL, NULL);
    if (rsa_key_pair == NULL) {
        perror("Error: Failed to generate RSA key pair");
        exit(EXIT_FAILURE);
    }

    // Print the public key in PEM format
    char pubkey_full_path[1024];
    snprintf(pubkey_full_path, sizeof(pubkey_full_path), "%s/%s", base_dir, pubkey_file_name);
    FILE* pub_key_file = fopen(pubkey_full_path, "w");
    if (pub_key_file == NULL) {
        perror("Error: Failed to open public key file");
        RSA_free(rsa_key_pair);
        exit(EXIT_FAILURE);
    }
    if (PEM_write_RSAPublicKey(pub_key_file, rsa_key_pair) != 1) {
        perror("Error: Failed to write public key to file");
        RSA_free(rsa_key_pair);
        fclose(pub_key_file);
        exit(EXIT_FAILURE);
    }
    fclose(pub_key_file);

    // Print the public key also to the public directory
    char pubkey_full_path_2[1024];
    snprintf(pubkey_full_path_2, sizeof(pubkey_full_path_2), "%s/%s", public_dir, pubkey_file_name);
    FILE* pub_key_file_2 = fopen(pubkey_full_path_2, "w");
    if (pub_key_file_2 == NULL) {
        perror("Error: Failed to open public key file");
        RSA_free(rsa_key_pair);
        exit(EXIT_FAILURE);
    }
    if (PEM_write_RSAPublicKey(pub_key_file_2, rsa_key_pair) != 1) {
        perror("Error: Failed to write public key to file");
        RSA_free(rsa_key_pair);
        fclose(pub_key_file_2);
        exit(EXIT_FAILURE);
    }
    fclose(pub_key_file_2);

    // Print the private key in PEM format
    char privkey_full_path[1024];
    snprintf(privkey_full_path, sizeof(privkey_full_path), "%s/%s", base_dir, privkey_file_name);
    FILE* priv_key_file = fopen(privkey_full_path, "w");
    if (priv_key_file == NULL) {
        perror("Error: Failed to open private key file");
        RSA_free(rsa_key_pair);
        exit(EXIT_FAILURE);
    }
    if (PEM_write_RSAPrivateKey(priv_key_file, rsa_key_pair, NULL, NULL, 0, NULL, NULL) != 1) {
        perror("Error: Failed to write private key to file");
        RSA_free(rsa_key_pair);
        fclose(priv_key_file);
        exit(EXIT_FAILURE);
    }
    fclose(priv_key_file);
    RSA_free(rsa_key_pair);
    printf(
        "[INFO] RSA key pair generated and saved to '%s' and '%s'\n",
        pubkey_full_path,
        privkey_full_path);
}

// Read the public key from PEM file as openssl RSA* struct.
RSA* readPublicKeyFromFile(const char* base_dir, const char* pubkey_file_name) {
    char pubkey_full_path[1024];
    snprintf(pubkey_full_path, sizeof(pubkey_full_path), "%s/%s", base_dir, pubkey_file_name);
    FILE* pub_key_file = fopen(pubkey_full_path, "r");
    if (pub_key_file == NULL) {
        fprintf(stderr, "Error: Failed to open public key file: %s\n", pubkey_full_path);
        return NULL;
    }
    RSA* rsa_key = PEM_read_RSAPublicKey(pub_key_file, NULL, NULL, NULL);
    if (rsa_key == NULL) {
        fprintf(stderr, "Error: Failed to read public key from file\n");
        ERR_print_errors_fp(stderr);
        fclose(pub_key_file);
        return NULL;
    }
    fclose(pub_key_file);

    return rsa_key;
}
// Read the private key from PEM file as openssl RSA* struct.
RSA* readPrivateKeyFromFile(const char* base_dir, const char* privkey_file_name) {
    char privkey_full_path[1024];
    snprintf(privkey_full_path, sizeof(privkey_full_path), "%s/%s", base_dir, privkey_file_name);
    FILE* priv_key_file = fopen(privkey_full_path, "r");
    if (priv_key_file == NULL) {
        fprintf(stderr, "Error: Failed to open private key file: %s\n", privkey_full_path);
        return 0;
    }

    RSA* rsa_key = PEM_read_RSAPrivateKey(priv_key_file, NULL, NULL, NULL);
    if (rsa_key == NULL) {
        fprintf(stderr, "Error: Failed to read private key from file %s\n", privkey_full_path);
        ERR_print_errors_fp(stderr);
        fclose(priv_key_file);
        return 0;
    }

    fclose(priv_key_file);
    return rsa_key;
}

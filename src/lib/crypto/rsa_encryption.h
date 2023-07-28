#ifndef RSA_ENCRYPTION_H_INCLUDED
#define RSA_ENCRYPTION_H_INCLUDED
/* ^^ these are the include guards */

/* Prototypes for the functions */
int encryptRSA(RSA* pub_key, const char *message, unsigned char **encrypted_msg, size_t *encrypted_msg_len);

int decryptRSA(RSA* priv_key, const unsigned char *encrypted_msg, size_t encrypted_msg_len, char **decrypted_msg);

int testEncryptionDecryption(
  const char* message, const char* base_dir, const char* pubkey_file_name, const char* privkey_file_name);

#endif
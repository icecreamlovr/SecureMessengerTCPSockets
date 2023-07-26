#ifndef RSA_ENCRYPTION_H_INCLUDED
#define RSA_ENCRYPTION_H_INCLUDED
/* ^^ these are the include guards */

/* Prototypes for the functions */
int testEncryptionDecryption(
  const char* message, const char* base_dir, const char* pubkey_file_name, const char* privkey_file_name);

#endif
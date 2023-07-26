#ifndef RSA_STORE_H_INCLUDED
#define RSA_STORE_H_INCLUDED
/* ^^ these are the include guards */

/* Prototypes for the functions */
int createRsaDirectoryIfNotExist(const char* directory_name);

char* getRsaPublicKeyFileName(const char* host_ip, int host_port);

char* getRsaPrivateKeyFileName(const char* host_ip, int host_port);

void generateKeyPairsAndSaveAsPem(
  const char* base_dir, const char* pubkey_file_name, const char* privkey_file_name);

#endif

#ifndef CLI_FLAGS_H_INCLUDED
#define CLI_FLAGS_H_INCLUDED
/* ^^ these are the include guards */

/* Prototypes for the functions */
int getListeningPortFromCliFlags(int argc, char *argv[]);

char* getFileDirectoryFromCliFlags(int argc, char* argv[], const char* default_value);

#endif

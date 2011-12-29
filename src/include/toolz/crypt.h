#ifndef TOOLZ_CRYPT_H
#define TOOLZ_CRYPT_H
#include <openssl/md5.h>
#include <openssl/evp.h>

int lz_crypt_md5_str(const void *data, size_t data_len, char *hash, int hash_len);
int lz_crypt_sha256_str(const void *data, size_t data_len, char *hash, int hash_len);
int lz_crypt_sha256(const void *data, size_t data_len, char *hash, int hash_len);


#endif


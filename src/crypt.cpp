#include "toolz/crypt.h"
#include <algorithm>
#include <string.h>

int lz_crypt_md5_str(const void *data, size_t data_len, char *hash, int hash_len){
    if(hash==NULL || hash_len == 0) {
        return -1;
    }
    EVP_MD_CTX mdctx;
    unsigned char md_value[EVP_MAX_MD_SIZE];
    unsigned int md_len;

    EVP_DigestInit(&mdctx, EVP_md5());
    EVP_DigestUpdate(&mdctx, data, (size_t) data_len);
    EVP_DigestFinal_ex(&mdctx, md_value, &md_len);
    EVP_MD_CTX_cleanup(&mdctx);
    char *hash_ptr = hash;
    for(int i=0; i<md_len; i++) {
        snprintf(hash_ptr, (hash_len-i*2), "%02x", md_value[i]);
        hash_ptr += 2;
    }
    return 0;
}

int lz_crypt_sha256_str(const void *data, size_t data_len, char *hash, int hash_len){
    if(hash==NULL || hash_len == 0) {
        return -1;
    }
    EVP_MD_CTX mdctx;
    unsigned char md_value[EVP_MAX_MD_SIZE];
    unsigned int md_len;

    EVP_DigestInit(&mdctx, EVP_sha256());
    EVP_DigestUpdate(&mdctx, data, (size_t) data_len);
    EVP_DigestFinal_ex(&mdctx, md_value, &md_len);
    EVP_MD_CTX_cleanup(&mdctx);
    char *hash_ptr = hash;
    for(int i=0; i<md_len; i++) {
        snprintf(hash_ptr, (hash_len-i*2), "%02x", md_value[i]);
        hash_ptr += 2;
    }
    return 0;
}

int lz_crypt_sha256(const void *data, size_t data_len, char *hash, int hash_len){
    if(hash==NULL || hash_len == 0) {
        return -1;
    }
    EVP_MD_CTX mdctx;
    unsigned char md_value[EVP_MAX_MD_SIZE];
    unsigned int md_len;

    EVP_DigestInit(&mdctx, EVP_sha256());
    EVP_DigestUpdate(&mdctx, data, (size_t) data_len);
    EVP_DigestFinal_ex(&mdctx, md_value, &md_len);
    EVP_MD_CTX_cleanup(&mdctx);
    memcpy(hash, md_value, std::min(hash_len, (int)md_len));
    return 0;
}


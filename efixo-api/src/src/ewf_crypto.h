#ifndef _EWF_CRYPTO_H_
#define _EWF_CRYPTO_H_

extern int ewf_crypto_hmac_sha256_mac(char *key, char *msg, char **result);

extern int ewf_crypto_sha256_hash(char *msg, char **result);

#endif

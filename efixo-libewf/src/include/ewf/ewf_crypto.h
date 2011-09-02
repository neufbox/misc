#ifndef _EWF_CRYPTO_H_
#define _EWF_CRYPTO_H_

extern int ewf_crypto_hmac_sha256_mac(const char *key, const char *msg, 
				      char **result);

extern int ewf_crypto_sha256_hash(const char *msg, char **result);

#endif

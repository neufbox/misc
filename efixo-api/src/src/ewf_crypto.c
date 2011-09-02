#include "ewf_crypto.h"

#include "ewf.h"

#include <string.h>
#include <stdlib.h>
#include <tropicssl/sha2.h>

#include "nbu/string.h"

/* 
 * return EWF_ERROR on error, otherwise EWF_SUCCESS 
 */
int ewf_crypto_hmac_sha256_mac(char *key, char *msg, char **result)
{
	sha2_context hmac;
	unsigned char digest[32];
	int i;

	if(msg == NULL)
	{
		return EWF_ERROR;
	}

	*result = calloc(1, 32 * 2 + 1);

	if (*result == NULL)
	{
		return EWF_ERROR;
	}

	sha2_hmac_starts(&hmac, key, strlen(key), 0);
	sha2_hmac_update(&hmac, (unsigned char*) msg, strlen(msg));
	sha2_hmac_finish(&hmac, digest);

	for (i = 0; i < 32; i++)
	{
		nbu_string_printf((*result) + 2 * i, 3, "%02x", (unsigned char) digest[i]);
	}

	return EWF_SUCCESS;
}

/* returns NULL on error, pointer to hash on success */
int ewf_crypto_sha256_hash(char *msg, char **result)
{
	sha2_context sha256;
	unsigned char digest[32];
	int i;

	if (msg == NULL)
	{
		return EWF_ERROR;
	}

	*result = calloc(1, 32 * 2 + 1);

	if (*result == NULL)
	{
		return EWF_ERROR;
	}

	sha2_starts(&sha256, 0);
	sha2_update(&sha256, (unsigned char*) msg, strlen(msg));
	sha2_finish(&sha256, digest);
	
	for (i = 0; i < 32; i++)
	{
		nbu_string_printf((*result) + 2 * i, 3, "%02x", (unsigned char) digest[i]);
	}

	return EWF_SUCCESS;
}

#include "ewf/ewf.h"
#include "ewf/ewf_crypto.h"

#include "yatest.h"

#include "nbu/nbu.h"

TEST_DEF(test_crypto_hmac_sha256)
{
        static struct {
                const char *k;
                const char *m;
                const char *r;
        } hmac_sha256_test[] = {
                { "nimportequoi",
                  "oloothvufhe8flybryic", 
                  "d298130dd066dea1d4e4d2ed06144fa7faf2a876bd75c65ca7f6a2a4c62489c5" },
        };
        unsigned int i;
        char *result;

        for(i = 0; i < NBU_ARRAY_SIZE(hmac_sha256_test); i++)
        {
                if(ewf_crypto_hmac_sha256_mac(hmac_sha256_test[i].k, 
                                              hmac_sha256_test[i].m, 
                                              &result) == EWF_SUCCESS)
                {
                        TEST_ASSERT(strcmp(result, hmac_sha256_test[i].r) == 0,
                                    "hmac_sha256(%s) != %s\n",
                                    result,
                                    hmac_sha256_test[i].r);
                        
                        free(result);
                }
                else
                {
                        PRINT_RUNTIME_ERROR("ewf_crypto_hmac_sha256_mac() failed");
                }
        }

}

TEST_DEF(test_crypto_sha256)
{
        static struct {
                const char *m;
                const char *r;
        } sha256_test[] = {
                { "oloothvufhe8flybryic", 
                  "d298130dd066dea1d4e4d2ed06144fa7faf2a876bd75c65ca7f6a2a4c62489c5" },
        };
        unsigned int i;
        char *result;

        for(i = 0; i < NBU_ARRAY_SIZE(sha256_test); i++)
        {
                if(ewf_crypto_sha256_hash(sha256_test[i].m, 
                                          &result) == EWF_SUCCESS)
                {
                        TEST_ASSERT(strcmp(result, sha256_test[i].r) == 0,
                                    "sha256(%s) != %s\n",
                                    result,
                                    sha256_test[i].r);

                        free(result);
                }
                else
                {
                        PRINT_RUNTIME_ERROR("ewf_crypto_sha256_hash() failed");
                }
        }
}

int main(void)
{
	TEST_INIT("ewf_crypto");
	
	TEST_RUN(test_crypto_hmac_sha256);
	TEST_RUN(test_crypto_sha256);
	
	return TEST_RETURN;
}

#include "ewf/ewf.h"
#include "ewf/core/ewf_theme.h"

#include "yatest.h"

TEST_DEF(test_theme_uri_basename)
{
        static struct {
                const char *uri;
                const char *basename;
        } uri_basename_test[] = {
                { "local://default", 
                  "default", },
                { "ehd://4819-456-145-81891a/themes/christmas",
                  "christmas", },
        };

        unsigned int i;
        const char *result;

        for(i = 0; i < NBU_ARRAY_SIZE(uri_basename_test); i++)
        {
                result = ewf_theme_uri_basename(uri_basename_test[i].uri);
                
                /* printf("basename=%s uri=%s (need %s)\n", */
                /*        result, */
                /*        uri_basename_test[i].uri, */
                /*        uri_basename_test[i].basename); */
                
                TEST_ASSERT(strcmp(result, uri_basename_test[i].basename) == 0,
                            "Invalid basename '%s' for uri '%s' (need %s !)",
                            result,
                            uri_basename_test[i].uri,
                            uri_basename_test[i].basename);
        }
        
}

int main(void)
{
	TEST_INIT("ewf_theme");
	
	TEST_RUN(test_theme_uri_basename);
	
	return TEST_RETURN;
}

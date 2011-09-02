#include "cfg.h"

#include "nbd.h"

static struct {
        char *cfg_name;
        char *old_value;
        char *new_value;
} value_cptb_mapping[] = {
        {"wlan_mode", "11n-only", "11n"},
        {"wlan_mode", "11n-legacy", "11ng"},
        {"wlan_mode", "legacy", "11g"},
};

int cfg_set(const char *cfg_name, const char *value)
{
        unsigned int i;
	int ret = cfg_set_error;
        
	for(i = 0; 
            i < NBU_ARRAY_SIZE(value_cptb_mapping); 
            i++)
	{
                if(nbu_string_compare(cfg_name,
                                      value_cptb_mapping[i].cfg_name)
                   == NBU_STRING_EQUAL
                   && nbu_string_compare(value,
                                         value_cptb_mapping[i].old_value)
                   == NBU_STRING_EQUAL)
                {
                        nbu_log_debug("value mapping '%s' -> '%s' for '%s'",
                                      value,
                                      value_cptb_mapping[i].new_value,
                                      cfg_name);
                        
                        value = value_cptb_mapping[i].new_value;
                        break;
                }
        }

        ret = nbd_nv_set(cfg_name, value);
        if(ret == NBD_SUCCESS)
        {
                ret = cfg_set_success;
        }
        else if(ret == NBD_UNCHANGED)
        {
                ret = cfg_set_unchanged;
        }
        else if(ret == -4)
        {
                /* invalid value (regex check, invalid characters, ) */
                ret = cfg_set_invalidvalue;
        }
        else
        {
                ret = cfg_set_error;
        }
        
        return ret;
}

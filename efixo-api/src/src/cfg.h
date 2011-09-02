#ifndef _EFIXO_API_CFG_H_
#define _EFIXO_API_CFG_H_

enum cfg_set_ret_enum_t {
        cfg_set_error = -100,
        cfg_set_invalidvalue,
        cfg_set_success = 0,
        cfg_set_unchanged,
};

int cfg_set(const char *cfg_name, const char *value);

#endif

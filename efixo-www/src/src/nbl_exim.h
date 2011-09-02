#ifndef _NBL_EXIM_H_
#define _NBL_EXIM_H_

#include <stdio.h>

extern int nbl_exim_config_user_import(FILE *config);

extern int nbl_exim_config_user_export(void);

extern int nbl_exim_diagnostic_report_export(void);

#endif

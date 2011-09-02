#ifndef _NBL_ADSL_H_
#define _NBL_ADSL_H_

#include <stdio.h>

/*
 * get `adsl info --stats` output
 */
extern int nbl_adsl_get_stats(char *buffer, size_t buffer_size);

/*
 * get `adsl info --linediag` output
 */
extern int nbl_adsl_get_linediag(char *buffer, size_t buffer_size);

/*
 * get `xdslctl info --stats --Bits` output
 */
extern int nbl_adsl_get_stats_bits(char *buffer, size_t buffer_size);
#endif

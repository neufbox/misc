#ifndef _NBL_LAN_H_
#define _NBL_LAN_H_

#include <stdio.h>

/*
 * get `int6ktone -i lan0 ...` output
 */
extern int nbl_plug_get_stats(char *buffer, size_t buffer_size,  const char *cpl_master,  const char *cpl_slave);

/*
 * get `int6keth -i lan0 ...` output
 */
extern int nbl_plug_get_info_speed(char *buffer, size_t buffer_size,  const char *cpl_master,  const char *cpl_slave);

/*
 * get `int6krate -n -i lan0 ...` output
 */
extern int nbl_plug_get_info_rate(char *buffer, size_t buffer_size,  const char *cpl_master,  const char *cpl_slave);
#endif

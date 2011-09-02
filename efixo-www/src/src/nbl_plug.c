#include "nbl.h"
#include "nbl_plug.h"

#include <nbu/nbu.h>

#include <errno.h>
#include <string.h>
#include <stdio.h>


int nbl_plug_get_stats(char *buf, size_t buf_size,  const char *cpl_master,  const char *cpl_slave)
{
        static FILE *stats = NULL;
	int n = 0;
        char int6ktone[70];

	if (stats == NULL)
	{

                nbu_string_printf(int6ktone, sizeof int6ktone, "int6ktone -i lan0 %s %s", cpl_master, cpl_slave);
                nbu_log_debug("int6ktone : %s", int6ktone);
                stats = popen(int6ktone, "r");
		if(stats == NULL)
		{
			nbu_log_debug("popen failed ! %s", strerror(errno));
			return -1;
		}
	}
        else
	{
		if(feof(stats) != 0)
		{
			nbu_log_debug("pclose() int6ktone -i lan0...");
			pclose(stats);
			stats = NULL;
			return -1;
		}
	}

	if(stats != NULL)
	{
		n = fread(buf, 1, buf_size - 1, stats);
		buf[n] = '\0';
		nbu_log_debug("%i, %s", n, buf);
	}

	return n;
}
int nbl_plug_get_info_speed(char *buf, size_t buf_size,  const char *cpl_master,  const char *cpl_slave)
{
        static FILE *stats = NULL;
	int n = 0;
        char int6keth[70];

        nbu_string_printf(int6keth, sizeof int6keth, "int6keth -i lan0 %s %s", cpl_master, cpl_slave);
        nbu_log_debug("int6keth : %s", int6keth);
        stats = popen(int6keth, "r");
        if(stats == NULL)
        {
                nbu_log_debug("popen failed ! %s", strerror(errno));
                return -1;
        }
        n = fread(buf, 1, buf_size - 1, stats);
        buf[n] = '\0';
        nbu_log_debug("%i, %s", n, buf);
        nbu_log_debug("pclose() int6keth -i lan0...");
        pclose(stats);
        stats = NULL;
        return -1;
}

int nbl_plug_get_info_rate(char *buf, size_t buf_size,  const char *cpl_master,  const char *cpl_slave)
{
        static FILE *stats = NULL;
	int n = 0;
        char int6krate[70];

        nbu_string_printf(int6krate, sizeof int6krate, "int6krate -n -i lan0 %s %s", cpl_master, cpl_slave);
        nbu_log_debug("int6krate : %s", int6krate);
        stats = popen(int6krate, "r");
        if(stats == NULL)
        {
                nbu_log_debug("popen failed ! %s", strerror(errno));
                return -1;
        }
        n = fread(buf, 1, buf_size - 1, stats);
        buf[n] = '\0';
        nbu_log_debug("%i, %s", n, buf);
        nbu_log_debug("pclose() int6krate -i lan0...");
        pclose(stats);
        stats = NULL;
        return -1;
}

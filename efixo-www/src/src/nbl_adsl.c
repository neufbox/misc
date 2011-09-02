#include "nbl.h"
#include "nbl_adsl.h"

#include <nbu/nbu.h>

#include <errno.h>
#include <string.h>
#include <stdio.h>

int nbl_adsl_get_stats(char *buf, size_t buf_size)
{
	static FILE *stats = NULL;
	int n = 0;

	printf("nbl_adsl_get_stats");

	if (stats == NULL)
	{
		nbu_log_debug("popen() adsl info --stats");

                        stats = popen("xdslctl info --stats", "r");
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
			nbu_log_debug("pclose() adsl info --stats");
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

int nbl_adsl_get_linediag(char *buf, size_t buf_size)
{
	static FILE *diag = NULL;
	int n = 0;

	if (diag == NULL)
	{
		nbu_log_debug("popen() xdslctl info --linediag");

		diag = popen("xdslctl info --linediag", "r");
		if(diag == NULL)
		{
			nbu_log_debug("popen failed ! %s", strerror(errno));
			return -1;
		}
	}
	else
	{
		if(feof(diag) != 0)
		{
			nbu_log_debug("pclose() xdslctl info --linediag");
			pclose(diag);
			diag = NULL;
			return -1;
		}
	}

	if(diag != NULL)
	{
		n = fread(buf, 1, buf_size - 1, diag);
		buf[n] = '\0';
		nbu_log_debug("%i, %s", n, buf);
	}

	return n;
}



int nbl_adsl_get_stats_bits(char *buf, size_t buf_size)
{
	static FILE *diag = NULL;
	int n = 0;

	if (diag == NULL)
	{
		nbu_log_debug("popen() xdslctl info --stats --Bits");

		diag = popen("xdslctl info --stats --Bits", "r");
		if(diag == NULL)
		{
			nbu_log_debug("popen failed ! %s", strerror(errno));
			return -1;
		}
	}
	else
	{
		if(feof(diag) != 0)
		{
			nbu_log_debug("pclose() xdslctl info --stats --Bits");
			pclose(diag);
			diag = NULL;
			return -1;
		}
	}

	if(diag != NULL)
	{
		n = fread(buf, 1, buf_size - 1, diag);
		buf[n] = '\0';
		nbu_log_debug("%i, %s", n, buf);
	}

	return n;
}

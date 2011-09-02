#ifndef LIBNBD_H
#define LIBNBD_H

#include <sys/types.h>
#include <unistd.h>

#include "ewf.h"

#include "nbh/startstop.h"

#include "nbd/core.h"
#include "nbd/nbctl.h"
#include "nbd/nvram.h"
#include "nbd/status.h"
#include "nbd/ping.h"
#include "nbd/traceroute.h"
#include "nbd/nat.h"
#include "nbd/voip.h"
#include "nbd/wlan.h"
#include "nbd/lan.h"
#include "nbd/hotspot.h"
#include "nbd/event.h"

#if defined(NB4) || defined(NB5)
#include "nbd/autoconf.h"
#endif

#define NBD_ETHER_SIZE 18

enum errors {
    NBD_SUCCESS   = 0,
    NBD_UNCHANGED = 1,
    NBD_ERROR     = -1
};

typedef struct _nbd_host {
    char *ipaddr;
    char *macaddr;
} nbd_host_t;

static inline int nbd_connect(void)
{
    setgid(1);
    setuid(1);

    return nbd_open();
}

static inline void nbd_disconnect(void)
{
    nbd_close();
}

/******************************************************************************/
/* WLAN                                                                       */
/******************************************************************************/

static inline void nbd_host_t_free(void *pair)
{
	nbd_host_t *p;

	if (pair != NULL)
	{
		p = (nbd_host_t *) pair;

		if (p->ipaddr) free(p->ipaddr);
		if (p->macaddr) free(p->macaddr);
		free(p);
	}
}

#endif /* LIBNBD_H */

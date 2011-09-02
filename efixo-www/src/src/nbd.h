#ifndef _WWW_NBD_H_
#define _WWW_NBD_H_

#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>

#include <ewf/ewf.h>

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
#include "nbd/uroute.h"
#include "nbd/hotspot.h"
#include "nbd/ddnsctl.h"
#include "nbd/autoconf.h"
#include "nbd/backup3g.h"
#include "nbd/stb.h"
#include "nbd/eco.h"
#include "nbd/sfp.h"
#include "nbd/dsl.h"

#if defined(HAVE_USB_SUPPORT)
#include "nbd/disk.h"
#endif

#include "dbl.h"

enum nbd_errors {
    NBD_SUCCESS   = 0,
    NBD_ERROR     = -1
};

static inline int nbd_connect(void)
{
	if (setgid(1) < 0)
	{
		return -1;
	}

	if (setuid(1) < 0)
	{
		return -1;
	}

        while(nbd_open() < 0)
        {
                sleep(1);
        }

	return EWF_SUCCESS;
}

static inline void nbd_disconnect(void)
{
	nbd_close();
}

#endif /* _WWW_NBD_H_ */

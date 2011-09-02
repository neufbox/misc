/*!
 * \file nbh/startstop.h
 *
 * \brief  Declare init scripts
 *
 * \author Copyright 2009 Anthony VIALLARD <anthony.viallard@efixo.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 * 
 * $Id: $
 */

#ifndef _NBH_STARTSTOP_H_
#define _NBH_STARTSTOP_H_

/** PPP ***************************************************************/
#define PPP_START_STOP          "/etc/init.d/ppp"

/** IPV6 ***************************************************************/
#define IPV6_START_STOP          "/etc/init.d/ipv6"

/** DSLDIAGS **********************************************************/
#define DSLDIAGS_START_STOP	 "/etc/init.d/dsldiags"

/** WLAN **************************************************************/
#define WLAN_START_STOP         "/etc/init.d/wlan"

/** FIREWALL **********************************************************/
#define FIREWALL_START_STOP     "/etc/init.d/firewall"

/** DHCPC *************************************************************/
#define DHCPC_START_STOP         "/etc/init.d/dhcpc"

/** DHCPD *************************************************************/
#define DHCPD_START_STOP         "/etc/init.d/dhcpd"

/** QOS ***************************************************************/
#define QOS_START_STOP         	 "/etc/init.d/qos"

/** BACKUP3G **********************************************************/
#define BACKUP3G_START_STOP     "/etc/init.d/backup3g"

/** UPNPIGD ***********************************************************/
#define UPNPIGD_START_STOP      "/etc/init.d/miniupnpd"

/** DDNSD *************************************************************/
 #define DDNSD_START_STOP        "/etc/init.d/yaddns"

/** SAMBA *************************************************************/
#define SAMBA_START_STOP        "/etc/init.d/samba"

/** USHARE ************************************************************/
#define USHARE_START_STOP        "/etc/init.d/ushare"

/** P910D *************************************************************/
#define P910D_START_STOP        "/etc/init.d/p910nd"

/** TV ***********************************************************/
#define TV_START_STOP           "/etc/init.d/igmp"

#endif

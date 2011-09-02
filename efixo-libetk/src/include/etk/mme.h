#ifndef _MME_H_
#define _MME_H_ 1

#include <linux/types.h>

#define __packed __attribute__ ((__packed__))

struct mme_header {
	__u8 mme_v;
	__le16 mme_type;
	__u8 mme_oui[3];
} __packed;

struct hpav_info {
	__u8 status;
	__u8 devid;
	__u8 len;
	char version[128];
	__u8 upgradeable;
	__u32 cookie;
} __packed;

struct hpav_station {
	__u8 MAC[ETHER_ADDR_LEN];
	__u8 TEI;
	__u8 BDA[ETHER_ADDR_LEN];
	__u8 AVGTX;
	__u8 AVGRX;
} __packed;

struct hpav_network {
	__u8 NID[7];
	__u8 SNID;
	__u8 TEI;
	__u8 ROLE;
	__u8 CCO_MACADDR[ETHER_ADDR_LEN];
	__u8 CCO_TEI;
	__u8 num;
	struct hpav_station station[1];
} __packed;

struct hpav_networks {
	__u8 num;
	struct hpav_network network[1];
} __packed;

static inline __le16 cpu_to_le16(uint16_t v)
{
#if  __BYTE_ORDER == __BIG_ENDIAN
	return __bswap_16(v);
#else
	return v;
#endif
}

#endif


#include <string.h>
#include <unistd.h>

#include <net/ethernet.h>
#include <arpa/inet.h>

#include <etk/common.h>
#include <etk/log.h>
#include <etk/mme.h>

int mme_request(int fd, void const *ether_shost,
		void const *ether_dhost, uint16_t query)
{
	char frame[ETH_FRAME_LEN];
	struct ether_header *eth_hdr;
	struct mme_header *mme_hdr;
	struct homeplug_info const *hpav;
	size_t len = 0;

	memset(frame, 0x00, sizeof(frame));
	/* build ethernet header */
	eth_hdr = (void *)(frame + len);
	memcpy(eth_hdr->ether_shost, ether_shost, sizeof(eth_hdr->ether_shost));
	memcpy(eth_hdr->ether_dhost, ether_dhost, sizeof(eth_hdr->ether_dhost));
	eth_hdr->ether_type = htons(0x88e1);	/* HomePlug frame */
	len += sizeof(*eth_hdr);

	/* build HomePlug header */
	mme_hdr = (void *)(frame + len);
	memset(mme_hdr, 0x00, sizeof(*mme_hdr));
	mme_hdr->mme_v = 0U;
	mme_hdr->mme_type = cpu_to_le16(query);	/* discover */
	mme_hdr->mme_oui[0] = 0x00;
	mme_hdr->mme_oui[1] = 0xB0;
	mme_hdr->mme_oui[2] = 0x52;
	len += sizeof(*mme_hdr);

	hpav = (void const *)(frame + len);
	len = max((int)len, ETHER_MIN_LEN);

	if (write(fd, frame, len) < 0) {
		fatal("%s()", "write");
		return -1;
	}

	return 0;
}

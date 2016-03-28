
/**************************************************************************************************

 DESCRIPTION:
  --

 CREATED: 08/03/2016, by Paolo Achdjian

 FILE: zcl_linkquality.h

***************************************************************************************************/
#ifndef __ZCL_LINK_QUALITY__H__
#define __ZCL_LINK_QUALITY__H__

#include "AF.h"

struct LinkQuality {
	uint8 LinkQuality;        /* The link quality of the received data frame */
	uint8 correlation;        /* The raw correlation value of the received data frame */
	int8  rssi;               /* The received RF power in units dBm */
	uint8 extAddr[Z_EXTADDR_LEN];
	struct LinkQuality * next;
};


void updateLinkQuality(afIncomingMSGPacket_t * inPck);
struct LinkQuality * getLinkQuality(uint8 extAddr[Z_EXTADDR_LEN]);


#endif
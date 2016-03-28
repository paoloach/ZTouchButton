/**************************************************************************************************

 DESCRIPTION:
  --

 CREATED: 14/10/2015, by Paolo Achdjian

 FILE: CS5463.h

***************************************************************************************************/
#ifndef __CS5463__H__
#define __CS5463__H__

#include "hal_types.h"

enum CS5463Register {
	IstantaneoCurrent=7,
	RMSVolt = 12,
	PeakVoltage=23,
	RMSCurrent = 11,
	status = 15,
	PeakCurrent=22,
	ActivePower=10,
	IstantaneusReactivePower=20,
	ApparentPower=27,
	PowerFactor=25
};

extern void CS5463_Init(void);

extern void CS5463_startConversion(void);
extern void CS5463_reset(void);

extern int32 getCS5463RegisterValue(enum CS5463Register regIndex);
extern void setCS5463RegisterValue(enum CS5463Register regIndex,unsigned char byte1,unsigned char  byte2,unsigned char  byte3);


#endif
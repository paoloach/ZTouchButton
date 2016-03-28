/**************************************************************************************************

 DESCRIPTION:
  Metering Cluster

 CREATED: 08/09/2015, by Paolo Achdjian

 FILE: ClusterMetering.h

***************************************************************************************************/

#ifndef __CLUSTER_METERING__H__
#define __CLUSTER_METERING__H__

#include "zclReadAttributeFn.h"
#include "zcl.h"
#include "ClusterOSALEvents.h"
#include "uint48.h"


#define ATTRID_METERING_CURRENT_SUMMATION_DELIVERED 0
#define ATTRID_METERING_STATUS_ATTRIBUTE 0x0200
#define ATTRID_METERING_UNIT_OF_MEASURE 0x0300
#define ATTRID_METERING_MULTIPLIER 0x0301
#define ATTRID_METERING_DIVISOR 0x0302
#define ATTRID_METERING_SUMMATION_FORMATTING 0x0303
#define ATTRID_METERING_DEMAND_FORMATTING 0x0304
#define ATTRID_METERING_HISTORICAL_CONSUMPTION_FORMATTING 0x0305
#define ATTRID_METERING_METERING_DEVICE_TYPE 0x0306

void meteringClusterReadAttribute(zclAttrRec_t *);


#endif
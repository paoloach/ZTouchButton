
/**************************************************************************************************

 DESCRIPTION:
  Metering Cluster

 CREATED: 08/09/2015, by Paolo Achdjian

 FILE: ClusterMetering.c

***************************************************************************************************/

#include "zcl.h"
#include "zcl_general.h"
#include "ClusterPower.h"


uint16 mainVoltage=0;
uint16 batteryVoltage=0;
uint8  batteryAlarmMask=0;

void powerClusterReadAttribute(zclAttrRec_t * attribute) {
	if (attribute == NULL){
		return;
	}
	attribute->accessControl = ACCESS_CONTROL_READ;
	attribute->status = ZCL_STATUS_SUCCESS;
	switch(attribute->attrId){
	case ATTRID_POWER_CFG_MAINS_VOLTAGE:
		attribute->dataType = ZCL_DATATYPE_UINT16;
		attribute->dataPtr = (void *)&mainVoltage;
		break;
	case ATTRID_POWER_CFG_BATTERY_VOLTAGE:
		attribute->dataType = ZCL_DATATYPE_UINT8;
		attribute->dataPtr = (void *)&batteryVoltage;
		break;
	case ATTRID_POWER_CFG_BAT_ALARM_MASK:
		attribute->dataType = ZCL_DATATYPE_BITMAP8;
		attribute->dataPtr = (void *)&batteryAlarmMask;
		break;	
	default:
		attribute->status = ZCL_STATUS_UNSUPPORTED_ATTRIBUTE;
	}
}

/**************************************************************************************************

 DESCRIPTION:
  Metering Cluster

 CREATED: 08/09/2015, by Paolo Achdjian

 FILE: ClusterMetering.c

***************************************************************************************************/

#include "ClusterMetering.h"


uint48 currentSummationDelivered={0,0,0,0,0,0};
uint8  status=0;
uint8  unitOfMeasure=0; // kW (kilo-Watts) & kWh (kilo-WattHours) in pure Binary format
uint8  summationFormatting=0;
uint8  metteringDeviceType=0; //Electric Metering


void meteringClusterReadAttribute(zclAttrRec_t * attribute){
	if (attribute == NULL){
		return;
	}
	attribute->accessControl = ACCESS_CONTROL_READ;
	attribute->status = ZCL_STATUS_SUCCESS;
	switch(attribute->attrId){
	case ATTRID_METERING_CURRENT_SUMMATION_DELIVERED:
		attribute->dataType = ZCL_DATATYPE_UINT48;
		attribute->dataPtr = (void *)&currentSummationDelivered;
		break;
	case ATTRID_METERING_STATUS_ATTRIBUTE:
		attribute->dataType = ZCL_DATATYPE_BITMAP8;
		attribute->dataPtr = (void *)&status;
		break;
	case ATTRID_METERING_UNIT_OF_MEASURE:
		attribute->dataType = ZCL_DATATYPE_ENUM8;
		attribute->dataPtr = (void *)&unitOfMeasure;
		break;
	case ATTRID_METERING_SUMMATION_FORMATTING:
		attribute->dataType = ZCL_DATATYPE_BITMAP8;
		attribute->dataPtr = (void *)&summationFormatting;
		break;
	case ATTRID_METERING_METERING_DEVICE_TYPE:
		attribute->dataType = ZCL_DATATYPE_BITMAP8;
		attribute->dataPtr = (void *)&metteringDeviceType;
		break;		
	default:
		attribute->status = ZCL_STATUS_UNSUPPORTED_ATTRIBUTE;
	}
}

/**************************************************************************************************
  Filename:       ClusterBasic.cpp

  Autorh:  Paolo Achdjia
  Created: 13/10/2015

**************************************************************************************************/

#include "ClusterBasic.h"
#include "osal.h"
#include "zcl_general.h"
#include "zcl.h"



const uint8 HWRevision = 1;
const uint8 ZCLVersion = 1;
const uint8 manufacturerName[] = { 20, 'A','c','h','d','j','i','a','n',' ','E','l','e','c','t',' ',' ',' ',' ',' ',' ' };
const uint8 modelId[] = { 16,'z', 'T','o','u','c','h','B','u','t','t','o','n',' ',' ',' ',' ' };
const uint8 dateCode[] = { 16, '2','0','1','6','0','3','3','0',' ',' ',' ',' ',' ',' ',' ',' ' };
const uint8 powerSource = POWER_SOURCE_DC;

uint8 locationDescription[17] = { 16, ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ' };
uint8 physicalEnvironment = 0;
uint8 deviceEnable = DEVICE_ENABLED;


void basicResetCB( void ){
  // Reset all attributes to default values
}

void basicClusterWriteAttribute(ZclWriteAttribute_t * writeAttribute) {
	if (writeAttribute == NULL){
		return;
	}
	writeAttribute->status=ZCL_STATUS_SUCCESS;
	switch(writeAttribute->attrId){
	case ATTRID_BASIC_HW_VERSION:
	case ATTRID_BASIC_ZCL_VERSION:
	case ATTRID_BASIC_MANUFACTURER_NAME:
	case ATTRID_BASIC_MODEL_ID:
	case ATTRID_BASIC_DATE_CODE:
	case ATTRID_BASIC_POWER_SOURCE:
		writeAttribute->status=ZCL_STATUS_READ_ONLY;
		break;
	case ATTRID_BASIC_LOCATION_DESC:
		if (writeAttribute->dataType == ZCL_DATATYPE_CHAR_STR)
			osal_memcpy((void *)&locationDescription, writeAttribute->dataPtr, writeAttribute->len);
		else
			writeAttribute->status = ZCL_STATUS_INVALID_DATA_TYPE;
		break;
	case ATTRID_BASIC_PHYSICAL_ENV:
		if (writeAttribute->dataType == ZCL_DATATYPE_ENUM8)
			osal_memcpy((void *)&physicalEnvironment, writeAttribute->dataPtr, writeAttribute->len);
		else 
			writeAttribute->status = ZCL_STATUS_INVALID_DATA_TYPE;
		break;
	case ATTRID_BASIC_DEVICE_ENABLED:
		if (writeAttribute->dataType == ZCL_DATATYPE_BOOLEAN)
			deviceEnable = *(uint8 *)writeAttribute->dataPtr;
		else 
			writeAttribute->status = ZCL_STATUS_INVALID_DATA_TYPE;
		break;
	default:
		writeAttribute->status = ZCL_STATUS_UNSUPPORTED_ATTRIBUTE;
	}
}

void basicClusterReadAttribute(zclAttrRec_t * statusRec){
	if (statusRec == NULL){
		return;
	}
	statusRec->accessControl = ACCESS_CONTROL_READ;
	statusRec->status=ZCL_STATUS_SUCCESS;
	switch(statusRec->attrId){
	case ATTRID_BASIC_HW_VERSION:
		statusRec->dataType = ZCL_DATATYPE_UINT8;
		statusRec->dataPtr = (void *)&HWRevision;
		break;
	case ATTRID_BASIC_ZCL_VERSION:
		statusRec->dataType = ZCL_DATATYPE_UINT8;
		statusRec->dataPtr = (void *)&ZCLVersion;
		break;
	case ATTRID_BASIC_MANUFACTURER_NAME:
		statusRec->dataType = ZCL_DATATYPE_CHAR_STR;
		statusRec->dataPtr = (void *)&manufacturerName;
		break;
	case ATTRID_BASIC_MODEL_ID:
		statusRec->dataType = ZCL_DATATYPE_CHAR_STR;
		statusRec->dataPtr = (void *)&modelId;
		break;
	case ATTRID_BASIC_DATE_CODE:
		statusRec->dataType = ZCL_DATATYPE_CHAR_STR;
		statusRec->dataPtr = (void *)&dateCode;
		break;
	case ATTRID_BASIC_POWER_SOURCE:
		statusRec->dataType = ZCL_DATATYPE_ENUM8;
		statusRec->dataPtr = (void *)&powerSource;
		break;
	case ATTRID_BASIC_LOCATION_DESC:
		statusRec->dataType = ZCL_DATATYPE_CHAR_STR;
		statusRec->dataPtr = (void *)&locationDescription;
		statusRec->accessControl = ACCESS_CONTROL_R_W; 
		break;
	case ATTRID_BASIC_PHYSICAL_ENV:
		statusRec->dataType = ZCL_DATATYPE_ENUM8;
		statusRec->dataPtr = (void *)&physicalEnvironment;
		statusRec->accessControl = ACCESS_CONTROL_R_W; 
		break;
	case ATTRID_BASIC_DEVICE_ENABLED:
		statusRec->dataType = ZCL_DATATYPE_BOOLEAN;
		statusRec->dataPtr = (void *)&deviceEnable;
		statusRec->accessControl = ACCESS_CONTROL_R_W; 
		break;
	default:
		statusRec->status = ZCL_STATUS_UNSUPPORTED_ATTRIBUTE;
	}
}

ZStatus_t processBasicClusterCommands( zclIncoming_t *pInMsg ){
	if ( zcl_ServerCmd( pInMsg->hdr.fc.direction ) ) {
		switch(pInMsg->hdr.commandID){
			case COMMAND_BASIC_RESET_FACT_DEFAULT:
				basicResetCB();
				return ZSuccess;
		default:
      		return ZFailure;   // Error ignore the command
		}
	}
	return ZSuccess;
}
/**************************************************************************************************
  Filename:       ClusterIdentify.cpp

  Autorh:  Paolo Achdjia
  Created: 28/10/2014

**************************************************************************************************/

#include "OSAL.h"
#include "OSAL_PwrMgr.h"
#include "hal_led.h"

#include "ClusterIdentify.h"
#include "zcl_general.h"
#include "ClusterOSALEvents.h"


#define ON_TIME 600
#define OFF_TIME 400

static uint16 identifyTime=0;

static byte mainAppTaskId;

static uint8  onOff;

__sfr __no_init volatile struct  {
	unsigned char DIR0_0: 1;
	unsigned char DIR0_1: 1;
	unsigned char DIR0_2: 1;
	unsigned char DIR0_3: 1;
	unsigned char DIR0_4: 1;
	unsigned char DIR0_5: 1;
	unsigned char DIR0_6: 1;
	unsigned char DIR0_7: 1;
} @ 0xFD;

__sfr __no_init volatile struct  {
	unsigned char P0SEL_0: 1;
	unsigned char P0SEL_1: 1;
	unsigned char P0SEL_2: 1;
	unsigned char P0SEL_3: 1;
	unsigned char P0SEL_4: 1;
	unsigned char P0SEL_5: 1;
	unsigned char P0SEL_6: 1;
	unsigned char P0SEL_7: 1;
} @ 0xF3;


void identifyClusterReadAttribute(zclAttrRec_t * attribute){
	if (attribute == NULL){
		return;
	}
	
	if (attribute->attrId == ATTRID_IDENTIFY_TIME){
		attribute->dataType = ZCL_DATATYPE_UINT16;
		attribute->dataPtr = (void *)&identifyTime;
		attribute->status = ZCL_STATUS_SUCCESS;
	} else {
		attribute->status = ZCL_STATUS_UNSUPPORTED_ATTRIBUTE;
	}
}
void identifyClusterWriteAttribute(ZclWriteAttribute_t * writeAttribute){
	if (writeAttribute == NULL){
		return;
	}
	
	if(writeAttribute->attrId == ATTRID_IDENTIFY_TIME){
		identifyTime = *(uint16 *)writeAttribute;
		writeAttribute->status=ZCL_STATUS_SUCCESS;
	} else {
		writeAttribute->status = ZCL_STATUS_UNSUPPORTED_ATTRIBUTE;
	}
}

void identifyInit(byte taskId){
	DIR0_1 = 1;
 	P0SEL_1 = 0;
 	P0_1 = 0;
	mainAppTaskId = taskId;
}

uint16 identifyLoop(uint16 events){
	if (onOff){
		osal_start_timerEx( mainAppTaskId, IDENTIFY_TIMEOUT_EVT, OFF_TIME );
		onOff=0;
		P0_1 = 1;
	} else {
		onOff=1;
		if ( identifyTime > 0 ){
    		identifyTime--;
		}
    	if (identifyTime>0){
			osal_start_timerEx( mainAppTaskId, IDENTIFY_TIMEOUT_EVT, ON_TIME );
			osal_pwrmgr_task_state(mainAppTaskId, PWRMGR_HOLD);
			P0_1 = 0;
		} else{
			osal_pwrmgr_task_state(mainAppTaskId, PWRMGR_CONSERVE);
			P0_1 = 0;
		}
	}
    return ( events ^ IDENTIFY_TIMEOUT_EVT );
}


/*********************************************************************
 * @fn      processIdentifyTimeChange
 *
 * @brief   Called to process any change to the IdentifyTime attribute.
 *
 * @param   none
 *
 * @return  none
 */
void processIdentifyTimeChange( void ){
	if ( identifyTime > 0 ) {
		osal_start_timerEx( mainAppTaskId, IDENTIFY_TIMEOUT_EVT, ON_TIME );
		onOff=1;
		P0_1 = 1;
		osal_pwrmgr_task_state(mainAppTaskId, PWRMGR_HOLD);
	}  else {
		osal_stop_timerEx( mainAppTaskId, IDENTIFY_TIMEOUT_EVT );
		osal_pwrmgr_task_state(mainAppTaskId, PWRMGR_CONSERVE);
	}
}

ZStatus_t processIdentifyClusterServerCommands( zclIncoming_t *pInMsg ){

	switch(pInMsg->hdr.commandID){
	case COMMAND_IDENTIFY:
		identifyTime = BUILD_UINT16( pInMsg->pData[0], pInMsg->pData[1] );
		processIdentifyTimeChange();
		return ZSuccess;
	case COMMAND_IDENTIFY_QUERY:
		if ( identifyTime > 0 ) {
			zcl_SendCommand( pInMsg->msg->endPoint, &pInMsg->msg->srcAddr, ZCL_CLUSTER_ID_GEN_IDENTIFY,
                         COMMAND_IDENTIFY_QUERY_RSP, TRUE,
                         ZCL_FRAME_SERVER_CLIENT_DIR, TRUE, 0, pInMsg->hdr.transSeqNum, 2, (uint8 *) &identifyTime );
			return  ZCL_STATUS_CMD_HAS_RSP;
	    } else {
			return ZSuccess;
		}
	default:
    	return ZFailure;   // Error ignore the command
	}
}

ZStatus_t processIdentifyClusterClientCommands( zclIncoming_t *pInMsg ){
	if ( pInMsg->hdr.commandID != COMMAND_IDENTIFY_QUERY_RSP )
		return ZFailure;

	return ZFailure;
}


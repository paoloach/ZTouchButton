/**************************************************************************************************

 DESCRIPTION:
  --

 CREATED: 14/10/2015, by Paolo Achdjian

 FILE: PowerMeter.c

***************************************************************************************************/

#include "ZComDef.h"
#include "OSAL.h"
#include "hal_drivers.h"
#include "AF.h"
#include "ZDApp.h"
#include "ZMAC.h"

#include "zcl.h"
#include "zcl_general.h"
#include "zcl_ha.h"
#include "ZTouchButton.h"
#include "AddrMgr.h"
#include "onboard.h"
/* HAL */
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"
#include "zclReadAttributeFn.h"
#include "zclWriteAttributeFn.h"
#include "clusters/ClusterOSALEvents.h"
#include "clusters/ClusterIdentify.h"
#include "clusters/ClusterBasic.h"
#include "clusters/ClusterPower.h"
#include "clusters/ClusterOnOff.h"
#include "clusters/ClusterMetering.h"
#include "clusters/ClusterOnOff.h"
#include "clusters/ClusterElectricityMeasure.h"
#include "clusters/ClusterTest.h"


_PRAGMA(vector=P0INT_VECTOR) __near_func __interrupt void port0Interrupt(void);



static byte ZTouchButtonTaskID;
static afAddrType_t onOffSendaddr;


static void ZTouchButton_ProcessIncomingMsg( zclIncomingMsg_t *msg );
static uint8 ZTouchButton_ProcessInReadRspCmd( zclIncomingMsg_t *pInMsg );
static uint8 ZTouchButton_ProcessInWriteRspCmd( zclIncomingMsg_t *pInMsg );
static uint8 ZTouchButton_ProcessInDefaultRspCmd( zclIncomingMsg_t *pInMsg );
#ifdef ZCL_DISCOVER
static uint8 ZTouchButton_ProcessInDiscRspCmd( zclIncomingMsg_t *pInMsg );
#endif
static ZStatus_t handleClusterCommands( zclIncoming_t *pInMsg );


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

__sfr __no_init volatile struct  {
	unsigned char P0IFG_0: 1;
	unsigned char P0IFG_1: 1;
	unsigned char P0IFG_2: 1;
	unsigned char P0IFG_3: 1;
	unsigned char P0IFG_4: 1;
	unsigned char P0IFG_5: 1;
	unsigned char P0IFG_6: 1;
	unsigned char P0IFG_7: 1;
} @ 0x89;

__sfr __no_init volatile struct  {
	unsigned char P0ICON: 1;
	unsigned char P1ICON_L: 1;
	unsigned char P1ICON_H: 1;
	unsigned char P2ICON: 1;
	unsigned char RESERVED_4: 1;
	unsigned char RESERVED_5: 1;
	unsigned char RESERVED_6: 1;
	unsigned char PADSC: 1;
} @ 0xF3;


/*********************************************************************
 * ZCL General Profile Callback table
 */
/*********************************************************************
 * @fn          temperatureHumiditySensor_Init
 *
 * @brief       Initialization function for the zclGeneral layer.
 *
 * @param       none
 *
 * @return      none
 */
void ZTouchButton_Init( byte task_id ){
	ZTouchButtonTaskID = task_id;
	
	zcl_registerPlugin( ZCL_CLUSTER_ID_GEN_BASIC,  ZCL_CLUSTER_ID_GEN_MULTISTATE_VALUE_BASIC, handleClusterCommands );
 	
	zclHA_Init( &OnOff_SimpleDesc );
	addReadAttributeFn(ENDPOINT_ONOFF_SWITCH, ZCL_CLUSTER_ID_GEN_BASIC,basicClusterReadAttribute);
	addWriteAttributeFn(ENDPOINT_ONOFF_SWITCH, ZCL_CLUSTER_ID_GEN_BASIC,basicClusterWriteAttribute);
	addReadAttributeFn(ENDPOINT_ONOFF_SWITCH, ZCL_CLUSTER_ID_GEN_IDENTIFY,identifyClusterReadAttribute);
	addWriteAttributeFn(ENDPOINT_ONOFF_SWITCH, ZCL_CLUSTER_ID_GEN_IDENTIFY,identifyClusterWriteAttribute);
	addReadAttributeFn(ENDPOINT_ONOFF_SWITCH,ZCL_CLUSTER_ID_GEN_POWER_CFG,powerClusterReadAttribute);
	
	zcl_registerForMsg( ZTouchButtonTaskID );
  
  	EA=1;
 	identifyInit(ZTouchButtonTaskID);
	ZMacSetTransmitPower(TX_PWR_PLUS_3);
	
	P0_0 = 0;
	DIR0_0 =0;
	P0SEL=0;
	P0ICON=0;
	P0IEN=0x01;
	P0IE=1;
	

	setRegisteredKeysTaskID(task_id);
	fastBlinkOn();
}

/*********************************************************************
 * @fn          zclSample_event_loop
 *
 * @brief       Event Loop Processor for zclGeneral.
 *
 * @param       none
 *
 * @return      none
 */
uint16 ZTouchButtonEventLoop( uint8 task_id, uint16 events ){
	afIncomingMSGPacket_t *MSGpkt;
	devStates_t zclSampleSw_NwkState;
  
	(void)task_id;  // Intentionally unreferenced parameter
	 if ( events & SYS_EVENT_MSG ){
		while ( (MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( ZTouchButtonTaskID )) )  {
			switch ( MSGpkt->hdr.event ) {
				case ZCL_INCOMING_MSG:
          			// Incoming ZCL Foundation command/response messages
          			ZTouchButton_ProcessIncomingMsg( (zclIncomingMsg_t *)MSGpkt );
          			break;
				case  KEY_CHANGE:
					if (((keyChange_t *)MSGpkt)->keys==HAL_KEY_SW_1){
						BindingEntry_t * bindEntry;
						bindEntry = bindFind(ENDPOINT_ONOFF_SWITCH, ZCL_CLUSTER_ID_GEN_ON_OFF,0);
						if( bindEntry){
							 AddrMgrEntry_t entry;
							 entry.user = ADDRMGR_USER_BINDING;
							entry.index = bindEntry->dstIdx;
							uint8 stat = AddrMgrEntryGet( &entry );
							if (stat){
								onOffSendaddr.addrMode = afAddr16Bit;
								onOffSendaddr.addr.shortAddr = entry.nwkAddr;
								onOffSendaddr.endPoint=bindEntry->dstEP;
								zcl_SendCommand(ENDPOINT_ONOFF_SWITCH, &onOffSendaddr, ZCL_CLUSTER_ID_GEN_ON_OFF,COMMAND_TOGGLE, TRUE, ZCL_FRAME_CLIENT_SERVER_DIR, TRUE, 0, 0, 0,NULL);
							}
						}
					}
					break;
				case ZDO_STATE_CHANGE:
          			zclSampleSw_NwkState = (devStates_t)(MSGpkt->hdr.status);
					if (zclSampleSw_NwkState == DEV_END_DEVICE){
						fastBlinkOff();
					}
		       	default:
        			break;
      		}

        osal_msg_deallocate( (uint8 *)MSGpkt );
    	}

    	return (events ^ SYS_EVENT_MSG);
	}
	
	if ( events & IDENTIFY_TIMEOUT_EVT ) {
		return identifyLoop(events);
	}
	
 	return 0;
}

/****************************************************************************** 
 * 
 *  Functions for processing ZCL Foundation incoming Command/Response messages
 *
 *****************************************************************************/

/*********************************************************************
 * @fn      ZTouchButton_ProcessIncomingMsg
 *
 * @brief   Process ZCL Foundation incoming message
 *
 * @param   pInMsg - pointer to the received message
 *
 * @return  none
 */
static void ZTouchButton_ProcessIncomingMsg( zclIncomingMsg_t *pInMsg){
	switch ( pInMsg->zclHdr.commandID ){
		case ZCL_CMD_READ_RSP:
			ZTouchButton_ProcessInReadRspCmd( pInMsg );
			break;
		case ZCL_CMD_WRITE_RSP:
			ZTouchButton_ProcessInWriteRspCmd( pInMsg );
			break;
#ifdef ZCL_REPORT
    // See ZCL Test Applicaiton (zcl_testapp.c) for sample code on Attribute Reporting
		case ZCL_CMD_CONFIG_REPORT:
      	//zclSampleLight_ProcessInConfigReportCmd( pInMsg );
			break;
    
		case ZCL_CMD_CONFIG_REPORT_RSP:
      //zclSampleLight_ProcessInConfigReportRspCmd( pInMsg );
			break;
    
		case ZCL_CMD_READ_REPORT_CFG:
      //zclSampleLight_ProcessInReadReportCfgCmd( pInMsg );
			break;
    
		case ZCL_CMD_READ_REPORT_CFG_RSP:
      //zclSampleLight_ProcessInReadReportCfgRspCmd( pInMsg );
			break;
    
		case ZCL_CMD_REPORT:
      	//zclSampleLight_ProcessInReportCmd( pInMsg );
			break;
#endif   
		case ZCL_CMD_DEFAULT_RSP:
			ZTouchButton_ProcessInDefaultRspCmd( pInMsg );
			break;
#ifdef ZCL_DISCOVER     
		case ZCL_CMD_DISCOVER_RSP:
			ZTouchButton_ProcessInDiscRspCmd( pInMsg );
			break;
#endif  
		default:
			break;
	}
  
	if ( pInMsg->attrCmd )
    	osal_mem_free( pInMsg->attrCmd );
}

/*********************************************************************
 * @fn      ZTouchButton_ProcessInReadRspCmd
 *
 * @brief   Process the "Profile" Read Response Command
 *
 * @param   pInMsg - incoming message to process
 *
 * @return  none
 */
static uint8 ZTouchButton_ProcessInReadRspCmd( zclIncomingMsg_t *pInMsg ){
	zclReadRspCmd_t *readRspCmd;
	uint8 i;

	readRspCmd = (zclReadRspCmd_t *)pInMsg->attrCmd;
	for (i = 0; i < readRspCmd->numAttr; i++){
		// Notify the originator of the results of the original read attributes attempt and, for each successfull request, the value of the requested attribute
	}

	return TRUE; 
}

/*********************************************************************
 * @fn      ZTouchButton_ProcessInWriteRspCmd
 *
 * @brief   Process the "Profile" Write Response Command
 *
 * @param   pInMsg - incoming message to process
 *
 * @return  none
 */
static uint8 ZTouchButton_ProcessInWriteRspCmd( zclIncomingMsg_t *pInMsg ){
 	zclWriteRspCmd_t *writeRspCmd;
 	uint8 i;

 	writeRspCmd = (zclWriteRspCmd_t *)pInMsg->attrCmd;
 	for (i = 0; i < writeRspCmd->numAttr; i++) {
		// Notify the device of the results of the its original write attributes command.
 	}

 	return TRUE; 
}

/*********************************************************************
 * @fn      ZTouchButton_ProcessInDefaultRspCmd
 *
 * @brief   Process the "Profile" Default Response Command
 *
 * @param   pInMsg - incoming message to process
 *
 * @return  none
 */
static uint8 ZTouchButton_ProcessInDefaultRspCmd( zclIncomingMsg_t *pInMsg ){
	// zclDefaultRspCmd_t *defaultRspCmd = (zclDefaultRspCmd_t *)pInMsg->attrCmd;
   
	// Device is notified of the Default Response command.
  	(void)pInMsg;
  
  	return TRUE; 
}

#ifdef ZCL_DISCOVER
/*********************************************************************
 * @fn      ZTouchButton_ProcessInDiscRspCmd
 *
 * @brief   Process the "Profile" Discover Response Command
 *
 * @param   pInMsg - incoming message to process
 *
 * @return  none
 */
static uint8 ZTouchButton_ProcessInDiscRspCmd( zclIncomingMsg_t *pInMsg ){
	zclDiscoverRspCmd_t *discoverRspCmd;
 	uint8 i;
  
 	discoverRspCmd = (zclDiscoverRspCmd_t *)pInMsg->attrCmd;
  	for ( i = 0; i < discoverRspCmd->numAttr; i++ ) {
		// Device is notified of the result of its attribute discovery command.
  	}
  
 	return TRUE;
}
#endif // ZCL_DISCOVER

/*********************************************************************
 * @brief      dispatch the cluster command to the right callback function
 *
 * @param   pInMsg - incoming message to process
 *
 * @return  none
 */
static ZStatus_t handleClusterCommands( zclIncoming_t *pInMsg ){
	ZStatus_t stat = ZFailure;

	if (zcl_ServerCmd( pInMsg->hdr.fc.direction ) ) {
		switch ( pInMsg->msg->clusterId ){
		    case ZCL_CLUSTER_ID_GEN_BASIC:
    		return processBasicClusterCommands(pInMsg);
	    case ZCL_CLUSTER_ID_GEN_IDENTIFY:
			return processIdentifyClusterServerCommands( pInMsg );
	    case ZCL_CLUSTER_ID_GEN_GROUPS:
    	case ZCL_CLUSTER_ID_GEN_SCENES:
	    case ZCL_CLUSTER_ID_GEN_ON_OFF:
    	case ZCL_CLUSTER_ID_GEN_LEVEL_CONTROL:
	    case ZCL_CLUSTER_ID_GEN_ALARMS:
    	case ZCL_CLUSTER_ID_GEN_LOCATION:
	    case ZCL_CLUSTER_ID_GEN_POWER_CFG:
    	case ZCL_CLUSTER_ID_GEN_DEVICE_TEMP_CONFIG:
	    case ZCL_CLUSTER_ID_GEN_ON_OFF_SWITCH_CONFIG:
    	case ZCL_CLUSTER_ID_GEN_TIME:
	    default:
    	  stat = ZFailure;
	      break;
  		}
	} else {
		switch ( pInMsg->msg->clusterId ){
	
  	    case ZCL_CLUSTER_ID_GEN_IDENTIFY:
			return processIdentifyClusterClientCommands( pInMsg );
	    default:
    	  stat = ZFailure;
	      break;
  		}
	}

  return ( stat );
}


HAL_ISR_FUNCTION( port0Interrupt, P0INT_VECTOR ){
	HAL_ENTER_ISR();

	if(P0IFG_0){
		P0IFG_0=0;
		P0IF=0;
	}

	CLEAR_SLEEP_MODE();
	HAL_EXIT_ISR();
}



/****************************************************************************
****************************************************************************/



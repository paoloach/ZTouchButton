/**************************************************************************************************
Filename:       PowerMeter_data.c
Autorh: 		 Paolo Achdjia
Created: 13/10/2015
**************************************************************************************************/

/*********************************************************************
* INCLUDES
*/
#include "ZComDef.h"
#include "OSAL.h"
#include "AF.h"
#include "ZDConfig.h"

#include "zcl.h"
#include "zcl_general.h"
#include "zcl_ha.h"

#include "ZTouchButton.h"
#include "clusters/ClusterIdentify.h"
#include "clusters/ClusterBasic.h"
#include "clusters/ClusterPower.h"
#include "clusters/ClusterOnOff.h"

/*********************************************************************
* CONSTANTS
*/

#define DEVICE_VERSION     0
#define FLAGS              0



/*********************************************************************
* ATTRIBUTE DEFINITIONS - Uses REAL cluster IDs
*/

/*********************************************************************
* SIMPLE DESCRIPTOR
*/
// This is the Cluster ID List and should be filled with Application specific cluster IDs.
const cId_t onOff_switch_InClusterList[] ={
	ZCL_CLUSTER_ID_GEN_BASIC,
	ZCL_CLUSTER_ID_GEN_IDENTIFY,
	ZCL_CLUSTER_ID_GEN_POWER_CFG
};

const cId_t onOff_switch_OutClusterList[] ={
	ZCL_CLUSTER_ID_GEN_BASIC,
	ZCL_CLUSTER_ID_GEN_ON_OFF,
    ZCL_CLUSTER_ID_GEN_IDENTIFY,
};


SimpleDescriptionFormat_t OnOff_SimpleDesc = {
	ENDPOINT_ONOFF_SWITCH,                  			//  int Endpoint;
	ZCL_HA_PROFILE_ID,                	//  uint16 AppProfId[2];
	ZCL_HA_DEVICEID_ON_OFF_LIGHT_SWITCH,     	//  uint16 AppDeviceId[2];
	DEVICE_VERSION,           			//  int   AppDevVer:4;
	FLAGS,                    			//  int   AppFlags:4;
	sizeof(onOff_switch_InClusterList)/2,              //  byte  AppNumInClusters;
	(cId_t *)onOff_switch_InClusterList, 	//  byte *pAppInClusterList;
	sizeof(onOff_switch_OutClusterList)/2,           //  byte  AppNumInClusters;
	(cId_t *)onOff_switch_OutClusterList 	//  byte *pAppInClusterList;
};




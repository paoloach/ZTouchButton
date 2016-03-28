/**************************************************************************************************
  Filename:       ClusterIdentify.h

  Autorh:  Paolo Achdjia
  Created: 28/10/2014

**************************************************************************************************/

#ifndef __CLUSTER_IDENTIFY__H_
#define __CLUSTER_IDENTIFY__H_

#include "zcl.h"
#include "zclReadAttributeFn.h"
#include "zclWriteAttributeFn.h"


void identifyInit(byte taskId);
void processIdentifyTimeChange( void );
uint16 identifyLoop(uint16 events);

ZStatus_t processIdentifyClusterServerCommands( zclIncoming_t *pInMsg );
ZStatus_t processIdentifyClusterClientCommands( zclIncoming_t *pInMsg );

void identifyClusterReadAttribute(zclAttrRec_t *);
void identifyClusterWriteAttribute(ZclWriteAttribute_t *);

#endif
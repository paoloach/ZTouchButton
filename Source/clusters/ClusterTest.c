
/**************************************************************************************************

 DESCRIPTION:
  Metering Cluster

 CREATED: 08/09/2015, by Paolo Achdjian

 FILE: ClusterMetering.c

***************************************************************************************************/

#include "zcl.h"
#include "zcl_general.h"
#include "ClusterTest.h"
#include "CS5463.h"


static int32 tmp;

void testClusterReadAttribute(zclAttrRec_t * attribute) {
	if (attribute == NULL){
		return;
	}
	attribute->accessControl = ACCESS_CONTROL_READ;
	attribute->status = ZCL_STATUS_SUCCESS;
	if ( attribute->attrId <=31){
		tmp = getCS5463RegisterValue(attribute->attrId);
		tmp &= 0xFFFFFF;
		attribute->dataType = ZCL_DATATYPE_UINT32;
		attribute->dataPtr = (void *)&tmp;
	} else {
		attribute->status = ZCL_STATUS_UNSUPPORTED_ATTRIBUTE;
	}
}
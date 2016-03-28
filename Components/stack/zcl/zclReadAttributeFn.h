#ifndef __ZCL_READ_ATTRIBUTE_FN__H__
#define __ZCL_READ_ATTRIBUTE_FN__H__

#include "hal_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

// Read Attribute Response Status record

typedef void (*AttributeWriteCB)(void);

typedef struct{
	uint16  attrId;         // Attribute ID
	uint8   dataType;       // Data Type - defined in AF.h
	uint8   accessControl;  // Read/write - bit field
	void    *dataPtr;       // Pointer to data field
	uint8  status;            // should be ZCL_STATUS_SUCCESS or error
} zclAttrRec_t;

typedef void  (*ReadAttributeFn)(zclAttrRec_t *);

struct ReadAttributeFnList  {
	uint8 endpoint;
	uint16 clusterId;
	ReadAttributeFn callback;
	struct ReadAttributeFnList * next;
};

ReadAttributeFn findReadAttributeFn(uint8 endpoint, uint16 clusterId);
void addReadAttributeFn(uint8 endpoint, uint16 cluster, ReadAttributeFn callback);

#ifdef __cplusplus
}
#endif
#endif
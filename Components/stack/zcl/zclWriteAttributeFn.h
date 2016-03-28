#ifndef __ZCL_WRITE_ATTRIBUTE_FN__H__
#define __ZCL_WRITE_ATTRIBUTE_FN__H__

#include "hal_types.h"

#ifdef __cplusplus
extern "C"
{
#endif
	
	
typedef struct{
  uint16  attrId;         // Attribute ID
  uint8   dataType;       // Data Type - defined in AF.h
  void    *dataPtr;       // Pointer to data field
  uint8   len;				// data length
  uint8  status;            // should be ZCL_STATUS_SUCCESS or error
} ZclWriteAttribute_t;


typedef void  (*WriteAttributeFn)(ZclWriteAttribute_t *);

struct WriteAttributeFnList  {
	uint8 endpoint;
	uint16 clusterId;
	WriteAttributeFn callback;
	struct WriteAttributeFnList * next;
};

WriteAttributeFn findWriteAttributeFn(uint8 endpoint, uint16 clusterId);
void addWriteAttributeFn(uint8 endpoint, uint16 cluster, WriteAttributeFn callback);
	
	
#ifdef __cplusplus
}
#endif


#endif
/**************************************************************************************************

 DESCRIPTION:
  --

 CREATED: 03/11/2015, by Paolo Achdjian

 FILE: zclWriteAttributeFn.c

***************************************************************************************************/

#include "zclWriteAttributeFn.h"

#include "OSAL_Memory.h"

#define ZCL_STATUS_UNSUPPORTED_ATTRIBUTE                0x86

static struct WriteAttributeFnList * head;

static void writeAttributeFnDefault(ZclWriteAttribute_t *);

void addWriteAttributeFn(uint8 endpoint, uint16 cluster, WriteAttributeFn callback){
	if (head == NULL){
		head = (struct WriteAttributeFnList *) osal_mem_alloc(sizeof(struct WriteAttributeFnList));
		head->endpoint = endpoint;
		head->clusterId = cluster;
		head->callback = callback;
		head->next = NULL;
	} else {
		struct WriteAttributeFnList * newHead = (struct WriteAttributeFnList *) osal_mem_alloc(sizeof(struct WriteAttributeFnList));
		newHead->endpoint = endpoint;
		newHead->clusterId = cluster;
		newHead->callback = callback;
		newHead->next = head;
		head = newHead;
	}
}

WriteAttributeFn findWriteAttributeFn(uint8 endpoint, uint16 clusterId){
	struct WriteAttributeFnList * iter = head;
	while (iter != NULL){
		if (iter->endpoint == endpoint && iter->clusterId == clusterId){
			return iter->callback;
		}
		iter = iter->next;
	}
	return &writeAttributeFnDefault;
}

void writeAttributeFnDefault(ZclWriteAttribute_t * writeAttribute){
	writeAttribute->status=ZCL_STATUS_UNSUPPORTED_ATTRIBUTE;
}
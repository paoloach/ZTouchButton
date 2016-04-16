/**************************************************************************************************

 DESCRIPTION:
  --

 CREATED: 01/04/2016, by Paolo Achdjian

 FILE: KeyChange.h

***************************************************************************************************/
#include "OSAL.h"

typedef struct
{
  osal_event_hdr_t hdr;
  uint8 state; // shift
  uint8 keys;  // keys
} keyChange_t;

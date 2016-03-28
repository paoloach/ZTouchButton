/**************************************************************************************************

 DESCRIPTION:
  --

 CREATED: 06/11/2015, by Paolo Achdjian

 FILE: mainTestcs5463.c

***************************************************************************************************/



#include "hal_adc.h"
#include "hal_flash.h"
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_drivers.h"
#include "OnBoard.h"
#include "OSAL.h"
#include "OSAL_Nv.h"

#include "clusters/ClusterIdentify.h"
#include "clusters/ClusterOnOff.h"
#include "CS5463.h"


static void identifyLED(void);
static void onOff(void);

/*********************************************************************
 * @fn      zmain_vdd_check
 * @brief   Check if the Vdd is OK to run the processor.
 * @return  Return if Vdd is ok; otherwise, flash LED, then reset
 *********************************************************************/
static void zmain_vdd_check( void )
{
  uint8 cnt = 16;
  
  do {
    while (!HalAdcCheckVdd(VDD_MIN_RUN));
  } while (--cnt);
}

volatile uint32 value5463;
volatile uint32 reg00;
volatile uint32 reg01;
volatile uint32 reg02;
volatile uint32 reg03;
volatile uint32 reg04;
volatile uint32 reg05;
volatile uint32 reg11;
volatile uint32 reg12;
volatile uint32 reg15;
volatile uint32 reg18;

int main() {
	 // Turn off interrupts
  //osal_int_disable( INTS_ALL );

  // Initialization for board related stuff such as LEDs
  //HAL_BOARD_INIT();

  // Make sure supply voltage is high enough to run
  zmain_vdd_check();

  // Initialize board I/O
  //InitBoard( OB_COLD );

  // Initialze HAL drivers
  //HalDriverInit();

  // Initialize NV System
 // osal_nv_init( NULL );
 // Initialize the operating system
  //osal_init_system();

  // Allow interrupts
  //osal_int_enable( INTS_ALL );

  // Final board initialization
 // InitBoard( OB_READY );

  identifyLED();
	onOffInit();
  CS5463_Init();
  CS5463_reset();
  setCS5463RegisterValue(5,0x00, 0x0F, 0xA0);
  setCS5463RegisterValue(0,0x01, 0x00, 0x01);
  setCS5463RegisterValue(18,0x00, 0x00, 0x60);
  CS5463_startConversion();
  
  while(1){
	  reg00 = getCS5463RegisterValue(0);
	  reg01 = getCS5463RegisterValue(1);
	  reg02 = getCS5463RegisterValue(2);
	  reg03 = getCS5463RegisterValue(3);
	  reg04 = getCS5463RegisterValue(4);
	  reg05 = getCS5463RegisterValue(5);
	  reg11 = getCS5463RegisterValue(RMSCurrent);
	  reg12 = getCS5463RegisterValue(RMSVolt);
	  reg18 = getCS5463RegisterValue(18);
	  
	  // MULT = 0.250 * 2000;
	  // DIV = 0x1000000
	 
	  
  		reg15 = getCS5463RegisterValue(status);
		if (P1_2 == 1){
			setLightStatus(LIGHT_ON);
		} else {
			setLightStatus(LIGHT_OFF);
		}
  }
  
}


static void identifyLED(void) {
	identifyInit(1);
	P0_1 = 1;	

}

static void onOff(void) {
	onOffInit();
	while(1){
		if (P1_2 == 1){
			setLightStatus(LIGHT_ON);
		} else {
			setLightStatus(LIGHT_OFF);
		}
	}
}
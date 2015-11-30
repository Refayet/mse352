/*
 * mainheader.h
 *
 *  Created on: 2015-10-19
 *      Author: Kirill
 */

#ifndef MAINHEADER_H_
#define MAINHEADER_H_

#include "inc/tm4c123gh6pm.h"
#include <stdint.h>
#include <stdbool.h>
//#include "inc/hw_ints.h"
#include "driverlib/adc.h"
#include "inc/hw_memmap.h"
//#include "inc/hw_nvic.h"
//#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/systick.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/timer.h"
#include "driverlib/rom_map.h"
//#include "drivers/pinout.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "driverlib/debug.h"
#include "driverlib/qei.h"
#include "driverlib/pwm.h"


#define RED_LED   	0x02
#define BLUE_LED  	0x04
#define GREEN_LED 	0x08

#define LED_RED    	0x02
#define LED_BLUE   	0x04
#define LED_GREEN  	0x08
#define LED_PURPLE 	0x06
#define LED_YELLOW 	0x0A
#define LED_CYAN   	0x0C
#define LED_OFF    	0x00

// The error routine that is called if the driver library encounters an error.
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

//****************************MAIN FUNCTION PROTOTYPES************************************

void blink_green_3_times(void);
interrupt void SycTickInt_isr();
uint32_t micros (void);
void rgb_led(uint8_t pins);

//**************INTIALIZATION FUNCTION PROTOTYPES**********
void initializeAll(void);
void initializeADC(void);
void initializeUART(void);
void initializeGPIO(void);
void initializePWM (void);

//***************PONG FUNCTIONS*******************************

void ADC_0_Int_isr (void);
void pong (void);
void printscore(void);
void UARTSend(const uint8_t *pui8Buffer, uint32_t ui32Count);

//**************LED MATRIX FUNCTIONS ***************************
void cycle_col_clock (uint32_t pulse_width);
void cycle_row_clock (uint32_t pulse_width);
void cycle_row_register (uint32_t pulse_width);
void cycle_col_register (uint32_t pulse_width);
void set_row (uint32_t row);
void updateMatrix (uint32_t screen[], int32_t shift_x, int32_t shift_y);
void transfer_registers (void);
void shift_row(void);
void erase_displays (void);

void row_sh_cp_high (void);
void row_ds_high (void);
void row_st_cp_high (void);
void row_mr_high (void);
void col_sh_cp_high (void);
void col_ds_high (void) 	;
void col_st_cp_high (void) 	;

void col_mr_high (void) 	;
void row_sh_cp_low (void) 	;
void row_ds_low (void) 	;
void row_st_cp_low (void);
void row_mr_low (void) 		;
void col_sh_cp_low (void) 	;
void col_ds_low (void) 		;
void col_st_cp_low (void) 	;
void col_mr_low (void) 	;

#endif /* MAINHEADER_H_ */


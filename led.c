/*
 * led.c
 *
 *  Created on: 2015-10-19
 *      Author: Kirill
 */

#include "mainheader.h"

//ROW_SH_CP = PC4
//ROW_DS = PC6
//ROW_ST_CP = PC5
//ROW_MR = PC7

// col_sh_cp = PD2
// col_ds = PD3
// col_st_cp = PD1
// col_mr  = PD0

uint32_t delay1 = 0;
uint32_t delay2 = 0;
uint32_t ui32SysClock = 50000000; //this will hold the value of clock speed
uint32_t clock_p_width = 5;
uint32_t clock_pulse_spacing = 10;
uint32_t light_on_period = 5000;


void cycle_col_clock (uint32_t pulse_width)
{
	GPIO_PORTD_DATA_R &= ~(GPIO_PIN_2);
	SysCtlDelay(pulse_width);
	GPIO_PORTD_DATA_R |= GPIO_PIN_2;
	SysCtlDelay(pulse_width);
	GPIO_PORTD_DATA_R &= ~(GPIO_PIN_2);
	SysCtlDelay(clock_pulse_spacing);
}

void cycle_row_clock (uint32_t pulse_width)
{
	GPIO_PORTC_DATA_R &= ~(GPIO_PIN_4);
	SysCtlDelay(pulse_width);
	GPIO_PORTC_DATA_R |= GPIO_PIN_4;
	SysCtlDelay(pulse_width);
	GPIO_PORTC_DATA_R &= ~(GPIO_PIN_4);
	SysCtlDelay(clock_pulse_spacing);
}

void cycle_col_register (uint32_t pulse_width)
{
	GPIO_PORTD_DATA_R &= ~(GPIO_PIN_1);
	SysCtlDelay(pulse_width);
	GPIO_PORTD_DATA_R |= GPIO_PIN_1;
	SysCtlDelay(pulse_width);
	GPIO_PORTD_DATA_R &= ~(GPIO_PIN_1);
	SysCtlDelay(clock_pulse_spacing);
}

void cycle_row_register (uint32_t pulse_width)
{
	GPIO_PORTC_DATA_R &= ~(GPIO_PIN_5);
	SysCtlDelay(pulse_width);
	GPIO_PORTC_DATA_R |= GPIO_PIN_5;
	SysCtlDelay(pulse_width);
	GPIO_PORTC_DATA_R &= ~(GPIO_PIN_5);
	SysCtlDelay(clock_pulse_spacing);
}

void set_row (uint32_t row)
{
	uint32_t current_bit = 1;

	uint16_t i = 0;
	for (i = 32; i > 0; i--)
	{
		if (row & current_bit)
		{
			GPIO_PORTD_DATA_R |= GPIO_PIN_3;
		}
		else
		{
			GPIO_PORTD_DATA_R &= ~(GPIO_PIN_3);
		}
		cycle_col_clock (clock_p_width);
		current_bit = current_bit << 1;
	}
}

void updateMatrix (uint32_t screen[], int32_t shift_x, int32_t shift_y)
{

	GPIO_PORTC_DATA_R |= GPIO_PIN_6; //Send 1 to row shift register data pin
	cycle_row_clock (clock_p_width);
	GPIO_PORTC_DATA_R &= ~(GPIO_PIN_6); //Only 1 row is enabled at a time
	uint16_t row = 0;

	for (row = 13 - shift_y; row <= (13 + 15 - shift_y); row++)  //set first 15 rows, shifting row after each one
	{

		if (shift_x >= 0)
		{
			set_row(screen[row] << shift_x);
		}
		else
		{
			set_row(screen[row] >> (-1*shift_x));
		}
		transfer_registers ();
		SysCtlDelay(light_on_period);
		cycle_row_clock (clock_p_width);
	}
}

void transfer_registers (void)
{

    cycle_col_register(clock_p_width);
    cycle_row_register(clock_p_width);
}

void shift_row(void)
{
	cycle_row_clock (clock_p_width);
	cycle_row_register(clock_p_width);
}

void erase_displays (void)
{
	GPIO_PORTC_DATA_R &= ~(GPIO_PIN_7);
	GPIO_PORTD_DATA_R &= ~(GPIO_PIN_0);
	SysCtlDelay(1);
	cycle_col_register(clock_p_width);
	cycle_row_register(clock_p_width);
	GPIO_PORTC_DATA_R |= GPIO_PIN_7;
	GPIO_PORTD_DATA_R |= GPIO_PIN_0;
	SysCtlDelay(1);
}

void row_sh_cp_high (void) 	{ GPIO_PORTC_DATA_R |= GPIO_PIN_4; }
void row_ds_high (void) 	{ GPIO_PORTC_DATA_R |= GPIO_PIN_6; }
void row_st_cp_high (void) 	{ GPIO_PORTC_DATA_R |= GPIO_PIN_5; }
void row_mr_high (void) 	{ GPIO_PORTC_DATA_R |= GPIO_PIN_7; }
void col_sh_cp_high (void) 	{ GPIO_PORTD_DATA_R |= GPIO_PIN_2; }
void col_ds_high (void) 	{ GPIO_PORTD_DATA_R |= GPIO_PIN_3; }
void col_st_cp_high (void) 	{ GPIO_PORTD_DATA_R |= GPIO_PIN_1; }
void col_mr_high (void) 	{ GPIO_PORTD_DATA_R |= GPIO_PIN_0; }

void row_sh_cp_low (void) 	{ GPIO_PORTC_DATA_R &= ~(GPIO_PIN_4);}
void row_ds_low (void) 		{ GPIO_PORTC_DATA_R &= ~(GPIO_PIN_6);}
void row_st_cp_low (void) 	{ GPIO_PORTC_DATA_R &= ~(GPIO_PIN_5);}
void row_mr_low (void) 		{ GPIO_PORTC_DATA_R &= ~(GPIO_PIN_7);}
void col_sh_cp_low (void) 	{ GPIO_PORTD_DATA_R &= ~(GPIO_PIN_2);}
void col_ds_low (void) 		{ GPIO_PORTD_DATA_R &= ~(GPIO_PIN_3);}
void col_st_cp_low (void) 	{ GPIO_PORTD_DATA_R &= ~(GPIO_PIN_1);}
void col_mr_low (void) 		{ GPIO_PORTD_DATA_R &= ~(GPIO_PIN_0);}


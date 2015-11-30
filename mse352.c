//*****************************************************************************
//MSE351 Project Firmware V.1
//Created on: Oct 18, 2015
//Author: kirill
//
// description description description description


//
//col_mr 	= 6;
//col_st_cp = 5;
//col_sh_cp = 4;
//col_ds 	= 3;
//row_sh_cp = 10;
//row_st_cp = 11;
//row_ds 	= 12;
//row_mr 	= 13;

//ROW_SH_CP = PC4
//ROW_DS = PC6
//ROW_ST_CP = PC5
//ROW_MR = PC7

// col_sh_cp = PD2
// col_ds = PD3
// col_st_cp = PD1
// col_mr  = PD0



#include "mainheader.h"

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

#define DEBUG_MESSAGES 1


uint32_t mse_rocks[42] = {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
                               0x0117DF00,
                               0x01B45000,
                               0x01F41000,
                               0x0157DF00,
                               0x01105000,
                               0x01145000,
                               0x0117DF00,
                               0x00000000,
                               0x7DF7D17C,
                               0x45145244,
                               0x45141440,
                               0x7D141C7C,
                               0x51141204,
                               0x49145144,
                               0x45F7D17C,
                               0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };

uint32_t pong_screen[16] = {   0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
								0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };


// System clock rate in Hz.
uint32_t g_ui32SysClock;

//Counters
volatile int16_t j = 1;
volatile int16_t i = 1;



volatile uint32_t millis = 0; //microsecond count

volatile uint32_t ui32ADCoutput[10];

volatile uint32_t joyLX = 2000;
volatile uint32_t joyLY = 2000;
volatile uint32_t joyRX = 2000;
volatile uint32_t joyRY = 2000;

volatile uint32_t score_l = 0;
volatile uint32_t score_r = 0;

volatile int32_t x_shift = 0;
volatile int32_t y_shift = 0;

volatile float pong_l_pos = 6;
volatile float pong_r_pos = 8;
volatile float 	pong_x_ball = 16.;
volatile float 	pong_y_ball = 3.;
volatile float 	pong_x_vel = -0.2;
volatile float 	pong_y_vel = 0.1;


//****************************FUNCTIONS******************************************

int main(void)
{

	initializeAll();

	blink_green_3_times(); //Confirm successful initialization


	//Ready to begin:
	millis = 0; //Reset the millis counter
	ROM_SysTickEnable();
	SysCtlDelay(1000);
	//Set all outputs to 0
	GPIO_PORTD_DATA_R = 0;
	GPIO_PORTC_DATA_R = 0;

	//Hold CLEAR pins high for LED matrix
	row_mr_high();
	col_mr_high();

	while(1)
	{
		ROM_PWMGenDisable(PWM1_BASE, PWM_GEN_1);
		//ROM_PWMGenDisable(PWM1_BASE, PWM_GEN_2);
		while(joyLY > 200)
		{
			UARTSend((uint8_t *)"MSE Rocks !!!\n\r", 16);
			//UARTprintf("MSE Rocks !!!  \nHold Left joystick up enter free scrolling mode. \n");
			for(i = -35; i <= 35; i++)
			{
				for(j = 0; j < 3; j++)
				{
					updateMatrix(mse_rocks, i, 0);
				}
			}
		}

		//UARTprintf("Use the left joystick to move the text\n Hold both joysticks up to play pong. \n");
		while(joyLY > 200 || joyRY > 200)

		{
			if((joyLX > 3000) && (x_shift > -27)) 		x_shift --;
			else if((joyLX < 1000) && (x_shift < 27))	x_shift ++;
			if((joyLY > 3000) && (y_shift < 13)) 		y_shift ++;
			else if((joyLY < 1000) && (y_shift > -13))	y_shift --;
			updateMatrix(mse_rocks, x_shift, y_shift);
		}

		score_r = score_l = 0;
		//UARTprintf("Score  %u:%u\n", score_l, score_r);
		ROM_PWMGenEnable(PWM1_BASE, PWM_GEN_1);
		//ROM_PWMGenEnable(PWM1_BASE, PWM_GEN_2);
		while((joyLX > 200 || joyRX > 200))
		{
			pong();
			updateMatrix(pong_screen, 0, 13);
		}

	}

}


void ADC_0_Int_isr (void)
{
	ADCSequenceDataGet(ADC0_BASE, 0, ui32ADCoutput);    // Read the value from the ADC. 0-RX 1-RY 2-LX 3-LY
	joyLX = ui32ADCoutput[2];
	joyLY = ui32ADCoutput[3];
	joyRX = ui32ADCoutput[0];
	joyRY = ui32ADCoutput[1];


	ADCIntClear(ADC0_BASE, 0);
}


void rgb_led(uint8_t pins)
{
	GPIOPinWrite(GPIO_PORTF_BASE, RED_LED|BLUE_LED|GREEN_LED, pins);
}


void blink_green_3_times(void)
{
    i = 0;
	while(i < 3)
    {
    	i++;
    	rgb_led(LED_GREEN);
    	SysCtlDelay(2000000);
    	rgb_led(LED_OFF);
    	SysCtlDelay(2000000);
    }
}

uint32_t micros (void)
{
	uint32_t temp = (1000 - (ROM_SysTickValueGet()/50));
	return ((millis*1000) + temp);
}

void SycTickInt_isr()
{
	millis++;
}

void pong (void)
{
	uint32_t row = 0;
	//Clear screen
	for(row = 0; row <= 16; row ++)
	{
		pong_screen[row] = 0x00000000;
	}
	//Draw left pad
	for(row = (uint32_t)pong_l_pos; row <= (uint32_t)pong_l_pos + 4; row ++)
	{
		pong_screen[row] |= 0x80000000;
	}
	//Draw right pad
	for(row = (uint32_t)pong_r_pos; row <= (uint32_t)pong_r_pos + 4; row ++)
	{
		pong_screen[row] |= 0x00000001;
	}
	//Draw ball
	pong_screen[(uint32_t)pong_y_ball] |= (0x80000000 >> (uint32_t)pong_x_ball);

	pong_x_ball = pong_x_ball + pong_x_vel;
	pong_y_ball = pong_y_ball + pong_y_vel;

	if (pong_y_ball <= 0 || pong_y_ball >= 15)
		{
			pong_y_vel = -1*pong_y_vel;
		}

	if (pong_x_ball <= 0)
	{
		if (pong_y_ball >= pong_l_pos && pong_y_ball <= (pong_l_pos + 4))
		{
			pong_x_vel = -1*pong_x_vel;
		}
		else
		{
			score_r ++;
			printscore();
			pong_x_ball = 16;
		}
	}
	if (pong_x_ball >= 31)
	{
		if(pong_y_ball >= pong_r_pos && pong_y_ball <= (pong_r_pos + 4))
		{
			pong_x_vel = -1*pong_x_vel;
		}
		else
		{
			score_l ++;
			printscore();
			pong_x_ball = 16;
		}
	}

	if(score_l >= 11)
	{
		score_l = score_r = 0;
		UARTSend((uint8_t *)"Left player wins!\n\r", 19);
		SysCtlDelay(20000000);


	}else if (score_r >= 11)
	{
		score_l = score_r = 0;
		UARTSend((uint8_t *)"Right player wins!\n\r", 20);
		SysCtlDelay(20000000);
	}

	if((joyLY < 1000) && (pong_l_pos > 0)) 			pong_l_pos -= 0.3;
	else if((joyLY > 3000) && (pong_l_pos < 11)) 	pong_l_pos += 0.3;
	if((joyRY < 1000) && (pong_r_pos > 0)) 			pong_r_pos -= 0.3;
	else if((joyRY > 3000) && (pong_r_pos < 11))	pong_r_pos += 0.3;

	uint8_t duty_cycle_r = 0;
	if ((uint32_t)pong_r_pos <= 0) duty_cycle_r = 112;
	else if ((uint32_t)pong_r_pos == 1) duty_cycle_r = 105;
	else if ((uint32_t)pong_r_pos == 2) duty_cycle_r = 100;
	else if ((uint32_t)pong_r_pos == 3) duty_cycle_r = 95;
	else if ((uint32_t)pong_r_pos == 4) duty_cycle_r = 90;
	else if ((uint32_t)pong_r_pos == 5) duty_cycle_r = 85;
	else if ((uint32_t)pong_r_pos == 6) duty_cycle_r = 80;
	else if ((uint32_t)pong_r_pos == 7) duty_cycle_r = 75;
	else if ((uint32_t)pong_r_pos == 8) duty_cycle_r = 70;
	else if ((uint32_t)pong_r_pos == 9) duty_cycle_r = 65;
	else if ((uint32_t)pong_r_pos == 10) duty_cycle_r = 60;
	else if ((uint32_t)pong_r_pos >= 11) duty_cycle_r = 57;
	else duty_cycle_r = 75;
	ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_2, duty_cycle_r * 15624 / 1000); //15624 is the pwm period in clock cycles for 50Hz PWM

	uint8_t duty_cycle_l = 0;
	if ((uint32_t)pong_l_pos >= 11) duty_cycle_l = 105;
	else if ((uint32_t)pong_l_pos == 10) duty_cycle_l = 100;
	else if ((uint32_t)pong_l_pos == 9) duty_cycle_l = 95;
	else if ((uint32_t)pong_l_pos == 8) duty_cycle_l = 90;
	else if ((uint32_t)pong_l_pos == 7) duty_cycle_l = 85;
	else if ((uint32_t)pong_l_pos == 6) duty_cycle_l = 80;
	else if ((uint32_t)pong_l_pos == 5) duty_cycle_l = 75;
	else if ((uint32_t)pong_l_pos == 4) duty_cycle_l = 75;
	else if ((uint32_t)pong_l_pos == 3) duty_cycle_l = 70;
	else if ((uint32_t)pong_l_pos == 2) duty_cycle_l = 65;
	else if ((uint32_t)pong_l_pos == 1) duty_cycle_l = 60;
	else if ((uint32_t)pong_l_pos <= 0) duty_cycle_l = 57;
	else duty_cycle_l = 75;
	ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_3, duty_cycle_l * 15624 / 1000); //15624 is the pwm period in clock cycles for 50Hz PWM

}

void printscore(void)
{
	uint32_t sl1 = 0;
	uint32_t sl2 = 0;
	uint32_t sr1 = 0;
	uint32_t sr2 = 0;
	sl1 = (score_l/10) + 48;
	sl2 = (score_l%10) + 48;
	sr1 = (score_r/10) + 48;
	sr2 = (score_r%10) + 48;
	UARTCharPutNonBlocking(UART0_BASE, 83); //S
	UARTCharPutNonBlocking(UART0_BASE, 99); //c
	UARTCharPutNonBlocking(UART0_BASE, 111);//o
	UARTCharPutNonBlocking(UART0_BASE, 114);//r
	UARTCharPutNonBlocking(UART0_BASE, 101);//e
	UARTCharPutNonBlocking(UART0_BASE, 32);// space
	UARTCharPutNonBlocking(UART0_BASE, sl1);
	UARTCharPutNonBlocking(UART0_BASE, sl2);
	UARTCharPutNonBlocking(UART0_BASE, 58) ; //Colon
	UARTCharPutNonBlocking(UART0_BASE, sr1);
	UARTCharPutNonBlocking(UART0_BASE, sr2);
	UARTCharPutNonBlocking(UART0_BASE, 10); //line feed
	UARTCharPutNonBlocking(UART0_BASE, 13); //carriage return
}

void UARTSend(const uint8_t *pui8Buffer, uint32_t ui32Count)
{
    //
    // Loop while there are more characters to send.
    //
    while(ui32Count--)
    {
        //
        // Write the next character to the UART.
        //
        ROM_UARTCharPut(UART0_BASE, *pui8Buffer++);
    }
}

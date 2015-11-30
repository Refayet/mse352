/*
 * initialization.c
 *
 *  Created on: 2015-10-19
 *      Author: Kirill
 */

#include "mainheader.h"

#define PWM_FREQUENCY 50


// System clock rate in Hz.
extern uint32_t g_ui32SysClock;


//Initialise all the stuff
void initializeAll(void)
{

    // Enable lazy stacking for interrupt handlers.  This allows floating-point
    // instructions to be used within interrupt handlers, but at the expense of
    // extra stack usage.
    ROM_FPUEnable();
    ROM_FPULazyStackingEnable();

    // Setup the system clock to run at 50 Mhz from PLL with crystal reference
    SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|
                    SYSCTL_OSC_MAIN);
    g_ui32SysClock = SysCtlClockGet(); //this will hold the value of clock speed

    SysCtlDelay(g_ui32SysClock/5);

	initializeGPIO();
	initializeUART();
	initializePWM();

	// Set up and enable the SysTick timer.  It will be used as a reference
	// for delay loops in the interrupt handlers.  The SysTick timer period
	// will be set up for 1000 times per second.
	ROM_SysTickPeriodSet(g_ui32SysClock / 1000);
	SysTickIntRegister(SycTickInt_isr);
	SysTickIntEnable();

	initializeADC();

	// Enable interrupts to the processor.
	IntMasterEnable();
	//ROM_IntMasterEnable();
}


// Configure the UART and its pins. This must be called before UARTprintf().

void initializeUART(void)
{
    // Enable UART0.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    //ROM_SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_UART0);

    // Enable the GPIO Peripheral used by the UART.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    //ROM_SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_GPIOA);

    ROM_IntMasterEnable();

    // Configure GPIO Pins for UART mode.
    //FROM UART ECHO (works for Helloworld as well)
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //FROM HELLO WORLD EXAMPLE (for UARTprintf functionality)
    // Initialize the UART for console I/O.

    //UARTStdioConfig(0, 115200, g_ui32SysClock);

    //FROM UART ECHO EXAMPLE (for ROM_UART functionality)
    // Configure the UART for 115,200, 8-N-1 operation.
    //
    ROM_UARTConfigSetExpClk(UART0_BASE, ROM_SysCtlClockGet(), 115200,
                           (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                            UART_CONFIG_PAR_NONE));
    // Enable the UART interrupt.
    //
    ROM_IntEnable(INT_UART0);
    ROM_UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);

}


void initializeGPIO(void)
{

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE); //For ADC

	//LED matrix pins
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD); //For LED matrix
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC); //For LED matrix
	GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_7|GPIO_PIN_6|GPIO_PIN_5|GPIO_PIN_4);
	GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

	//LED
	// FROM "BLINKY" EXAMPLE FOR 123GXL
    // Enable the GPIO port that is used for the on-board LED.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
    // Delay for a bit after peripheral enable
    SysCtlDelay(g_ui32SysClock / (5));
}


void initializeADC(void)
{

	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	//
	// Enable the first sample sequencer to capture the value of channel 0 when
	// the processor trigger occurs.
	//
	ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_TIMER, 0);
	ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH0); //Read PE3
	ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ADC_CTL_CH1); //Read PE2
	ADCSequenceStepConfigure(ADC0_BASE, 0, 2, ADC_CTL_CH2); //Read PE1
	ADCSequenceStepConfigure(ADC0_BASE, 0, 3, ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH3); //Read PE0

	ADCIntRegister(ADC0_BASE, 0, ADC_0_Int_isr);

	ADCIntEnable(ADC0_BASE, 0);
	ADCSequenceEnable(ADC0_BASE, 0);

	//Initialise timer to trigger ADC every 2ms
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
	uint32_t ui32Period = 0;
	ui32Period = g_ui32SysClock / 500; //Period = 1/500s
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1);
	TimerControlTrigger(TIMER0_BASE, TIMER_A, true);  //Enable ADC trigger for timer
	TimerEnable(TIMER0_BASE, TIMER_A);
}

void initializePWM (void)
{
	volatile uint32_t ui32Load;
	volatile uint32_t ui32PWMClock;
	volatile uint8_t ui8Adjust;

	ui8Adjust = 75;
	ROM_SysCtlPWMClockSet(SYSCTL_PWMDIV_64);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
	//ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);

	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	ROM_GPIOPinTypePWM(GPIO_PORTE_BASE, GPIO_PIN_4);
	ROM_GPIOPinTypePWM(GPIO_PORTA_BASE, GPIO_PIN_7);

	ROM_GPIOPinConfigure(GPIO_PE4_M1PWM2);
	ROM_GPIOPinConfigure(GPIO_PA7_M1PWM3);

	ui32PWMClock = SysCtlClockGet() / 64;
	ui32Load = (ui32PWMClock / PWM_FREQUENCY) - 1;
	PWMGenConfigure(PWM1_BASE, PWM_GEN_1, PWM_GEN_MODE_DOWN);
	//PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_1, ui32Load);
	//PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, ui32Load);
	ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_2, ui8Adjust * ui32Load / 1000);
	ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_3, ui8Adjust * ui32Load / 1000);
	ROM_PWMOutputState(PWM1_BASE, PWM_OUT_2_BIT, true);
	ROM_PWMOutputState(PWM1_BASE, PWM_OUT_3_BIT, true);
	ROM_PWMGenEnable(PWM1_BASE, PWM_GEN_1);
	//ROM_PWMGenEnable(PWM1_BASE, PWM_GEN_2);

}





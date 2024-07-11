#pragma arm section zidata="non_initialized"
#pragma anon_unions

#ifdef DEBUG
#pragma no_inline
#define INLINE
#else
#pragma inline
#define INLINE				__forceinline
#endif

#include "common/LPC1100L.h"
#include "common/util.h"
#include "config.h"

#ifndef USING_XTAL12
#define USING_XTAL12		0
#endif
#ifndef USING_WDT
#define USING_WDT			0
#endif
#ifndef USING_TIMER
#define USING_TIMER			0
#endif
#ifndef USING_SPI
#define USING_SPI			0
#endif
#ifndef USING_I2C
#define USING_I2C			0
#endif
#ifndef USING_UART
#define USING_UART			0
#endif
#ifndef USING_ADC
#define USING_ADC			0
#endif
#ifndef USING_GPIO_BUTTON
#define USING_GPIO_BUTTON	0
#endif
#ifndef USING_T2_IR
#define USING_T2_IR			0
#endif
#ifndef USING_T1_IR2
#define USING_T1_IR2		0
#endif
#ifndef WALLCLOCK_T3
#define WALLCLOCK_T3		0
#endif
#ifndef SYNCHRONIZED_FEED
#define SYNCHRONIZED_FEED	0
#endif

#ifndef ADD_GPIO0_INTERRUPT
#define ADD_GPIO0_INTERRUPT	0
#endif
#ifndef ADD_GPIO1_INTERRUPT
#define ADD_GPIO1_INTERRUPT	0
#endif
#ifndef ADD_OTHER_INTERRUPT
#define ADD_OTHER_INTERRUPT	0
#endif

#ifndef PRIORITY_SVC
#define PRIORITY_SVC		0
#endif
#ifndef PRIORITY_PendSV
#define PRIORITY_PendSV		0
#endif
#ifndef PRIORITY_SysTick
#define PRIORITY_SysTick	0
#endif
#ifndef PRIORITY_TIMER16_0
#define PRIORITY_TIMER16_0	0
#endif
#ifndef PRIORITY_TIMER16_1
#define PRIORITY_TIMER16_1	0
#endif
#ifndef PRIORITY_TIMER32_0
#define PRIORITY_TIMER32_0	0
#endif
#ifndef PRIORITY_TIMER32_1
#define PRIORITY_TIMER32_1	0
#endif
#ifndef PRIORITY_I2C
#define PRIORITY_I2C		0
#endif
#ifndef PRIORITY_UART
#define PRIORITY_UART		0
#endif
#ifndef PRIORITY_ADC
#define PRIORITY_ADC		0
#endif
#ifndef PRIORITY_PIOINT0
#define PRIORITY_PIOINT0	0
#endif
#ifndef PRIORITY_PIOINT1
#define PRIORITY_PIOINT1	0
#endif
#ifndef PRIORITY_PIOINT2
#define PRIORITY_PIOINT2	0
#endif
#ifndef PRIORITY_PIOINT3
#define PRIORITY_PIOINT3	0
#endif

#if USING_TIMER
#if WALLCLOCK_T3
#define wallclock			((int32_t)TMR32B0TC)
#else
#define wallclock			((int16_t)TMR16B0TC)
#endif
#endif

#if USING_WDT
#ifndef WDT_RELOAD
#define WDT_RELOAD			1199999	//time-out after 0.1s
#endif
INLINE void _feedWatchDog(){
#if SYNCHRONIZED_FEED
	SEI();
#endif
	WDFEED=0xAA;
	WDFEED=0x55;				//feed watch dog
#if SYNCHRONIZED_FEED
	CLI();
#endif
}
#if SYNCHRONIZED_FEED
#define feedWatchDog()		_feedWatchDog()
#endif
#endif

#if USING_SPI
#ifndef SPI_WORDLEN
#define SPI_WORDLEN			8
#endif
#ifndef SPI_FREQDIV
#define SPI_FREQDIV			4
#endif
#endif

#if USING_GPIO_BUTTON
#include "common/button_h.h"
#endif
#if USING_T2_IR||USING_T1_IR2
#include "common/ir_h.h"
#endif
#if USING_I2C
#include "common/i2c_h.h"
#endif
#if USING_UART
#include "common/uart_h.h"
#endif
#if USING_ADC
#include "common/adc_h.h"
#endif

#if USING_GPIO_BUTTON
#include "common/button_c.h"
#endif
#if USING_T2_IR||USING_T1_IR2
#include "common/ir_c.h"
#endif
#if USING_I2C
#include "common/i2c_c.h"
#endif
#if USING_UART
#include "common/uart_c.h"
#endif
#if USING_ADC
#include "common/adc_c.h"
#endif

//user code
#include "main.h"

void Reset_Handler(){
//BOD settings
	BODCTRL=0x11;				//set BOD reset at 2.06~2.15V
//clock settings
#if USING_XTAL12
	PDRUNCFG&=~PDRUNCFG_SYSOSC;	//XTAL power-on
	SYSPLLCLKSEL=0x01;			//select PLL clock source to XTAL
	SYSPLLCLKUEN=0x01;
	SYSPLLCLKUEN=0x00;
	SYSPLLCLKUEN=0x01;			//update PLL clock source
#endif
	SYSPLLCTRL=0x23;			//set PLL to 48MHz(12MHz*4)
	PDRUNCFG&=~PDRUNCFG_SYSPLL;	//system PLL power-on
	while (!(SYSPLLSTAT&0x01));	//wait until PLL lock
	MAINCLKSEL=0x03;			//select main clock source to PLL output
	MAINCLKUEN=0x01;
	MAINCLKUEN=0x00;
	MAINCLKUEN=0x01;			//update main clock source
	//SYSAHBCLKDIV=0x01;		//set system clock to 48MHz(48MHz/1)
								//enable clock for peripherals
	SYSAHBCLKCTRL=	SYSAHBCLKCTRL_BASE
#if USING_WDT
					|SYSAHBCLKCTRL_WDT
#endif
#if USING_TIMER
					|SYSAHBCLKCTRL_TIMER
#endif
#if USING_I2C
					|SYSAHBCLKCTRL_I2C
#endif
#if USING_SPI
					|SYSAHBCLKCTRL_SSP0
#endif
#if USING_UART
					|SYSAHBCLKCTRL_UART
#endif
#if USING_ADC
					|SYSAHBCLKCTRL_ADC
#endif
				;
#if USING_WDT
	WDTCLKSEL=0x01;				//select WDT clock source to main clock
	WDTCLKUEN=0x01;
	WDTCLKUEN=0x00;
	WDTCLKUEN=0x01;				//update WDT clock source
	WDTCLKDIV=0x01;				//set WDT clock to 12MHz(48MHz/4/1)
#endif
#if USING_XTAL12
	PDRUNCFG|=PDRUNCFG_IRC;		//internal clock power-off
#endif
#if USING_I2C||USING_SPI
	PRESETCTRL=	0
#if USING_I2C
					|PRESETCTRL_I2C
#endif
#if USING_SPI
					|PRESETCTRL_SSP0
#endif
				;
#endif
#if USING_SPI
	SSP0CLKDIV=0x01;			//set SPI clock to 48MHz(48MHz/1)
#endif
#if USING_UART
	UARTCLKDIV=0x01;			//set UART clock to 48MHz(48MHz/1)
#endif
#if USING_ADC
	PDRUNCFG&=~PDRUNCFG_ADC;	//ADC power-on
#endif
//IO port settings
//previous error handling
	ioport_init();
//interrupt settings
#if ADD_OTHER_INTERRUPT||ADD_GPIO0_INTERRUPT||ADD_GPIO1_INTERRUPT||USING_TIMER||USING_I2C||USING_UART||USING_ADC
								//enable interrupts
	ISER=ADD_OTHER_INTERRUPT
#if USING_TIMER
			|ISER_TIMER16_0
			|ISER_TIMER16_1
			|ISER_TIMER32_0
			|ISER_TIMER32_1
#endif
#if USING_I2C
			|ISER_I2C
#endif
#if USING_UART
			|ISER_UART
#endif
#if USING_ADC
			|ISER_ADC
#endif
#if ADD_GPIO0_INTERRUPT
			|ISER_PIOINT0
#endif
#if ADD_GPIO1_INTERRUPT
			|ISER_PIOINT1
#endif
		;
#endif
//set priority
#if PRIORITY_SVC
	SHPR2=(uint32_t)PRIORITY_SVC<<SHPR2_SVC;
#endif
#if PRIORITY_PendSV||PRIORITY_SysTick
	SHPR3=0
			|(uint32_t)PRIORITY_PendSV<<SHPR3_PendSV
			|(uint32_t)PRIORITY_SysTick<<SHPR3_SysTick
	;
#endif
#if PRIORITY_TIMER16_0||PRIORITY_TIMER16_1||PRIORITY_TIMER32_0||PRIORITY_TIMER32_1
	IPR4=0
			|(uint32_t)PRIORITY_TIMER16_0<<IPR4_TIMER16_0
			|(uint32_t)PRIORITY_TIMER16_1<<IPR4_TIMER16_1
			|(uint32_t)PRIORITY_TIMER32_0<<IPR4_TIMER32_0
			|(uint32_t)PRIORITY_TIMER32_1<<IPR4_TIMER32_1
	;
#endif
#if PRIORITY_I2C
	IPR3=(uint32_t)PRIORITY_I2C<<IPR3_I2C;
#endif
#if PRIORITY_UART
	IPR5=(uint32_t)PRIORITY_UART<<IPR5_UART;
#endif
#if PRIORITY_ADC
	IPR6=(uint32_t)PRIORITY_ADC<<IPR6_ADC;
#endif
#if PRIORITY_PIOINT0||PRIORITY_PIOINT1||PRIORITY_PIOINT2||PRIORITY_PIOINT3
	IPR7=0
			|(uint32_t)PRIORITY_PIOINT0<<IPR7_PIOINT0
			|(uint32_t)PRIORITY_PIOINT1<<IPR7_PIOINT1
			|(uint32_t)PRIORITY_PIOINT2<<IPR7_PIOINT2
			|(uint32_t)PRIORITY_PIOINT3<<IPR7_PIOINT3
	;
#endif
//WDT settings
#if USING_WDT
	WDTC=WDT_RELOAD;			//time-out by config
	WDMOD=0x03;					//enanble WDT (start on first feed)
#endif
//wall clock settings
#if USING_TIMER
#if WALLCLOCK_T3
	TMR32B0PR=7999;				//set period 8000(6000Hz)
	TMR32B0TCR=0x01;			//start counter
#else
	TMR16B0PR=7999;				//set period 8000(6000Hz)
	TMR16B0TCR=0x01;			//start counter
#endif
#endif
//SPI settings
#if USING_SPI
	SSP0CR0=SPI_WORDLEN-1;		//set SPI data length
	SSP0CPSR=SPI_FREQDIV;		//set SCK frequency
	SSP0CR1=0x02;				//enable SPI
#endif

/*** idle process start ***/

#if USING_GPIO_BUTTON
	button_init();
#endif
#if USING_T2_IR||USING_T1_IR2
	ir_init();
#endif
#if USING_I2C
	i2c_init();
#endif
#if USING_UART
	uart_init();
#endif
#if USING_ADC
	adc_init();
#endif
	init();

	for (;;){//main loop
#if USING_WDT
		_feedWatchDog();
#endif
#if USING_GPIO_BUTTON
		button_idle();
#endif
#if USING_T2_IR||USING_T1_IR2
		ir_idle();
#endif
#if USING_I2C
		i2c_idle();
#endif
#if USING_UART
		uart_idle();
#endif
#if USING_ADC
		adc_idle();
#endif
		idle();
	}
}

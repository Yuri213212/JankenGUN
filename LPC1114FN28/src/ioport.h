#define P0_BUTTON1			0x002
#define P0_BUTTON2			0x008
#define P0_MO				0x080
#define P0_LED1A			0x100
#define P0_LED1B			0x200
#define P0_LED2A			0x400
#define P0_LED2B			0x800
#define P1_LEDr				0x200

#define P0_ALL				0x00A
#define P0_DOWNST			0x001

//IO port settings
INLINE void ioport_init(){
	//IOCON_RESET_PIO0_0=	IOCON_A0_RESET|	IOCON_DIGITAL|IOCON_PU;				//pulled up input for reset
	IOCON_PIO0_1=			IOCON_A1_GPIO	|IOCON_DIGITAL|IOCON_PU|IOCON_HYS;	//pulled up input for button 1
	IOCON_PIO0_2=			IOCON_A2_T1C	|IOCON_DIGITAL|IOCON_PU;			//pulled up input for IR2
	IOCON_PIO0_3=			IOCON_A3_GPIO	|IOCON_DIGITAL|IOCON_PU|IOCON_HYS;	//pulled up input for button 2
	IOCON_PIO0_4=			IOCON_A4_SCL	|IOCON_I2CFAST;						//open drain io for I2C_SCL
	IOCON_PIO0_5=			IOCON_A5_SDA	|IOCON_I2CFAST;						//open drain io for I2C_SDA
	IOCON_PIO0_6=			IOCON_A6_GPIO	|IOCON_DIGITAL|IOCON_PU|IOCON_HYS;	//pulled up input for SPI+sw
	IOCON_PIO0_7=			IOCON_A7_GPIO	|IOCON_DIGITAL;						//standard output for monitor
	IOCON_PIO0_8=			IOCON_A8_GPIO	|IOCON_DIGITAL;						//standard output for LED1A
	IOCON_PIO0_9=			IOCON_A9_GPIO	|IOCON_DIGITAL;						//standard output for LED1B
	IOCON_SWCLK_PIO0_10=	IOCON_A10_GPIO	|IOCON_DIGITAL;						//standard output for LED2A
	IOCON_R_PIO0_11=		IOCON_A11_GPIO	|IOCON_DIGITAL;						//standard output for LED2B
	IOCON_R_PIO1_0=			IOCON_B0_AD1	|IOCON_ANALOG;						//analog input for AD1
	IOCON_R_PIO1_1=			IOCON_B1_AD2	|IOCON_ANALOG;						//analog input for AD2
	IOCON_R_PIO1_2=			IOCON_B2_T41	|IOCON_DIGITAL;						//standard output for PWMch1
	IOCON_SWDIO_PIO1_3=		IOCON_B3_T42	|IOCON_DIGITAL;						//standard output for PWMch2
	IOCON_PIO1_4=			IOCON_B4_T43	|IOCON_DIGITAL;						//standard output for PWMch3
	IOCON_PIO1_5=			IOCON_B5_GPIO	|IOCON_DIGITAL;						//standard output for buzzer
	IOCON_PIO1_6=			IOCON_B6_RXD	|IOCON_DIGITAL|IOCON_PU;			//pulled up input for UART_RXD
	IOCON_PIO1_7=			IOCON_B7_TXD	|IOCON_DIGITAL;						//standard output for UART_TXD
	IOCON_PIO1_8=			IOCON_B8_T2C	|IOCON_DIGITAL|IOCON_PU;			//pulled up input for IR
	IOCON_PIO1_9=			IOCON_B9_GPIO	|IOCON_DIGITAL|IOCON_OD;			//open drain output for red LED
	GPIO0DIR=0xFB0;
	GPIO1DIR=0x2BC;				//set IO direction
	GPIO0DATA[0xFB0]=0xF10;
	GPIO1DATA[0x2BC]=0x2BC;		//init output
#if USING_WDT
//previous error handling
	if (SYSRSTSTAT&0x04){		//reset by WDT
		GPIO1DATA[P1_LEDr]=0;	//red LED on
		for (;;);				//halt
	}
#endif
}

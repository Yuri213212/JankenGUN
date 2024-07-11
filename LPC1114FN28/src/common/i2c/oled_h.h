//device 0x78	SSD1306		128*64 OLED dot matrix screen
//device 0x7A	SSD1306		128*64 OLED dot matrix screen

#if USING_I2C_OLED
#define I2COLED_W			0x78
#endif
#if USING_I2C_OLED2
#define I2COLED2_W			0x7A
#endif

#define I2COLED_CMD_INS		0x00
#define I2COLED_CMD_DATA	0x40
#define I2COLED_CMD_1INS	0x80
#define I2COLED_CMD_1DATA	0xC0

#define I2COLED_CONTRAST	0x81	//need 1 byte of data (def=0x7F)
#define I2COLED_NOTTEST		0xA4	//normal mode (def)
#define I2COLED_TEST		0xA5	//full screen test
#define I2COLED_NOTINVERSE	0xA6	//normal mode (def)
#define I2COLED_INVERSE		0xA7	//set background to white and forground to black
#define I2COLED_NOTDISPLAY	0xAE	//disable output (def)
#define I2COLED_DISPLAY		0xAF	//enable output

#define I2COLED_ADDRMODE	0x20	//need 1 byte of data (def=0x02)
#define I2COLED_ADDR_LO		0x00	//last 4 bit is data
#define I2COLED_ADDR_HI		0x10	//last 4 bit is data
#define I2COLED_ADDR_ROW	0xB0	//last 3 bit is data

#define I2COLED_FIRSTLINE	0x40	//last 6 bit is data (def=0x00)
#define I2COLED_NOTHFLIP	0xA0	//normal mode (def)
#define I2COLED_HFLIP		0xA1	//flip horizontally
#define I2COLED_NOTSREVERSE	0xC0	//normal mode (def)
#define I2COLED_SREVERSE	0xC8	//scan from bottom to top
#define I2COLED_VSCROLL		0xD3	//need 1 byte of data (def=0x00)
#define I2COLED_FREQ		0xD5	//need 1 byte of data (def=0x80)//F0 for max frequency
#define I2COLED_CHARGE		0xD9	//need 1 byte of data (def=0x22)
#define I2COLED_NOP			0xE3

#define I2COLED_POWER		0x8D	//need 1 byte of data (def=0x10)//10=off,14=on

#define OLED_FONT_PENDING	0x80
#define OLED_FONT_INV		0x40
#define OLED_FONT_ICO		0x01
#define OLED_FONT_8H		0x02
#define OLED_FONT_8C		0x03
#define OLED_FONT_16HU		0x04
#define OLED_FONT_16HD		0x05
#define OLED_FONT_16CU		0x06
#define OLED_FONT_16CD		0x07
#define OLED_FONT_16H		0x0D
#define OLED_FONT_16C		0x0F

#if USING_I2C_OLED
const uint8_t i2c_oled_reset[]={
	I2COLED_W,I2CCMD_DATA,5,
		I2COLED_CMD_INS,
		I2COLED_FREQ,0xF0,
		I2COLED_POWER,0x14,
		I2COLED_DISPLAY,
	I2CCMD_END
};
#endif
#if USING_I2C_OLED2
const uint8_t i2c_oled2_reset[]={
	I2COLED2_W,I2CCMD_DATA,5,
		I2COLED_CMD_INS,
		I2COLED_FREQ,0xF0,
		I2COLED_POWER,0x14,
		I2COLED_DISPLAY,
	I2CCMD_END
};
#endif

#if USING_I2C_OLED
int8_t i2c_oled_refresh,i2c_oled_i;
uint8_t i2c_oled_out[21];
union REG16 i2c_oled_outbuf[128];
#endif
#if USING_I2C_OLED2
int8_t i2c_oled2_refresh,i2c_oled2_i;
uint8_t i2c_oled2_out[21];
union REG16 i2c_oled2_outbuf[128];
#endif

#if USING_I2C_OLED
INLINE void oled_char2buf_small(uint8_t font,uint8_t i,uint8_t c){
	i2c_oled_outbuf[i].B0=c;
	i2c_oled_outbuf[i].B1=OLED_FONT_PENDING|font;
}

INLINE void oled_char2buf_big(uint8_t font,uint8_t i,uint8_t c){
	register int temp;

	temp=((i<<1)&0x60)|(i&0x0F);
	i2c_oled_outbuf[temp].B0=c;
	i2c_oled_outbuf[temp].B1=OLED_FONT_PENDING|(font&~0x09);
	temp|=0x10;
	i2c_oled_outbuf[temp].B0=c;
	i2c_oled_outbuf[temp].B1=OLED_FONT_PENDING|(font&~0x08);
}
#endif

#if USING_I2C_OLED2
INLINE void oled2_char2buf_small(uint8_t font,uint8_t i,uint8_t c){
	i2c_oled2_outbuf[i].B0=c;
	i2c_oled2_outbuf[i].B1=OLED_FONT_PENDING|font;
}

INLINE void oled2_char2buf_big(uint8_t font,uint8_t i,uint8_t c){
	register int temp;

	temp=((i<<1)&0x60)|(i&0x0F);
	i2c_oled2_outbuf[temp].B0=c;
	i2c_oled2_outbuf[temp].B1=OLED_FONT_PENDING|(font&~0x09);
	temp|=0x10;
	i2c_oled2_outbuf[temp].B0=c;
	i2c_oled2_outbuf[temp].B1=OLED_FONT_PENDING|(font&~0x08);
}
#endif

//user code
#include "../../i2c_oled.h"

#ifndef USING_OLED_FONT_16H
#define USING_OLED_FONT_16H	0
#endif
#ifndef USING_OLED_FONT_16C
#define USING_OLED_FONT_16C	0
#endif
#ifndef USING_OLED_FONT_8H
#define USING_OLED_FONT_8H	0
#endif
#ifndef USING_OLED_FONT_8C
#define USING_OLED_FONT_8C	0
#endif
#ifndef USING_OLED_FONT_ICO
#define USING_OLED_FONT_ICO	0
#endif

#include "oled_char.h"

#if USING_OLED_FONT_ICO
//user code
#include "../../i2c_oled_ico.h"
#endif

#if USING_I2C_OLED
/*
I2COLED_W,I2CCMD_DATA,6,
	I2COLED_CMD_1INS,<I2COLED_ADDR_LO>,
	I2COLED_CMD_1INS,<I2COLED_ADDR_HI>,
	I2COLED_CMD_1INS,<I2COLED_ADDR_ROW>,
	I2COLED_CMD_DATA,
I2CCMD_DATA,7,
	<0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77>,
I2CCMD_END
*/
INLINE void oled_printblock(int8_t pos,union REG16 data){
	register int i,inv;

	i2c_oled_out[4]=I2COLED_ADDR_LO|((pos<<3)&0x08);
	i2c_oled_out[6]=I2COLED_ADDR_HI|((pos>>1)&0x07);
	i2c_oled_out[8]=I2COLED_ADDR_ROW|((pos>>4)&0x07);
	inv=data.B1&OLED_FONT_INV
		?0xFF
		:0x00
	;
	switch (data.B1&~OLED_FONT_INV){
#if USING_OLED_FONT_16H
	case OLED_FONT_16HU:
		for (i=0;i<8;++i){
			i2c_oled_out[i+12]=oled_hex16[0][((data.B0&0xF0)>>1)|i]^inv;
		}
		break;
	case OLED_FONT_16HD:
		for (i=0;i<8;++i){
			i2c_oled_out[i+12]=oled_hex16[1][((data.B0&0x0F)<<3)|i]^inv;
		}
		break;
#endif
#if USING_OLED_FONT_16C
	case OLED_FONT_16CU:
		for (i=0;i<8;++i){
			i2c_oled_out[i+12]=oled_char16[0][(data.B0<<3)|i]^inv;
		}
		break;
	case OLED_FONT_16CD:
		for (i=0;i<8;++i){
			i2c_oled_out[i+12]=oled_char16[1][(data.B0<<3)|i]^inv;
		}
		break;
#endif
#if USING_OLED_FONT_8H
	case OLED_FONT_8H:
		for (i=0;i<4;++i){
			i2c_oled_out[i+12]=oled_hex8[0][((data.B0&0xF0)>>2)|i]^inv;
		}
		for (i=0;i<4;++i){
			i2c_oled_out[i+16]=oled_hex8[1][((data.B0&0x0F)<<2)|i]^inv;
		}
		break;
#endif
#if USING_OLED_FONT_8C
	case OLED_FONT_8C:
		for (i=0;i<8;++i){
			i2c_oled_out[i+12]=oled_char8[(data.B0<<3)|i]^inv;
		}
		break;
#endif
#if USING_OLED_FONT_ICO
	case OLED_FONT_ICO:
		for (i=0;i<8;++i){
			i2c_oled_out[i+12]=oled_ico8[(data.B0<<3)|i]^inv;
		}
		break;
#endif
	default:
		for (i=12;i<20;++i){
			i2c_oled_out[i]=0xFF;
		}
	}
}

INLINE void i2c_oled_init(struct i2ctransaction *control){
	register int i;

	i2c_oled_refresh=-1;
	i2c_oled_i=0;
	i2c_oled_out[0]=I2COLED_W;
	i2c_oled_out[1]=I2CCMD_DATA;
	i2c_oled_out[2]=6;
	i2c_oled_out[3]=I2COLED_CMD_1INS;
	i2c_oled_out[5]=I2COLED_CMD_1INS;
	i2c_oled_out[7]=I2COLED_CMD_1INS;
	i2c_oled_out[9]=I2COLED_CMD_DATA;
	i2c_oled_out[10]=I2CCMD_DATA;
	i2c_oled_out[11]=7;
	i2c_oled_out[20]=I2CCMD_END;
	for (i=0;i<I2COLED_INITLEN;++i){
		oled_char2buf(I2COLED_INITFONT,i,i2c_oled_initdata[i]);
	}
	i2c_oled_initbufferex();

	control->state=0x81;
	control->cmd=i2c_oled_reset;
}

INLINE void i2c_oled_idle(){
	switch (i2c_control[I2C_OLED].state){
	case 0x01:
		i2c_control[I2C_OLED].cmd=i2c_oled_out;
		i2c_control[I2C_OLED].state=0x00;
	case 0x00:
		if (i2c_oled_i>=0){
			for (;i2c_oled_i>=0;++i2c_oled_i){
				if (!(i2c_oled_outbuf[i2c_oled_i].B1&OLED_FONT_PENDING)) continue;
				i2c_oled_outbuf[i2c_oled_i].B1&=~OLED_FONT_PENDING;
				oled_printblock(i2c_oled_i,i2c_oled_outbuf[i2c_oled_i]);
				i2c_control[I2C_OLED].state=0x80;
				break;
			}
		}else if (i2c_oled_refresh){
			i2c_oled_preparebuffer();
			i2c_oled_i=0;
			i2c_oled_refresh=0;
		}
	}
}
#endif

#if USING_I2C_OLED2
/*
I2COLED2_W,I2CCMD_DATA,6,
	I2COLED_CMD_1INS,<I2COLED_ADDR_LO>,
	I2COLED_CMD_1INS,<I2COLED_ADDR_HI>,
	I2COLED_CMD_1INS,<I2COLED_ADDR_ROW>,
	I2COLED_CMD_DATA,
I2CCMD_DATA,7,
	<0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77>,
I2CCMD_END
*/
INLINE void oled2_printblock(int8_t pos,union REG16 data){
	register int i,inv;

	i2c_oled2_out[4]=I2COLED_ADDR_LO|((pos<<3)&0x08);
	i2c_oled2_out[6]=I2COLED_ADDR_HI|((pos>>1)&0x07);
	i2c_oled2_out[8]=I2COLED_ADDR_ROW|((pos>>4)&0x07);
	inv=data.B1&OLED_FONT_INV
		?0xFF
		:0x00
	;
	switch (data.B1&~OLED_FONT_INV){
#if USING_OLED_FONT_16H
	case OLED_FONT_16HU:
		for (i=0;i<8;++i){
			i2c_oled2_out[i+12]=oled_hex16[0][((data.B0&0xF0)>>1)|i]^inv;
		}
		break;
	case OLED_FONT_16HD:
		for (i=0;i<8;++i){
			i2c_oled2_out[i+12]=oled_hex16[1][((data.B0&0x0F)<<3)|i]^inv;
		}
		break;
#endif
#if USING_OLED_FONT_16C
	case OLED_FONT_16CU:
		for (i=0;i<8;++i){
			i2c_oled2_out[i+12]=oled_char16[0][(data.B0<<3)|i]^inv;
		}
		break;
	case OLED_FONT_16CD:
		for (i=0;i<8;++i){
			i2c_oled2_out[i+12]=oled_char16[1][(data.B0<<3)|i]^inv;
		}
		break;
#endif
#if USING_OLED_FONT_8H
	case OLED_FONT_8H:
		for (i=0;i<4;++i){
			i2c_oled2_out[i+12]=oled_hex8[0][((data.B0&0xF0)>>2)|i]^inv;
		}
		for (i=0;i<4;++i){
			i2c_oled2_out[i+16]=oled_hex8[1][((data.B0&0x0F)<<2)|i]^inv;
		}
		break;
#endif
#if USING_OLED_FONT_8C
	case OLED_FONT_8C:
		for (i=0;i<8;++i){
			i2c_oled2_out[i+12]=oled_char8[(data.B0<<3)|i]^inv;
		}
		break;
#endif
#if USING_OLED_FONT_ICO
	case OLED_FONT_ICO:
		for (i=0;i<8;++i){
			i2c_oled2_out[i+12]=oled_ico8[(data.B0<<3)|i]^inv;
		}
		break;
#endif
	default:
		for (i=12;i<20;++i){
			i2c_oled2_out[i]=0xFF;
		}
	}
}

INLINE void i2c_oled2_init(struct i2ctransaction *control){
	register int i;

	i2c_oled2_refresh=-1;
	i2c_oled2_i=0;
	i2c_oled2_out[0]=I2COLED2_W;
	i2c_oled2_out[1]=I2CCMD_DATA;
	i2c_oled2_out[2]=6;
	i2c_oled2_out[3]=I2COLED_CMD_1INS;
	i2c_oled2_out[5]=I2COLED_CMD_1INS;
	i2c_oled2_out[7]=I2COLED_CMD_1INS;
	i2c_oled2_out[9]=I2COLED_CMD_DATA;
	i2c_oled2_out[10]=I2CCMD_DATA;
	i2c_oled2_out[11]=7;
	i2c_oled2_out[20]=I2CCMD_END;
	for (i=0;i<I2COLED2_INITLEN;++i){
		oled2_char2buf(I2COLED2_INITFONT,i,i2c_oled2_initdata[i]);
	}
	i2c_oled2_initbufferex();

	control->state=0x81;
	control->cmd=i2c_oled2_reset;
}

INLINE void i2c_oled2_idle(){
	switch (i2c_control[I2C_OLED2].state){
	case 0x01:
		i2c_control[I2C_OLED2].cmd=i2c_oled2_out;
		i2c_control[I2C_OLED2].state=0x00;
	case 0x00:
		if (i2c_oled2_i>=0){
			for (;i2c_oled2_i>=0;++i2c_oled2_i){
				if (!(i2c_oled2_outbuf[i2c_oled2_i].B1&OLED_FONT_PENDING)) continue;
				i2c_oled2_outbuf[i2c_oled2_i].B1&=~OLED_FONT_PENDING;
				oled2_printblock(i2c_oled2_i,i2c_oled2_outbuf[i2c_oled2_i]);
				i2c_control[I2C_OLED2].state=0x80;
				break;
			}
		}else if (i2c_oled2_refresh){
			i2c_oled2_preparebuffer();
			i2c_oled2_i=0;
			i2c_oled2_refresh=0;
		}
	}
}
#endif

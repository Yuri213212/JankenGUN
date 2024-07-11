#if USING_I2C_OLED||USING_I2C_OLED2
#include "i2c/oled_c.h"
#endif

INLINE void i2c_init(){
	register int temp;

//I2C settings
	I2C0SCLH=45;
	I2C0SCLL=75;				//set SCL to 400KHz(48MHz/(45+75))
	//I2C0CONCLR=0x6C;			//reset state
	I2C0CONSET=0x40;			//enable I2C

	i2c_busy=-1;
	i2c_errc=0;
#if USING_I2C_OLED
	i2c_oled_init(&i2c_control[I2C_OLED]);
#endif
#if USING_I2C_OLED2
	i2c_oled2_init(&i2c_control[I2C_OLED2]);
#endif
	for (temp=I2C_COUNT-2;temp>=0;--temp){
		i2c_control[temp].next=&i2c_control[temp+1];
	}
	i2c_control[I2C_COUNT-1].next=&i2c_control[0];
	i2c_cp=&i2c_control[I2C_COUNT-1];
}

INLINE void i2c_idle(){
	register int temp;

#if USING_I2C_OLED
	i2c_oled_idle();
#endif
#if USING_I2C_OLED2
	i2c_oled2_idle();
#endif
	if (i2c_busy<0&&(int16_t)(i2c_resetwait-wallclock)<=0){
		i2c_busy=0;
	}
	if (!i2c_busy){
		for (temp=I2C_COUNT;temp;--temp){
			i2c_cp=i2c_cp->next;
			if (i2c_cp->state&0x80){
				i2c_busy=1;
				i2c_cp->cmdi=0;
				I2C0CONSET=I2CCMD_RESTART;	//start I2C
				break;
			}
		}
	}
}

//I2C master
void I2C_IRQHandler(){
	register int c;

	switch (I2C0STAT){
	default:	//error
		I2C0CONSET=I2CCMD_END;
		i2c_busy=0;
		++i2c_errc;
		break;
	case 0x08:	//start
	case 0x10:	//restart
		i2c_cp->count=0;
		i2c_cp->ini=0;
		I2C0DAT=i2c_cp->cmd[i2c_cp->cmdi++];
		I2C0CONCLR=I2CCMD_RESTART;
		break;
	case 0x18:	//acceptW
	case 0x28:	//dataW
		if (!i2c_cp->count){
			c=i2c_cp->cmd[i2c_cp->cmdi++];
			if (c){
				if (c==I2CCMD_END){
					i2c_cp->state&=~0x80;
					i2c_busy=0;
				}
				I2C0CONSET=c;
			}else{
				i2c_cp->count=i2c_cp->cmd[i2c_cp->cmdi++];
				I2C0DAT=i2c_cp->cmd[i2c_cp->cmdi++];
			}
		}else{
			I2C0DAT=i2c_cp->cmd[i2c_cp->cmdi++];
			--i2c_cp->count;
		}
		break;
	case 0x40:	//acceptR
		i2c_cp->count=i2c_cp->cmd[i2c_cp->cmdi++];
		if (i2c_cp->count){
			I2C0CONSET=0x04;
		}
		break;
	case 0x50:	//dataR
		i2c_cp->in[i2c_cp->ini++]=I2C0DATA_BUFFER;
		if (!(--i2c_cp->count)){
			I2C0CONCLR=0x04;
		}
		break;
	case 0x58:	//disconnectR
		i2c_cp->in[i2c_cp->ini++]=I2C0DATA_BUFFER;
		c=i2c_cp->cmd[i2c_cp->cmdi++];
		if (c==I2CCMD_END){
			i2c_cp->state&=~0x80;
			i2c_busy=0;
		}
		I2C0CONSET=c;
	}
	I2C0CONCLR=0x08;			//acknoledgement
}

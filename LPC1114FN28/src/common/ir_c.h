//user code
#include "../ir.h"

INLINE void ir_init(){
//timer settings
#if USING_T2_IR
	TMR16B1PR=479;				//set period 480(10us)
	TMR16B1CCR=0x06;			//set interrupt at falling edge on capture
	TMR16B1TCR=0x01;			//start counter
#endif
#if USING_T1_IR2
	TMR16B0PR=479;				//set period 480(10us)
	TMR16B0CCR=0x06;			//set interrupt at falling edge on capture
	TMR16B0TCR=0x01;			//start counter
#endif

#if USING_T2_IR
	ir_state=0;
	ir_repeat=0;
	ir_data=0;
#endif
#if USING_T1_IR2
	ir2_state=0;
	ir2_repeat=0;
	ir2_data=0;
#endif
}

INLINE void ir_idle(){
	register int temp;

#if USING_T2_IR
	temp=-1;
	if (ir_repeat<0){
		ir_repeat=0;
		temp=0;
	}else if (ir_repeat>5){
		--ir_repeat;
		temp=1;
	}
	if (temp>=0){
		irbutton(ir_data,temp);
	}
#endif
#if USING_T1_IR2
	temp=-1;
	if (ir2_repeat<0){
		ir2_repeat=0;
		temp=0;
	}else if (ir2_repeat>5){
		--ir2_repeat;
		temp=1;
	}
	if (temp>=0){
		ir2button(ir2_data,temp);
	}
#endif
}

#if USING_T2_IR
void TIMER16_1_IRQHandler(){
	register int time;

	TMR16B1IR=TMR16B1IR;		//acknowledgement
	time=ir_prevtime;
	ir_prevtime=TMR16B1CR0;
	time=ir_prevtime-time;
	if (time>1300&&time<2000){//start,13500us
		ir_databuf=0;
		ir_state=32;
	}else if (time>1000&&time<1300){//repeat,11250us
		ir_repeat+=1;
	}else if (time>100&&time<300){//data
		if (ir_state){
			ir_databuf=(ir_databuf<<1)|(time>200?0x01:0x00);//1125us:0,2250us:1
			--ir_state;
			if (!ir_state){
				ir_data=ir_databuf;
				ir_repeat=0x80;
			}
		}
	}
}
#endif

#if USING_T1_IR2
void TIMER16_0_IRQHandler(){
	register int time;

	TMR16B0IR=TMR16B0IR;		//acknowledgement
	time=ir2_prevtime;
	ir2_prevtime=TMR16B0CR0;
	time=ir2_prevtime-time;
	if (time>1300&&time<2000){//start,13500us
		ir2_databuf=0;
		ir2_state=32;
	}else if (time>1000&&time<1300){//repeat,11250us
		ir2_repeat+=1;
	}else if (time>100&&time<300){//data
		if (ir2_state){
			ir2_databuf=(ir2_databuf<<1)|(time>200?0x01:0x00);//1125us:0,2250us:1
			--ir2_state;
			if (!ir2_state){
				ir2_data=ir2_databuf;
				ir2_repeat=0x80;
			}
		}
	}
}
#endif

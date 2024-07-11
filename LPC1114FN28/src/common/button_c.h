//user code
#include "../button.h"

INLINE void button_init(){
	button_waitto=0;
	button_state=0;
}

INLINE void button_idle(){
	register int temp;

	if (P0_DOWNST&~button_state){
		if (!button_state){
			temp=P0_ALL&~GPIO0DATA[P0_ALL];
			if (temp){
				button_sample=temp;
				button_state=P0_DOWNST;
				button_waitto=wallclock+button_wait;
			}
		}else if (!(button_state&~GPIO0DATA[P0_ALL])){
			button_state|=P0_DOWNST;
			button_sample=0;
			button_waitto=wallclock+button_wait;
		}
	}else if ((int16_t)(button_waitto-wallclock)>0){
		if (button_sample){
			temp=button_sample&~GPIO0DATA[P0_ALL];
			if (temp){
				button_sample=temp;
			}else{
				button_state=0;
			}
		}else if (!(GPIO0DATA[P0_ALL]&button_state)){
			button_state&=~P0_DOWNST;
		}
	}else{
		temp=(-button_sample)&button_sample;
		switch (temp){//button down
		case P0_BUTTON1:
			button1down();
			break;
		case P0_BUTTON2:
			button2down();
			break;
		default:
			switch (button_state&~P0_DOWNST){//button up
			case P0_BUTTON1:
				button1up();
				break;
			case P0_BUTTON2:
				button2up();
				break;
			}
		}
		button_state=temp;
	}
}

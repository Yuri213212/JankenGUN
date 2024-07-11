INLINE void button1down(){//stop
	buffer[BF_START]=0x00;

	if (test){//test mode only
		if (state[0]>=ST_NULL){
			state[0]=0;
		}else{
			++state[0];
		}
		if (state[1]>=ST_NULL){
			state[1]=0;
		}else{
			++state[1];
		}
		i2c_oled_refresh=1;
		i2c_oled2_refresh=1;
	}
}

INLINE void button2down(){//start
	buffer[BF_START]=0x01;

	if (test){//test mode only
		state[0]=ST_NULL;
		state[1]=ST_NULL;
		i2c_oled_refresh=1;
		i2c_oled2_refresh=1;
	}
}

INLINE void button1up(){
}

INLINE void button2up(){
}

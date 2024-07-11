INLINE void init(){
	register int i;

//init sound
	sound_init();

//init variables
	test=GPIO0DATA[P0_BUTTON2]
		?0
		:1
	;
	start=0;
	flashcounter[0]=0;
	flashcounter[1]=0;
	randomrefresh=0;
	random=1;
	next[0]=1;
	next[1]=1;
	state[0]=ST_NULL;
	state[1]=ST_NULL;
	for (i=0;i<BF_DISPLAYTIME;++i){
		buffer[i]=0x00;
	}
	buffer[BF_DISPLAYTIME]=defdisplaytime;
	buffer[BF_CLEARTIME]=defcleartime;
	buffer[BF_LOSETIME]=deflosetime;
	buffer[BF_START]=0x80;
}

INLINE void idle(){
//random state generator
	random=((((random>>1)^random)&0x01)<<14)|(random>>1);
	if (random&0x03){
		next[randomrefresh]=random&0x03;
		randomrefresh^=1;
	}

//game logic
	SVC();
}

void SVC_Handler(){
//LED flash logic
	if ((int16_t)(flashwaitto[0]-wallclock)<=0){
		flashwaitto[0]+=flashwait;
		GPIO0DATA[P0_LED1B]=GPIO0DATA[P0_LED1A]
			?P0_LED1B
			:0
		;
		if (flashcounter[0]){
			--flashcounter[0];
			GPIO0DATA[P0_LED1A]^=P0_LED1A;
		}
	}
	if ((int16_t)(flashwaitto[1]-wallclock)<=0){
		flashwaitto[1]+=flashwait;
		GPIO0DATA[P0_LED2B]=GPIO0DATA[P0_LED2A]
			?P0_LED2B
			:0
		;
		if (flashcounter[1]){
			--flashcounter[1];
			GPIO0DATA[P0_LED2A]^=P0_LED2A;
		}
	}

	if (!test){//normal mode only
//trigger logic
		if (!buffer[BF_START]){
			buffer[BF_START]=0x80;
			if (start){
				start=0;
				state[0]=ST_NULL;
				state[1]=ST_NULL;
				i2c_oled_refresh=1;
				i2c_oled2_refresh=1;
			}
		}else if (buffer[BF_START]==0x01){
			buffer[BF_START]=0x81;
			if (!start){
				start=1;
				refreshwaitto[0]=wallclock;
				refreshwaitto[1]=wallclock;
			}
		}
//game status change on time up
		if (start){
			if ((int32_t)(refreshwaitto[0]-wallclock)<=0){
				switch (state[0]){
				case ST_ROCK:
				case ST_SCISSORS:
				case ST_PAPER:
				case ST_LOSE:
				case ST_DRAW:
					refreshwaitto[0]+=buffer[BF_CLEARTIME]*6000;
					state[0]=ST_NULL;
					break;
				case ST_WIN:
				case ST_NULL:
				default:
					refreshwaitto[0]+=buffer[BF_DISPLAYTIME]*6000;
					state[0]=next[0]-1;
					break;
				}
				i2c_oled_refresh=1;
			}
			if ((int32_t)(refreshwaitto[1]-wallclock)<=0){
				switch (state[1]){
				case ST_ROCK:
				case ST_SCISSORS:
				case ST_PAPER:
				case ST_LOSE:
				case ST_DRAW:
					refreshwaitto[1]+=buffer[BF_CLEARTIME]*6000;
					state[1]=ST_NULL;
					break;
				case ST_WIN:
				case ST_NULL:
				default:
					refreshwaitto[1]+=buffer[BF_DISPLAYTIME]*6000;
					state[1]=next[1]-1;
					break;
				}
				i2c_oled2_refresh=1;
			}
		}
	}
}

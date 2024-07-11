#if USING_T2_IR
INLINE void irbutton(uint32_t irdata,int isrepeat){
	switch (ir_data){
	case IR_A:
		switch (state[0]){
		case ST_ROCK:
			goto DRAW1;
		case ST_SCISSORS:
			goto WIN1;
		case ST_PAPER:
			goto LOSE1;
		}
		break;
	case IR_B:
		switch (state[0]){
		case ST_ROCK:
			goto LOSE1;
		case ST_SCISSORS:
			goto DRAW1;
		case ST_PAPER:
			goto WIN1;
		}
		break;
	case IR_C:
		switch (state[0]){
		case ST_ROCK:
			goto WIN1;
		case ST_SCISSORS:
			goto LOSE1;
		case ST_PAPER:
			goto DRAW1;
		}
		break;
WIN1:
		++buffer[BF_WIN];
		refreshwaitto[0]=wallclock+buffer[BF_CLEARTIME]*6000;
		state[0]=ST_WIN;
		flashwaitto[0]=wallclock+flashwait;
		flashcounter[0]=flashcount;
		GPIO0DATA[P0_LED1A]=P0_LED1A;
		sound_play(0,SND_WIN);
		i2c_oled_refresh=1;
		break;
DRAW1:
		++buffer[BF_DRAW];
		state[0]=ST_DRAW;
		sound_play(0,SND_DRAW);
		i2c_oled_refresh=1;
		break;
LOSE1:
		++buffer[BF_LOSE];
		refreshwaitto[0]+=buffer[BF_LOSETIME]*6000;
		state[0]=ST_LOSE;
		sound_play(0,SND_LOSE);
		i2c_oled_refresh=1;
		break;
	}
}
#endif

#if USING_T1_IR2
INLINE void ir2button(uint32_t irdata,int isrepeat){
	switch (ir2_data){
	case IR_A:
		switch (state[1]){
		case ST_ROCK:
			goto DRAW2;
		case ST_SCISSORS:
			goto WIN2;
		case ST_PAPER:
			goto LOSE2;
		}
		break;
	case IR_B:
		switch (state[1]){
		case ST_ROCK:
			goto LOSE2;
		case ST_SCISSORS:
			goto DRAW2;
		case ST_PAPER:
			goto WIN2;
		}
		break;
	case IR_C:
		switch (state[1]){
		case ST_ROCK:
			goto WIN2;
		case ST_SCISSORS:
			goto LOSE2;
		case ST_PAPER:
			goto DRAW2;
		}
		break;
WIN2:
		++buffer[BF_WIN];
		refreshwaitto[1]=wallclock+buffer[BF_CLEARTIME]*6000;
		state[1]=ST_WIN;
		flashwaitto[1]=wallclock+flashwait;
		flashcounter[1]=flashcount;
		GPIO0DATA[P0_LED2A]=P0_LED2A;
		sound_play(1,SND_WIN);
		i2c_oled2_refresh=1;
		break;
DRAW2:
		++buffer[BF_DRAW];
		state[1]=ST_DRAW;
		sound_play(1,SND_DRAW);
		i2c_oled2_refresh=1;
		break;
LOSE2:
		++buffer[BF_LOSE];
		refreshwaitto[1]+=buffer[BF_LOSETIME]*6000;
		state[1]=ST_LOSE;
		sound_play(1,SND_LOSE);
		i2c_oled2_refresh=1;
		break;
	}
}
#endif

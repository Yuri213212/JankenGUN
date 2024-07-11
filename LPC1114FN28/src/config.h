#define USING_XTAL12		0	//whether to use external 12MHz crystal
#define USING_WDT			1	//whether to use WDT
#define USING_TIMER			1	//whether to use timers
#define USING_I2C			1	//whether to use I2C (require timers)
#define USING_UART			1	//whether to use UART (require timers)
#define USING_GPIO_BUTTON	1	//whether to add GPIO button support (require timers)
#define USING_T2_IR			1	//whether to add IR input support @T2 (require timers)
#define USING_T1_IR2		1	//whether to add IR2 input support @T1 (require timers)
#define WALLCLOCK_T3		1	//whether to use T3 instead of T1 for wall clock (require timers)
#define SYNCHRONIZED_FEED	1	//whether to use synchronized feedWatchDog() (require WDT)

#define PRIORITY_SVC		2	//0=rt(def),1=hi,2=mid,3=lo
#define PRIORITY_TIMER16_0	1
#define PRIORITY_TIMER16_1	1
#define PRIORITY_TIMER32_1	0
#define PRIORITY_I2C		3
#define PRIORITY_UART		2

#if USING_WDT
#define WDT_RELOAD			1199999	//time-out after 0.1s
#endif

#include "ioport.h"

#define flashwait			600		//0.1s(10fps)
#define flashcount			20
#define bufferlen			8
#define buffermask			0x07
#define defdisplaytime		5
#define defcleartime		2
#define deflosetime			3

enum STATE_ENUM{
	ST_ROCK=0,
	ST_SCISSORS,
	ST_PAPER,
	ST_LOSE,
	ST_DRAW,
	ST_WIN,
	ST_NULL,
};

enum BUFFER_ENUM{
	BF_WIN=0,
	BF_LOSE,
	BF_DRAW,
	BF_TID,
	BF_DISPLAYTIME,
	BF_CLEARTIME,
	BF_LOSETIME,
	BF_START,
};

int32_t refreshwaitto[2];
int16_t flashwaitto[2];
uint16_t random;
uint8_t flashcounter[2],test,start,randomrefresh,next[2],state[2],buffer[bufferlen];

#include "sound.h"

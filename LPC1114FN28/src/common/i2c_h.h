//user code
#include "../i2c.h"

#ifndef USING_I2C_OLED
#define USING_I2C_OLED		0
#endif
#ifndef USING_I2C_OLED2
#define USING_I2C_OLED2		0
#endif

#ifndef i2c_resetwait
#define i2c_resetwait		60	//10ms
#endif

enum I2C_DeviceEnum{
#if USING_I2C_OLED
	I2C_OLED,
#endif
#if USING_I2C_OLED2
	I2C_OLED2,
#endif
	I2C_COUNT,
};

#define I2CCMD_DATA			0x00
#define I2CCMD_END			0x10
#define I2CCMD_RESTART		0x20

struct i2ctransaction{
	uint8_t state;		//loaded,phase(7)
	uint8_t cmdi;		//set 0 on start
	uint8_t count;		//set 0 on start/restart
	uint8_t ini;		//set 0 on start/restart
	uint8_t const *cmd;	//{addr(e),ctrl,[max,[data],ctrl]},{addr(o),max,ctrl}	//ctrl:0x20=restart,0x10=stop,0x00=data//max:datalen-1
	uint8_t *in;
	struct i2ctransaction *next;
};

struct i2ctransaction i2c_control[I2C_COUNT];
struct i2ctransaction *i2c_cp;
int8_t i2c_busy;
uint32_t i2c_errc;

#if USING_I2C_OLED||USING_I2C_OLED2
#include "i2c/oled_h.h"
#endif

#define IR_POWER		0x08F71BE4
#define IR_A			0x08F71FE0
#define IR_B			0x08F71EE1
#define IR_C			0x08F71AE5
#define IR_CENTER		0x08F704FB
#define IR_UP			0x08F705FA
#define IR_DOWN			0x08F700FF
#define IR_LEFT			0x08F708F7
#define IR_RIGHT		0x08F701FE
#define IR_UPLEFT		0x08F78D72
#define IR_UPRIGHT		0x08F7847B
#define IR_DOWNLEFT		0x08F78877
#define IR_DOWNRIGHT	0x08F7817E

#if USING_T2_IR
int16_t ir_prevtime;
uint8_t ir_state;
int8_t ir_repeat;
uint32_t ir_databuf,ir_data;
#endif
#if USING_T1_IR2
int16_t ir2_prevtime;
uint8_t ir2_state;
int8_t ir2_repeat;
uint32_t ir2_databuf,ir2_data;
#endif

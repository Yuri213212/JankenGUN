union REG16{
	uint16_t W;
	int16_t s;
	uint8_t B[2];
	struct{
		uint8_t B0;
		uint8_t B1;
	};
	int8_t c[2];
	struct{
		int8_t c0;
		int8_t c1;
	};
};

union REG32{
	uint32_t D;
	int32_t i;
	uint16_t W[2];
	struct{
		uint16_t W0;
		uint16_t W1;
	};
	int16_t s[2];
	struct{
		int16_t s0;
		int16_t s1;
	};
	uint8_t B[4];
	struct{
		uint8_t B0;
		uint8_t B1;
		uint8_t B2;
		uint8_t B3;
	};
	int8_t c[4];
	struct{
		int8_t c0;
		int8_t c1;
		int8_t c2;
		int8_t c3;
	};
};

INLINE uint8_t toHex(register uint8_t x){
	return (x<10
		?'0'
		:'A'-10
	)+x;
}

INLINE uint16_t div10(register uint16_t x){
	return (x*0xCCCD)>>19;
}

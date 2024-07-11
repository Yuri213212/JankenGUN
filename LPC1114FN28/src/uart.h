INLINE void uart_write(uint16_t addr,uint8_t data){
	buffer[addr&buffermask]=data;
}

INLINE uint8_t uart_read(uint16_t addr){
	return buffer[addr&buffermask];
}

INLINE void uart_uploaded(uint16_t addr,uint16_t length){
	register int i;

	feedWatchDog();
	for (i=0;i<length;++i){
		buffer[(addr+i)&buffermask]=uart_buffer[i];
	}
}

INLINE void uart_downloading(uint16_t addr,uint16_t length){
	register int i;

	feedWatchDog();
	for (i=0;i<length;++i){
		uart_buffer[i]=buffer[(addr+i)&buffermask];
	}
}

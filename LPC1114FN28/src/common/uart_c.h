//user code
#include "../uart.h"

INLINE void uart_init(){
//UART settings
	U0LCR=0x80;					//enter setting mode
	//U0DLM=0;
	U0DLL=26;					//set baud rate 115200(about 48MHz/16/26)
	U0LCR=0x0B;					//exit setting mode, set 8b/B, 1 stop bit, odd parity
	U0FCR=0x47;					//enable and reset buffer, set word length 4
	U0IER=0x05;					//enable interrupts (for error and receive)

	uart_state=UARTSTATE_INIT;
}

INLINE void uart_idle(){
	if (uart_state<0&&(int16_t)(uart_waitto-wallclock)<=0){
		++uart_state;
		if (!uart_state){
			U0THR=UARTRSP_ERR;
			U0THR=uart_command.B1;
			U0THR=uart_command.B2;
			U0THR=uart_command.B3;
		}
	}
}

//UART slave
#pragma push
#pragma diag_suppress 550		//suppress warning for temp
void UART_IRQHandler(){
	register int temp;			//intended to do meaningless read, causes warning

	switch (U0IIR&0x0E){
	case 0x06://error
	case 0x0C://receive fragment bytes
		while (U0LSR&0x01){
			temp=U0RBR;
		}
		uart_state=UARTSTATE_ERROR;
		uart_waitto=wallclock+uart_wait;
		break;
	case 0x04://word received
		uart_wordbuf.B0=U0RBR;
		uart_wordbuf.B1=U0RBR;
		uart_wordbuf.B2=U0RBR;
		uart_wordbuf.B3=U0RBR;
		switch (uart_state){
		case UARTSTATE_INIT:
			if (uart_wordbuf.B0==UARTCMD_PING){
				uart_command.D=uart_wordbuf.D;
				uart_state=UARTSTATE_READY;
				break;
			}else{
		case UARTSTATE_DOWNLOADING:
				uart_state=UARTSTATE_ERROR;
		case UARTSTATE_ERROR:
				uart_command.D=uart_wordbuf.D;
				uart_waitto=wallclock+uart_wait;
				return;
			}
		case UARTSTATE_READY:
			uart_command.D=uart_wordbuf.D;
			switch (uart_command.B0){
			case UARTCMD_PING:
				break;
			case UARTCMD_WRITE:
			case UARTCMD_WRITE_NR:
				uart_write(uart_command.W1,uart_command.B1);
				if (uart_command.B0==UARTCMD_WRITE_NR) return;
				break;
			case UARTCMD_READ:
				uart_command.B1=uart_read(uart_command.W1);
				break;
			case UARTCMD_UPLOAD:
				uart_length=(uart_command.B1<<2)+4;
				uart_xor.D=0;
				uart_i=0;
				uart_waitto=wallclock+uart_wait;
				uart_state=UARTSTATE_UPLOADING;
				return;
			case UARTCMD_DOWNLOAD:
				uart_length=(uart_command.B1<<2)+4;
				uart_downloading(uart_command.W1,uart_length);
				uart_xor.D=0;
				uart_i=0;
				U0IER|=0x02;	//enable interrupt of send
				uart_state=UARTSTATE_DOWNLOADING;
				break;
			default:
				uart_state=UARTSTATE_ERROR;
				uart_waitto=wallclock+uart_wait;
				return;
			}
			break;
		case UARTSTATE_UPLOADING:
			uart_xor.D^=uart_wordbuf.D;
			if (uart_i<uart_length){//data
				uart_buffer[uart_i++]=uart_wordbuf.B0;
				uart_buffer[uart_i++]=uart_wordbuf.B1;
				uart_buffer[uart_i++]=uart_wordbuf.B2;
				uart_buffer[uart_i++]=uart_wordbuf.B3;
				uart_waitto=wallclock+uart_wait;
				return;
			}else if (!uart_xor.D){//parity OK
				uart_uploaded(uart_command.W1,uart_length);
				uart_state=UARTSTATE_READY;
				break;
			}else{//parity NG
				uart_state=UARTSTATE_ERROR;
				uart_waitto=wallclock+uart_wait;
				return;
			}
		default:
			;
		}
		U0THR=UARTRSP_OK;
		U0THR=uart_command.B1;
		U0THR=uart_command.B2;
		U0THR=uart_command.B3;
		break;
	case 0x02://send
		if (uart_i>=uart_length){
			U0IER&=~0x02;		//disable interrupt of send
			U0THR=uart_xor.B0;
			U0THR=uart_xor.B1;
			U0THR=uart_xor.B2;
			U0THR=uart_xor.B3;
			uart_state=UARTSTATE_READY;
		}else{
			U0THR=uart_wordbuf.B0=uart_buffer[uart_i++];
			U0THR=uart_wordbuf.B1=uart_buffer[uart_i++];
			U0THR=uart_wordbuf.B2=uart_buffer[uart_i++];
			U0THR=uart_wordbuf.B3=uart_buffer[uart_i++];
			uart_xor.D^=uart_wordbuf.D;
		}
		break;
	default:
		;
	}
}
#pragma pop

#ifndef uart_wait
#define uart_wait			60		//10ms
#endif

#define UARTCMD_PING		'P'
#define UARTCMD_WRITE		'W'
#define UARTCMD_WRITE_NR	'w'
#define UARTCMD_READ		'R'
#define UARTCMD_UPLOAD		'U'
#define UARTCMD_DOWNLOAD	'D'

#define UARTRSP_OK			'O'
#define UARTRSP_ERR			'E'

enum uartstateenum{
	UARTSTATE_UPLOADING=-2,
	UARTSTATE_ERROR,
	UARTSTATE_INIT,
	UARTSTATE_READY,
	UARTSTATE_DOWNLOADING,
};

int16_t uart_waitto;
union REG32 uart_command,uart_wordbuf,uart_xor;
uint16_t uart_i,uart_length;
int8_t uart_state;
uint8_t uart_buffer[1024];

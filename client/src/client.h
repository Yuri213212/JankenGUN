#include <w32api.h>

#define WINVER		Windows95
#define _WIN32_WINDOWS	Windows95

#define UNICODE
#define _UNICODE

#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include "uart.h"

#define clwidth		642
#define clheight	390
#define tbuflen		1024
#define tcpinterval	200
#define getcount	1
#define postcount	1
#define cominterval	100
#define executetime	5
#define transmittime	5
#define flashinterval	100
#define flashcount	20

enum dlgChildEnum{
	DLG_STATIC_S_Win=1,
	DLG_EDIT_S_IP,
	DLG_EDIT_S_Port,
	DLG_EDIT_S_InPath,
	DLG_EDIT_S_OutPath,
	DLG_EDIT_S_Status,
	DLG_EDIT_S_Response,
	DLG_EDIT_S_Win,
	DLG_EDIT_S_Draw,
	DLG_EDIT_S_Lose,
	DLG_EDIT_S_Score,
	DLG_CHK_S_GameState,
	DLG_CHK_S_DoubleScore,
	DLG_BTN_S_Start,
	DLG_BTN_S_Test,
	DLG_BTN_S_Stop,
	DLG_BTN_S_GameStart,
	DLG_BTN_S_GameEnd,

	DLG_COMBO_M_COM,
	DLG_COMBO_M_BaudRate,
	DLG_COMBO_M_ByteSize,
	DLG_COMBO_M_StopBits,
	DLG_COMBO_M_Parity,
	DLG_STATIC_M_Screen1,
	DLG_STATIC_M_Screen2,
	DLG_STATIC_M_LED1,
	DLG_STATIC_M_LED2,
	DLG_EDIT_M_Status,
	DLG_EDIT_M_Response,
	DLG_EDIT_M_Win,
	DLG_EDIT_M_Draw,
	DLG_EDIT_M_Lose,
	DLG_EDIT_M_DisplayTime,
	DLG_EDIT_M_ClearTime,
	DLG_EDIT_M_LoseTime,
	DLG_CHK_M_IR1,
	DLG_CHK_M_IR2,
	DLG_BTN_M_Start,
	DLG_BTN_M_Test,
	DLG_BTN_M_Stop,
	DLG_BTN_M_Rock,
	DLG_BTN_M_Scissors,
	DLG_BTN_M_Paper,
};

enum timerEnum{
	TMR_TCP=1,
	TMR_COM,
	TMR_EMU1,
	TMR_EMU2,
	TMR_LED1,
	TMR_LED2,
};

enum postResultEnum{
	PR_WIN=0,
	PR_DRAW,
	PR_LOSE,

	PR_COUNT,
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

	BF_SYNCSTATE,
	BF_COUNT,
};

enum STATE_ENUM{
	ST_ROCK=0,
	ST_SCISSORS,
	ST_PAPER,
	ST_LOSE,
	ST_DRAW,
	ST_WIN,
	ST_NULL,
};

const struct _DLGTEMPLATE{
	DLGTEMPLATE dlgt;
	int dummy;	//DialogBoxIndirectParamW need extra 4 bytes of 0 to read
}_dlgt={
	{WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|DS_CENTER,WS_EX_DLGMODALFRAME,0,0,0,clwidth/2,clheight/2},
	0
};

const wchar_t initIP[]=L"163.43.185.149";
const wchar_t initPort[]=L"1880";
const wchar_t initInPath[]=L"/state";
const wchar_t initOutPath[]=L"/hit";

const char getHeader[]=
	"GET %s HTTP/1.1\r\n"
	"Host: %s:%d\r\n"
	"Connection: Close\r\n"
	"\r\n"
;
const char postHeader[]=
	"POST %s HTTP/1.1\r\n"
	"Host: %s:%d\r\n"
	"Content-Type: application/json\r\n"
	"Content-Length: %d\r\n"
	"Connection: Close\r\n"
	"\r\n"
	"%s"
;
const char getStartContents[]="{\"state\":\"start\"}";
const char getEndContents[]="{\"state\":\"end\"}";
const char getWaitContents[]="{\"state\":\"wait\"}";
const char postWinContents[]="{\"result\":\"win\"}";
const char postVictoryContents[]="{\"result\":\"victory\"}";
const char postDrawContents[]="{\"result\":\"draw\"}";
const char postLoseContents[]="{\"result\":\"lose\"}";

struct posttransaction{
	int count;
	int pending;
	int length;
	char data[tbuflen];
	struct posttransaction *next;
};

WSADATA wsadata;
SOCKET sock;
SOCKADDR_IN sain;
int port,getlen,getcounter,postcounter,doublescore=1,gamestate=0;
char ip[tbuflen],path[tbuflen];
char get[tbuflen],tcpbuf[tbuflen+1];
struct posttransaction postcontrol[PR_COUNT];
struct posttransaction *postp;

COMMTIMEOUTS cto={
	MAXDWORD,1,1,
	1,1
};

const uint8_t initcmdtemplate[8]={
	UARTCMD_PING,0xFF,0xFF,0xFF,
	UARTCMD_UPLOAD,0x00,BF_TID,0x00,//0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
};
/*response:
	UARTRSP_OK,0xFF,0xFF,0xFF,
	UARTRSP_OK,0x00,BF_TID,0x00,
*/
const uint8_t tickcmdtemplate[4]={
	UARTCMD_DOWNLOAD,0x00,BF_WIN,0x00,
};
/*response:
	UARTRSP_OK,0x00,BF_WIN,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
*/
const uint8_t startcmdtemplate[4]={
	UARTCMD_WRITE,0x01,BF_START,0x00,
};
/*response:
	UARTRSP_OK,0x01,BF_START,0x00,
*/
const uint8_t stopcmdtemplate[4]={
	UARTCMD_WRITE,0x00,BF_START,0x00,
};
/*response:
	UARTRSP_OK,0x00,BF_START,0x00,
*/

HANDLE hcom=INVALID_HANDLE_VALUE;
DCB dcb;
COMSTAT comstat;
uint8_t outbuf[tbuflen],inbuf[tbuflen];
uint8_t mcustatus[BF_COUNT];

int emu=0,emustate1,emustate2,punishing1,punishing2,flashcounter1,flashcounter2;
wchar_t wbuf[tbuflen],wbuf2[tbuflen];
char cbuf[tbuflen];

#include "ledPattern.txt"
#include "imageList.h"

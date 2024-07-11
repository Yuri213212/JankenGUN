#include "client.h"

int UART_ExchangeData(int writelen,int wait){
	static COMSTAT comstat;
	int length,i,temp;

	if (!ClearCommError(hcom,NULL,&comstat)) return -1;
	if (comstat.cbInQue){//clear receive buffer
		PurgeComm(hcom,PURGE_RXABORT|PURGE_RXCLEAR);
	}
	WriteFile(hcom,outbuf,writelen,NULL,NULL);
	WaitCommEvent(hcom,(LPDWORD)&temp,NULL);//wait for send complete
	for (i=executetime;i;--i){//wait for response
		Sleep(1);
		ClearCommError(hcom,NULL,&comstat);
		if (comstat.cbInQue) break;
	}
	if (!i) return 0;//time-out error
	ClearCommError(hcom,NULL,&comstat);
	temp=comstat.cbInQue;
	for (i=transmittime;i;--i){//wait for receive complete
		Sleep(1);
		ClearCommError(hcom,NULL,&comstat);
		if (comstat.cbInQue==temp) break;
	}
	Sleep(wait);
	ReadFile(hcom,inbuf,tbuflen,(LPDWORD)&length,NULL);
	return length;
}

BOOL CALLBACK EnumChildProc(HWND hwndChild,LPARAM lParam){
	SendMessage(hwndChild,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);
	return TRUE;
}

BOOL CALLBACK DlgProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam){
	RECT rectParent;
	HWND hwndParent,hwndChild;
	HINSTANCE hInstanceDlg;
	LARGE_INTEGER l;
	int deltaw,deltah,i,temp;

	switch (message){
	case WM_INITDIALOG:
		hwndParent=GetParent(hwnd);
		if (!hwndParent){
			hwndParent=GetDesktopWindow();
		}
		GetWindowRect(hwndParent,&rectParent);
		deltaw=GetSystemMetrics(SM_CXFIXEDFRAME)*2;
		deltah=GetSystemMetrics(SM_CYFIXEDFRAME)*2+GetSystemMetrics(SM_CYCAPTION);
		MoveWindow(hwnd,rectParent.left+(rectParent.right-rectParent.left-clwidth-deltaw)/2,rectParent.top+(rectParent.bottom-rectParent.top-clheight)/2-deltah,clwidth+deltaw,clheight+deltah,TRUE);

		SetWindowTextW(hwnd,L"JankenGUN Client");
		hInstanceDlg=(HINSTANCE)GetWindowLongW(hwnd,GWL_HINSTANCE);

		hwndChild=CreateWindowW(L"button",L"Server",WS_CHILD|WS_VISIBLE|BS_GROUPBOX,12,12,618,165,hwnd,NULL,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"static",L"IP",WS_CHILD|WS_VISIBLE|SS_LEFT,18,37,60,12,hwnd,NULL,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"edit",initIP,WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL|WS_TABSTOP,84,34,100,20,hwnd,(HMENU)DLG_EDIT_S_IP,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"static",L"Port",WS_CHILD|WS_VISIBLE|SS_LEFT,18,66,60,12,hwnd,NULL,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"edit",initPort,WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL|WS_TABSTOP,84,63,100,20,hwnd,(HMENU)DLG_EDIT_S_Port,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"static",L"InPath",WS_CHILD|WS_VISIBLE|SS_LEFT,18,95,60,12,hwnd,NULL,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"edit",initInPath,WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL|WS_TABSTOP,84,92,100,20,hwnd,(HMENU)DLG_EDIT_S_InPath,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"static",L"OutPath",WS_CHILD|WS_VISIBLE|SS_LEFT,18,124,60,12,hwnd,NULL,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"edit",initOutPath,WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL|WS_TABSTOP,84,121,100,20,hwnd,(HMENU)DLG_EDIT_S_OutPath,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"button",L"Start",WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_PUSHBUTTON,76,148,50,23,hwnd,(HMENU)DLG_BTN_S_Start,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"button",L"Test",WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_PUSHBUTTON,134,148,50,23,hwnd,(HMENU)DLG_BTN_S_Test,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"button",L"Stop",WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_PUSHBUTTON,18,148,50,23,hwnd,(HMENU)DLG_BTN_S_Stop,hInstanceDlg,NULL);
		EnableWindow(hwndChild,0);

		hwndChild=CreateWindowW(L"edit",L"",WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL|ES_READONLY,190,150,262,20,hwnd,(HMENU)DLG_EDIT_S_Status,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"edit",L"",WS_CHILD|WS_VISIBLE|WS_BORDER|WS_VSCROLL|WS_HSCROLL|ES_MULTILINE|ES_WANTRETURN|ES_READONLY,190,34,262,108,hwnd,(HMENU)DLG_EDIT_S_Response,hInstanceDlg,NULL);

		hwndChild=CreateWindowW(L"static",L"Victory",WS_CHILD|WS_VISIBLE|SS_LEFT,458,37,60,12,hwnd,(HMENU)DLG_STATIC_S_Win,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"edit",L"",WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL|ES_RIGHT|ES_READONLY,544,34,80,20,hwnd,(HMENU)DLG_EDIT_S_Win,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"static",L"Draw",WS_CHILD|WS_VISIBLE|SS_LEFT,458,66,60,12,hwnd,NULL,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"edit",L"",WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL|ES_RIGHT|ES_READONLY,544,63,80,20,hwnd,(HMENU)DLG_EDIT_S_Draw,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"static",L"Lose",WS_CHILD|WS_VISIBLE|SS_LEFT,458,95,60,12,hwnd,NULL,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"edit",L"",WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL|ES_RIGHT|ES_READONLY,544,92,80,20,hwnd,(HMENU)DLG_EDIT_S_Lose,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"static",L"Score",WS_CHILD|WS_VISIBLE|SS_LEFT,458,124,60,12,hwnd,NULL,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"button",L"*2",WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_CHECKBOX,502,123,36,16,hwnd,(HMENU)DLG_CHK_S_DoubleScore,hInstanceDlg,NULL);
		SendMessage(hwndChild,BM_SETCHECK,BST_CHECKED,0);
		hwndChild=CreateWindowW(L"edit",L"",WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL|ES_RIGHT|ES_READONLY,544,121,80,20,hwnd,(HMENU)DLG_EDIT_S_Score,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"button",L"End",WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_PUSHBUTTON,458,148,50,23,hwnd,(HMENU)DLG_BTN_S_GameEnd,hInstanceDlg,NULL);
		EnableWindow(hwndChild,0);
		hwndChild=CreateWindowW(L"button",L"Start",WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_PUSHBUTTON,514,148,50,23,hwnd,(HMENU)DLG_BTN_S_GameStart,hInstanceDlg,NULL);
		EnableWindow(hwndChild,0);
		hwndChild=CreateWindowW(L"button",L"State",WS_CHILD|WS_VISIBLE|BS_CHECKBOX,570,152,54,16,hwnd,(HMENU)DLG_CHK_S_GameState,hInstanceDlg,NULL);
		EnableWindow(hwndChild,0);


		hwndChild=CreateWindowW(L"button",L"MCU",WS_CHILD|WS_VISIBLE|BS_GROUPBOX,12,184,618,194,hwnd,NULL,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"static",L"COM",WS_CHILD|WS_VISIBLE|SS_LEFT,18,209,60,12,hwnd,NULL,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"combobox",L"",WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_TABSTOP|CBS_DROPDOWNLIST|CBS_HASSTRINGS,84,206,100,200,hwnd,(HMENU)DLG_COMBO_M_COM,hInstanceDlg,NULL);
		for (i=1;i<=256;++i){
			swprintf(wbuf,L"COM%d",i);
			SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)wbuf);
		}
		SendMessage(hwndChild,CB_SETCURSEL,2,0);
		hwndChild=CreateWindowW(L"static",L"BaudRate",WS_CHILD|WS_VISIBLE|SS_LEFT,18,238,60,12,hwnd,NULL,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"combobox",L"",WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_TABSTOP|CBS_DROPDOWNLIST|CBS_HASSTRINGS,84,235,100,200,hwnd,(HMENU)DLG_COMBO_M_BaudRate,hInstanceDlg,NULL);
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)L"230400");
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)L"115200");
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)L"57600");
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)L"38400");
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)L"28800");
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)L"19200");
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)L"14400");
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)L"9600");
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)L"7200");
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)L"4800");
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)L"2400");
		SendMessage(hwndChild,CB_SETCURSEL,1,0);
		hwndChild=CreateWindowW(L"static",L"ByteSize",WS_CHILD|WS_VISIBLE|SS_LEFT,18,267,60,12,hwnd,NULL,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"combobox",L"",WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_TABSTOP|CBS_DROPDOWNLIST|CBS_HASSTRINGS,84,264,100,200,hwnd,(HMENU)DLG_COMBO_M_ByteSize,hInstanceDlg,NULL);
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)L"8");
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)L"7");
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)L"6");
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)L"5");
		SendMessage(hwndChild,CB_SETCURSEL,0,0);
		hwndChild=CreateWindowW(L"static",L"StopBits",WS_CHILD|WS_VISIBLE|SS_LEFT,18,296,60,12,hwnd,NULL,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"combobox",L"",WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_TABSTOP|CBS_DROPDOWNLIST|CBS_HASSTRINGS,84,293,100,200,hwnd,(HMENU)DLG_COMBO_M_StopBits,hInstanceDlg,NULL);
		swprintf(wbuf,L"%d: 1",ONESTOPBIT);
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)wbuf);
		swprintf(wbuf,L"%d: 1.5",ONE5STOPBITS);
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)wbuf);
		swprintf(wbuf,L"%d: 2",TWOSTOPBITS);
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)wbuf);
		SendMessage(hwndChild,CB_SETCURSEL,0,0);
		hwndChild=CreateWindowW(L"static",L"Parity",WS_CHILD|WS_VISIBLE|SS_LEFT,18,325,60,12,hwnd,NULL,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"combobox",L"",WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_TABSTOP|CBS_DROPDOWNLIST|CBS_HASSTRINGS,84,322,100,200,hwnd,(HMENU)DLG_COMBO_M_Parity,hInstanceDlg,NULL);
		swprintf(wbuf,L"%d: None",NOPARITY);
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)wbuf);
		swprintf(wbuf,L"%d: Odd",ODDPARITY);
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)wbuf);
		swprintf(wbuf,L"%d: Even",EVENPARITY);
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)wbuf);
		swprintf(wbuf,L"%d: 1",MARKPARITY);
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)wbuf);
		swprintf(wbuf,L"%d: 0",SPACEPARITY);
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)wbuf);
		SendMessage(hwndChild,CB_SETCURSEL,1,0);
		hwndChild=CreateWindowW(L"button",L"Start",WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_PUSHBUTTON,76,349,50,23,hwnd,(HMENU)DLG_BTN_M_Start,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"button",L"Test",WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_PUSHBUTTON,134,349,50,23,hwnd,(HMENU)DLG_BTN_M_Test,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"button",L"Stop",WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_PUSHBUTTON,18,349,50,23,hwnd,(HMENU)DLG_BTN_M_Stop,hInstanceDlg,NULL);
		EnableWindow(hwndChild,0);

		hwndChild=CreateWindowW(L"static",NULL,WS_CHILD|WS_VISIBLE|SS_BITMAP,190,204,128,64,hwnd,(HMENU)DLG_STATIC_M_Screen1,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"static",NULL,WS_CHILD|WS_VISIBLE|SS_BITMAP,324,204,128,64,hwnd,(HMENU)DLG_STATIC_M_Screen2,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"button",L"",WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX,247,274,15,14,hwnd,(HMENU)DLG_CHK_M_IR1,hInstanceDlg,NULL);
		SendMessage(hwndChild,BM_SETCHECK,BST_CHECKED,0);
		EnableWindow(hwndChild,0);
		hwndChild=CreateWindowW(L"button",L"",WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX,380,274,15,14,hwnd,(HMENU)DLG_CHK_M_IR2,hInstanceDlg,NULL);
		SendMessage(hwndChild,BM_SETCHECK,BST_CHECKED,0);
		EnableWindow(hwndChild,0);
		hwndChild=CreateWindowW(L"static",L"",WS_CHILD|WS_VISIBLE|SS_CENTER,190,274,32,32,hwnd,(HMENU)DLG_STATIC_M_LED1,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"static",L"",WS_CHILD|WS_VISIBLE|SS_CENTER,420,274,32,32,hwnd,(HMENU)DLG_STATIC_M_LED2,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"button",L"Rock",WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_PUSHBUTTON,190,320,82,23,hwnd,(HMENU)DLG_BTN_M_Rock,hInstanceDlg,NULL);
		EnableWindow(hwndChild,0);
		hwndChild=CreateWindowW(L"button",L"Scissors",WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_PUSHBUTTON,280,320,82,23,hwnd,(HMENU)DLG_BTN_M_Scissors,hInstanceDlg,NULL);
		EnableWindow(hwndChild,0);
		hwndChild=CreateWindowW(L"button",L"Paper",WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_PUSHBUTTON,370,320,82,23,hwnd,(HMENU)DLG_BTN_M_Paper,hInstanceDlg,NULL);
		EnableWindow(hwndChild,0);

		hwndChild=CreateWindowW(L"edit",L"",WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL|ES_READONLY,190,351,128,20,hwnd,(HMENU)DLG_EDIT_M_Status,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"edit",L"",WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL|ES_READONLY,324,351,128,20,hwnd,(HMENU)DLG_EDIT_M_Response,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"static",L"Win",WS_CHILD|WS_VISIBLE|SS_LEFT,458,209,110,12,hwnd,NULL,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"edit",L"",WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL|ES_RIGHT|ES_READONLY,574,206,50,20,hwnd,(HMENU)DLG_EDIT_M_Win,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"static",L"Draw",WS_CHILD|WS_VISIBLE|SS_LEFT,458,238,110,12,hwnd,NULL,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"edit",L"",WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL|ES_RIGHT|ES_READONLY,574,235,50,20,hwnd,(HMENU)DLG_EDIT_M_Draw,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"static",L"Lose",WS_CHILD|WS_VISIBLE|SS_LEFT,458,267,110,12,hwnd,NULL,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"edit",L"",WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL|ES_RIGHT|ES_READONLY,574,264,50,20,hwnd,(HMENU)DLG_EDIT_M_Lose,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"static",L"DisplayTime",WS_CHILD|WS_VISIBLE|SS_LEFT,458,296,110,12,hwnd,NULL,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"edit",L"5",WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL|WS_TABSTOP|ES_RIGHT,574,293,50,20,hwnd,(HMENU)DLG_EDIT_M_DisplayTime,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"static",L"ClearTime",WS_CHILD|WS_VISIBLE|SS_LEFT,458,325,110,12,hwnd,NULL,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"edit",L"2",WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL|WS_TABSTOP|ES_RIGHT,574,322,50,20,hwnd,(HMENU)DLG_EDIT_M_ClearTime,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"static",L"LoseTime",WS_CHILD|WS_VISIBLE|SS_LEFT,458,354,110,12,hwnd,NULL,hInstanceDlg,NULL);
		hwndChild=CreateWindowW(L"edit",L"3",WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL|WS_TABSTOP|ES_RIGHT,574,351,50,20,hwnd,(HMENU)DLG_EDIT_M_LoseTime,hInstanceDlg,NULL);

		EnumChildWindows(hwnd,EnumChildProc,0);
		QueryPerformanceCounter(&l);
		srand(l.LowPart);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)){
		case DLG_BTN_S_Start:
			EnableWindow(hwnd,0);
			GetDlgItemTextA(hwnd,DLG_EDIT_S_IP,ip,tbuflen);
			sain.sin_addr.S_un.S_addr=inet_addr(ip);
			GetDlgItemTextA(hwnd,DLG_EDIT_S_Port,cbuf,tbuflen);
			sscanf(cbuf,"%d",&port);
			sain.sin_port=htons(port);
			GetDlgItemTextA(hwnd,DLG_EDIT_S_InPath,path,tbuflen);
			sprintf(get,getHeader,path,ip,port);
			getlen=strlen(get);
			temp=WSAStartup(MAKEWORD(2,0),&wsadata);
			if (temp){
				swprintf(wbuf,L"WSAStartup error %d",temp);
				SetDlgItemTextW(hwnd,DLG_EDIT_S_Status,wbuf);
				goto end_DLG_BTN_S_Start;
			}
			sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
			if (sock==INVALID_SOCKET){
				WSACleanup();
				swprintf(wbuf,L"socket error %d",WSAGetLastError());
				SetDlgItemTextW(hwnd,DLG_EDIT_S_Status,wbuf);
				goto end_DLG_BTN_S_Start;
			}
			temp=connect(sock,(SOCKADDR *)&sain,sizeof(SOCKADDR_IN));
			if (temp){
				closesocket(sock);
				WSACleanup();
				swprintf(wbuf,L"connect error %d",temp);
				SetDlgItemTextW(hwnd,DLG_EDIT_S_Status,wbuf);
				goto end_DLG_BTN_S_Start;
			}
			if (send(sock,get,getlen,0)==SOCKET_ERROR){
				closesocket(sock);
				WSACleanup();
				SendMessage(hwnd,WM_COMMAND,DLG_BTN_S_Stop,0);
				SetDlgItemTextW(hwnd,DLG_EDIT_S_Status,L"Disconnected");
				return TRUE;
			}
			for (i=0;i<tbuflen;i+=temp){
				temp=recv(sock,tcpbuf+i,tbuflen-i,0);
				if (temp==SOCKET_ERROR){
					closesocket(sock);
					WSACleanup();
					SendMessage(hwnd,WM_COMMAND,DLG_BTN_S_Stop,0);
					SetDlgItemTextW(hwnd,DLG_EDIT_S_Status,L"Disconnected");
					return TRUE;
				}
				if (temp<=0) break;
			}
			tcpbuf[i]=0;
			closesocket(sock);
			WSACleanup();
			SetDlgItemTextA(hwnd,DLG_EDIT_S_Response,tcpbuf);
			if (strstr(tcpbuf,"HTTP/1.1 200")!=tcpbuf){
				SetDlgItemTextW(hwnd,DLG_EDIT_S_Status,L"Bad response");
			}else{
				GetDlgItemTextA(hwnd,DLG_EDIT_S_OutPath,path,tbuflen);
				if (doublescore){
					sprintf(postcontrol[PR_WIN].data,postHeader,path,ip,port,strlen(postVictoryContents),postVictoryContents);
				}else{
					sprintf(postcontrol[PR_WIN].data,postHeader,path,ip,port,strlen(postWinContents),postWinContents);
				}
				postcontrol[PR_WIN].length=strlen(postcontrol[PR_WIN].data);
				sprintf(postcontrol[PR_DRAW].data,postHeader,path,ip,port,strlen(postDrawContents),postDrawContents);
				postcontrol[PR_DRAW].length=strlen(postcontrol[PR_DRAW].data);
				sprintf(postcontrol[PR_LOSE].data,postHeader,path,ip,port,strlen(postLoseContents),postLoseContents);
				postcontrol[PR_LOSE].length=strlen(postcontrol[PR_LOSE].data);
				postcontrol[PR_WIN].pending=0;
				postcontrol[PR_DRAW].pending=0;
				postcontrol[PR_LOSE].pending=0;
				postp=&postcontrol[PR_WIN];
				SetDlgItemTextW(hwnd,DLG_EDIT_S_Win,L"");
				SetDlgItemTextW(hwnd,DLG_EDIT_S_Draw,L"");
				SetDlgItemTextW(hwnd,DLG_EDIT_S_Lose,L"");
				SetDlgItemTextW(hwnd,DLG_EDIT_S_Score,L"");
				SetDlgItemTextW(hwnd,DLG_EDIT_S_Status,L"Ready");
				EnableWindow(GetDlgItem(hwnd,DLG_EDIT_S_IP),0);
				EnableWindow(GetDlgItem(hwnd,DLG_EDIT_S_Port),0);
				EnableWindow(GetDlgItem(hwnd,DLG_EDIT_S_InPath),0);
				EnableWindow(GetDlgItem(hwnd,DLG_EDIT_S_OutPath),0);
				EnableWindow(GetDlgItem(hwnd,DLG_CHK_S_DoubleScore),0);
				EnableWindow(GetDlgItem(hwnd,DLG_BTN_S_Start),0);
				EnableWindow(GetDlgItem(hwnd,DLG_BTN_S_Test),0);
				EnableWindow(GetDlgItem(hwnd,DLG_BTN_S_Stop),1);
				EnableWindow(GetDlgItem(hwnd,DLG_BTN_S_GameStart),0);
				EnableWindow(GetDlgItem(hwnd,DLG_BTN_S_GameEnd),0);
				getcounter=1;
				postcounter=1;
				SetTimer(hwnd,TMR_TCP,tcpinterval,NULL);
			}
end_DLG_BTN_S_Start:
			EnableWindow(hwnd,1);
			return TRUE;
		case DLG_BTN_S_Test:
			EnableWindow(hwnd,0);
			postp=NULL;
			postcontrol[PR_WIN].count=0;
			postcontrol[PR_DRAW].count=0;
			postcontrol[PR_LOSE].count=0;
			postcontrol[PR_WIN].pending=0;
			postcontrol[PR_DRAW].pending=0;
			postcontrol[PR_LOSE].pending=0;
			SetDlgItemTextW(hwnd,DLG_EDIT_S_Win,L"0");
			SetDlgItemTextW(hwnd,DLG_EDIT_S_Draw,L"0");
			SetDlgItemTextW(hwnd,DLG_EDIT_S_Lose,L"0");
			SetDlgItemTextW(hwnd,DLG_EDIT_S_Score,L"0");
			SetDlgItemTextW(hwnd,DLG_EDIT_S_Status,L"Test mode");
			SetDlgItemTextW(hwnd,DLG_EDIT_S_Response,L"");
			EnableWindow(GetDlgItem(hwnd,DLG_EDIT_S_IP),0);
			EnableWindow(GetDlgItem(hwnd,DLG_EDIT_S_Port),0);
			EnableWindow(GetDlgItem(hwnd,DLG_EDIT_S_InPath),0);
			EnableWindow(GetDlgItem(hwnd,DLG_EDIT_S_OutPath),0);
			EnableWindow(GetDlgItem(hwnd,DLG_CHK_S_DoubleScore),0);
			EnableWindow(GetDlgItem(hwnd,DLG_BTN_S_Start),0);
			EnableWindow(GetDlgItem(hwnd,DLG_BTN_S_Test),0);
			EnableWindow(GetDlgItem(hwnd,DLG_BTN_S_Stop),1);
			EnableWindow(GetDlgItem(hwnd,DLG_BTN_S_GameStart),1);
			EnableWindow(GetDlgItem(hwnd,DLG_BTN_S_GameEnd),1);
			SetTimer(hwnd,TMR_TCP,tcpinterval,NULL);
			EnableWindow(hwnd,1);
			return TRUE;
		case DLG_BTN_S_Stop:
			EnableWindow(hwnd,0);
			KillTimer(hwnd,TMR_TCP);
			SetDlgItemTextW(hwnd,DLG_EDIT_S_Status,L"");
			EnableWindow(GetDlgItem(hwnd,DLG_EDIT_S_IP),1);
			EnableWindow(GetDlgItem(hwnd,DLG_EDIT_S_Port),1);
			EnableWindow(GetDlgItem(hwnd,DLG_EDIT_S_InPath),1);
			EnableWindow(GetDlgItem(hwnd,DLG_EDIT_S_OutPath),1);
			EnableWindow(GetDlgItem(hwnd,DLG_CHK_S_DoubleScore),1);
			EnableWindow(GetDlgItem(hwnd,DLG_BTN_S_Start),1);
			EnableWindow(GetDlgItem(hwnd,DLG_BTN_S_Test),1);
			EnableWindow(GetDlgItem(hwnd,DLG_BTN_S_Stop),0);
			EnableWindow(GetDlgItem(hwnd,DLG_BTN_S_GameStart),0);
			EnableWindow(GetDlgItem(hwnd,DLG_BTN_S_GameEnd),0);
			EnableWindow(hwnd,1);
			return TRUE;
		case DLG_BTN_S_GameStart:
			if (!gamestate){
				gamestate=1;
				SendDlgItemMessage(hwnd,DLG_CHK_S_GameState,BM_SETCHECK,BST_CHECKED,0);
				if (hcom!=INVALID_HANDLE_VALUE){
					memcpy(outbuf,startcmdtemplate,4);
					temp=UART_ExchangeData(4,0);
					if (temp<0){
						SendMessage(hwnd,WM_COMMAND,DLG_BTN_M_Stop,0);
						SetDlgItemTextW(hwnd,DLG_EDIT_M_Status,L"Disconnected");
						return TRUE;
					}
					wbuf[0]=0;
					for (i=0;i<temp;++i){
						swprintf(wbuf+i*3,L"%02X ",inbuf[i]);
					}
					SetDlgItemTextW(hwnd,DLG_EDIT_M_Response,wbuf);
					outbuf[0]=UARTRSP_OK;
					if (temp!=4||memcmp(outbuf,inbuf,4)){
						SetDlgItemTextW(hwnd,DLG_EDIT_M_Status,L"Bad response");
						return TRUE;
					}
					SetDlgItemTextW(hwnd,DLG_EDIT_M_Status,L"OK");
				}else if (emu){
					emustate1=ST_NULL;
					emustate2=ST_NULL;
					SendMessage(hwnd,WM_TIMER,TMR_EMU1,0);
					SendMessage(hwnd,WM_TIMER,TMR_EMU2,0);
				}
			}
			return TRUE;
		case DLG_BTN_S_GameEnd:
			if (gamestate){
				gamestate=0;
				SendDlgItemMessage(hwnd,DLG_CHK_S_GameState,BM_SETCHECK,BST_UNCHECKED,0);
				if (hcom!=INVALID_HANDLE_VALUE){
					memcpy(outbuf,stopcmdtemplate,4);
					temp=UART_ExchangeData(4,0);
					if (temp<0){
						SendMessage(hwnd,WM_COMMAND,DLG_BTN_M_Stop,0);
						SetDlgItemTextW(hwnd,DLG_EDIT_M_Status,L"Disconnected");
						return TRUE;
					}
					wbuf[0]=0;
					for (i=0;i<temp;++i){
						swprintf(wbuf+i*3,L"%02X ",inbuf[i]);
					}
					SetDlgItemTextW(hwnd,DLG_EDIT_M_Response,wbuf);
					outbuf[0]=UARTRSP_OK;
					if (temp!=4||memcmp(outbuf,inbuf,4)){
						SetDlgItemTextW(hwnd,DLG_EDIT_M_Status,L"Bad response");
						return TRUE;
					}
					SetDlgItemTextW(hwnd,DLG_EDIT_M_Status,L"OK");
				}else if (emu){
					KillTimer(hwnd,TMR_EMU1);
					KillTimer(hwnd,TMR_EMU2);
					KillTimer(hwnd,TMR_LED1);
					KillTimer(hwnd,TMR_LED2);
					SendDlgItemMessage(hwnd,DLG_STATIC_M_Screen1,STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)(image[ST_NULL]));
					SendDlgItemMessage(hwnd,DLG_STATIC_M_Screen2,STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)(image[ST_NULL]));
					SetDlgItemTextW(hwnd,DLG_STATIC_M_LED1,ledPattern[2]);
					SetDlgItemTextW(hwnd,DLG_STATIC_M_LED2,ledPattern[2]);
				}
			}
			return TRUE;
		case DLG_CHK_S_DoubleScore:
			doublescore^=1;
			if (doublescore){
				SendDlgItemMessage(hwnd,DLG_CHK_S_DoubleScore,BM_SETCHECK,BST_CHECKED,0);
				SetDlgItemTextW(hwnd,DLG_STATIC_S_Win,L"Victory");
			}else{
				SendDlgItemMessage(hwnd,DLG_CHK_S_DoubleScore,BM_SETCHECK,BST_UNCHECKED,0);
				SetDlgItemTextW(hwnd,DLG_STATIC_S_Win,L"Win");
			}
			return TRUE;
		case DLG_BTN_M_Start:
			EnableWindow(hwnd,0);
			GetDlgItemTextW(hwnd,DLG_COMBO_M_COM,wbuf,tbuflen);
			hcom=CreateFileW(wbuf,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
			if (hcom==INVALID_HANDLE_VALUE){
				swprintf(wbuf2,L"Open %s failed",wbuf);
				SetDlgItemTextW(hwnd,DLG_EDIT_M_Status,wbuf2);
				goto end_DLG_BTN_M_Start;
			}
			SetupComm(hcom,tbuflen,tbuflen);
			SetCommTimeouts(hcom,&cto);
			SetCommMask(hcom,EV_TXEMPTY);
			GetCommState(hcom,&dcb);
			GetDlgItemTextW(hwnd,DLG_COMBO_M_BaudRate,wbuf,tbuflen);
			swscanf(wbuf,L"%d",&dcb.BaudRate);
			GetDlgItemTextW(hwnd,DLG_COMBO_M_ByteSize,wbuf,tbuflen);
			swscanf(wbuf,L"%d",&dcb.ByteSize);
			GetDlgItemTextW(hwnd,DLG_COMBO_M_StopBits,wbuf,tbuflen);
			swscanf(wbuf,L"%d",&dcb.StopBits);
			GetDlgItemTextW(hwnd,DLG_COMBO_M_Parity,wbuf,tbuflen);
			swscanf(wbuf,L"%d",&dcb.Parity);
			SetCommState(hcom,&dcb);
			mcustatus[BF_SYNCSTATE]=0;
			mcustatus[BF_TID]=rand()%0xFF+1;
			GetDlgItemTextW(hwnd,DLG_EDIT_M_DisplayTime,wbuf,tbuflen);
			swscanf(wbuf,L"%d",&temp);
			mcustatus[BF_DISPLAYTIME]=temp;
			GetDlgItemTextW(hwnd,DLG_EDIT_M_ClearTime,wbuf,tbuflen);
			swscanf(wbuf,L"%d",&temp);
			mcustatus[BF_CLEARTIME]=temp;
			GetDlgItemTextW(hwnd,DLG_EDIT_M_LoseTime,wbuf,tbuflen);
			swscanf(wbuf,L"%d",&temp);
			mcustatus[BF_LOSETIME]=temp;
			memcpy(outbuf,initcmdtemplate,8);
			outbuf[1]=rand();
			outbuf[2]=rand();
			outbuf[3]=rand();
			memcpy(outbuf+8,mcustatus+BF_TID,4);
			memcpy(outbuf+12,mcustatus+BF_TID,4);
			temp=UART_ExchangeData(16,10);
			if (temp<0){
				CloseHandle(hcom);
				hcom=INVALID_HANDLE_VALUE;
				SetDlgItemTextW(hwnd,DLG_EDIT_M_Status,L"Disconnected");
				goto end_DLG_BTN_M_Start;
			}
			if (!temp){
				CloseHandle(hcom);
				hcom=INVALID_HANDLE_VALUE;
				SetDlgItemTextW(hwnd,DLG_EDIT_M_Status,L"No response");
				SetDlgItemTextW(hwnd,DLG_EDIT_M_Response,L"");
				goto end_DLG_BTN_M_Start;
			}
			for (i=0;i<temp;++i){
				swprintf(wbuf+i*3,L"%02X ",inbuf[i]);
			}
			SetDlgItemTextW(hwnd,DLG_EDIT_M_Response,wbuf);
			outbuf[4]=outbuf[0]=UARTRSP_OK;
			if (temp!=8||memcmp(outbuf,inbuf,8)){
				CloseHandle(hcom);
				hcom=INVALID_HANDLE_VALUE;
				SetDlgItemTextW(hwnd,DLG_EDIT_M_Status,L"Bad response");
				goto end_DLG_BTN_M_Start;
			}
			SetDlgItemTextW(hwnd,DLG_EDIT_M_Status,L"Ready");
			EnableWindow(GetDlgItem(hwnd,DLG_COMBO_M_COM),0);
			EnableWindow(GetDlgItem(hwnd,DLG_COMBO_M_BaudRate),0);
			EnableWindow(GetDlgItem(hwnd,DLG_COMBO_M_ByteSize),0);
			EnableWindow(GetDlgItem(hwnd,DLG_COMBO_M_StopBits),0);
			EnableWindow(GetDlgItem(hwnd,DLG_COMBO_M_Parity),0);
			EnableWindow(GetDlgItem(hwnd,DLG_EDIT_M_DisplayTime),0);
			EnableWindow(GetDlgItem(hwnd,DLG_EDIT_M_ClearTime),0);
			EnableWindow(GetDlgItem(hwnd,DLG_EDIT_M_LoseTime),0);
			EnableWindow(GetDlgItem(hwnd,DLG_BTN_M_Start),0);
			EnableWindow(GetDlgItem(hwnd,DLG_BTN_M_Test),0);
			EnableWindow(GetDlgItem(hwnd,DLG_BTN_M_Stop),1);
			EnableWindow(GetDlgItem(hwnd,DLG_CHK_M_IR1),0);
			EnableWindow(GetDlgItem(hwnd,DLG_CHK_M_IR2),0);
			EnableWindow(GetDlgItem(hwnd,DLG_BTN_M_Rock),0);
			EnableWindow(GetDlgItem(hwnd,DLG_BTN_M_Scissors),0);
			EnableWindow(GetDlgItem(hwnd,DLG_BTN_M_Paper),0);
			SetTimer(hwnd,TMR_COM,cominterval,NULL);
end_DLG_BTN_M_Start:
			EnableWindow(hwnd,1);
			return TRUE;
		case DLG_BTN_M_Test:
			EnableWindow(hwnd,0);
			GetDlgItemTextW(hwnd,DLG_EDIT_M_DisplayTime,wbuf,tbuflen);
			swscanf(wbuf,L"%d",&temp);
			mcustatus[BF_DISPLAYTIME]=temp;
			GetDlgItemTextW(hwnd,DLG_EDIT_M_ClearTime,wbuf,tbuflen);
			swscanf(wbuf,L"%d",&temp);
			mcustatus[BF_CLEARTIME]=temp;
			GetDlgItemTextW(hwnd,DLG_EDIT_M_LoseTime,wbuf,tbuflen);
			swscanf(wbuf,L"%d",&temp);
			mcustatus[BF_LOSETIME]=temp;
			emu=1;
			SendDlgItemMessage(hwnd,DLG_STATIC_M_Screen1,STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)(image[ST_NULL]));
			SendDlgItemMessage(hwnd,DLG_STATIC_M_Screen2,STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)(image[ST_NULL]));
			SetDlgItemTextW(hwnd,DLG_STATIC_M_LED1,ledPattern[2]);
			SetDlgItemTextW(hwnd,DLG_STATIC_M_LED2,ledPattern[2]);
			SetDlgItemTextW(hwnd,DLG_EDIT_M_Win,L"");
			SetDlgItemTextW(hwnd,DLG_EDIT_M_Draw,L"");
			SetDlgItemTextW(hwnd,DLG_EDIT_M_Lose,L"");
			SetDlgItemTextW(hwnd,DLG_EDIT_M_Status,L"Test mode");
			SetDlgItemTextW(hwnd,DLG_EDIT_M_Response,L"");
			EnableWindow(GetDlgItem(hwnd,DLG_COMBO_M_COM),0);
			EnableWindow(GetDlgItem(hwnd,DLG_COMBO_M_BaudRate),0);
			EnableWindow(GetDlgItem(hwnd,DLG_COMBO_M_ByteSize),0);
			EnableWindow(GetDlgItem(hwnd,DLG_COMBO_M_StopBits),0);
			EnableWindow(GetDlgItem(hwnd,DLG_COMBO_M_Parity),0);
			EnableWindow(GetDlgItem(hwnd,DLG_EDIT_M_DisplayTime),0);
			EnableWindow(GetDlgItem(hwnd,DLG_EDIT_M_ClearTime),0);
			EnableWindow(GetDlgItem(hwnd,DLG_EDIT_M_LoseTime),0);
			EnableWindow(GetDlgItem(hwnd,DLG_BTN_M_Start),0);
			EnableWindow(GetDlgItem(hwnd,DLG_BTN_M_Test),0);
			EnableWindow(GetDlgItem(hwnd,DLG_BTN_M_Stop),1);
			EnableWindow(GetDlgItem(hwnd,DLG_CHK_M_IR1),1);
			EnableWindow(GetDlgItem(hwnd,DLG_CHK_M_IR2),1);
			EnableWindow(GetDlgItem(hwnd,DLG_BTN_M_Rock),1);
			EnableWindow(GetDlgItem(hwnd,DLG_BTN_M_Scissors),1);
			EnableWindow(GetDlgItem(hwnd,DLG_BTN_M_Paper),1);
			EnableWindow(hwnd,1);
			return TRUE;
		case DLG_BTN_M_Stop:
			EnableWindow(hwnd,0);
			KillTimer(hwnd,TMR_COM);
			KillTimer(hwnd,TMR_EMU1);
			KillTimer(hwnd,TMR_EMU2);
			KillTimer(hwnd,TMR_LED1);
			KillTimer(hwnd,TMR_LED2);
			if (hcom!=INVALID_HANDLE_VALUE){
				CloseHandle(hcom);
				hcom=INVALID_HANDLE_VALUE;
			}
			emu=0;
			SendDlgItemMessage(hwnd,DLG_STATIC_M_Screen1,STM_SETIMAGE,IMAGE_BITMAP,0);
			SendDlgItemMessage(hwnd,DLG_STATIC_M_Screen2,STM_SETIMAGE,IMAGE_BITMAP,0);
			SetDlgItemTextW(hwnd,DLG_STATIC_M_LED1,L"");
			SetDlgItemTextW(hwnd,DLG_STATIC_M_LED2,L"");
			SetDlgItemTextW(hwnd,DLG_EDIT_M_Status,L"");
			EnableWindow(GetDlgItem(hwnd,DLG_COMBO_M_COM),1);
			EnableWindow(GetDlgItem(hwnd,DLG_COMBO_M_BaudRate),1);
			EnableWindow(GetDlgItem(hwnd,DLG_COMBO_M_ByteSize),1);
			EnableWindow(GetDlgItem(hwnd,DLG_COMBO_M_StopBits),1);
			EnableWindow(GetDlgItem(hwnd,DLG_COMBO_M_Parity),1);
			EnableWindow(GetDlgItem(hwnd,DLG_EDIT_M_DisplayTime),1);
			EnableWindow(GetDlgItem(hwnd,DLG_EDIT_M_ClearTime),1);
			EnableWindow(GetDlgItem(hwnd,DLG_EDIT_M_LoseTime),1);
			EnableWindow(GetDlgItem(hwnd,DLG_BTN_M_Start),1);
			EnableWindow(GetDlgItem(hwnd,DLG_BTN_M_Test),1);
			EnableWindow(GetDlgItem(hwnd,DLG_BTN_M_Stop),0);
			EnableWindow(GetDlgItem(hwnd,DLG_CHK_M_IR1),0);
			EnableWindow(GetDlgItem(hwnd,DLG_CHK_M_IR2),0);
			EnableWindow(GetDlgItem(hwnd,DLG_BTN_M_Rock),0);
			EnableWindow(GetDlgItem(hwnd,DLG_BTN_M_Scissors),0);
			EnableWindow(GetDlgItem(hwnd,DLG_BTN_M_Paper),0);
			EnableWindow(hwnd,1);
			return TRUE;
		case DLG_BTN_M_Rock:
			if (!gamestate) return TRUE;
			if (SendDlgItemMessageW(hwnd,DLG_CHK_M_IR1,BM_GETCHECK,0,0)==BST_CHECKED){
				switch (emustate1){
				case ST_ROCK:
					++postcontrol[PR_DRAW].pending;
					emustate1=ST_DRAW;
					SendDlgItemMessage(hwnd,DLG_STATIC_M_Screen1,STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)(image[emustate1]));
					break;
				case ST_SCISSORS:
					++postcontrol[PR_WIN].pending;
					emustate1=ST_WIN;
					SendDlgItemMessage(hwnd,DLG_STATIC_M_Screen1,STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)(image[emustate1]));
					KillTimer(hwnd,TMR_EMU1);
					SetTimer(hwnd,TMR_EMU1,mcustatus[BF_CLEARTIME]*1000,NULL);
					flashcounter1=flashcount;
					SendMessage(hwnd,WM_TIMER,TMR_LED1,0);
					break;
				case ST_PAPER:
					++postcontrol[PR_LOSE].pending;
					emustate1=ST_LOSE;
					punishing1=1;
					SendDlgItemMessage(hwnd,DLG_STATIC_M_Screen1,STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)(image[emustate1]));
					break;
				}
			}
			if (SendDlgItemMessageW(hwnd,DLG_CHK_M_IR2,BM_GETCHECK,0,0)==BST_CHECKED){
				switch (emustate2){
				case ST_ROCK:
					++postcontrol[PR_DRAW].pending;
					emustate2=ST_DRAW;
					SendDlgItemMessage(hwnd,DLG_STATIC_M_Screen2,STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)(image[emustate2]));
					break;
				case ST_SCISSORS:
					++postcontrol[PR_WIN].pending;
					emustate2=ST_WIN;
					SendDlgItemMessage(hwnd,DLG_STATIC_M_Screen2,STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)(image[emustate2]));
					KillTimer(hwnd,TMR_EMU2);
					SetTimer(hwnd,TMR_EMU2,mcustatus[BF_CLEARTIME]*1000,NULL);
					flashcounter2=flashcount;
					SendMessage(hwnd,WM_TIMER,TMR_LED2,0);
					break;
				case ST_PAPER:
					++postcontrol[PR_LOSE].pending;
					emustate2=ST_LOSE;
					punishing2=1;
					SendDlgItemMessage(hwnd,DLG_STATIC_M_Screen2,STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)(image[emustate2]));
					break;
				}
			}
			return TRUE;
		case DLG_BTN_M_Scissors:
			if (!gamestate) return TRUE;
			if (SendDlgItemMessageW(hwnd,DLG_CHK_M_IR1,BM_GETCHECK,0,0)==BST_CHECKED){
				switch (emustate1){
				case ST_ROCK:
					++postcontrol[PR_LOSE].pending;
					emustate1=ST_LOSE;
					punishing1=1;
					SendDlgItemMessage(hwnd,DLG_STATIC_M_Screen1,STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)(image[emustate1]));
					break;
				case ST_SCISSORS:
					++postcontrol[PR_DRAW].pending;
					emustate1=ST_DRAW;
					SendDlgItemMessage(hwnd,DLG_STATIC_M_Screen1,STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)(image[emustate1]));
					break;
				case ST_PAPER:
					++postcontrol[PR_WIN].pending;
					emustate1=ST_WIN;
					SendDlgItemMessage(hwnd,DLG_STATIC_M_Screen1,STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)(image[emustate1]));
					KillTimer(hwnd,TMR_EMU1);
					SetTimer(hwnd,TMR_EMU1,mcustatus[BF_CLEARTIME]*1000,NULL);
					flashcounter1=flashcount;
					SendMessage(hwnd,WM_TIMER,TMR_LED1,0);
					break;
				}
			}
			if (SendDlgItemMessageW(hwnd,DLG_CHK_M_IR2,BM_GETCHECK,0,0)==BST_CHECKED){
				switch (emustate2){
				case ST_ROCK:
					++postcontrol[PR_LOSE].pending;
					emustate2=ST_LOSE;
					punishing2=1;
					SendDlgItemMessage(hwnd,DLG_STATIC_M_Screen2,STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)(image[emustate2]));
					break;
				case ST_SCISSORS:
					++postcontrol[PR_DRAW].pending;
					emustate2=ST_DRAW;
					SendDlgItemMessage(hwnd,DLG_STATIC_M_Screen2,STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)(image[emustate2]));
					break;
				case ST_PAPER:
					++postcontrol[PR_WIN].pending;
					emustate2=ST_WIN;
					SendDlgItemMessage(hwnd,DLG_STATIC_M_Screen2,STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)(image[emustate2]));
					KillTimer(hwnd,TMR_EMU2);
					SetTimer(hwnd,TMR_EMU2,mcustatus[BF_CLEARTIME]*1000,NULL);
					flashcounter2=flashcount;
					SendMessage(hwnd,WM_TIMER,TMR_LED2,0);
					break;
				}
			}
			return TRUE;
		case DLG_BTN_M_Paper:
			if (!gamestate) return TRUE;
			if (SendDlgItemMessageW(hwnd,DLG_CHK_M_IR1,BM_GETCHECK,0,0)==BST_CHECKED){
				switch (emustate1){
				case ST_ROCK:
					++postcontrol[PR_WIN].pending;
					emustate1=ST_WIN;
					SendDlgItemMessage(hwnd,DLG_STATIC_M_Screen1,STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)(image[emustate1]));
					KillTimer(hwnd,TMR_EMU1);
					SetTimer(hwnd,TMR_EMU1,mcustatus[BF_CLEARTIME]*1000,NULL);
					flashcounter1=flashcount;
					SendMessage(hwnd,WM_TIMER,TMR_LED1,0);
					break;
				case ST_SCISSORS:
					++postcontrol[PR_LOSE].pending;
					emustate1=ST_LOSE;
					punishing1=1;
					SendDlgItemMessage(hwnd,DLG_STATIC_M_Screen1,STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)(image[emustate1]));
					break;
				case ST_PAPER:
					++postcontrol[PR_DRAW].pending;
					emustate1=ST_DRAW;
					SendDlgItemMessage(hwnd,DLG_STATIC_M_Screen1,STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)(image[emustate1]));
					break;
				}
			}
			if (SendDlgItemMessageW(hwnd,DLG_CHK_M_IR2,BM_GETCHECK,0,0)==BST_CHECKED){
				switch (emustate2){
				case ST_ROCK:
					++postcontrol[PR_WIN].pending;
					emustate2=ST_WIN;
					SendDlgItemMessage(hwnd,DLG_STATIC_M_Screen2,STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)(image[emustate2]));
					KillTimer(hwnd,TMR_EMU2);
					SetTimer(hwnd,TMR_EMU2,mcustatus[BF_CLEARTIME]*1000,NULL);
					flashcounter2=flashcount;
					SendMessage(hwnd,WM_TIMER,TMR_LED2,0);
					break;
				case ST_SCISSORS:
					++postcontrol[PR_LOSE].pending;
					emustate2=ST_LOSE;
					punishing2=1;
					SendDlgItemMessage(hwnd,DLG_STATIC_M_Screen2,STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)(image[emustate2]));
					break;
				case ST_PAPER:
					++postcontrol[PR_DRAW].pending;
					emustate2=ST_DRAW;
					SendDlgItemMessage(hwnd,DLG_STATIC_M_Screen2,STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)(image[emustate2]));
					break;
				}
			}
			return TRUE;
		}
		break;
	case WM_TIMER:
		switch (wParam){
		case TMR_TCP:
			if (!postp){
				temp=0;
				for (i=0;i<PR_COUNT;++i){
					if (postcontrol[i].pending){
						temp=1;
						postcontrol[i].count+=postcontrol[i].pending;
						postcontrol[i].pending=0;
					}
				}
				if (temp){
					swprintf(wbuf,L"%d",postcontrol[PR_WIN].count);
					SetDlgItemTextW(hwnd,DLG_EDIT_S_Win,wbuf);
					swprintf(wbuf,L"%d",postcontrol[PR_DRAW].count);
					SetDlgItemTextW(hwnd,DLG_EDIT_S_Draw,wbuf);
					swprintf(wbuf,L"%d",postcontrol[PR_LOSE].count);
					SetDlgItemTextW(hwnd,DLG_EDIT_S_Lose,wbuf);
					swprintf(wbuf,L"%d",postcontrol[PR_WIN].count*(doublescore+1));
					SetDlgItemTextW(hwnd,DLG_EDIT_S_Score,wbuf);
				}
			}else{
				--getcounter;
				--postcounter;
				if (postcounter<=0){
					postcounter=1;
					for (i=PR_COUNT;i;--i){
						postp=postp->next;
						if (postp->pending){
							temp=WSAStartup(MAKEWORD(2,0),&wsadata);
							if (temp) goto TMR_TCPERR;
							sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
							if (sock==INVALID_SOCKET){
								WSACleanup();
								goto TMR_TCPERR;
							}
							temp=connect(sock,(SOCKADDR *)&sain,sizeof(SOCKADDR_IN));
							if (temp){
								closesocket(sock);
								WSACleanup();
								goto TMR_TCPERR;
							}
							if (send(sock,postp->data,postp->length,0)==SOCKET_ERROR){
								closesocket(sock);
								WSACleanup();
								goto TMR_TCPERR;
							}
							for (i=0;i<tbuflen;i+=temp){
								temp=recv(sock,tcpbuf+i,tbuflen-i,0);
								if (temp==SOCKET_ERROR){
									closesocket(sock);
									WSACleanup();
									goto TMR_TCPERR;
								}
								if (temp<=0) break;
							}
							tcpbuf[i]=0;
							closesocket(sock);
							WSACleanup();
							SetDlgItemTextA(hwnd,DLG_EDIT_S_Response,tcpbuf);
							postcounter=postcount;
							if (strstr(tcpbuf,"HTTP/1.1 200")==tcpbuf){
								SetDlgItemTextW(hwnd,DLG_EDIT_S_Status,L"OK");
								--postp->pending;
								return TRUE;
							}
							SetDlgItemTextW(hwnd,DLG_EDIT_S_Status,L"Bad response");
						}
					}
				}
				if (getcounter<=0){
					getcounter=1;
					temp=WSAStartup(MAKEWORD(2,0),&wsadata);
					if (temp) goto TMR_TCPERR;
					sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
					if (sock==INVALID_SOCKET){
						WSACleanup();
						goto TMR_TCPERR;
					}
					temp=connect(sock,(SOCKADDR *)&sain,sizeof(SOCKADDR_IN));
					if (temp){
						closesocket(sock);
						WSACleanup();
						goto TMR_TCPERR;
					}
					if (send(sock,get,getlen,0)==SOCKET_ERROR){
						closesocket(sock);
						WSACleanup();
						goto TMR_TCPERR;
					}
					for (i=0;i<tbuflen;i+=temp){
						temp=recv(sock,tcpbuf+i,tbuflen-i,0);
						if (temp==SOCKET_ERROR){
							closesocket(sock);
							WSACleanup();
							goto TMR_TCPERR;
						}
						if (temp<=0) break;
					}
					tcpbuf[i]=0;
					closesocket(sock);
					WSACleanup();
					SetDlgItemTextA(hwnd,DLG_EDIT_S_Response,tcpbuf);
					getcounter=getcount;
					if (strstr(tcpbuf,getStartContents)){
						SetDlgItemTextW(hwnd,DLG_EDIT_S_Status,L"OK");
						if (!gamestate){
							SendMessage(hwnd,WM_COMMAND,DLG_BTN_S_GameStart,0);
						}
					}else if (strstr(tcpbuf,getEndContents)||strstr(tcpbuf,getWaitContents)){
						SetDlgItemTextW(hwnd,DLG_EDIT_S_Status,L"OK");
						if (gamestate){
							SendMessage(hwnd,WM_COMMAND,DLG_BTN_S_GameEnd,0);
						}
					}else{
						SetDlgItemTextW(hwnd,DLG_EDIT_S_Status,L"Bad response");
					}

				}
			}
			return TRUE;
TMR_TCPERR:
			SetDlgItemTextW(hwnd,DLG_EDIT_S_Status,L"Reconnecting");
			return TRUE;
		case TMR_COM:
			memcpy(outbuf,tickcmdtemplate,4);
			temp=UART_ExchangeData(4,0);
			if (temp<0){
				SendMessage(hwnd,WM_COMMAND,DLG_BTN_M_Stop,0);
				SetDlgItemTextW(hwnd,DLG_EDIT_M_Status,L"Disconnected");
				return TRUE;
			}
			wbuf[0]=0;
			for (i=0;i<temp;++i){
				swprintf(wbuf+i*3,L"%02X ",inbuf[i]);
			}
			SetDlgItemTextW(hwnd,DLG_EDIT_M_Response,wbuf);
			outbuf[0]=UARTRSP_OK;
			if (temp!=12||memcmp(outbuf,inbuf,4)||memcmp(inbuf+4,inbuf+8,4)||mcustatus[BF_TID]!=inbuf[4+BF_TID]){
				SendMessage(hwnd,WM_COMMAND,DLG_BTN_M_Stop,0);
				SetDlgItemTextW(hwnd,DLG_EDIT_M_Status,L"Reconnecting");
				SendMessage(hwnd,WM_COMMAND,DLG_BTN_M_Start,0);
				return TRUE;
			}
			SetDlgItemTextW(hwnd,DLG_EDIT_M_Status,L"OK");
			if (mcustatus[BF_SYNCSTATE]){
				postcontrol[PR_WIN].pending+=(uint8_t)(inbuf[4+BF_WIN]-mcustatus[BF_WIN]);
				postcontrol[PR_DRAW].pending+=(uint8_t)(inbuf[4+BF_DRAW]-mcustatus[BF_DRAW]);
				postcontrol[PR_LOSE].pending+=(uint8_t)(inbuf[4+BF_LOSE]-mcustatus[BF_LOSE]);
			}else{
				mcustatus[BF_SYNCSTATE]=1;
			}
			memcpy(mcustatus,inbuf+4,4);
			swprintf(wbuf,L"%02X",mcustatus[BF_WIN]);
			SetDlgItemTextW(hwnd,DLG_EDIT_M_Win,wbuf);
			swprintf(wbuf,L"%02X",mcustatus[BF_DRAW]);
			SetDlgItemTextW(hwnd,DLG_EDIT_M_Draw,wbuf);
			swprintf(wbuf,L"%02X",mcustatus[BF_LOSE]);
			SetDlgItemTextW(hwnd,DLG_EDIT_M_Lose,wbuf);
			return TRUE;
		case TMR_EMU1:
			KillTimer(hwnd,TMR_EMU1);
			switch (emustate1){
			case ST_LOSE:
				if (punishing1){
					SetTimer(hwnd,TMR_EMU1,mcustatus[BF_LOSETIME]*1000,NULL);
					punishing1=0;
					break;
				}
			case ST_ROCK:
			case ST_SCISSORS:
			case ST_PAPER:
			case ST_DRAW:
				SetTimer(hwnd,TMR_EMU1,mcustatus[BF_CLEARTIME]*1000,NULL);
				emustate1=ST_NULL;
				break;
			case ST_WIN:
			default:
				SetTimer(hwnd,TMR_EMU1,mcustatus[BF_DISPLAYTIME]*1000,NULL);
				emustate1=rand()%3;
				break;
			}
			SendDlgItemMessage(hwnd,DLG_STATIC_M_Screen1,STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)(image[emustate1]));
			return TRUE;
		case TMR_EMU2:
			KillTimer(hwnd,TMR_EMU2);
			switch (emustate2){
			case ST_LOSE:
				if (punishing2){
					SetTimer(hwnd,TMR_EMU2,mcustatus[BF_LOSETIME]*1000,NULL);
					punishing2=0;
					break;
				}
			case ST_ROCK:
			case ST_SCISSORS:
			case ST_PAPER:
			case ST_DRAW:
				SetTimer(hwnd,TMR_EMU2,mcustatus[BF_CLEARTIME]*1000,NULL);
				emustate2=ST_NULL;
				break;
			case ST_WIN:
			default:
				SetTimer(hwnd,TMR_EMU2,mcustatus[BF_DISPLAYTIME]*1000,NULL);
				emustate2=rand()%3;
				break;
			}
			SendDlgItemMessage(hwnd,DLG_STATIC_M_Screen2,STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)(image[emustate2]));
			return TRUE;
		case TMR_LED1:
			KillTimer(hwnd,TMR_LED1);
			if (!flashcounter1){
				SetDlgItemTextW(hwnd,DLG_STATIC_M_LED1,ledPattern[2]);
			}else{
				SetTimer(hwnd,TMR_LED1,flashinterval,NULL);
				SetDlgItemTextW(hwnd,DLG_STATIC_M_LED1,ledPattern[flashcounter1&1]);
				--flashcounter1;
			}
			return TRUE;
		case TMR_LED2:
			KillTimer(hwnd,TMR_LED2);
			if (!flashcounter2){
				SetDlgItemTextW(hwnd,DLG_STATIC_M_LED2,ledPattern[2]);
			}else{
				SetTimer(hwnd,TMR_LED2,flashinterval,NULL);
				SetDlgItemTextW(hwnd,DLG_STATIC_M_LED2,ledPattern[flashcounter2&1]);
				--flashcounter2;
			}
			return TRUE;
		}
		break;
	case WM_CLOSE:
		KillTimer(hwnd,TMR_TCP);
		KillTimer(hwnd,TMR_COM);
		KillTimer(hwnd,TMR_EMU1);
		KillTimer(hwnd,TMR_EMU2);
		KillTimer(hwnd,TMR_LED1);
		KillTimer(hwnd,TMR_LED2);
		if (hcom!=INVALID_HANDLE_VALUE){
			CloseHandle(hcom);
		}
		EndDialog(hwnd,0);
		return TRUE;
	}
	return FALSE;
}

int main(){
	static int argc;
	static WCHAR **argv;
	wchar_t *errmsg;

	argv=CommandLineToArgvW(GetCommandLineW(),&argc);
	errmsg=loadImages();
	if (errmsg){
		swprintf(wbuf,L"Can't open '%s'!",errmsg);
		MessageBoxW(NULL,wbuf,argv[0],MB_ICONERROR);
		return 0;
	}
	sain.sin_family=AF_INET;
	postcontrol[PR_WIN].next=&postcontrol[PR_DRAW];
	postcontrol[PR_DRAW].next=&postcontrol[PR_LOSE];
	postcontrol[PR_LOSE].next=&postcontrol[PR_WIN];
	DialogBoxIndirectParamW(GetModuleHandleW(NULL),&_dlgt.dlgt,NULL,DlgProc,0);
	destroyImages();
	LocalFree(argv);
	return 0;
}

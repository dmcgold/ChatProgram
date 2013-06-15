/*********************************************************************/
/******					Client application						******/
/******				Author: Danny McGoldrick					******/
/******															******/
/******				Diploma Information Technology				******/
/******							2013							******/
/*********************************************************************/
#include "client.h"

#pragma warning(disable: 4996)

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Settings(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	ListtoServers(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK ChangeNickName(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
					   _In_opt_ HINSTANCE hPrevInstance,
					   _In_ LPTSTR    lpCmdLine,
					   _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	setlocale( LC_ALL, "Australia" );

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_CLIENT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}
	_tcscpy(client.ipAddress,TEXT("127.0.0.1"));
	_tcscpy(client.nickName,TEXT("Guest"));
	client.portNo=6000;
	client.next='\0';
	client.inSocket=0;
	_tcscpy(toServer.ipAddress,TEXT("192.168.1.110"));
	toServer.portNo=client.portNo;
	toServer.isConnected=FALSE;

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLIENT));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLIENT));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+2);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_CLIENT);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable

	/*hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);*/
	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		0, 0,1000, 700, NULL, NULL, hInstance, NULL);
	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	int bufferSize=BUFFER_SIZE;
	TCHAR *inBuffer = (TCHAR *) _malloca (sizeof(TCHAR)*BUFFER_SIZE);
	RECT cRect = { 0 };
	int userLength=100;
	clientStruct *tmpClient=&client;

	switch (message)
	{
	case WM_LBUTTONDOWN:
		{
			ShowWindow(userWin,SW_HIDE);
		}
		break;
	case WM_SIZE:
		{
			GetClientRect(hWnd,&cRect);														// Get windows dimmensions if changed
			cRect.bottom-=50;
			SetWindowPos(chatWin,NULL,cRect.left,cRect.top,cRect.right,cRect.bottom,SWP_NOZORDER);	// Resize all the windows`
			cRect.right-=50;
			SetWindowPos(inputWin,NULL,0,cRect.bottom,cRect.right,40,SWP_NOZORDER);
			SetWindowPos(sendBTN,NULL,cRect.right, cRect.bottom, 50, 40,SWP_NOZORDER);
		}
		break;
	case WM_CREATE:
		{
			RECT	Rect,
				rChat;

			GetClientRect(hWnd,&Rect);											// Get client window dimmensions
			rChat = Rect;
			rChat.left=0;
			rChat.top=0;
			rChat.bottom -=20;

			chatWin = CreateWindow(
				TEXT("LISTBOX"),
				NULL,
				WS_CHILD ,
				rChat.left,rChat.top,
				rChat.right,rChat.bottom+5,
				hWnd,
				NULL,
				hInst,
				NULL);

			inputWin=CreateWindow(
				TEXT("EDIT"),
				NULL,
				WS_CHILD|ES_LEFT|ES_AUTOHSCROLL,
				0,rChat.bottom,
				rChat.right-35,40,
				hWnd,
				HMENU(IDE_SEND_TEXT),
				hInst,
				NULL);

			sendBTN=CreateWindow(
				TEXT("BUTTON"),
				TEXT("Send"),
				WS_CHILD|ES_LEFT|WS_BORDER,
				rChat.right-50,rChat.bottom,
				35,40,
				hWnd,
				HMENU(IDB_SEND_TEXT),
				hInst,
				NULL);

			EnableWindow(inputWin,FALSE);

			userWin = CreateWindow (
				TEXT("LISTBOX"),
				TEXT("Users Online"),
				WS_CHILD|ES_LEFT|ES_AUTOHSCROLL,
				200,0,200,500,
				hWnd,
				HMENU(IDC_LIST_USER),
				hInst,
				NULL);
		}
		break;
	case WM_CLIENT_SOCKET:
		{
			int wmEvent=WSAGETSELECTEVENT(lParam);
			int wmError=WSAGETSELECTERROR(lParam);

			switch(wmEvent)
			{
			case FD_CONNECT:
				{
					if(wmError!=NULL)
						DisplayError(NULL,TEXT("Connection Error"),wmError,ALL,ERRORFOUND);
					else
					{
						EnableWindow(GetDlgItem(hWnd,IDB_SEND_TEXT),TRUE);
					}
				}
				break;
			case FD_READ:
				{
					if(wmError!=NULL)
						DisplayError(NULL,TEXT("Read Error"),wmError,ALL,ERRORFOUND);
					ClientIncoming(toServer.inSocket,&client);
				}
				break;
			}
		}
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_LISTUSERS:
			{
				//DisplayUsers(userWin);
			}
			break;
		case IDB_SEND_TEXT:
			{
				_tcscpy(client.sendTo,_T("PUBLIC"));
				SendText(inputWin,toServer.inSocket,&client);
				SendMessage(userWin, LB_ADDSTRING, 0, (LPARAM)client.sendMSG);
				SendMessage(inputWin,WM_SETTEXT,256,(LPARAM)"");
			}
			break;
		case IDM_NICKNAME:
			DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_NICKNAME), hWnd, ChangeNickName,(LPARAM)tmpClient);
			break;
		case IDM_CONNECT:
			{
				if(toServer.isConnected)
					break;
				ConnectToServer(hWnd,&toServer,client);
				if(toServer.isConnected)
				{
					ShowWindow(chatWin,SW_SHOW);
					ShowWindow(inputWin,SW_SHOW);
					ShowWindow(sendBTN,SW_SHOW);
					EnableWindow(inputWin, TRUE);
				}
			}
			break;
		case IDM_DISCONNECT:
			{
				if(!toServer.isConnected)
					break;
				ShutDown(toServer.inSocket);
				ShowWindow(chatWin,SW_HIDE);
				ShowWindow(inputWin,SW_HIDE);
				ShowWindow(sendBTN,SW_HIDE);
				EnableWindow(inputWin, FALSE);
				toServer.isConnected=FALSE;
			}
			break;
		case IDM_LIST_SERVERS:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_LIST_SERVERS), hWnd, ListtoServers);
			break;
		case IDM_SETTINGS:
			{
				//if(!toServer.isConnected)
				//	ZeroMemory(&client,sizeof(client));
				DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_SETTINGS), hWnd, Settings,(LPARAM)&toServer);
			}
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		{
			hdc = BeginPaint(hWnd, &ps);

			EndPaint(hWnd, &ps);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	client=(*tmpClient);
	_freea(inBuffer);
	return 0;
}

INT_PTR CALLBACK Settings(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	BOOL isSigned=FALSE;
	BOOL *didPass=FALSE;
	int len=0;
	static serverStruct *tmptoServer;

	switch (message)
	{
	case WM_INITDIALOG:
		{
			tmptoServer=(serverStruct*)lParam;

			SetDlgItemInt(hDlg,IDC_PORTNO,tmptoServer->portNo,isSigned);
			SetDlgItemText(hDlg,IDC_IPADDRESS,(LPCWSTR)tmptoServer->ipAddress);

			return (INT_PTR)TRUE;
		}

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			if (LOWORD(wParam) == IDOK )
			{																		// then save the new data
				tmptoServer->portNo=GetDlgItemInt(hDlg, IDC_PORTNO, didPass,isSigned);
				GetDlgItemText(hDlg, IDC_IPADDRESS,tmptoServer->ipAddress,INET_ADDRSTRLEN );
			}
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK ListtoServers(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK ChangeNickName(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static clientStruct *tmpClient;

	switch (message)
	{
	case WM_INITDIALOG:
		tmpClient=(clientStruct *)lParam;
		SendDlgItemMessage(hDlg,IDC_NICKNAME,EM_LIMITTEXT,255,0);
		SetDlgItemText(hDlg,IDC_NICKNAME,tmpClient->nickName);
		return (INT_PTR)FALSE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			int len = GetWindowTextLength(GetDlgItem(hDlg, IDC_NICKNAME));
			if((len>0) && (LOWORD(wParam) == IDOK))
				GetDlgItemText(hDlg, IDC_NICKNAME,tmpClient->nickName,len+1);
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
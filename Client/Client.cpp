// Chat Program.cpp : Defines the entry point for the application.
//
#pragma warning(disable: 4996)
#include "stdafx.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
chatStruct client;
boolean isConnected=FALSE;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	ListServers(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Connect(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Disconnect(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Settings(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	NickName(HWND, UINT, WPARAM, LPARAM);
HWND chatWindow;
WSADATA wsaData = {0};
sockaddr_in sAddr;

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

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_CHATPROGRAM, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CHATPROGRAM));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0) > 0)
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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CHATPROGRAM));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_CHATPROGRAM);
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

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

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
	int wmId,
		wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	SOCKADDR_IN clientAddr={0,0,0,0};

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_SETTINGS:
			DialogBox(hInst,MAKEINTRESOURCE(IDD_SETTINGS),hWnd,Settings);
			break;
		case IDM_LISTSERVERS:
			DialogBox(hInst,MAKEINTRESOURCE(IDD_LISTSERVERS),hWnd,ListServers);
			break;
		case IDM_CONNECT:
			{
				if ( WSAStartup(MAKEWORD(2,2), &wsaData ) !=0 )
					return 1;

				sAddr.sin_family = AF_INET;
				sAddr.sin_addr.s_addr = inet_addr( client.ipAddress );
				sAddr.sin_port = htons( client.portNo );

				if ( client.socketClient = socket(AF_INET, SOCK_STREAM,IPPROTO_TCP) == INVALID_SOCKET )
				{
					MessageBox(NULL,TEXT("Socket Initialisation Error"),TEXT("Invalid Socket"),MB_OK );
					return 1;
				}

				connect(client.socketClient,(SOCKADDR*)&clientAddr,sizeof(SOCKADDR));

				chatWindow = CreateWindow(	"LISTBOX",
					"Chat Window",
					WS_CHILD|WS_VISIBLE|ES_LEFT|WS_BORDER|
					WS_VSCROLL|WS_HSCROLL|WS_MAXIMIZEBOX|
					WS_MINIMIZEBOX|WS_OVERLAPPEDWINDOW,
					50,50,
					1000,400,
					hWnd,
					(HMENU)IDW_CHAT,
					hInst,
					NULL);
				isConnected=TRUE;
			}
			break;
		case IDM_DISCONNECT:
			{
				if(isConnected)
				{
					DialogBox(hInst,MAKEINTRESOURCE(IDD_DISCONNECT ),hWnd,Disconnect);
					WSACleanup();
				}
			}
			break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			{
				int iResult = closesocket(client.socketClient);
				if (iResult == SOCKET_ERROR)
				{
					int errorStatus=WSAGetLastError();
					DisplayError("Close socket failed", errorStatus,FALSE);
				}
				DestroyWindow(hWnd);
				WSACleanup();
			}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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

INT_PTR CALLBACK ListServers(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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

INT_PTR CALLBACK Disconnect(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	boolean closeWindow;

	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:

		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			closeWindow = (IsDlgButtonChecked(hDlg,IDC_ACTIVE) || IsDlgButtonChecked(hDlg,IDC_ALL));
			if ((LOWORD(wParam) == IDOK) && closeWindow)
			{
				DestroyWindow(chatWindow);
				int iResult = closesocket(client.socketClient);
				if (iResult == SOCKET_ERROR)
				{
					int errorStatus=WSAGetLastError();
					DisplayError("Close socket failed", errorStatus,FALSE);
				}
			}
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK Connect(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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

INT_PTR CALLBACK Settings(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	BOOL isSigned=FALSE;
	BOOL *didPass=FALSE;

	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			if (LOWORD(wParam))
			{
				int len = GetWindowTextLength(GetDlgItem(hDlg, IDC_NICKNAME));
				if (len>0)
					GetDlgItemText(hDlg, IDC_NICKNAME, client.nickName,len+1);
				else
					strcpy(client.nickName,"User");

				client.privateChat=IsDlgButtonChecked(hDlg,IDC_PRIVATE_CHAT);

				if ( GetWindowTextLength(GetDlgItem(hDlg, IDC_PORTNO)) >0)
					client.portNo=GetDlgItemInt(hDlg,IDC_PORTNO,didPass,isSigned);
				else
					client.portNo=6000;

				len = GetWindowTextLength(GetDlgItem(hDlg, IDC_IPADDRESS));
				if (len>0)
					GetDlgItemText(hDlg, IDC_IPADDRESS, client.ipAddress,len+1);
				else
					strcpy(client.ipAddress,"192.168.1.110");

				MessageBox(hDlg, client.nickName, (client.privateChat ? "Private":"Public"),MB_OK);
			}
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK NickName(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
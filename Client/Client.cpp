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
HWND chatWindow;
WSADATA wsaData = {0};
sockaddr_in sAddr;
SOCKADDR_IN clientAddr={0,0,0,0};

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	ListServers(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Disconnect(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Settings(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	NickName(HWND, UINT, WPARAM, LPARAM);
int DisconnectSocket(void);
int ConnectSocket(HWND &,HWND);

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
	TCHAR	*inBuffer,
		*tmpBuffer;
	sockaddr_storage targetAddress;
	char ipAddressStr[INET_ADDRSTRLEN]; // IP4, IP6 = INET6_ADDRSTRLEN

	switch (message)
	{
	case WM_CLIENT_SOCKET:
		{
			int wmEvent=WSAGETSELECTEVENT(lParam);
			int wmError=WSAGETSELECTERROR(lParam);

			switch(wmEvent)
			{
			case FD_CONNECT:
				{
					if(wmError!=NULL)
						DisplayError("Connection Error",wmError,FALSE);
					else
						isConnected=TRUE;
				}
				break;
			case FD_READ:
				{
					if(wmError!=NULL)
						DisplayError("Read Error",wmError,FALSE);
					inBuffer = new TCHAR[100];
					tmpBuffer = new TCHAR[130];
					inBuffer="";
					recv(client.clientSocket,inBuffer,strlen(inBuffer)+1,0);
					sockaddr_in *s = (sockaddr_in *)&targetAddress;
					inet_ntop(AF_INET, &s->sin_addr , ipAddressStr, sizeof ipAddressStr);
					wsprintf(tmpBuffer,"From Server: %s",inBuffer);

					SendMessage(chatWindow,LB_ADDSTRING, 0, (LPARAM)tmpBuffer);
					delete []inBuffer;
					delete []tmpBuffer;
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
		case WM_CHAR:
			if(wParam==13)
			{
				// Get current selection in listbox
				int itemIndex = (int) ::SendMessage(chatWindow, LB_GETCURSEL, (WPARAM)0, (LPARAM) 0);
				if (itemIndex == LB_ERR)
				{
					// No selection
					break;
				}

				// Get length of text in listbox
				int textLen = (int) ::SendMessage(chatWindow, LB_GETTEXTLEN, (WPARAM) itemIndex, 0);

				// Allocate buffer to store text (consider +1 for end of string)
				TCHAR * textBuffer = new TCHAR[textLen + 1];

				// Get actual text in buffer
				::SendMessage(chatWindow, LB_GETTEXT, (WPARAM) itemIndex, (LPARAM) textBuffer );

				// Show it
				::MessageBox(NULL, textBuffer, _T("Selected Text from Listbox:"), MB_OK);

				// Free text
				delete [] textBuffer;

				// Avoid dangling references
				textBuffer = NULL;
			}
			break;
		case IDM_SETTINGS:
			DialogBox(hInst,MAKEINTRESOURCE(IDD_SETTINGS),hWnd,Settings);
			break;
		case IDM_LISTSERVERS:
			DialogBox(hInst,MAKEINTRESOURCE(IDD_LISTSERVERS),hWnd,ListServers);
			break;
		case IDM_CONNECT:
			client.clientSocket=ConnectSocket(chatWindow,hWnd);
			break;
		case IDM_DISCONNECT:
			if(isConnected)
				DialogBox(hInst,MAKEINTRESOURCE(IDD_DISCONNECT ),hWnd,Disconnect);
			break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			{
				DisconnectSocket();
				DestroyWindow(hWnd);
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
				DisconnectSocket();
			}
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

int ConnectSocket(HWND &chatWindow,HWND hWnd)
{
	addrinfo	hints,
		*sAddr;
	char strPortNo[10]="";
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	int serverSocket;

	chatWindow = CreateWindow(	"LISTBOX",
		"Chat Window",
		WS_CHILD|WS_VISIBLE|ES_LEFT|WS_BORDER|
		ES_READONLY|WS_VSCROLL|WS_HSCROLL|WS_MAXIMIZEBOX|
		WS_MINIMIZEBOX|WS_OVERLAPPEDWINDOW,
		50,50,
		1000,400,
		hWnd,
		(HMENU)IDC_CLIENT,
		hInst,
		NULL
		);
	SecureZeroMemory (&hints, sizeof(hints));

	if ((WSAStartup(wVersionRequested, &wsaData) != 0) || //not correct Winsock version
		(LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2))
	{
		WSACleanup();
		DisplayError("WSA Error",WSAGetLastError(),TRUE);
	}
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	_itoa(client.portNo,strPortNo,10);

	if ( getaddrinfo(client.ipAddress,strPortNo, &hints, &sAddr) !=0 )
		DisplayError("Error with address",WSAGetLastError(),TRUE);

	serverSocket = socket(sAddr->ai_family, sAddr->ai_socktype, sAddr->ai_protocol);

	if ( WSAAsyncSelect( serverSocket,hWnd,WM_CLIENT_SOCKET,FD_CONNECT | FD_READ))
	{
		int wsaError=WSAGetLastError(); // in case it changes after close socket
		closesocket(serverSocket);
		WSACleanup();
		DisplayError( "WSAAsyncSelect error",wsaError,TRUE);
	}

	boolean bOptVal = TRUE;

	if(setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char *) &bOptVal, sizeof(int)) == SOCKET_ERROR)
	{
		DisplayError("getsockopt for SO_KEEPALIVE failed with error: ", WSAGetLastError(),FALSE);
	}

	isConnected = (connect(serverSocket,(SOCKADDR*)&sAddr,sizeof(SOCKADDR))!=SOCKET_ERROR);
	return serverSocket;
}

int DisconnectSocket(void)
{
	if(!isConnected)
		return 1;
	int errorStatus=0;
	if (closesocket(client.clientSocket) == SOCKET_ERROR)
	{
		errorStatus=WSAGetLastError();
		DisplayError("Close socket failed", errorStatus,FALSE);
	}
	WSACleanup();
	isConnected=FALSE;
	return errorStatus;
}
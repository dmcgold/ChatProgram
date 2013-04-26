// Server.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#pragma warning(disable: 4996)

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
serverStruct server = {6000,10,AF_INET,"192.168.1.110"};
BOOLEAN serverStarted=FALSE;
HWND chatWindow;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Settings(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	ListClients(HWND, UINT, WPARAM, LPARAM);
int ShutDown(HWND ,serverStruct);

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
	//if (gethostname(server.ipAddress, sizeof(server.ipAddress)) == SOCKET_ERROR)
	//	DisplayError("Error ",WSAGetLastError(),NO_EXIT);
	//struct hostent *host = gethostbyname(server.ipAddress);

	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_SERVER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SERVER));

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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SERVER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_SERVER);
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
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR	*receiveBuffer,
		*tmpBuffer;
	addrinfo hints,
		*sAddr;
	sockaddr_storage incomingAddress;
	socklen_t addressSize;
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2, 2);
	char ipAddressStr[INET_ADDRSTRLEN]; // IP4, IP6 = INET6_ADDRSTRLEN
	char strPortNo[10]="";

	switch (message)
	{
	case WM_CREATE:
		break;
	case WM_LBUTTONDOWN:
		{
			char szFileName[MAX_PATH];
			HINSTANCE hInstance = GetModuleHandle(NULL);

			GetModuleFileName(hInstance,(LPTSTR) szFileName, MAX_PATH);
			MessageBox(hWnd, (LPCTSTR)(long)szFileName,(LPCTSTR)L"This program is:", MB_OK | MB_ICONINFORMATION);
		}
		break;
	case WM_SERVER_SOCKET:
		{
			int wmEvent=WSAGETSELECTEVENT(lParam);
			int wmError=WSAGETSELECTERROR(lParam);

			switch(wmEvent)
			{
			case FD_ACCEPT:
				{
					addressSize = sizeof(incomingAddress);
					server.socketConnection=accept(server.socketServer,(sockaddr *)&incomingAddress,&addressSize);
				}
				break;
			case FD_READ:
				{
					receiveBuffer = new TCHAR[100];
					tmpBuffer = new TCHAR[130];
					receiveBuffer="";
					recv(server.socketConnection,receiveBuffer,strlen(receiveBuffer)+1,0);
					sockaddr_in *s = (sockaddr_in *)&incomingAddress;
					inet_ntop(AF_INET, &s->sin_addr , ipAddressStr, sizeof ipAddressStr);
					wsprintf(tmpBuffer,"From IP %s:%d : %s",ipAddressStr,ntohs(s->sin_port),receiveBuffer);

					SendMessage(chatWindow,LB_ADDSTRING, 0, (LPARAM)tmpBuffer);
					delete []receiveBuffer;
					delete []tmpBuffer;
				}
				break;
			case FD_CLOSE:
				{
					closesocket(server.socketServer);
					closesocket(server.socketConnection);
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
		case IDM_SETTINGS:
			{
				DialogBox(hInst, MAKEINTRESOURCE(IDD_SETTINGS), hWnd, Settings);
			}
			break;
		case IDM_LISTCLIENTS:
			{
				DialogBox(hInst, MAKEINTRESOURCE(IDD_CLIENTS), hWnd, ListClients);
			}
			break;
		case IDM_STARTSERVER:
			{
				if(serverStarted)
				{
					DisplayError("Serer already started",0,NO_EXIT);
					break;
				}
				else
					serverStarted=TRUE;
				chatWindow = CreateWindow(	"LISTBOX",
					"Chat Window",
					WS_CHILD|WS_VISIBLE|ES_LEFT|WS_BORDER|
					ES_READONLY|WS_VSCROLL|WS_HSCROLL|WS_MAXIMIZEBOX|
					WS_MINIMIZEBOX|WS_OVERLAPPEDWINDOW,
					50,50,
					1000,400,
					hWnd,
					(HMENU)IDE_CHAT,
					hInst,
					NULL);
				SecureZeroMemory (&hints, sizeof(hints));

				if ((WSAStartup(wVersionRequested, &wsaData) != 0) || //not correct Winsock version
					(LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2))
				{
					WSACleanup();
					DisplayError("WSA Error",WSAGetLastError(),EXIT);
				}
				hints.ai_family = server.family;
				hints.ai_socktype = SOCK_STREAM;
				hints.ai_protocol = IPPROTO_TCP;

				_itoa(server.portNo,strPortNo,10);

				if ( getaddrinfo(server.ipAddress,strPortNo, &hints, &sAddr) !=0 )
					DisplayError("Error with address",WSAGetLastError(),EXIT);

				server.socketServer = socket(sAddr->ai_family, sAddr->ai_socktype, sAddr->ai_protocol);

				if ( WSAAsyncSelect( server.socketServer,hWnd,WM_SERVER_SOCKET,FD_ACCEPT | FD_READ ))
				{
					int wsaError=WSAGetLastError(); // in case it changes after close socket
					closesocket(server.socketServer);
					WSACleanup();
					DisplayError( "WSAAsyncSelect error",wsaError,TRUE);
				}

				if ( bind(server.socketServer, sAddr->ai_addr, sAddr->ai_addrlen) == SOCKET_ERROR )
					DisplayError( "Bind error",WSAGetLastError(), TRUE );

				if (listen(server.socketServer, server.noConnections) == SOCKET_ERROR)
					DisplayError("Listening error",WSAGetLastError(), TRUE );
			}
			break;
		case IDM_STOPSERVER:
			{
				if(serverStarted)
					ShutDown(chatWindow,server);
				serverStarted=FALSE;
			}
			break;
		case IDM_ABOUT:
			{
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			}
			break;
		case IDM_EXIT:
			{
				if(serverStarted)
					ShutDown(chatWindow,server);
				serverStarted=FALSE;
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
	case WM_CLOSE:
		DestroyWindow(hWnd);
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
			if (LOWORD(wParam) == IDOK )
			{
				int tmp = server.portNo=GetDlgItemInt(hDlg, IDC_PORTNO, didPass,isSigned);
				if (tmp!=0) // user entered data
					server.portNo=tmp; // default port
				tmp=server.noConnections=GetDlgItemInt(hDlg, IDC_NO_CONNECTIONS, didPass,isSigned);
				if (tmp!=0)
					server.noConnections=tmp; // default no of connections
			}
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK ListClients(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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

int ShutDown(HWND hWnd,serverStruct s)
{
	int errorStatus=0;
	char closeMSG[]="Server Shutting down";
	send(s.socketConnection,closeMSG,strlen(closeMSG)+1,0); //?????
	DestroyWindow(hWnd);
	int iResult = closesocket(s.socketServer);
	if (iResult == SOCKET_ERROR)
	{
		errorStatus=WSAGetLastError();
		DisplayError("Close socket failed", errorStatus,NO_EXIT);
	}
	WSACleanup(); // Server has been stopped so clean up anyway
	return errorStatus;
}
/*********************************************************************/
/******					Server application						******/
/******				Author: Danny McGoldrick					******/
/******															******/
/******				Diploma Information Technology				******/
/******							2013							******/
/*********************************************************************/

#include "server.h"

#pragma warning(disable: 4996)

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								    // current instance
TCHAR szTitle[MAX_LOADSTRING];					    // The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			    // the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);

LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
					   _In_opt_ HINSTANCE hPrevInstance,
					   _In_ LPTSTR    lpCmdLine,
					   _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	MSG msg;

	setlocale( LC_ALL, "Australia" );

	// Initialise server Structure with default values

	server->portNo=6000;
	server->isConnected=FALSE;
	_tcscpy(server->ipAddress,TEXT("192.168.1.110"));
	server->maxConnections=10;
	server->noConnections=0;

	clients=NULL;

	HACCEL hAccelTable;

	// Initialize global strings
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
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	_freea(clients);
	_freea(server);

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
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+2);
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

	switch (message)
	{
	case WM_CLOSE:
		{
			DestroyWindow(logWindow);					// Destroy all windows
			DestroyWindow(hWnd);
			delete [] clients;							// Delete client list
			exit (ShutDown(server->inSocket));			// Shutdown and exit
		}
		break;
	case WM_SERVER_SOCKET:

		ReadSocket(logWindow,lParam,server,&clients);
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_START_SERVER:
			{
				if(!server->isConnected)                      // If server is not up then start it
					if (StartServer(hWnd,server)!=0)          // There is was a problem starting the server
						DisplayError(NULL,TEXT("Couldn't start server"),0,ALL,ERRORFOUND);  // Log the error
			}
			break;
		case IDM_VIEW_LOG:
			{
				if (server->isConnected)   // If the log window is'nt open then open it
					DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_LOG_WINDOW), hWnd, LogWindow,(LPARAM)clients);                // Activate window
			}
			break;
		case IDM_DISCONNECT:
			{
				DisplayError(NULL,TEXT("Disconnecting Server"),0,ALL,NOTICE);         // Log the fact were disconnecting server
				delete [] clients;											// Delete the list of xlients
				ShowWindow(logWindow,SW_HIDE);
				closesocket(server->inSocket);                              // Close the socket
				WSACleanup();												// Tidy uo
				server->isConnected=FALSE;									// We are'nt connected anymore
			}
			break;
		case IDM_CLIENT_LIST:
			DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_CLIENTS), hWnd, ListClients,(LPARAM)clients);
			break;
		case IDM_SETTINGS:
			DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_SETTINGS), hWnd, Settings,(LPARAM)server);
			break;
		case IDM_EXIT:
			{
				DestroyWindow(logWindow);									// Destroy all windows
				DestroyWindow(hWnd);
				exit (ShutDown(server->inSocket));							// Shutdown then exit program
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
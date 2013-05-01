// client.cpp : Defines the entry point for the application.
//

#pragma warning(disable: 4996)
#include "stdafx.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
serverStruct server = {6000,"192.168.1.110",0,AF_INET,10,0};
clientStruct *client;
BOOLEAN serverStarted=FALSE;
HWND chatWindow;
fd_set userList;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Settings(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	ListClients(HWND, UINT, WPARAM, LPARAM);
int ShutDown(HWND ,SOCKET);
void StartServer(HWND &, HWND);
void *GetIPAddress(struct sockaddr *s);
int AddConnection(fd_set *,clientStruct **);
int GetIncoming(fd_set ,clientStruct *);
char *GetMSG(char *,clientStruct *,boolean);
SOCKET GetTo(char *,clientStruct *);

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
				AddConnection(&userList,&client);
				break;
			case FD_READ:
				GetIncoming(userList,client);
				break;
			case FD_CLOSE:
				{
					char *test="test";
					for(int i=1;userList.fd_array[i]!=0;i++)
					{
						if(FD_ISSET(userList.fd_array[i],&userList))
						{
							if(send((SOCKET)i,test,strlen(test)+1,0) < 1)
								FD_CLR(i,&userList);
						}
					}
				}
				break;
			}
		}
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_SETTINGS:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_SETTINGS), hWnd, Settings);
			break;
		case IDM_LISTCLIENTS:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_CLIENTS), hWnd, ListClients);
			break;
		case IDM_STARTSERVER:
			{
				if(serverStarted)
				{
					DisplayError(chatWindow,WSAGetLastError());
					break ;
				}
				else
					StartServer(chatWindow,hWnd);
			}
			break;
		case IDM_STOPSERVER:
			ShutDown(chatWindow,server.inSocket);
			break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			{
				ShutDown(chatWindow,server.inSocket);
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
					server.noConnections; // default no of connections
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

int ShutDown(HWND hWnd,SOCKET s)
{
	if(!serverStarted)
		return 1;
	int errorStatus=0;
	char closeMSG[]="Server Shutting down";
	send(s,closeMSG,strlen(closeMSG)+1,0); //?????
	DestroyWindow(hWnd);
	if (closesocket(s) == SOCKET_ERROR)
	{
		errorStatus=WSAGetLastError();
		DisplayError(chatWindow,WSAGetLastError());
	}
	WSACleanup(); // Server has been stopped so clean up anyway
	serverStarted=FALSE;
	return errorStatus;
}

void StartServer(HWND &chatWindow,HWND hWnd)
{
	addrinfo	hints,
		*sAddr;
	char strPortNo[10]="";
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;

	client = new clientStruct;

	client=NULL;

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
	FD_ZERO(&userList);

	if ((WSAStartup(wVersionRequested, &wsaData) != 0) || //not correct Winsock version
		(LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2))
	{
		WSACleanup();
		DisplayError(chatWindow,WSAGetLastError());
		exit (1);
	}
	hints.ai_family = server.family;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	_itoa(server.portNo,strPortNo,10);

	if ( getaddrinfo(server.ipAddress,strPortNo, &hints, &sAddr) !=0 )
	{
		DisplayError(chatWindow,WSAGetLastError());
		exit (1);
	}

	server.inSocket = socket(sAddr->ai_family, sAddr->ai_socktype, sAddr->ai_protocol);

	if ( WSAAsyncSelect( server.inSocket,hWnd,WM_SERVER_SOCKET,FD_ACCEPT | FD_READ | FD_CLOSE))
	{
		int wsaError=WSAGetLastError(); // in case it changes after close socket
		closesocket(server.inSocket);
		WSACleanup();
		DisplayError(chatWindow,WSAGetLastError());
		exit (1);
	}

	boolean bOptVal = TRUE;

	if(setsockopt(server.inSocket, SOL_SOCKET, SO_REUSEADDR, (char *) &bOptVal, sizeof(int)) == SOCKET_ERROR)
	{
		DisplayError(chatWindow,WSAGetLastError());
	}

	if ( bind(server.inSocket, sAddr->ai_addr, sAddr->ai_addrlen) == SOCKET_ERROR )
	{
		DisplayError(chatWindow,WSAGetLastError());
		exit (1);
	}

	if (listen(server.inSocket, server.noConnections) == SOCKET_ERROR)
	{
		DisplayError(chatWindow,WSAGetLastError());
		exit (1);
	}
	FD_SET(server.inSocket, &userList);

	serverStarted=TRUE;
}

void *GetIPAddress(struct sockaddr *s)
{
	switch (s->sa_family)
	{
	case AF_INET : return &(((struct sockaddr_in*)s)->sin_addr);
	case AF_INET6 : return &(((struct sockaddr_in6*)s)->sin6_addr);
	case AF_IPX :
	case AF_NETBIOS :
	case AF_UNSPEC : return 0;
	}
	return 0;
}

int AddConnection(fd_set *userList,clientStruct **client)
{
	TCHAR	*receiveBuffer = new TCHAR[256],
		*tmpBuffer = new TCHAR[256];
	sockaddr_storage incomingAddress;
	int addrLength = sizeof(sockaddr_storage);
	clientStruct *tmpClient;

	tmpClient = new clientStruct;

	tmpClient->inSocket=accept(server.inSocket,(struct sockaddr *)&incomingAddress,&addrLength);
	inet_ntop(incomingAddress.ss_family,GetIPAddress((struct sockaddr*)&incomingAddress),tmpClient->ipAddress, sizeof(tmpClient->ipAddress));
	FD_SET(tmpClient->inSocket,userList);
	wsprintf(tmpBuffer,"New Connection from IP: %s ",tmpClient->ipAddress);
	SendMessage(chatWindow,LB_ADDSTRING,0,LPARAM(tmpBuffer));
	char Welcome[]="Welcome User";
	if(send(tmpClient->inSocket,Welcome,(int) strlen(Welcome)+1,0)==SOCKET_ERROR)
		DisplayError(chatWindow,WSAGetLastError());
	if(send(tmpClient->inSocket,"REQ_DATA",(int) strlen(Welcome)+1,0)==SOCKET_ERROR)  // we'll send a request for data e.g NickName
		DisplayError(chatWindow,WSAGetLastError());
	tmpClient->next=(*client);
	(*client)=tmpClient;

	return tmpClient->inSocket;
}

int GetIncoming(fd_set userList,clientStruct *client)
{
	int bufferSize=256;
	TCHAR	*receiveBuffer = new TCHAR[bufferSize],
		*tmpBuffer = new TCHAR[bufferSize];
	clientStruct *clients=client;
	boolean PublicMSG=TRUE;
	char *realMSG;

	//for(int i=1;userList.fd_array[i]!=0;i++) // We start at 1 as the first element will be the main socket
	while(clients!=NULL)
	{
		// if(FD_ISSET(userList.fd_array[i],&userList))
		if(clients->inSocket!=0) // Go through the valid socket to get message
		{
			ZeroMemory(receiveBuffer,bufferSize);
			if(recv(clients->inSocket,receiveBuffer,bufferSize,0)==SOCKET_ERROR)
			{
				DisplayError(chatWindow,WSAGetLastError());
				return -1;
			}

			realMSG=GetMSG(receiveBuffer,clients,PublicMSG);
			wsprintf(tmpBuffer,"Message is %s from %s ",realMSG,client->nickName);
			SendMessage(chatWindow,LB_ADDSTRING, 0, (LPARAM)tmpBuffer);

			clientStruct *outClient=client;
			//for(int j=1;userList.fd_array[j]!=0;j++) // Send incoming to all expect whoever sent it
			if(PublicMSG) // send to all
				while(outClient!=NULL)
				{
					//if(FD_ISSET(userList.fd_array[j],&userList))
					if(outClient->inSocket!=clients->inSocket)
					{
						/*if(userList.fd_array[j] != userList.fd_array[i])
						{*/
						if (send(outClient->inSocket,realMSG,bufferSize,0) == SOCKET_ERROR)
							DisplayError(chatWindow,WSAGetLastError());
						//}
					}
					outClient=outClient->next;
				}
			else
				if (send(GetTo(client->sendTo,client),realMSG,bufferSize,0) == SOCKET_ERROR)
					DisplayError(chatWindow,WSAGetLastError());
		}
		clients=clients->next;
	}
	delete []receiveBuffer;
	delete []tmpBuffer;
	return 0;
}

char *GetMSG(char *receiveBuffer,clientStruct *client,boolean sendToPublic)
{
	char *str=receiveBuffer,
		*str2=str;

	str=strtok_s(receiveBuffer,"|",&str2);
	// USER|peter|PUBLIC| format of messages USER nick public of private
	while((str2!=NULL) && (strlen(str2)>0))
	{
		if(strcmp(str,"USER")==0)
		{
			str=strtok_s(NULL,"|",&str2);
			strcpy(client->nickName,str);
		}
		if(strcmp(str,"PUBLIC")==0)
		{
			str=strtok_s(NULL,"|",&str2);
			sendToPublic=TRUE;
		}
		if(strcmp(str,"PRIVATE")==0)
		{
			str=strtok_s(NULL,"|",&str2);
			sendToPublic=FALSE;
		}
		str=strtok_s(NULL,"|",&str2);
	}
	return str; // Only the message should remain
}

SOCKET GetTo(char *user,clientStruct *clients)
{
	while((clients!=NULL) && (strcmp(user,clients->nickName)!=0))
		clients=clients->next;
	return clients->inSocket;
}
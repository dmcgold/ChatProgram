/*********************************************************************/
/******						Network file						******/
/******			File contains all network related functions		******/
/******				These functions are used in both the		******/
/******				server and client applications				******/
/******							2013							******/
/*********************************************************************/
#include "Network.h"

#pragma warning(disable:4996)

/*******************************************/
/******                               ******/
/******   Server specific functions   ******/
/******                               ******/
/*******************************************/

int StartServer(HWND hWnd,serverStruct *server)
{
	sockaddr_in sAddr;
	char strPortNo[10]="";
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	BOOLEAN bOptVal = TRUE;
	int wsaError=0;
	int ipSize=INET_ADDRSTRLEN;

	TCHAR hostName[50];

	DisplayError(NULL,TEXT(__FUNCSIG__),0,LOG,NOTICE);

	memset( &sAddr, 0, sizeof (sAddr));

	if ((WSAStartup(wVersionRequested, &wsaData) != 0) || //not correct Winsock version
		(LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2))
	{
		WSACleanup();
		wsaError=WSAGetLastError();
		DisplayError(NULL,TEXT("Error in StartServer - WSA Start-up "),wsaError,ALL,ERRORFOUND);
		return wsaError;
	}

	_tcscpy(hostName,TEXT(""));

	if(GetHostNameW(hostName,50)!=SOCKET_ERROR)
	{
		TCHAR hostNameEXT[50];
		StringCbPrintf(hostNameEXT,100,_T("Server on hostname: %s"),hostName);
		DisplayError(NULL,hostNameEXT,0,LOG,NOTICE);
	}
	else
		DisplayError(NULL,TEXT("Can't get host"),WSAGetLastError(),LOG|POPMSG,ERRORFOUND);

	WSAStringToAddress(server->ipAddress,AF_INET,NULL,(LPSOCKADDR) &sAddr,&ipSize);

	sAddr.sin_port = htons(server->portNo);
	server->inSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(server->inSocket==INVALID_SOCKET )
	{
		wsaError=WSAGetLastError();
		DisplayError(NULL,TEXT("Socket Error"),server->inSocket,ALL,ERRORFOUND);
		return wsaError;
	}
	if ( WSAAsyncSelect( server->inSocket,hWnd,WM_SERVER_SOCKET,FD_ACCEPT | FD_READ | FD_CLOSE)== SOCKET_ERROR)
	{
		wsaError=WSAGetLastError(); // in case it changes after close socket
		closesocket(server->inSocket);
		WSACleanup();
		DisplayError(NULL,TEXT("WSAAsyncSelect Error"),wsaError,ALL,ERRORFOUND);
		return wsaError;
	}

	if(setsockopt(server->inSocket, SOL_SOCKET, SO_REUSEADDR, (char *) &bOptVal, sizeof(int)) == SOCKET_ERROR)
	{
		wsaError=WSAGetLastError();
		DisplayError(NULL,TEXT("SetSockOpt Error"),WSAGetLastError(),ALL,ERRORFOUND);
		return wsaError;
	}

	if ( bind(server->inSocket, (SOCKADDR *) &sAddr, sizeof(sAddr)) == SOCKET_ERROR )
	{
		wsaError=WSAGetLastError();
		DisplayError(NULL,TEXT("Bind Error"),wsaError,ALL,ERRORFOUND);
		return wsaError;
	}

	if (listen(server->inSocket, server->noConnections) == SOCKET_ERROR)
	{
		wsaError=WSAGetLastError();
		DisplayError(NULL,TEXT("Listen Error"),wsaError,ALL,ERRORFOUND);
		return wsaError;
	}
	server->isConnected=(wsaError==0);
	return wsaError;
}

int GetIncoming(clientStruct *clients,int maxSockets)
{
	int sender=0;
	int szRead=0;
	int structSize=sizeof(clientStruct)*sizeof(TCHAR);


	DisplayError(NULL,TEXT(__FUNCSIG__),0,LOG|WINDOW,NOTICE);

	while((clients!=NULL) && (szRead==0))
	{	// Loop through all the known sockets
		// If data is waiting to be read recv returns size of data read
		// If it returns 0 the connection is closed
		sender=clients->inSocket;
		szRead=recv(sender,(char*)&clients,structSize,0);
		if(0>szRead)
			clients=(clients->next);
	}
	return sender;
}

int AddConnection(clientStruct **clients,int inSocket,int noConnections)
{
	DisplayError(NULL,TEXT(__FUNCSIG__),0,LOG,NOTICE);

	SOCKET newSocket;
	sockaddr_storage incomingAddress;
	int addrLength = INET_ADDRSTRLEN*sizeof(TCHAR);

	clientStruct	*head=*clients,
		*newClient;

	newClient = new clientStruct;
	_tcscpy(newClient->nickName,_T("PUBLIC")); // default

	newSocket = accept(inSocket,(struct sockaddr*)&incomingAddress,&addrLength);
	InetNtopW(AF_INET,&incomingAddress,newClient->ipAddress,addrLength);

	noConnections++;
	newClient->inSocket=newSocket;

	newClient->next=head;
	head=newClient;

	*clients=head;
	return noConnections;
}

SOCKET GetClosedSocket(struct clientStruct *clients)
{// Send recv to all known sockets, if it returns 0 then it's been closed
	while(clients!=NULL)
		if(recv(clients->inSocket,NULL,NULL,0)==0)
			return clients->inSocket;
		else
			clients=clients->next;
	return 0;
}

int ShutDown(SOCKET s)
{
	int errorStatus=0;
	DisplayError(NULL,TEXT("Shutting Down "),0,ALL,NOTICE);
	if (closesocket(s) == SOCKET_ERROR)
	{
		errorStatus=WSAGetLastError();
		DisplayError(NULL,TEXT("Problem closing socket "),WSAGetLastError(),ALL,ERRORFOUND);
	}
	WSACleanup(); // Server has been stopped so clean up anyway
	return errorStatus;
}

void ReadSocket(HWND hWnd,LPARAM lParam,serverStruct *server,clientStruct **clients)
{
	int wmEvent=WSAGETSELECTEVENT(lParam);
	int wmError=WSAGETSELECTERROR(lParam);

	TCHAR incomingMSG[BUFFER_SIZE];
	int bufferSize=BUFFER_SIZE*sizeof(TCHAR);
	clientStruct clientMSG={'\0'};

	_tcscpy(incomingMSG,_T(""));

	switch(wmEvent)
	{
	case FD_ACCEPT:
		{
			if(wmError!=NULL)
				DisplayError(NULL,TEXT("Connection Error"),wmError,ALL,ERRORFOUND);
			else
			{
				server->noConnections=AddConnection(&(*clients),server->inSocket,server->noConnections);	// Add new connection
				if(hWnd!=NULL)												// Don't display message if window not open
				{
					StringCbPrintf(incomingMSG, bufferSize,TEXT("[No %d/%d] connection(s) from IP: %s"),server->noConnections,server->maxConnections,(*clients)->ipAddress);
					SendMessage(hWnd,LB_ADDSTRING,0,(LPARAM)incomingMSG);
				}
			}
		}
		break;
	case FD_READ:
		{
			clientStruct *tmpC=(*clients);
			int sender=GetIncoming(&(*tmpC),server->noConnections); // Get incoming message
			if(sender>0)
				RelayMSG(hWnd,(*clients),sender);							// Send message to relevant clients
		}
		break;
	case FD_CLOSE:
		{
			SOCKET closedSocket=GetClosedSocket((*clients));                  // Find out which socket closed
			if((closesocket(closedSocket)==0) && (IsWindowVisible(hWnd)))// Socket found, only display info if hWnd open
			{
				StringCbPrintf(incomingMSG,100,TEXT("Disconnected : %s"),DeleteClient(&(*clients),closedSocket));  // Display msg IP of client and data & time
				SendMessage(hWnd,LB_ADDSTRING,0, (LPARAM)incomingMSG);
			}
		}
		break;
	}
}

TCHAR *DeleteClient(clientStruct **clients,int clientSocket)
{
	clientStruct *currentClient=*clients;
	clientStruct *tmpClient=*clients;
	TCHAR *tmpUser=tmpClient->ipAddress;

	DisplayError(NULL,TEXT(__FUNCSIG__),0,ALL,NOTICE);

	while(currentClient)
	{
		if(currentClient->inSocket==clientSocket)
		{
			*clients=tmpClient->next;
			tmpUser=tmpClient->ipAddress;
			delete(tmpClient);
			return tmpUser;
		}
		else
			currentClient=currentClient->next;
	}
	delete(currentClient);
	return '\0';
}

// Used by the client

void SendText(HWND inputWin,SOCKET outSocket,clientStruct *newMSG) // Send text from user to server - Public Chat
{
	int sentSize=0;
	int msgSize=sizeof(clientStruct) *sizeof(TCHAR);

	SendMessage(inputWin, WM_GETTEXT,BUFFER_SIZE,(LPARAM)&newMSG->sendMSG);

	if (GetWindowTextLength(inputWin)!=0)  // Only char can be sent
	{
		sentSize=send(outSocket, (char*)&newMSG[0] , msgSize, 0);
		if(sentSize==SOCKET_ERROR)
			DisplayError(NULL,_T("Send Error"),WSAGetLastError(),POPMSG,ERRORFOUND);
	}
}

void RelayMSG(HWND hWnd,clientStruct *clients,SOCKET outSocket)
{
	//	This function will relay messages to the designated client(s)

	BOOLEAN sendAll = FALSE;
	TCHAR tmpBuffer[BUFFER_SIZE];
	clientStruct *tmpClients=clients;

	DisplayError(NULL,TEXT(__FUNCSIG__),0,LOG|WINDOW,NOTICE);

	// Find the Sender
	while((tmpClients!=NULL) && (tmpClients->inSocket!=outSocket))
	{
		tmpClients=tmpClients->next;
	}
	// Sender not found so leave

	if(tmpClients==NULL)
		return;

	if(_tcscmp(clients->sendTo,TEXT("PUBLIC"))==0)
	{
		sendAll=TRUE;
		StringCbPrintf(tmpBuffer,BUFFER_SIZE,TEXT("Relaying incoming message to %s "),TEXT("all users"));
	}
	else
		StringCbPrintf(tmpBuffer,BUFFER_SIZE,TEXT("Relaying incoming message to %s "),clients->sendTo);

	DisplayError(NULL,tmpBuffer,0,LOG,NOTICE);
	// Store sender details

	if (sendAll) // Send to all except the sender
	{
#pragma warning(suppress: 6303)
		// Display message in server log window if it's open
		if(hWnd!=NULL)
		{
			StringCbPrintf(tmpBuffer,BUFFER_SIZE,TEXT("Message from %s: %s "),clients->nickName,clients->sendMSG);
			SendMessage(hWnd,LB_ADDSTRING, 0, (LPARAM)tmpBuffer);
		}

		while(tmpClients!=NULL)													// Send to all users except the sender
		{
			if(tmpClients->inSocket!=outSocket)
				send(tmpClients->inSocket,(char*)&clients,sizeof(clientStruct),0);
			tmpClients=tmpClients->next;
		}
	}
	else																		// Send a single user
		send(GetClient(clients,clients->sendTo),(char *)&clients,sizeof(clientStruct),0);
}

SOCKET GetClient(clientStruct *clients,TCHAR *user)								// Get the socket of a particular user
{
	// Find the socket of Client based on nickName
	while(clients!=NULL)
	{
		if(_tcscmp(clients->nickName,user)==0)
			return clients->inSocket;
		clients=clients->next;
	}
	return 0;
}

/*******************************************/
/******                               ******/
/******   Client specific functions   ******/
/******                               ******/
/*******************************************/

int GetIncoming(clientStruct *clientMSG,SOCKET socket)
{
	DisplayError(NULL,TEXT(__FUNCSIG__),0,ALL,NOTICE);

	return recv(socket,(char *)&clientMSG,sizeof(clientMSG),0);
}

BOOLEAN ConnectToServer(HWND hWnd,serverStruct *server,clientStruct client)
{
	sockaddr_in sAddr;
	char strPortNo[10]="";
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	int connErr=0;
	int ipSize;

	DisplayError(NULL,TEXT(__FUNCSIG__),0,LOG,NOTICE);

	ipSize=sizeof(sAddr);
	server->isConnected=TRUE;													// It will be changed to FALSE on errors
	wVersionRequested = MAKEWORD(2, 2);

	if (WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		server->isConnected=FALSE;
		return server->isConnected;
	}

	if ( LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
		server->isConnected=FALSE;
		return server->isConnected;
	}

	memset( &sAddr, 0, sizeof (sAddr));
	server->inSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(server->inSocket==INVALID_SOCKET)
	{
		connErr=WSAGetLastError();
		DisplayError(NULL,TEXT("Invalid Socket"),WSAGetLastError(),ALL,ERRORFOUND);
		server->isConnected=FALSE;
		return server->isConnected;
	}
	if(  WSAAsyncSelect( server->inSocket, hWnd,WM_CLIENT_SOCKET, FD_CONNECT | FD_READ ) == SOCKET_ERROR )
	{
		connErr=WSAGetLastError();
		DisplayError(hWnd,TEXT("WSAAsyncSelect Error"),connErr,ALL,ERRORFOUND);
		server->isConnected=FALSE;
		closesocket(server->inSocket);
		WSACleanup();
		return server->isConnected;
	}

	WSAStringToAddress(server->ipAddress,AF_INET,NULL,(LPSOCKADDR) &sAddr,&ipSize);
	sAddr.sin_port = htons(server->portNo);

	if(connErr=connect(server->inSocket,(SOCKADDR*)&sAddr,sizeof(SOCKADDR))==SOCKET_ERROR)
	{
		connErr=WSAGetLastError();
		if(connErr!=10035)                  // This error can be ignore so we are connected
		{
			DisplayError(NULL,TEXT("Connection Error"),connErr,LOG,ERRORFOUND);
			server->isConnected=FALSE;
		}
		else
			DisplayError(NULL,TEXT("WSA Block"),connErr,LOG,WARNING);
	}

	return server->isConnected;
}

/******************************************/
/***                                    ***/
/*** Server & Client specific functions ***/
/***                                    ***/
/******************************************/
#include "stdafx.h"

#pragma warning(disable: 4996)

using namespace std;

void StartServer(serverStruct server)
{

	struct addrinfo hints,
					*sAddr;
	struct sockaddr_storage incomingAddress;
	socklen_t addresSize;	
	int newConnection, 
		orgSocket;
	WSADATA wsaData;
	
    SecureZeroMemory (&hints, sizeof(hints));

	if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
		DisplayError("WSA Start-up Failure",WSAGetLastError(),TRUE);

	
	hints.ai_family = server.family;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	char strPortNo[10]="";

	_itoa(server.portNo,strPortNo,10);

	if (getaddrinfo(server.ipAddress,strPortNo, &hints, &sAddr) !=0 )
		DisplayError(WSAError(WSAGetLastError()),WSAGetLastError(),EXIT);

	orgSocket = socket(sAddr->ai_family, sAddr->ai_socktype, sAddr->ai_protocol);
	
	hostent *remoteHost = gethostbyaddr((char *) &sAddr->ai_addr, sAddr->ai_addrlen , sAddr->ai_family);	

	if(remoteHost == NULL)
		DisplayError(WSAError(WSAGetLastError()), WSAGetLastError(), EXIT );

	if ( bind(orgSocket, sAddr->ai_addr, sAddr->ai_addrlen) == SOCKET_ERROR )  
	{
        DisplayError(WSAError(WSAGetLastError()), WSAGetLastError(), TRUE );       
	}
	
	char hostName[50];

	if (gethostname(hostName,sizeof(hostName))!=0)
		DisplayError(WSAError(WSAGetLastError()), WSAGetLastError(), FALSE );

	//MessageBox(NULL,TEXT(remoteHost->h_name),TEXT("Listening to"),MB_OK);
	MessageBox(NULL,TEXT(hostName),TEXT("Listening to"),MB_OK);

	//do {
	//		listen(serverSocket,SOMAXCONN);
	//	} while ( serverSocket!=SOCKET_ERROR );
	if (listen(orgSocket, SOMAXCONN) == SOCKET_ERROR)
        DisplayError("ERROR listening in the server socket",WSAGetLastError(), TRUE );

	addresSize = sizeof(incomingAddress);
	newConnection = accept(orgSocket,(struct sockaddr *)&incomingAddress,&addresSize);
	
	int iResult = closesocket(orgSocket);
    if (iResult == SOCKET_ERROR) {
        wprintf(L"close socket function failed with error %d\n", WSAGetLastError());
        WSACleanup();      
    }

}


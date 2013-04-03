#include "stdafx.h"

#pragma warning(disable: 4996)

using namespace std;

void StartServer(serverStruct server)
{
	struct addrinfo hints,
					*sAddr;

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

	SOCKET serverSocket = socket(sAddr->ai_family, sAddr->ai_socktype, sAddr->ai_protocol);
	
	hostent *remoteHost = gethostbyaddr((char *) &sAddr->ai_addr, sAddr->ai_addrlen , sAddr->ai_family);	

	if(remoteHost == NULL)
		DisplayError(WSAError(WSAGetLastError()), WSAGetLastError(), EXIT );

	if ( bind(serverSocket, sAddr->ai_addr, sAddr->ai_addrlen) == SOCKET_ERROR )  
	{
        DisplayError(WSAError(WSAGetLastError()), WSAGetLastError(), TRUE );       
	}
	
	MessageBox(NULL,TEXT(remoteHost->h_name),TEXT("Listening to"),MB_OK);

	//do {
	//		listen(serverSocket,SOMAXCONN);
	//	} while ( serverSocket!=SOCKET_ERROR );
	 if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
        DisplayError("ERROR listening in the server socket",WSAGetLastError(), TRUE );
	int iResult = closesocket(serverSocket);
    if (iResult == SOCKET_ERROR) {
        wprintf(L"closesocket function failed with error %d\n", WSAGetLastError());
        WSACleanup();      
    }

}


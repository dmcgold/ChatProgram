#include "Connect.h"

#pragma warning(disable: 4996)
using namespace std;

int ConnectChat( chatStruct callWho )
{
	WSADATA wsaData = {0};

	sockaddr_in sAddr;
	SOCKET serverSocket;

	if ( WSAStartup(MAKEWORD(2,2), &wsaData ) !=0 )
		return 1;
	
	sAddr.sin_family = AF_INET;
	sAddr.sin_addr.s_addr = inet_addr( callWho.ipAddress );
	sAddr.sin_port = htons( callWho.portNo );

	if ( serverSocket = socket(AF_INET, SOCK_STREAM,IPPROTO_TCP) == INVALID_SOCKET )
	{
		MessageBox(NULL,TEXT("Socket Initialisation Error"),TEXT("Invalid Socket"),MB_OK );
		return 1;
	} 
	hostent *remoteHost = gethostbyaddr((char *) &sAddr.sin_addr, sizeof(sAddr.sin_addr) , sAddr.sin_family);
	
	if ( bind(serverSocket, (SOCKADDR*)&sAddr,sizeof(SOCKADDR_IN)) == SOCKET_ERROR )  
		DisplayError("Host Error",WSAGetLastError(), ABORT );        

	if ( listen(serverSocket,0) == SOCKET_ERROR )
		DisplayError("ERROR listening in the server socket",WSAGetLastError(), ABORT );

	MessageBox(NULL,TEXT("Connection OK"),TEXT("Status"),MB_OK);
	return 0;
}

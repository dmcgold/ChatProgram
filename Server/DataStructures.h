#include <tchar.h>
#include <WinSock2.h>

#pragma once

#define BUFFER_SIZE 1024

struct clientStruct {
	SOCKET	inSocket;
	TCHAR	nickName[50],
		sendTo[50],
		sendMSG[BUFFER_SIZE];
	u_short portNo;
	TCHAR	ipAddress[46];
	clientStruct *next;
};

struct clientStructA {
	char sendFrom[50],
		sendTo[50],
		sendMSG[BUFFER_SIZE];
};

struct serverStruct {
	SOCKET	inSocket;
	u_short portNo;
	TCHAR ipAddress[46];
	int maxConnections,
		noConnections,
		isConnected;
};

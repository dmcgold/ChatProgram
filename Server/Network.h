#include <WinSock2.h>
#include <ws2tcpip.h>
#include "Help.h"
#include <Mstcpip.h>
#include "Strings.h"

#pragma comment(lib, "Ws2_32.lib")

#pragma once
#pragma warning(disable: 4996)

#define WM_SERVER_SOCKET WM_USER+200
#define WM_CLIENT_SOCKET WM_USER+201

#define WIN32_LEAN_AND_MEAN
#define BUFFER_SIZE 1024

struct clientStruct {
	SOCKET	inSocket;
	TCHAR	nickName[50],
			sendTo[50],
			sendMSG[BUFFER_SIZE];
	u_short portNo;
	TCHAR	ipAddress[INET_ADDRSTRLEN];
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
	TCHAR ipAddress[INET_ADDRSTRLEN];
	int maxConnections,
		noConnections,
		isConnected;
};

// Server

int StartServer(HWND ,serverStruct *);
int GetIncoming(clientStruct *,int );
int AddConnection(clientStruct **,int, int);
SOCKET GetClosedSocket(struct clientStruct *);
int ShutDown(SOCKET);
void ReadSocket(HWND ,LPARAM ,serverStruct *,clientStruct **);
TCHAR *DeleteClient(clientStruct **,int );
void SendText(HWND ,SOCKET, clientStruct *);
void RelayMSG(HWND ,clientStruct *,SOCKET);
SOCKET GetClient(clientStruct *,TCHAR *);

// Client

int GetIncoming(clientStruct *);
BOOLEAN ConnectToServer(HWND,serverStruct *,clientStruct );
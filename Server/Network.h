#include <WinSock2.h>
#include <ws2tcpip.h>
#include "Help.h"
#include "Strings.h"
#include "DataStructures.h"

#pragma comment(lib, "Ws2_32.lib")

#pragma once
#pragma warning(disable: 4996)

#define WM_SERVER_SOCKET WM_USER+200
#define WM_CLIENT_SOCKET WM_USER+201

#define WIN32_LEAN_AND_MEAN
#define BUFFER_SIZE 1024

// Server

TCHAR *DeleteClient(clientStruct **,int);
int StartServer(HWND ,serverStruct *);
void ReadSocket(HWND ,LPARAM ,serverStruct *,clientStruct **);
int AddConnection(clientStruct **,int, int);
SOCKET GetClosedSocket(struct clientStruct *);
int ShutDown(SOCKET);
void SendText(HWND ,SOCKET, clientStruct *);
void RelayMSG(HWND ,clientStruct *,SOCKET);

// Client

clientStruct FindClient(clientStruct *,TCHAR *);
int ClientIncoming(SOCKET,clientStruct *);
BOOLEAN ConnectToServer(HWND,serverStruct *,clientStruct );
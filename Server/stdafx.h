// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#undef UNICODE

#include "targetver.h"
#include "resource.h"
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <windows.h>
#include <string>
#include "Utils.h"

#define WM_SERVER_SOCKET WM_USER+200

#define WIN32_LEAN_AND_MEAN

struct serverStruct {
						u_short portNo;	
						int noConnections;
						short family;
						char ipAddress[15];	
						SOCKET	socketServer,
								socketConnection;
					};

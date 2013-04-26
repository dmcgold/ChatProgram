#pragma once
#pragma comment(lib,"ws2_32.lib")

#include "targetver.h"
#include "resource.h"
#include <WinSock2.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <windows.h>
#include <string>
#include "Utils.h"

#define WIN32_LEAN_AND_MEAN

struct chatStruct {
					u_short portNo;
					BOOLEAN privateChat;
					char nickName[100];
					char ipAddress[15];
					SOCKET	socketClient,
							socketConnection;
				};

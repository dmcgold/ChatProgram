#include "stdafx.h"

#ifndef CONNECT_H
#define CONNECT_H
#define ABORT TRUE

struct chatStruct {
					u_short portNo;
					BOOLEAN privateChat;
					char nickName[100];
					char ipAddress[15];
					SOCKET	socketClient,
							socketConnection;
				};

int ConnectChat( chatStruct );


#endif


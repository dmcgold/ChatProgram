#include "Network.h"
#include "Help.h"
#include<TCHAR.H>
#include <strsafe.h>

#pragma once

void RelayMSG(HWND ,clientStruct *,SOCKET);
SOCKET GetClient(clientStruct *,TCHAR *);
TCHAR *GetTimeDateStr(void);

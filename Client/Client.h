#include "..\Server\Network.h"
#include <windows.h>
#include <string.h>
#include "..\Server\Help.h"
#include "resource.h"
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include "..\Server\Strings.h"

#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

serverStruct server;
clientStruct client;

HWND	chatWin,
	inputWin,
	sendBTN,
	userWin;
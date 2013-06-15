#include "DataStructures.h"             // Client Structure, Server Structure FindClient function,Delete client function
#include "Network.h"                    // Functions to manipulate networking
#include <windows.h>
#include <string.h>
#include "Help.h"
#include "resource.h"
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include "Strings.h"
#include "GUI.h"

#pragma once

#define extern BUFFER_SIZE 512                 // Only n characters will be sent and received at a time
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

// Global variables

clientStruct *clients=(clientStruct *) _malloca(sizeof(clientStruct)); 	// Storage of client information
serverStruct *server=(serverStruct *) _malloca(sizeof(serverStruct));   // Storage structure for Server information, load with defaults

HWND	logWindow;	
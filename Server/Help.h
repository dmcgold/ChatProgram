#include <Windows.h>
#include <stdio.h>
#include <string>
#include <TCHAR.H>
#include <strsafe.h>

#pragma once

#define helpFile "HelpFile.dat"

#define HELP_BUFFER_SIZE 0x400

#define POPMSG 0x001
#define LOG 0x002
#define LOGPOP 0x003
#define WINDOW 0x004
#define LOGWIN 0x006
#define POPWIN 0x007
#define ALL 0x00F
#define NOTICE 0x010
#define WARNING 0x011
#define ERRORFOUND 0x012
#define MAX_SIZE (HELP_BUFFER_SIZE * sizeof(TCHAR))

//#define FULL_HELP 1  // Display full help
//#define LO_OK 1  // Log to file of not

#define LOGFILE "LogFile.txt"

struct helpStruc {
	int errorCode;
	TCHAR	errorStr[50],
		errorHelp[150],
		moreHelp[899];
};

BOOLEAN FindError(int , helpStruc &);
void DisplayError(HWND ,TCHAR *,int ,int, int);
void DisplayToMSGBox(helpStruc );
void DisplayToMSGBox(TCHAR *);
void DisplayToLogFile(TCHAR *,int);
TCHAR *GetErrorStr(int );
void DisplayToLogFile(helpStruc,int);
void DisplayToWindow(HWND hWnd, helpStruc);
void DisplayToWindow(HWND hWnd, TCHAR *);

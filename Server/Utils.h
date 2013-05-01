#include <stdio.h>
#include <Windows.h>
#define helpFile "WSA_Errors.dat"

struct helpStruc {
	int errorCode;
	char	errorCodeStr[50],
		errorHelp[100],
		detailedHelp[800];
};

#define EXIT TRUE
#define NO_EXIT FALSE

void DisplayError(HWND,int);
boolean WSAError(int , helpStruc &);
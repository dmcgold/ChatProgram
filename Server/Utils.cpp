#include "Utils.h"

#pragma warning(disable: 4996)

void DisplayError( char *errorMSG,int errorCode,BOOLEAN exitProgram)
{
	char str[10];	
	char str2[50]="Error Code : ";
	
	itoa(errorCode, str, 10);
	strcat_s(str2,50,str);

	MessageBox(NULL, TEXT(errorMSG) ,TEXT(str2) ,MB_OK);
	if(exitProgram)
		exit(1);
}

char *WSAError(int errorCode)
{
	switch (errorCode)
	{
	case WSATRY_AGAIN		: return ("A temporary failure in name resolution occurred."); break;
	case WSAEINVAL			: return ("An invalid value was provided for the ai_flags\n member of the pHints parameter.");break;
	case WSANO_RECOVERY		: return ("A non recoverable failure \n in name resolution \n occurred. ");break;
	case WSAEAFNOSUPPORT	: return ("The ai_family member of \n the pHints parameter \n is not supported.");break; 
	case 8					: return ("A memory allocation failure occurred.");break;
	case WSAHOST_NOT_FOUND	: return ("The name does not resolve for\n the supplied \n parameters or the pNodeName \n and pServiceName parameters were not provided.");break;
	case WSATYPE_NOT_FOUND	: return ("The pServiceName parameter \n is not supported \n for the specified ai_socktype \n member of the pHints parameter.");break;
	case WSAESOCKTNOSUPPORT	: return ("The ai_socktype member of \n the pHints parameter\n is not supported.");break;
	}
	return("Unknown error");
}

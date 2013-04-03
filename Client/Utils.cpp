#include "Utils.h"

void DisplayError( char *errorMSG,int errorCode,BOOLEAN exitProgram)
{
	char str[10];
	char str2[]="Error Code : ";
	itoa(errorCode, str, 10);
	strcat(str2,str);

	MessageBox(NULL, errorMSG, str2 ,MB_OK);
	if(exitProgram)
		exit(1);
}


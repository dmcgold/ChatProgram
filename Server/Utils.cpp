#include "Utils.h"
#pragma warning(disable: 4996)

void DisplayError(HWND hWnd,int errorCode)
{
	helpStruc helpDetails;
	TCHAR	tmpBuffer[1000];
	int len=100;

	SecureZeroMemory(&helpDetails,sizeof(helpDetails));
	SecureZeroMemory(tmpBuffer,sizeof(tmpBuffer));

	WSAError(errorCode,helpDetails);

	wsprintf(tmpBuffer,"Error Code: %d",helpDetails.errorCode);
	SendMessage(hWnd,LB_ADDSTRING, 0, (LPARAM)tmpBuffer);
	wsprintf(tmpBuffer,"Error name: %s",helpDetails.errorCodeStr);
	SendMessage(hWnd,LB_ADDSTRING, 0, (LPARAM)tmpBuffer);
	wsprintf(tmpBuffer,"Brief explanation: %s",helpDetails.errorHelp);
	SendMessage(hWnd,LB_ADDSTRING, 0, (LPARAM)tmpBuffer);
	SendMessage(hWnd,LB_ADDSTRING , 0, (LPARAM)"More information: ");
	for(int a=0;a<strlen(helpDetails.detailedHelp);a+=len)
	{
		memcpy( tmpBuffer, &helpDetails.detailedHelp[a], len );
		tmpBuffer[len]='\n';
		SendMessage(hWnd,LB_ADDSTRING , len, (LPARAM)tmpBuffer);
		SecureZeroMemory(tmpBuffer,sizeof(tmpBuffer));
	}
}

boolean WSAError(int errorCode,helpStruc &help)
{
	FILE *fpHelp=fopen(helpFile,"rb");

	if(fpHelp==NULL)
	{
		strcpy(help.errorCodeStr,"Can't open help file");
		help.errorCode=-1;
		return (false);
	}

	fread(&help,sizeof(help),1,fpHelp);
	while ((!feof(fpHelp)) && (help.errorCode!=errorCode))
		fread(&help,sizeof(help),1,fpHelp);
	fclose(fpHelp);
	if(help.errorCode==errorCode)
		return (true);
	else
	{
		strcpy(help.errorCodeStr,"Error not found");
		help.errorCode=-1;
		return (false);
	}
}
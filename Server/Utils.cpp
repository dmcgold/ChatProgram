#include "Utils.h"
#pragma warning(disable: 4996)

void DisplayError(char *errorCodeStr,int errorCode,BOOLEAN exitProgram)
{
	char str[50];
	char helpText[100]="";
	char strCode[6];
	helpStruc helpDetails;
	/*HWND helpWindow;
	TCHAR tmpBuffer[1000];*/

	SecureZeroMemory(&helpDetails,sizeof(helpDetails));

	if(errorCode!=0)
	{
		strcpy(str,errorCodeStr);
		strcat_s(str,50," : ");
		_itoa(errorCode, strCode, 10);
		strcat_s(str,strlen(str)+strlen(strCode)+1,strCode);
		if(WSAError(errorCode,helpDetails))
		{
			strcpy(helpText,helpDetails.errorCodeStr);
			strcat_s(helpText,100," : \n");
			strcat_s(helpText,100,helpDetails.errorHelp);
		}
		/*helpWindow = CreateWindow(	"LISTBOX",
		"Help",
		WS_VISIBLE|ES_LEFT|WS_BORDER|
		ES_READONLY|WS_VSCROLL|WS_HSCROLL,
		50,50,
		1000,400,
		NULL,
		NULL,
		NULL,
		NULL);
		if(helpWindow==NULL)
		{
		char errorStr[100];
		_itoa(GetLastError(),errorStr,10);
		MessageBox(NULL, TEXT("Problem with Window") ,TEXT(errorStr) ,MB_OK);
		}
		else
		{
		wsprintf(tmpBuffer,"Error Code: %d",helpDetails.errorCode);
		SendMessage(helpWindow,LB_ADDSTRING, 0, (LPARAM)tmpBuffer);
		wsprintf(tmpBuffer,"Error name: %s",helpDetails.errorCodeStr);
		SendMessage(helpWindow,LB_ADDSTRING, 0, (LPARAM)tmpBuffer);
		wsprintf(tmpBuffer,"Brief explanation: %s",helpDetails.errorHelp);
		SendMessage(helpWindow,LB_ADDSTRING, 0, (LPARAM)tmpBuffer);
		wsprintf(tmpBuffer,"More information: %s",helpDetails.detailedHelp);
		SendMessage(helpWindow,LB_ADDSTRING|LB_SETHORIZONTALEXTENT , 1000, (LPARAM)tmpBuffer);
		}*/

		MessageBox(NULL, TEXT(helpText) ,TEXT(str) ,MB_OK);
	}
	else
		MessageBox(NULL, TEXT(errorCodeStr), TEXT("Warning") ,MB_OK);
	if(exitProgram)
		exit(1);
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
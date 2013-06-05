#include "Strings.h"

#pragma warning(disable:4996)

TCHAR *ProcessData (TCHAR *sendTo,TCHAR *sendMSG,TCHAR *inBuffer)
{
	DisplayError(NULL,TEXT(__FUNCSIG__),0,ALL,NOTICE);

	TCHAR *firstMSG,
		*nextMSG,
		*userName;

	int strLen=0;

	firstMSG = new TCHAR[100];
	nextMSG = new TCHAR[100];
	userName = new TCHAR[50];

	firstMSG=nextMSG=userName=TEXT("");

	firstMSG=_tcstok_s(inBuffer,TEXT("|"),&nextMSG);

	if(nextMSG!=NULL)
		strLen=_tcslen(nextMSG);

	while(strLen>0)
	{
		if(_tcscmp(firstMSG,TEXT("REQ_DATA"))==0)
		{
			_tcscpy(inBuffer,firstMSG);
			break;
		}
		if(_tcscmp(firstMSG,TEXT("USER"))==0)
		{
			firstMSG=_tcstok_s(NULL,TEXT("|"),&nextMSG);
			userName=firstMSG;
		}
		if(_tcscmp(firstMSG,TEXT("SENDTO"))==0)
		{
			firstMSG=_tcstok_s(NULL,TEXT("|"),&nextMSG);
			sendTo=firstMSG;
		}

		firstMSG=_tcstok_s(NULL,TEXT("|"),&nextMSG);
		if(nextMSG!=NULL)
			strLen=_tcslen(nextMSG);
		else
			strLen=0;
	}
	sendMSG=firstMSG;
	return userName;
}

TCHAR *GetTimeDateStr()															// Return date and time [01/01/2013]:[22:29:22]
{
	TCHAR *tStr = new TCHAR[50];
	SYSTEMTIME time;

	GetLocalTime(&time);

	StringCbPrintf(tStr,50,TEXT("[%02d/%02d/%04d]:[%02d:%02d:%02d]"),time.wDay,time.wMonth,time.wYear, time.wHour,time.wMinute,time.wSecond);
	return tStr;
}
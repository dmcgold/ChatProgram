#include"Help.h"

#pragma warning(disable: 4996)

BOOLEAN FindError(int errorCode,helpStruc &help)
{
	FILE *fpHelp=fopen(helpFile,"rb");

	if(fpHelp==NULL)
	{
		_tcscpy_s(help.errorStr,25*sizeof(TCHAR),TEXT("Can't open help file"));
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
		_tcscpy_s(help.errorStr,50,TEXT("Error not found"));
		help.errorCode=-1;
		return (false);
	}
}

void DisplayError(HWND hWnd,TCHAR *msg,int errorCode,int where,int errorLevel)
{
	/*
	hWnd - Window used for output, can be null
	msg - User defined msg to be used when errorCode is 0
	where -  Where the output is to go either Log file, MessageBox  Window
	errorLevel - Refers to level of error either NOTICE = just a msg, WARNING could be a problem, ERROR it's a problem
	if errorLevel is used then output will always be to LogFile
	*/

	SYSTEMTIME time;
	GetLocalTime(&time);
	helpStruc helpDetails={0};

	if(!FindError(errorCode,helpDetails))
		errorCode=0;

	if((errorCode>0) && (where!=ALL) && !(LOG & where))
	where=LOG & where;  // Errorcodes > 0 should go to logfile

	switch(where)
	{
	case POPMSG:
		{
			if(errorCode>0)
				DisplayToMSGBox(helpDetails);
			else
				DisplayToMSGBox(msg);
		}
		break;
	case LOGPOP:
		{
			if(errorCode>0)
			{
				DisplayToLogFile(helpDetails,errorLevel);
				DisplayToMSGBox(helpDetails);
			}
			else
			{
				DisplayToLogFile(msg,errorLevel);
				DisplayToMSGBox(msg);
			}
		}
		break;
	case LOG:
		{
			if(errorCode>0)
				DisplayToLogFile(helpDetails,errorLevel);
			else
				DisplayToLogFile(msg,errorLevel);
		}
		break;
	case WINDOW:
		{
			if(errorCode>0)
				DisplayToWindow(hWnd,helpDetails);
			else
				DisplayToWindow(hWnd,msg);
		}
		break;

	case LOGWIN:
		{
			if(errorCode>0)
			{
#ifdef LOG_OK   // Only log to file when needed
				DisplayToLogFile(helpDetails,errorLevel);
#endif
				DisplayToWindow(hWnd,helpDetails);
			}
			else
			{
#ifdef LOG_OK   // Only log to file when needed
				DisplayToLogFile(msg,errorLevel);
#endif
				DisplayToWindow(hWnd,msg);
			}
		}
		break;
	case POPWIN:
		{
			if(errorCode>0)
			{
				DisplayToMSGBox(helpDetails);
				DisplayToWindow(hWnd,helpDetails);
			}
			else
			{
				DisplayToMSGBox(msg);
				DisplayToWindow(hWnd,msg);
			}
		}
		break;
	case ALL:
		{
			if(errorCode>0)														// if errorcode is > 0 display help detailsg
			{
				DisplayToMSGBox(helpDetails);
				DisplayToLogFile(helpDetails,errorLevel);
				DisplayToWindow(hWnd,helpDetails);
			}
			else																// Otherwise display msg
			{
				DisplayToMSGBox(msg);
				DisplayToLogFile(msg,errorLevel);
				DisplayToWindow(hWnd,msg);
			}
		}
		break;
	}
}

void DisplayToMSGBox(helpStruc help)
{
	TCHAR outBuffer[HELP_BUFFER_SIZE];
	_tcscpy_s(outBuffer,HELP_BUFFER_SIZE,_T(""));

	StringCbPrintf(outBuffer,MAX_SIZE,_T("Error Code : %d\n Error Help : %s\n"),help.errorCode,help.errorHelp );

#ifdef FULL_HELP							// If full help break full details into smaller strings to display in messagebox
	_tcscat(outBuffer,TEXT("More information: \n"));
	int strLen=_tcslen(help.moreHelp);
	TCHAR tmpSTR[512] =_T("");

	for(int a=0;a<strLen;a+=100)
	{
		memset ( (void *)&tmpSTR, '\0', sizeof(TCHAR) * 1000);

		memcpy(tmpSTR, &help.moreHelp[a], 100);
		tmpSTR[100]='\n';
		_tcscat(outBuffer,tmpSTR);
	}
#endif
	MessageBox(NULL,outBuffer,help.errorStr,MB_OK);
}

void DisplayToMSGBox(TCHAR *msg)
{
	MessageBox(NULL,msg,TEXT("System Message"),MB_OK);
}

void DisplayToLogFile(helpStruc help,int errorLevel)		// Send the help details out to the log file
{
	FILE *fpLogFile;
	TCHAR outBuffer[HELP_BUFFER_SIZE];
	SYSTEMTIME time;
	GetLocalTime(&time);
	fpLogFile=fopen(LOGFILE,"a");
	char line[80];

	memset(&line,'-',79);
	line[79]='\0';
	_tcscpy_s(outBuffer,HELP_BUFFER_SIZE,_T(""));

	if(fpLogFile==NULL)
	{
		StringCbPrintf(outBuffer,MAX_SIZE,_T("Can't open %hs file\nPress any key to cont\n"),LOGFILE);
		MessageBox(NULL,(LPWSTR)outBuffer,_T("File Error"),MB_OK);
		return;
	}

	_ftprintf(fpLogFile,_T("%s logged [%02d/%02d/%04d]:[%02d:%02d:%02d]\n"),GetErrorStr(errorLevel),time.wDay,time.wMonth,time.wYear, time.wHour,time.wMinute,time.wSecond);
	_ftprintf(fpLogFile,_T("Error Number : %d\n"),help.errorCode);
	_ftprintf(fpLogFile,_T("Error Code: %s\n"),help.errorStr);
	_ftprintf(fpLogFile,_T("Error Description: %s\n"),help.errorHelp);

#ifdef FULL_HELP
	_ftprintf(fpLogFile,_T("Full description: %s\n"),help.moreHelp);
#endif

	fprintf(fpLogFile,"%s\n",line);
	fclose(fpLogFile);
}

TCHAR *GetErrorStr(int level)
{
	switch(level)
	{
	case ERRORFOUND:
		return _T("Serious Error");
		break;
	case WARNING:
		return _T("Warning message");
		break;
	case NOTICE:
		return _T("Notice");
		break;
	}
	return _T("Wrong Parameter");
}

void DisplayToLogFile(TCHAR *msg,int errorLevel)			// Send user defined msg out to the Log file
{
	FILE *fpLogFile;
	TCHAR outBuffer[HELP_BUFFER_SIZE];
	SYSTEMTIME time;
	GetLocalTime(&time);
	char line[80];
	TCHAR logFile[]=_T(LOGFILE);

	memset(&line,'-',79);
	line[79]='\0';

	_tcscpy_s(outBuffer,HELP_BUFFER_SIZE,_T(""));
	fpLogFile=fopen(LOGFILE,"a");

	if(fpLogFile==NULL)
	{
		StringCbPrintf(outBuffer,MAX_SIZE,_T("Can't open %s file\nPress any key to cont\n"),logFile);
		MessageBox(NULL,(LPWSTR)outBuffer,_T("File Error"),MB_OK);
		return;
	}
	_ftprintf(fpLogFile,_T("%s logged: [%02d/%02d/%04d]:[%02d:%02d%02d]\n"),GetErrorStr(errorLevel),time.wDay,time.wMonth,time.wYear, time.wHour,time.wMinute,time.wSecond);
	_ftprintf(fpLogFile,_T("%s\n"),msg);
	fprintf(fpLogFile,"%s\n",line);
	fclose(fpLogFile);
}

void DisplayToWindow(HWND hWnd, helpStruc help)											// Display help in the log window
{
	TCHAR outBuffer[HELP_BUFFER_SIZE] =_T("");
	SYSTEMTIME time;
	GetLocalTime(&time);

	_tcscpy_s(outBuffer,HELP_BUFFER_SIZE,_T(""));
	StringCbPrintf(outBuffer,MAX_SIZE,_T("Logged [%02d/%02d/%04d]:[%02d:%02d%02d]"),time.wDay,time.wMonth,time.wYear, time.wHour,time.wMinute,time.wSecond);
	SendMessage(hWnd,LB_ADDSTRING,0,(LPARAM)outBuffer);

	StringCbPrintf(outBuffer,MAX_SIZE,_T("Error Number : %d, Error Code   : %s"),help.errorCode,help.errorStr);
	SendMessage(hWnd,LB_ADDSTRING,0,(LPARAM)outBuffer);
	StringCbPrintf(outBuffer,MAX_SIZE,_T("Error Description: %s"),help.errorHelp);
	SendMessage(hWnd,LB_ADDSTRING,0,(LPARAM)outBuffer);
#ifdef FULL_HELP
	StringCbPrintf(outBuffer,MAX_SIZE,_T("Full Description: %s"),help.moreHelp);
	SendMessage(hWnd,LB_ADDSTRING,0,(LPARAM)outBuffer);
#endif
}

void DisplayToWindow(HWND hWnd,TCHAR *msg)
{
	TCHAR outBuffer[HELP_BUFFER_SIZE];
	SYSTEMTIME time;
	GetLocalTime(&time);

	_tcscpy_s(outBuffer,HELP_BUFFER_SIZE,_T(""));

	StringCbPrintf(outBuffer,MAX_SIZE,_T("Logged [%02d/%02d/%04d]:[%02d:%02d%02d]"),time.wDay,time.wMonth,time.wYear, time.wHour,time.wMinute,time.wSecond);
	SendMessage(hWnd,LB_ADDSTRING,0,(LPARAM)outBuffer);
	SendMessage(hWnd,LB_ADDSTRING,0,(LPARAM)msg);
}
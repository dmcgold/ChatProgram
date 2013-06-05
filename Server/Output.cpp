#include "Output.h"

void RelayMSG(HWND hWnd,clientStruct *clients,SOCKET inSocket)
{
	//	This function will relay messages to the designated client(s)

	SOCKET sendSocket=inSocket;
	TCHAR *tmpBuffer,
		*nickName,
		*sendMSG;
	BOOLEAN sendAll = FALSE;
	clientStruct *tmpClients = clients;

	DisplayError(NULL,TEXT(__FUNCTION__),0,ALL,NOTICE);

	tmpBuffer= new TCHAR[BUFFER_SIZE];

	// Find the Sender
	while((tmpClients!=NULL) && (tmpClients->inSocket!=inSocket))
	{
		tmpClients=tmpClients->next;
	}
	// Sender not found so leave

	if(tmpClients==NULL)
		return;

	if(_tcscmp(tmpClients->sendTo,TEXT("PUBLIC"))==0)
	{
		sendAll=TRUE;
		StringCbPrintf(tmpBuffer,BUFFER_SIZE,TEXT("Relaying incoming message to %s "),TEXT("all users"));
	}
	else
		StringCbPrintf(tmpBuffer,BUFFER_SIZE,TEXT("Relaying incoming message to %s "),clients->sendTo);

	DisplayError(NULL,tmpBuffer,0,ALL,NOTICE);
	// Store sender details

	nickName=tmpClients->nickName;
	sendMSG=tmpClients->sendMSG;

	if (sendAll) // Send to all except the sender
	{
#pragma warning(suppress: 6303)
		// Display message in server log window if it's open
		if(hWnd!=NULL)
		{
			StringCbPrintf(tmpBuffer,BUFFER_SIZE,TEXT("Message from %s: %s "),nickName,sendMSG);
			SendMessage(hWnd,LB_ADDSTRING, 0, (LPARAM)tmpBuffer);
		}
		tmpClients=clients;
		while(tmpClients!=NULL)													// Send to all users
		{
			if(tmpClients->inSocket!=inSocket)
				send(tmpClients->inSocket,(char *)sendMSG,_tcslen(tmpBuffer),0);
			tmpClients=tmpClients->next;
		}
	}
	else																		// Send a single user
		send(GetClient(clients,clients->sendTo),(char *)sendMSG,_tcslen(tmpBuffer),0);
	delete [] tmpBuffer; // is new [] is used then so it delete []
}

SOCKET GetClient(clientStruct *clients,TCHAR *user)								// Get the socket of a particulare user
{
	// Find the socket of Client based on nickName
	while(clients!=NULL)
	{
		if(_tcscmp(clients->sendTo,user)==0)
			return clients->inSocket;
	}
	return 0;
}

TCHAR *GetTimeDateStr()															// Return date and time [01/01/2013]:[22:29:22]
{
	TCHAR *tStr = new TCHAR[50];
	SYSTEMTIME time;

	GetLocalTime(&time);

	StringCbPrintf(tStr,50,TEXT("[%02d/%02d/%04d]:[%02d:%02d:%02d]"),time.wDay,time.wMonth,time.wYear, time.wHour,time.wMinute,time.wSecond);
	return tStr;
}
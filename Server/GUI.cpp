#include "GUI.h"

INT_PTR CALLBACK Settings(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	BOOL isSigned=FALSE;
	BOOL *didPass=FALSE;

	static serverStruct *tmpServer;

	switch (message)
	{
	case WM_INITDIALOG:
		{
			tmpServer=(serverStruct*)lParam;
			SetDlgItemInt(hDlg, IDC_PORTNO, tmpServer->portNo,isSigned);							// Initialise Edit boxes
			SetDlgItemInt(hDlg, IDC_NO_CONNECTIONS, tmpServer->maxConnections,isSigned);
			SetDlgItemText(hDlg, IDC_IPADDRESS, tmpServer->ipAddress);
		}
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			if (LOWORD(wParam) == IDOK )														// If the user clicked on Ok
			{																					// then save the new data
				tmpServer->portNo=GetDlgItemInt(hDlg, IDC_PORTNO, didPass,isSigned);
				tmpServer->maxConnections=GetDlgItemInt(hDlg, IDC_NO_CONNECTIONS, didPass,isSigned);
				GetDlgItemText(hDlg, IDC_IPADDRESS,tmpServer->ipAddress,INET_ADDRSTRLEN );
			}
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK ListClients(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	LVCOLUMN lvCol={0};
	LVITEM lvItem={0};
	clientStruct *tmpClients;
	RECT Rect;
	HWND clientList=NULL;
	int counter=0;

	//	HINSTANCE hInst;

	switch (message)
	{
	case WM_INITDIALOG:
		{
			// Window to display a list of clients
			tmpClients=(clientStruct*)lParam;
			clientList = GetDlgItem(hDlg,IDC_CLIENT_LIST);
			SetFocus(GetDlgItem(hDlg,IDOK));

			GetWindowRect(hDlg,&Rect);
			ListView_SetExtendedListViewStyle(clientList,WS_VSCROLL |WS_HSCROLL );
			lvCol.mask = LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH  | LVCF_FMT;
			lvCol.fmt  = LVCFMT_LEFT;
			int quarterScreen = ((Rect.right-Rect.left) / 4);
			lvCol.iSubItem	= 0;
			lvCol.cx		= quarterScreen;
			lvCol.pszText	= TEXT("Client's IP Address");
			ListView_InsertColumn(clientList, 0, &lvCol);

			lvCol.iSubItem	= 1;
			lvCol.cx		= quarterScreen * 3;
			lvCol.pszText	= TEXT("Client's User Name");
			ListView_InsertColumn(clientList, 1, &lvCol);
			while(tmpClients!=NULL) // Get the list of clients
			{
				lvItem.iItem = counter; // Create a new column
				ListView_InsertItem(clientList, &lvItem); // Insert new column
				ListView_SetItemText(clientList, counter, 0, tmpClients->ipAddress);
				ListView_SetItemText(clientList, counter, 1, tmpClients->nickName);
				counter++;
				tmpClients=tmpClients->next;
			}
			ShowWindow(clientList,SW_SHOW);
		}
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			ShowWindow(clientList,SW_HIDE);
			DestroyWindow(clientList);
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK LogWindow(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		int wmID=LOWORD(wParam);
		int wmEvent=HIWORD(wParam);
		switch(wmID)
		{
		case IDOK:
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		default:
			return DefWindowProc(hDlg,message,wParam,lParam);
		}
		break;
	}
	return (INT_PTR)FALSE;
}
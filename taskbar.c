#include "taskbar.h"

// MyTaskBarAddIcon - adds an icon to the taskbar notification area.
// Returns TRUE if successful or FALSE otherwise.
// hwnd - handle of the window to receive callback messages
// uID - identifier of the icon
// hicon - handle of the icon to add
// lpszTip - tooltip text
BOOL MyTaskBarAddIcon(HWND hwnd, UINT uID, HICON hicon, LPSTR lpszTip)
{
	BOOL res;
	NOTIFYICONDATA tnid;

	tnid.cbSize = sizeof(NOTIFYICONDATA);
	tnid.hWnd = hwnd;
	tnid.uID = uID;
	tnid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	tnid.uCallbackMessage = WM_USER+1;
	tnid.hIcon = hicon;
	if (lpszTip)
		lstrcpyn(tnid.szTip, lpszTip, sizeof(tnid.szTip));
	else
	    tnid.szTip[0] = '\0';

	res = Shell_NotifyIcon(NIM_ADD, &tnid);

	if (hicon)
		DestroyIcon(hicon);

	return res;
}
// MyTaskBarModifyIcon - modifies an icon in the taskbar notification area.
// Returns TRUE if successful or FALSE otherwise.
// hwnd - handle of the window to receive callback messages
// uID - identifier of the icon
// hicon - handle of the icon to add
// lpszTip - tooltip text
static BOOL MyTaskBarModifyIcon(HWND hwnd, UINT uID, HICON hicon, LPSTR lpszTip)
{
	BOOL res;
	NOTIFYICONDATA tnid;

	tnid.cbSize = sizeof(NOTIFYICONDATA);
	tnid.hWnd = hwnd;
	tnid.uID = uID;
	tnid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	tnid.uCallbackMessage = WM_USER+1;
	tnid.hIcon = hicon;
	if (lpszTip)
		lstrcpyn(tnid.szTip, lpszTip, sizeof(tnid.szTip));
	else
	    tnid.szTip[0] = '\0';

	res = Shell_NotifyIcon(NIM_MODIFY, &tnid);

	if (hicon)
		DestroyIcon(hicon);

	return res;
}

// MyTaskBarDeleteIcon - deletes an icon from the taskbar
//     notification area.
// Returns TRUE if successful or FALSE otherwise.
// hwnd - handle of the window that added the icon
// uID - identifier of the icon to delete
BOOL MyTaskBarDeleteIcon(HWND hwnd, UINT uID)
{
	BOOL res;
	NOTIFYICONDATA tnid;

	tnid.cbSize = sizeof(NOTIFYICONDATA);
	tnid.hWnd = hwnd;
	tnid.uID = uID;

	res = Shell_NotifyIcon(NIM_DELETE, &tnid);
	return res;
}

void ModifyTrayIcon (HWND hMsgDlg, int iconresource, LPSTR tooltip )
{
	HICON hicon;
	hicon=LoadIcon((HINSTANCE)0x400000,MAKEINTRESOURCE(iconresource));
	MyTaskBarModifyIcon(hMsgDlg, 1234, hicon, tooltip);
}

void LoadTrayMenu(HWND mMsgDlg)
{
	MENUITEMINFO mninfo;

	memset ( &mninfo, 0, sizeof(MENUITEMINFO));
	hmenu=GetSubMenu(LoadMenu((HINSTANCE)0x400000,MAKEINTRESOURCE(MN_POPUP)),0);
	mninfo.cbSize=sizeof(MENUITEMINFO);
	mninfo.fMask=MIIM_STATE;
	mninfo.fState=MFS_DEFAULT;
	SetMenuItemInfo ( hmenu, MN_CONFIG, FALSE, &mninfo );
	mninfo.fMask=MIIM_ID;
	mninfo.wID=MN_SUBADD;
	SetMenuItemInfo ( hmenu, 4, TRUE, &mninfo );
	TranslateMenu(hmenu);
}

void CreateTrayIcon(HWND hMsgDlg,HINSTANCE hinst,BOOL active)
{
	HICON hicon;

	if ( active ) {
		hicon=LoadIcon((HINSTANCE)0x400000,MAKEINTRESOURCE(ID_TRAYICON));
		MyTaskBarAddIcon(hMsgDlg, 1234, hicon, VERNAME);
	}
	else {
		hicon=LoadIcon((HINSTANCE)0x400000,MAKEINTRESOURCE(ID_ICODISABLED));
		MyTaskBarAddIcon(hMsgDlg, 1234, hicon, (char*)TranslateText(ID_CLICKOFFDISABLED,MSG_CLICKOFFDISABLED));
	}
	LoadTrayMenu(hMsgDlg);
}

static void AddWindowToMenu (HWND hwnd, int *menuID )
{
	if ( IsWindowVisible ( hwnd ) ) {

		MENUITEMINFO mninfo;
		ICONINFO iconInfo;

		memset(&mninfo,0,sizeof(mninfo));
		memset(&iconInfo,0,sizeof(iconInfo));

		LPSTR lpBuffer = malloc ( GetWindowTextLength ( hwnd )+1 );
		GetWindowText(hwnd,lpBuffer,4096);
		if ( strcmp ( lpBuffer,"" ) != 0 && strcmp ( lpBuffer,"Program Manager" ) != 0 ) {
			HBITMAP hIconOfWnd=(HBITMAP)GetClassLong(hwnd,GCL_HICONSM);
			GetIconInfo(hIconOfWnd,&iconInfo);

			mninfo.cbSize=sizeof(mninfo);
			mninfo.fMask=MIIM_ID | MIIM_TYPE | MIIM_CHECKMARKS;
			mninfo.fType=MFT_STRING;
			mninfo.fState=MFS_ENABLED | MFS_UNCHECKED;
			mninfo.wID=*menuID;
			//mninfo.hSubMenu=hmenu;
			mninfo.dwTypeData=lpBuffer;
			mninfo.hbmpUnchecked=iconInfo.hbmColor;
			InsertMenuItem(hmenu,MN_ADDWIN,FALSE,&mninfo);
			(*menuID)++;
		}
		free(lpBuffer);
	}
}

static BOOL CALLBACK EnumWindowsProc(HWND hWindow, LPARAM  lParam)
{
	AddWindowToMenu(hWindow,(int*)lParam);
	return TRUE;
}

int ShowMenu(HWND hwnd, int active)
{
	POINT Point;
	int menuID=MN_ADDWIN+1;
	RECT rect;
	MENUITEMINFO mninfo;
	memset(&rect,0,sizeof(RECT));

	GetCursorPos(&Point);
	CheckMenuItem(hmenu,MN_ACTIVE,MF_CHECKED*active);
	for ( int item=MN_ADDWIN+1; GetMenuState(hmenu,item,MF_BYCOMMAND)!=0xFFFFFFFF; item++ )
		DeleteMenu(hmenu,item,MF_BYCOMMAND);
	EnumWindows( (WNDENUMPROC) EnumWindowsProc, (LPARAM)&menuID );
	//Delete the dummy item
	DeleteMenu(hmenu,MN_ADDWIN,MF_BYCOMMAND);

	SetForegroundWindow(hwnd);
	SetFocus(hwnd);
	TrackPopupMenu(hmenu,TPM_RIGHTALIGN | TPM_LEFTBUTTON,Point.x,Point.y,0,hwnd,&rect);

	//Insert the dummy item again
	mninfo.cbSize=sizeof(mninfo);
	mninfo.fMask= MIIM_ID | MIIM_TYPE;
	mninfo.fType=MFT_SEPARATOR;
	mninfo.fState=MFS_ENABLED;
	mninfo.wID = MN_ADDWIN;
	mninfo.hSubMenu=hmenu;
	InsertMenuItem(hmenu,MN_ADDWIN+1,FALSE,&mninfo);
	return 1;
}

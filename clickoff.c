/*------------------------------------------------------------------------
Module:        d:\dokumente\c\clickoff1.7\clickoff.c
Author:        JOHANNES HÜBNER
Project:       ClickOff
State:         Debug
Creation Date: Mon May 08 15:45:33 2000
Description:   Main module of ClickOff.
------------------------------------------------------------------------*/

/*@@ Wedit generated application. Written Mon May 08 15:45:33 2000
Do not edit outside the indicated areas */
/*<---------------------------------------------------------------------->*/
/*<---------------------------------------------------------------------->*/

#include "clickoff.h"

HWND hActWindow;		//For click+return store active window at t-1
static CLKCFG config;	//Stores configuration variables

HWND hCfgDlg=0, hwndMain=0;

static BOOL CALLBACK InfoDlgFunc(HWND , UINT , WPARAM , LPARAM );
static BOOL CALLBACK DialogFunc(HWND , UINT , WPARAM , LPARAM );
int Startup ( LPSTR );
/*
Win main just registers a class of the same type that the dialog class, and
then calls DialogBox. Then it exits. The return value is the return value of
the dialog procedure.
*/

int APIENTRY WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASS wc;

	memset(&wc,0,sizeof(wc));
	wc.lpfnWndProc = DefDlgProc;
	wc.cbWndExtra = DLGWINDOWEXTRA;
	wc.hInstance = hinst;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon ( hinst, MAKEINTRESOURCE(ID_MAINICON) );
	wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	wc.lpszClassName = "clickoff";
	RegisterClass(&wc);

	return Startup (lpCmdLine);
}

static void DisplayHelp (HWND hwnd)
{
	ShellExecute(hwnd,"open",TranslateText(10006,"getting_started_ge.html"),NULL,NULL,SW_SHOWNORMAL);
}

void ToggleActive (HWND hParent)
{
	config.bActive=!config.bActive;
	SendDlgItemMessage ( hCfgDlg, ID_ACTIVE, BM_SETCHECK, config.bActive, 0);
	if (config.bActive) {
		ModifyTrayIcon (hParent, ID_TRAYICON, VERNAME);
		SetTimer ( hParent, 2, config.nScanTime, NULL);
	}
	else {
		ModifyTrayIcon (hParent, ID_ICODISABLED, (char*)TranslateText(ID_CLICKOFFDISABLED,MSG_CLICKOFFDISABLED));
		KillTimer ( hParent,2 );
	}
}

static void AddWindowToList ( int menuitem )
{
	char buffer[512];
	LPWININFO wi=WI_newItem();

	GetMenuString(hmenu,menuitem,buffer,sizeof(buffer),MF_BYCOMMAND);

	setWindowTitle(wi,buffer);

	wi->clkevent=CLK_ALWAYS;
	wi->clkmethod=MTD_CLOSE;
	push_back(clickoffList,wi);
	RefreshList(hCfgDlg);
	if ( SendDlgItemMessage ( hCfgDlg, ID_LIST, LB_SETCURSEL,
		SendDlgItemMessage ( hCfgDlg, ID_LIST, LB_GETCOUNT, 0, 0 )-1, 0 ) != LB_ERR ) {
		EnableWindow ( GetDlgItem ( hCfgDlg, ID_REMOVE ), 1 );
		EnableWindow ( GetDlgItem ( hCfgDlg, ID_ADVANCED ), 1 );
	}
}

static void WriteLogFile(HWND hwnd, LPWININFO lpItemwi)
{
	FILE *logFile = fopen(config.logFile, "a");
	SYSTEMTIME t;
	size_t wLen = GetWindowTextLength(hwnd) + 1;
	size_t bLen = GetWindowTextLength(GetDlgItem(hwnd, lpItemwi->bdlgitem)) + 1;
	size_t mLen = GetWindowTextLength(GetDlgItem(hwnd, lpItemwi->mdlgitem)) + 1;
	char wText[wLen];
	char bText[bLen];
	char mText[mLen];
	GetWindowText(hwnd, wText, wLen);
	GetDlgItemText(hwnd, lpItemwi->bdlgitem, bText, bLen);
	GetDlgItemText(hwnd, lpItemwi->mdlgitem, mText, mLen);

	GetSystemTime(&t);
	if (logFile)
	{
		fprintf(logFile, "[%d-%.2d-%.2d %.2d:%.2d:%.2d] processing \"%s\" button: \"%s\" message: \"%s\"\n",
				t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond,
				wText, bText, mText);
	    fclose(logFile);
	}
}

static void SimulateClick ( HWND hButton, int lr, POINT pos )
{
	RECT buttonRect;
	POINT mousepos;
	int xClick=0, yClick=0;
	if (hButton)
	{
		GetWindowRect ( hButton, &buttonRect );
		xClick = buttonRect.right-((buttonRect.right-buttonRect.left)/2);
		yClick = buttonRect.bottom-((buttonRect.bottom-buttonRect.top)/2);
	}
	else
	{
		xClick = pos.x;
		yClick = pos.y;
	}
	GetCursorPos ( &mousepos );
	SetCursorPos ( xClick, yClick );
	if (lr==MTD_LEFTCLICK){
		mouse_event (MOUSEEVENTF_LEFTDOWN, xClick, yClick, 0, 0);
		mouse_event (MOUSEEVENTF_LEFTUP, xClick, yClick, 0, 0);
	}
	else if (lr==MTD_RIGHTCLICK){
		mouse_event (MOUSEEVENTF_RIGHTDOWN, xClick, yClick, 0, 0);
		mouse_event (MOUSEEVENTF_RIGHTUP, xClick, yClick, 0, 0);
	}
	SetCursorPos ( mousepos.x, mousepos.y );
}

static void ClickOff ( HWND hParent, HWND hButton, LPWININFO lpItemwi )
{
	POINT bpos = {0, 0};
	RECT  windowRect;

	if (lpItemwi->wait<0) { //Nachlaufzeit
		resetCheckSum();
		if (lpItemwi->timecount<0) {
			lpItemwi->timecount+=config.nScanTime;
			if ( lpItemwi->timecount >= 0 ) lpItemwi->timecount=0;
			else return;
		}
		else lpItemwi->timecount=lpItemwi->wait;
	}
	else if (lpItemwi->wait>0) {  //Vorlaufzeit
		resetCheckSum();
		if (lpItemwi->timecount>0) {
			lpItemwi->timecount-=config.nScanTime;
			if ( lpItemwi->timecount <= 0 ) lpItemwi->timecount=0;
			else return;
		}
		else {
			lpItemwi->timecount=lpItemwi->wait;
			return;
		}
	}

	WriteLogFile(hParent, lpItemwi);

	switch ( lpItemwi->clkmethod ) {
	case MTD_LEFTCLICK:
		BringWindowToTop ( hParent );
		SimulateClick ( hButton, MTD_LEFTCLICK, bpos );
		lpItemwi->timesclicked++;
		break;
	case MTD_RIGHTCLICK:
		BringWindowToTop ( hParent );
		SimulateClick ( hButton, MTD_RIGHTCLICK, bpos );
		lpItemwi->timesclicked++;
		break;
	case MTD_CLICKPOS:
		BringWindowToTop ( hParent );
		GetWindowRect(hParent, &windowRect);
		bpos.x = lpItemwi->bposx + windowRect.left;
		bpos.y = lpItemwi->bposy + windowRect.top;
		SimulateClick ( NULL, MTD_LEFTCLICK, bpos );
		lpItemwi->timesclicked++;
		break;
	case MTD_CLOSE:
		PostMessage(hParent,WM_CLOSE,1,0);
		lpItemwi->timesclicked++;
		break;
	case MTD_MAXIMIZE:
		if (!IsZoomed(hParent)) {
			ShowWindow ( hParent, SW_MAXIMIZE );
			lpItemwi->timesclicked++;
		}
		break;
	case MTD_MINIMIZE:
		if (!IsIconic(hParent)) {
			ShowWindow ( hParent, SW_MINIMIZE );
			lpItemwi->timesclicked++;
		}
		break;
	case MTD_CLKMSG:
		if ( hButton ) {
			SendMessage(hButton,BM_CLICK, 0, 0);
			lpItemwi->timesclicked++;
		}
		break;
	case MTD_MSGACT:
		if ( hButton ) {
			SendMessage(hButton,BM_CLICK, 0, 0);
			Sleep(1000);
			SetForegroundWindow ( hActWindow );
			lpItemwi->timesclicked++;
		}
		break;
	case MTD_ENTERTEXT:
		if ( hButton ) {
			SendMessage ( hButton, WM_SETTEXT, 0, (LPARAM) lpItemwi->ButtonText );
			lpItemwi->timesclicked++;
		}
		break;
	}
}

HWND CheckRunning(void)
{
	HWND hwnd=NULL;
	FILE *hwndFile=fopen(PrependAppData("clickoff.hwnd",config.cfgloc),"r");

	if (hwndFile) {
		fscanf(hwndFile,"%d",&hwnd);
		fclose(hwndFile);
		if(GetWindowLong(hwnd,DWL_USER)!=UID) hwnd=NULL;
	}

	return hwnd;
}

int Startup ( LPSTR lpCmdline )
{
	int ret, isSetup=!strcmpi ( lpCmdline, "/setup" );
	char exePath[128];
	HWND hwndRunning;

	config.trayIcon = strcmpi (lpCmdline, "/notray");
	clearList(clickoffList);

	GetModuleFileName(NULL, exePath, sizeof(exePath));
	*(strrchr(exePath, '\\') + 1) = 0;

	if ( isSetup ) {
		SetupDlg ( (HINSTANCE)0x400000, &config );
		LoadLngFile(config.lngFile);
		DisplayHelp(NULL);
	}
	else {
		ReadConfig(&config);
		strcat(exePath, config.lngFile);
		LoadLngFile(exePath);
	}

	hwndRunning=CheckRunning();
	if ( !strcmpi (lpCmdline, "/exit") ) {
		SendMessage(hwndRunning,WM_CLOSE,0,0);
		return 0;
	}
	if(hwndRunning) {
    	SendMessage(hwndRunning,WM_USER+1,0,WM_LBUTTONDBLCLK);
		return 0;
	}

	ret=ReadFromFile(clickoffList,PrependAppData("clickoff.lst",config.cfgloc));
	if (!(ret==IOERR_NOERR || ret==IOERR_NOTEXIST && isSetup))
		MessageBox(HWND_DESKTOP,TranslateText(IOERR2ID(ret),IOERR2MSG(ret)),
			TranslateText(ID_IOERR,MSG_IOERR),MB_OK | MB_ICONWARNING);
	ret=DialogBox((HINSTANCE)0x400000, MAKEINTRESOURCE (DLG_EMPTY), NULL, (DLGPROC) DialogFunc);

	return ret;
}

static int InitializeApp(HWND hDlg,WPARAM wParam, LPARAM lParam)
{
	FILE *hwndFile=fopen(PrependAppData("clickoff.hwnd",config.cfgloc),"w");
	if(hwndFile) {
		fprintf(hwndFile,"%d",(int)hDlg);
		fclose(hwndFile);
	}

	RegisterHotKey ( hDlg, HOTKEY, HOTKEYFMOD(HIBYTE(config.wHotkey)), LOBYTE ( config.wHotkey ) );
	if (config.trayIcon) CreateTrayIcon ( hDlg, (HINSTANCE)0x400000, config.bActive );
	InitCfgDlg(&config);
	hwndMain = hDlg;
	SetTimer ( hDlg, 1, 50, NULL );
	SetWindowLong(hDlg,DWL_USER,UID);

	if (config.bActive)
		SetTimer ( hDlg, 2, config.nScanTime, NULL);

	AddToBCQueue(hDlg);
	hwndAdv=0;

	return 1;
}

static int InitializeInfoDlg ( HWND hwnd, WPARAM wparam, LPARAM lParam )
{
	SetWindowLong(hwnd,DWL_USER,DLG_INFO);
	TranslateDialog(hwnd);
	AddToBCQueue(hwnd);
	int sumclicks=0;
	for ( ITEM i=first(clickoffList); !isLast(i); i=next(i) )
		sumclicks+=((LPWININFO)getData(i))->timesclicked;
	SetDlgItemInt ( hwnd, ID_CLICKSUM, sumclicks, FALSE );
	return 1;
}

static void AddQuickItem (VOID)
{
	LPWININFO wi=WI_newItem();
	if (DetectControl(wi,0)) {
		push_back(clickoffList,wi);
		RefreshList(hCfgDlg);
		SaveToFile(clickoffList,PrependAppData("clickoff.lst",config.cfgloc));
	}
}

void ExitClickOff(void)
{
	unlink(PrependAppData("clickoff.hwnd",config.cfgloc));
	SaveToFile(clickoffList,PrependAppData("clickoff.lst",config.cfgloc));
	SaveConfig (&config);
	WinHelp ( hwndMain, "ClickOff.hlp", HELP_QUIT, 0 );
	KillTimer ( hwndMain, 2);
	MyTaskBarDeleteIcon( hwndMain, 1234);
	DestroyMenu ( hmenu );
	EndDialog(hwndMain,0);
}

static BOOL CALLBACK InfoDlgFunc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_INITDIALOG:
		InitializeInfoDlg(hwndDlg,wParam,lParam);
		return TRUE;
	case WM_DESTROY:
		RemoveFromBCQueue(hwndDlg);
		return 0;
	case CLM_LNGCHANGE:
		TranslateDialog(hwndDlg);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_SHOWDOC:
			DisplayHelp(hwndDlg);
			//ShellExecute ( hwndDlg, "open", "http://www.johanneshuebner.com", "", "", 1 );
			break;
		case ID_EMAIL:
			ShellExecute ( hwndDlg, "open", MAILVERSTRING, "", "", 1 );
			break;
		case IDCANCEL:
			EndDialog(hwndDlg,0);
			return TRUE;
		}
		return 0;
	case WM_CLOSE:
		EndDialog(hwndDlg,0);
		return 0;
	}
	return FALSE;
}

static void OnCheckTimer ( void )
{
	fwitem fi=CheckWindows(clickoffList);
	if (fi.item)
		ClickOff(fi.hwnd,fi.hbutton,getData(fi.item));
	hActWindow = GetForegroundWindow();
}

static void OnSettings(void)
{
	KillTimer(hwndMain,2);
	if (SettingsDlg(&config))
		SaveConfig(&config);

	SetTimer(hwndMain,2,config.nScanTime,NULL);
}

static BOOL CALLBACK DialogFunc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static UINT TaskbarRestart;

	switch (msg) {
	case WM_INITDIALOG:
		InitializeApp(hwndDlg, wParam, lParam);
		TaskbarRestart = RegisterWindowMessage(TEXT("TaskbarCreated"));
		return TRUE;
	case WM_HOTKEY:
		if ( wParam == HOTKEY )
			if (hwndAdv)
				SendMessage(hwndAdv,WM_HOTKEY,HOTKEY,0);
			else
				AddQuickItem ();
		break;
	case WM_ENDSESSION:
	case WM_CLOSE:
		ExitClickOff();
		return 0;
	case WM_COMMAND:
		switch ( LOWORD (wParam) ) {
		case MN_CONFIG:
			ShowCfgDlg ();
			return TRUE;
		case MN_SETTINGS:
			OnSettings();
			return TRUE;
		case MN_ACTIVE:
			ToggleActive ( hwndDlg );
			return TRUE;
		case MN_INFO:
			DialogBox((HINSTANCE)0x400000, MAKEINTRESOURCE(DLG_INFO), NULL, (DLGPROC) InfoDlgFunc);
			return TRUE;
		case MN_HELP:
			DisplayHelp ( hwndDlg );
			return TRUE;
		case MN_EXIT:
			ExitClickOff();
			return TRUE;
		}
		if (LOWORD(wParam)>MN_ADDWIN && LOWORD(wParam)<MN_ADDWIN+20) {
			AddWindowToList(LOWORD(wParam));
			SaveToFile(clickoffList,PrependAppData("clickoff.lst",config.cfgloc));
			return TRUE;
		}
		break;
	case WM_TIMER:
		switch (wParam) {
		case 1:
			if (ShowWindow(hwndDlg,SW_HIDE))
				KillTimer (hwndDlg, 1);
			HideCfgDlg ( );
			break;
		case 2:
			OnCheckTimer();
			break;
		}
		break;
	case CLM_LNGCHANGE:
		DestroyMenu(hmenu);
		LoadTrayMenu(hwndDlg);
		break;
	case WM_USER+1:
		switch (lParam)
		{
		case WM_LBUTTONDBLCLK:
			ShowCfgDlg ();
			return TRUE;
		case WM_RBUTTONUP:
			ShowMenu( hwndDlg, config.bActive );
			return TRUE;
		}
	default:
		if(msg == TaskbarRestart)
           if (config.trayIcon) CreateTrayIcon ( hwndDlg, (HINSTANCE)0x400000, config.bActive );
        break;
	}
	return FALSE;
}


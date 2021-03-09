#include "settingsdlg.h"

LPCLKCFG config;

void AddLngFiles (HWND hwndDlg)
{
	long findfirstResult;
	struct _finddata_t fd;
	int findnextResult=0;

	for ( findfirstResult = findfirst("*.lng",&fd); !findnextResult; findnextResult=findnext ( findfirstResult, &fd ) )
				SendDlgItemMessage ( hwndDlg, ID_LANG, CB_ADDSTRING, 0,(LPARAM) fd.name ); //add language file to ComboBox
}

static void InitializeSettingsDlg ( HWND hwnd )
{
	HWND hwndHot=NULL;

	SetWindowLong(hwnd,DWL_USER,DLG_SETTINGS);
	TranslateDialog(hwnd);

	InitCommonControls();
	hwndHot=CreateWindowEx( 0, HOTKEY_CLASS, "", WS_CHILD | WS_VISIBLE, 125, 53, 130, 19,
		hwnd, (HMENU)ID_HOTKEY, (HINSTANCE)0x400000, NULL );
	SendMessage(hwndHot, HKM_SETRULES, (WPARAM) HKCOMB_NONE | HKCOMB_S, MAKELPARAM(HOTKEYF_ALT, 0));
	SendMessage(hwndHot, HKM_SETHOTKEY, config->wHotkey, 0);
	SendMessage(hwndHot, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(TRUE, 0) );
	SetDlgItemInt ( hwnd, ID_SCANTIME, config->nScanTime, FALSE ); // set scantime EDIT

	SendDlgItemMessage ( hwnd, ID_LANG, CB_RESETCONTENT, 0, 0 );
	AddLngFiles ( hwnd );
	SendDlgItemMessage ( hwnd, ID_LANG, CB_SELECTSTRING,-1, (LPARAM) config->lngFile ); // select current language in ComboBox
	SetDlgItemText(hwnd, ID_LOGFILE, config->logFile);
}

static BOOL CALLBACK SetupDlgFunc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if ( msg == WM_INITDIALOG ) {
		AddLngFiles ( hwndDlg );
		ReadConfig(config);
		SendDlgItemMessage ( hwndDlg, ID_LANG, CB_SELECTSTRING,-1, (LPARAM) config->lngFile ); // select current language in ComboBox
		return TRUE;
	}
	else if ( msg == WM_COMMAND ) {
		if (LOWORD(wParam)==ID_OK) {
			if ( SendDlgItemMessage ( hwndDlg, ID_COMMON, BM_GETCHECK, 0, 0 ) )
				config->cfgloc=ALLUSERS;
			else
				config->cfgloc=CURRENTUSER;
			CreateDirectory(PrependAppData("",config->cfgloc),NULL);
			GetDlgItemText(hwndDlg,ID_LANG,config->lngFile,sizeof(config->lngFile));
			config->bActive=TRUE;
			config->nScanTime=100;
			config->wHotkey=1604;
			config->windowRect.left=config->windowRect.top=config->windowRect.right=config->windowRect.bottom=0;
			SaveConfig(config);
			EndDialog ( hwndDlg, 0 );
			return 0;
		}
	}
	return FALSE;
}

static void OnSettingsOk ( HWND hwndDlg )
{
	char lngFile[64];
	config->wHotkey = (WORD)SendDlgItemMessage ( hwndDlg, ID_HOTKEY, HKM_GETHOTKEY, 0, 0 );
	config->nScanTime = GetDlgItemInt ( hwndDlg, ID_SCANTIME, NULL, FALSE );
	if (config->nScanTime<1) config->nScanTime=1;
	GetDlgItemText(hwndDlg,ID_LANG,lngFile,64);
	SaveConfig (config);

	UnregisterHotKey ( hwndMain, HOTKEY );
	RegisterHotKey ( hwndMain, HOTKEY, HOTKEYFMOD(HIBYTE(config->wHotkey)), LOBYTE ( config->wHotkey ) );
	EndDialog ( hwndDlg, 1 );
	if (strcmp(config->lngFile,lngFile)) {
		LoadLngFile(lngFile);
		strcpy(config->lngFile,lngFile);
	}
	GetDlgItemText(hwndDlg, ID_LOGFILE, config->logFile, sizeof(config->logFile));
}

static void OnBrowseLogFile(HWND hParent)
{
	char filename[sizeof(config->logFile)];
	OPENFILENAME ofn;

	memset(&filename,0,sizeof(filename));
	memset(&ofn,0,sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hParent;
	ofn.lpstrFilter = "*.log\0*.*\0\0";
	ofn.lpstrFile = filename;
	ofn.nMaxFile = sizeof(filename);
	ofn.Flags = OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST;

	if (GetSaveFileName(&ofn) != 0)
		SetDlgItemText(hParent, ID_LOGFILE, filename);
}

static BOOL CALLBACK SettingsDlgFunc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_INITDIALOG:
		InitializeSettingsDlg(hwndDlg);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_CANCEL:
			EndDialog(hwndDlg, 0);
			return TRUE;
		case ID_OK:
			OnSettingsOk ( hwndDlg );
			return TRUE;
		case ID_HOTKEY:
			if (!SendDlgItemMessage ( hwndDlg, ID_HOTKEY, HKM_GETHOTKEY, 0, 0 ))
				MessageBox(hwndDlg,TranslateText(10013,
					"Wenn kein Hotkey definiert ist, können keine Fenster hinzugefügt werden!"),
					TranslateText(10005,"Warnung"),MB_OK | MB_ICONWARNING);
			return TRUE;
		case ID_BROWSELOGFILE:
			OnBrowseLogFile(hwndDlg);
			return TRUE;
		}
		return 0;
	case WM_CLOSE:
		EndDialog(hwndDlg, 0);
		return TRUE;
	}
	return FALSE;
}

int SettingsDlg (LPCLKCFG cfg)
{
	config=cfg;
	return DialogBox ( (HINSTANCE)0x400000, MAKEINTRESOURCE ( DLG_SETTINGS ), HWND_DESKTOP, (DLGPROC) SettingsDlgFunc );
}

void SetupDlg (HINSTANCE hinst, LPCLKCFG cfg)
{
	config=cfg;
	DialogBoxParam ( hinst, MAKEINTRESOURCE ( DLG_SETUP ), NULL, (DLGPROC) SetupDlgFunc, (LPARAM)hinst );
}

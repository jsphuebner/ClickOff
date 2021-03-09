#include "advanceddlg.h"

static int InitializeAdvancedDlg ( HWND hwnd, WPARAM wParam,LPARAM lParam )
{
	char buffer[64];
	LPWININFO wi=(LPWININFO)lParam;

	AddToBCQueue(hwnd);
	SetWindowLong(hwnd,DWL_USER,DLG_ADVANCED);
	TranslateDialog(hwnd);

	hwndAdv=hwnd;

	SetDlgItemText(hwnd,ID_WINDOWTITLE,wi->WindowTitle);
	SetDlgItemText(hwnd,ID_BUTTONTEXT,wi->ButtonText);
	if (wi->clkmethod==MTD_ENTERTEXT)
		SetDlgItemText(hwnd, ID_STATICBTEXT, TranslateText(10014, "Ausfüllen:"));
	//SetWindowLong ( GetDlgItem ( hwnd, ID_BUTTONTEXT ), GWL_USERDATA, wi->bdlgitem );
	for ( int methindex=0; methindex<NUMMTD; methindex++ ) {
		LoadString ( (HINSTANCE)0x400000, MSG_METHODSTRING+methindex, buffer, sizeof(buffer) );
		SendDlgItemMessage(hwnd,ID_CLICKOFFMETHOD,CB_ADDSTRING,0,(LPARAM) TranslateText(MSG_METHODSTRING+methindex,buffer ));
	}
	SendDlgItemMessage(hwnd,ID_CLICKOFFMETHOD,CB_SETCURSEL,wi->clkmethod,0);
	if (MTD_CLICKPOS == wi->clkmethod)
		EnableWindow(GetDlgItem(hwnd, ID_CLICKOFFMETHOD), 0);
	switch(wi->clkevent & 0xF) {
	case CLK_NEVER:
		SendDlgItemMessage ( hwnd, ID_NEVERCLICK, BM_SETCHECK, 1, 0);
		break;
	case CLK_ALWAYS:
		SendDlgItemMessage ( hwnd, ID_ALWAYSCLICK, BM_SETCHECK, 1, 0);
		break;
	case CLK_WHENTEXT:
		SendDlgItemMessage ( hwnd, ID_CLICKWHENTEXT, BM_SETCHECK, 1, 0);
		EnableWindow ( GetDlgItem (hwnd, ID_MSGTEXT), 1 );
		EnableWindow ( GetDlgItem (hwnd, ID_GETTEXT), 1 );
		break;
	}
	if (wi->clkevent & CLK_ONCE)
		SendDlgItemMessage ( hwnd, ID_CLICKONCE, BM_SETCHECK, 1, 0);
	SetDlgItemText ( hwnd, ID_MSGTEXT, wi->MsgText );
	SetWindowLong ( GetDlgItem ( hwnd, ID_MSGTEXT ), GWL_USERDATA, wi->mdlgitem );
	//ShowWindow ( GetDlgItem ( hwnd, 520 ), SW_HIDE );
	SetDlgItemInt ( hwnd, ID_CLICKNUMBER, wi->timesclicked, FALSE);
	SetWindowLong(hwnd,GWL_USERDATA,(LONG)wi);
	if (!wi->wait) {
		SendDlgItemMessage ( hwnd, ID_NODELAY, BM_SETCHECK, 1, 0);
		EnableWindow ( GetDlgItem (hwnd, ID_DELAY), 0 );
	}
	else if (wi->wait < 0) {
		SendDlgItemMessage ( hwnd, ID_POSTDELAY, BM_SETCHECK, 1, 0);
		SetDlgItemInt(hwnd,ID_DELAY,-wi->wait/1000,FALSE);
	}
	else {
		SendDlgItemMessage ( hwnd, ID_PREDELAY, BM_SETCHECK, 1, 0);
		SetDlgItemInt(hwnd,ID_DELAY,wi->wait/1000,FALSE);
	}
	SendDlgItemMessage(hwnd,ID_DELAY,EM_LIMITTEXT,6,0);
	return 1;
}

static void SaveAdvSettings (HWND hwnd)
{
	LPWININFO wi=(LPWININFO)GetWindowLong(hwnd,GWL_USERDATA);

	wi->WindowTitle=realloc(wi->WindowTitle,GetWindowTextLength(GetDlgItem(hwnd,ID_WINDOWTITLE))+1);
	wi->ButtonText=realloc(wi->ButtonText,GetWindowTextLength(GetDlgItem(hwnd,ID_BUTTONTEXT))+1);
	wi->MsgText=realloc(wi->MsgText,GetWindowTextLength(GetDlgItem(hwnd,ID_MSGTEXT))+1);

	GetDlgItemText(hwnd,ID_WINDOWTITLE,wi->WindowTitle,4096);
	GetDlgItemText(hwnd,ID_BUTTONTEXT,wi->ButtonText,4096);
	GetDlgItemText(hwnd,ID_MSGTEXT,wi->MsgText,4096);
	wi->mdlgitem = GetWindowLong ( GetDlgItem ( hwnd, ID_MSGTEXT ), GWL_USERDATA );
	wi->clkmethod=SendDlgItemMessage(hwnd,ID_CLICKOFFMETHOD,CB_GETCURSEL,0,0);
	if (SendDlgItemMessage ( hwnd, ID_NEVERCLICK, BM_GETCHECK,0,0))
		wi->clkevent=CLK_NEVER;
	else if (SendDlgItemMessage ( hwnd, ID_ALWAYSCLICK, BM_GETCHECK,0,0))
		wi->clkevent=CLK_ALWAYS;
	else if (SendDlgItemMessage ( hwnd, ID_CLICKWHENTEXT, BM_GETCHECK,0,0))
		wi->clkevent=CLK_WHENTEXT;
	if (SendDlgItemMessage ( hwnd, ID_CLICKONCE, BM_GETCHECK,0,0))
		wi->clkevent|=CLK_ONCE;
	if (SendDlgItemMessage ( hwnd, ID_NODELAY, BM_GETCHECK,0,0))
		wi->wait=0;
	else if (SendDlgItemMessage ( hwnd, ID_PREDELAY, BM_GETCHECK,0,0))
		wi->wait=GetDlgItemInt(hwnd,ID_DELAY,NULL,FALSE)*1000;
	else if (SendDlgItemMessage ( hwnd, ID_POSTDELAY, BM_GETCHECK,0,0))
		wi->wait=GetDlgItemInt(hwnd,ID_DELAY,NULL,FALSE)*(-1000);
}

static void DetectText ( HWND hwnd )
{
	LPWININFO wi=WI_newItem();

	//EnableWindow ( GetDlgItem ( hwnd, ID_GETTEXT ), 0 );
	if(DetectControl(wi,1)) {
		SetDlgItemText(hwnd,ID_MSGTEXT,wi->MsgText);
		SetWindowLong(GetDlgItem(hwnd,ID_MSGTEXT), GWL_USERDATA, wi->mdlgitem);
	}
	wiDestructor(wi);
	//EnableWindow ( GetDlgItem ( hwnd, ID_GETTEXT ), 1 );
}

static BOOL CALLBACK AdvancedDlgFunc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_INITDIALOG:
		InitializeAdvancedDlg(hwnd,wParam,lParam);
		return TRUE;
	case WM_HOTKEY:
		if ( wParam == HOTKEY )
			DetectText(hwnd);
		break;
	case WM_DESTROY:
		hwndAdv=0;
		RemoveFromBCQueue(hwnd);
		return 0;
	case CLM_LNGCHANGE:
		TranslateDialog(hwnd);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDCANCEL:
		case ID_CANCEL:
			EndDialog(hwnd, 0);
			break;
		case IDOK:
		case ID_OK:
			SaveAdvSettings(hwnd);
			EndDialog (hwnd, 1);
			break;
		case ID_NEVERCLICK:
		case ID_ALWAYSCLICK:
			EnableWindow ( GetDlgItem (hwnd, ID_MSGTEXT), 0 );
			EnableWindow ( GetDlgItem (hwnd, ID_GETTEXT), 0 );
			break;
		case ID_CLICKWHENTEXT:
			EnableWindow ( GetDlgItem (hwnd, ID_MSGTEXT), 1 );
			EnableWindow ( GetDlgItem (hwnd, ID_GETTEXT), 1 );
			SetFocus ( GetDlgItem (hwnd, ID_MSGTEXT) );
			break;
		case ID_NODELAY:
		case ID_CLICKONCE:
			EnableWindow ( GetDlgItem (hwnd, ID_DELAY), 0 );
			break;
		case ID_PREDELAY:
		case ID_POSTDELAY:
			EnableWindow ( GetDlgItem (hwnd, ID_DELAY), 1 );
		}
		break;
		/* By default, WM_CLOSE is equivalent to CANCEL */
	case WM_CLOSE:
		EndDialog(hwnd,0);
		return TRUE;
	}
	return FALSE;
}

int AdvPropsDlg(LPWININFO wi, HWND hParent)
{
	return DialogBoxParam ((HINSTANCE)0x400000, MAKEINTRESOURCE(DLG_ADVANCED),hParent,
   			 (DLGPROC) AdvancedDlgFunc, (LPARAM)wi );
}

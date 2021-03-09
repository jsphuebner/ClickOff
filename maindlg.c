#include "maindlg.h"

#define LEQ(s1,s2) (stricmp(s1,s2)<=0)

static LRESULT CALLBACK ListBoxProc(HWND , UINT , WPARAM , LPARAM );
static BOOL CALLBACK CfgDlgFunc(HWND, UINT, WPARAM, LPARAM);

LPCLKCFG config;

//Vergleichsfunktionen zum sortieren
static int _cmpwin ( LPWININFO w1, LPWININFO w2 )
{
	return LEQ ( w1->WindowTitle, w2->WindowTitle );
}

static int _ncmpwin ( LPWININFO w1, LPWININFO w2 )
{
	return LEQ ( w2->WindowTitle, w1->WindowTitle );
}

static int _cmpbut ( LPWININFO w1, LPWININFO w2 )
{
	return LEQ ( w1->ButtonText, w2->ButtonText );
}

static int _ncmpbut ( LPWININFO w1, LPWININFO w2 )
{
	return LEQ ( w2->ButtonText, w1->ButtonText );
}

static int _cmpmsg ( LPWININFO w1, LPWININFO w2 )
{
	return LEQ ( w1->MsgText, w2->MsgText );
}

static int _ncmpmsg ( LPWININFO w1, LPWININFO w2 )
{
	return LEQ ( w2->MsgText, w1->MsgText );
}

static int _cmpcl ( LPWININFO w1, LPWININFO w2 )
{
	return w1->timesclicked<=w2->timesclicked;
}

static void InitializeCfgDlg ( HWND hwnd )
{
	WINDOWPLACEMENT wndpl;
	WNDPROC lpPrevWndFunc=(WNDPROC)GetWindowLong ( GetDlgItem ( hwnd, ID_LIST ), GWL_WNDPROC );

	SendDlgItemMessage ( hwnd, ID_ACTIVE, BM_SETCHECK, config->bActive, 0); // set Active CheckBox

	SetWindowLong ( GetDlgItem ( hwnd, ID_LIST ), GWL_WNDPROC, (LONG)ListBoxProc );
	SetWindowLong ( GetDlgItem ( hwnd, ID_LIST ), GWL_USERDATA, (LONG) lpPrevWndFunc );
	//Restore window size from configuration file
	wndpl.length=sizeof(WINDOWPLACEMENT);
	wndpl.flags=0;
	wndpl.showCmd=config->showCmd;
	wndpl.rcNormalPosition.left=config->windowRect.left;
	wndpl.rcNormalPosition.top=config->windowRect.top;
	wndpl.rcNormalPosition.right=config->windowRect.right;
	wndpl.rcNormalPosition.bottom=config->windowRect.bottom;
	SetWindowPlacement ( hwnd, &wndpl );
	SetWindowLong(hwnd,DWL_USER,IDD_MAINDIALOG);
	TranslateDialog(hwnd);

	RefreshList ( hwnd );
}

void InitCfgDlg ( LPCLKCFG cfg )
{
	config=cfg;
}

void ShowCfgDlg ( void )
{
	KillTimer (hwndMain, 2);

	if (!IsWindow ( hCfgDlg ))
		hCfgDlg=CreateDialog ( (HINSTANCE)0x400000, MAKEINTRESOURCE ( IDD_MAINDIALOG ), HWND_DESKTOP, (DLGPROC) CfgDlgFunc );
	SetForegroundWindow (hCfgDlg);
	AddToBCQueue(hCfgDlg);
}

void HideCfgDlg (void)
{
	RemoveFromBCQueue(hCfgDlg);
	DestroyWindow ( hCfgDlg );
	if (config->bActive)
		SetTimer (hwndMain, 2, config->nScanTime, NULL);
}

void RefreshList (HWND hwnd)
{
	if ( IsWindow ( hwnd )) {
		SendDlgItemMessage ( hwnd, ID_LIST, LB_RESETCONTENT, 0, 0 );

		for ( ITEM i=first(clickoffList); !isLast(i); i=next(i) )
			SendDlgItemMessage ( hwnd, ID_LIST, LB_ADDSTRING, 0, (LPARAM)((LPWININFO)getData(i))->WindowTitle );
		EnableWindow ( GetDlgItem ( hwnd, ID_REMOVE ), 0 );
		EnableWindow ( GetDlgItem ( hwnd, ID_ADVANCED ), 0 );
	}
	resetCheckSum();
}

static void RemoveString(HWND hwndParent)
{
	int n=SendDlgItemMessage(hwndParent,ID_LIST, LB_GETCOUNT, 0, 0);

	for ( int i=0,j=0; i<n; ++i ) {
		if ( SendDlgItemMessage(hwndParent,ID_LIST,LB_GETSEL,i,0) )
			wiDestructor( delete(clickoffList,clickoffList[j]) );
		else
		    j++;
	}
	EnableWindow ( GetDlgItem ( hwndParent, ID_REMOVE ), 0 );
	EnableWindow ( GetDlgItem ( hwndParent, ID_ADVANCED ), 0 );
	RefreshList(hwndParent);
}

static int exactMatch (LPWININFO w1, LPWININFO w2)
{
	return !(strcmp ( w1->WindowTitle, w2->WindowTitle ) ||
		     strcmp ( w1->ButtonText, w2->ButtonText ) ||
		     strcmp ( w1->MsgText, w2->MsgText ) );
}

static void RemoveDuplicates (void)
{
	deleteDuplicates(clickoffList, (CMPPROC)exactMatch, (DESTRUCTOR)wiDestructor);
	RefreshList(hCfgDlg);
}

static void InvertSelection(HWND hList)
{
	int n=SendMessage(hList,LB_GETCOUNT,0,0);
	for ( int i=0; i<n; i++ )
		SendMessage(hList,LB_SETSEL,!SendMessage(hList,LB_GETSEL,i,0),i);
}

static void ExportList(HWND hList)
{
	char filename[2048];
	OPENFILENAME ofn;
	list tmp;

	memset(&filename,0,sizeof(filename));
	memset(&ofn,0,sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hCfgDlg;
	ofn.lpstrFilter = TranslateText(10012, "Listen\0*.lst\0Alle Dateien\0*.*\0\0");
	ofn.lpstrFile = filename;
	ofn.nMaxFile = sizeof(filename);
	ofn.Flags = OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

	GetSaveFileName(&ofn);

	if (!hList)
		SaveToFile(clickoffList,ofn.lpstrFile);
	else {
		clearList(tmp);
		ITEM itm=first(clickoffList);
		int n=SendMessage(hList,LB_GETCOUNT,0,0);
		for ( int i=0; i<n; i++ ) {
			if ( SendMessage(hList,LB_GETSEL,i,0) )
				push_back(tmp,getData(itm));
			itm = next(itm);
		}
		SaveToFile(tmp,ofn.lpstrFile);
		freeList(tmp,NULL);
	}
}

static void ImportList (void)
{
	char filename[2048];
	OPENFILENAME ofn;

	memset(&filename,0,sizeof(filename));
	memset(&ofn,0,sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwndMain;
	ofn.lpstrFilter = TranslateText(10012, "Listen\0*.lst\0Alle Dateien\0*.*\0\0");
	ofn.lpstrFile = filename;
	ofn.nMaxFile = sizeof(filename);
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST;

	GetOpenFileName(&ofn);
	ReadFromFile ( clickoffList, ofn.lpstrFile );
	RefreshList(hCfgDlg);
}

static int ShowListMenu(void)
{
	POINT Point;
	HMENU listMenu;
	GetCursorPos(&Point);
	MENUITEMINFO mninfo;
	memset ( &mninfo, 0, sizeof(MENUITEMINFO));

	listMenu = GetSubMenu(LoadMenu((HINSTANCE)0x400000,MAKEINTRESOURCE(MN_POPUP)),1);

	mninfo.cbSize=sizeof(MENUITEMINFO);
	mninfo.fMask=MIIM_ID;
	mninfo.wID=MN_SUBSORT;
	SetMenuItemInfo ( listMenu, 0, TRUE, &mninfo );
	mninfo.wID=MN_SUBEXPORT;
	SetMenuItemInfo ( listMenu, 1, TRUE, &mninfo );

	TranslateMenu(listMenu);

	TrackPopupMenu(listMenu,TPM_LEFTALIGN | TPM_LEFTBUTTON,Point.x,Point.y,0,hCfgDlg,NULL);
	return 1;
}

BOOL ResizeControls ( HWND hwnd, LPRECT windowRect )
{
	SetWindowPos ( GetDlgItem(hwnd, ID_LIST ),0,0,0,windowRect->right-windowRect->left-20,
		windowRect->bottom-windowRect->top-241, SWP_NOMOVE | SWP_NOZORDER );
	SetWindowPos ( GetDlgItem(hwnd, ID_REMOVE ),0,9,
		windowRect->bottom-windowRect->top-124,0,0, SWP_NOSIZE | SWP_NOZORDER );
	SetWindowPos ( GetDlgItem(hwnd, ID_ADVANCED ),0,windowRect->right-windowRect->left-108,
		windowRect->bottom-windowRect->top-124,0,0, SWP_NOSIZE | SWP_NOZORDER );
	SetWindowPos ( GetDlgItem(hwnd, 104 ),0,9,
		windowRect->bottom-windowRect->top-95,0,0, SWP_NOSIZE | SWP_NOZORDER );
	SetWindowPos ( GetDlgItem(hwnd, ID_CLOSE ),0,9,
		windowRect->bottom-windowRect->top-40,0,0, SWP_NOSIZE | SWP_NOZORDER );
	SetWindowPos ( GetDlgItem(hwnd, ID_EXIT ),0,windowRect->right-windowRect->left-108,
		windowRect->bottom-windowRect->top-40,0,0, SWP_NOSIZE | SWP_NOZORDER );
	return 0;
}

static void AdaptButtonState(void)
{
	if ( SendDlgItemMessage (hCfgDlg, ID_LIST, LB_GETSELCOUNT, 0, 0) > 0) {
		EnableWindow ( GetDlgItem ( hCfgDlg, ID_REMOVE ), 1 );
		EnableWindow ( GetDlgItem ( hCfgDlg, ID_ADVANCED ), 1 );
	}
	else {
		EnableWindow ( GetDlgItem ( hCfgDlg, ID_REMOVE ), 0 );
		EnableWindow ( GetDlgItem ( hCfgDlg, ID_ADVANCED ), 0 );
	}
}

static void ShowAdvancedProps(void)
{
	int nCursel=SendDlgItemMessage(hCfgDlg,ID_LIST,LB_GETCURSEL,0,0);
	LPWININFO wi = getData(clickoffList[nCursel]);
	if (wi)
		if(AdvPropsDlg(wi,hCfgDlg))
			RefreshList (hCfgDlg);
}

static BOOL CALLBACK CfgDlgFunc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LPMINMAXINFO lpmmi;
	RECT size;
	WINDOWPLACEMENT wndpl;

	switch (msg) {
	case WM_INITDIALOG:
		InitializeCfgDlg(hwndDlg);
		return 1;
	case WM_GETMINMAXINFO:
		lpmmi=(LPMINMAXINFO)lParam;
		lpmmi->ptMinTrackSize.x=272;
		lpmmi->ptMinTrackSize.y=344;
		break;
	case WM_SIZE:
		if ( wParam==SIZE_MAXIMIZED || wParam==SIZE_RESTORED ) {
			size.left=0;
			size.top=0;
			size.right=LOWORD(lParam);
			size.bottom=HIWORD(lParam);
			ResizeControls ( hwndDlg, &size );
			return 0;
		}
		break;
	case WM_CLOSE:
		HideCfgDlg ();
		break;
	case WM_DESTROY:
		RemoveFromBCQueue(hwndDlg);
		wndpl.length=sizeof(WINDOWPLACEMENT);
		GetWindowPlacement ( hCfgDlg, &wndpl );
		config->windowRect=wndpl.rcNormalPosition;
		config->showCmd=wndpl.showCmd;
		SaveConfig(config);
		return 0;
	case CLM_LNGCHANGE:
		TranslateDialog(hwndDlg);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDCANCEL:
			HideCfgDlg();
			break;
		case IDOK:
		case ID_CLOSE:
			HideCfgDlg();
			SaveToFile(clickoffList,PrependAppData("clickoff.lst",config->cfgloc));
			break;
		case ID_EXIT:
			ExitClickOff();
			break;
		/*case ID_ADD:
			OnAdd ( hwndDlg );
			break;*/
		case ID_REMOVE:
			RemoveString ( hwndDlg );
			break;
		case ID_LIST:
			AdaptButtonState();
			break;
		case ID_ADVANCED:
			ShowAdvancedProps();
			break;
		case ID_ACTIVE:
			ToggleActive(hwndMain);
			break;
		case MN_DEL:
			RemoveString ( hwndDlg );
			break;
		case MN_SORT_BYWIN_DES:
			sort(clickoffList, (CMPPROC)_cmpwin);
			RefreshList(hwndDlg);
			break;
		case MN_SORT_BYWIN_ASC:
			sort(clickoffList, (CMPPROC)_ncmpwin);
			RefreshList(hwndDlg);
			break;
		case MN_SORT_BYBUT_DES:
			sort(clickoffList, (CMPPROC)_cmpbut);
			RefreshList(hwndDlg);
			break;
		case MN_SORT_BYBUT_ASC:
			sort(clickoffList, (CMPPROC)_ncmpbut);
			RefreshList(hwndDlg);
			break;
		case MN_SORT_BYMSG_DES:
			sort(clickoffList, (CMPPROC)_cmpmsg);
			RefreshList(hwndDlg);
			break;
		case MN_SORT_BYMSG_ASC:
			sort(clickoffList, (CMPPROC)_ncmpmsg);
			RefreshList(hwndDlg);
			break;
		case MN_SORT_BYCL_ASC:
			sort(clickoffList, (CMPPROC)_cmpcl);
			RefreshList(hwndDlg);
			break;
		case MN_SEL_ALL:
			SendDlgItemMessage(hwndDlg,ID_LIST,LB_SETSEL,1,-1);
			AdaptButtonState();
			break;
		case MN_INV_SEL:
			InvertSelection(GetDlgItem(hwndDlg,ID_LIST));
			AdaptButtonState();
			break;
		case MN_EXPORT_ALL:
			ExportList(NULL);
			break;
		case MN_EXPORT_SEL:
			ExportList(GetDlgItem(hwndDlg,ID_LIST));
			break;
		case MN_IMPORT:
			ImportList();
			break;
		case MN_REMDUPL:
			RemoveDuplicates();
			break;
		}
		return FALSE;
	}
	return FALSE;
}

static LRESULT CALLBACK ListBoxProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch ( msg ) {
	case WM_LBUTTONDBLCLK:
		if ( SendMessage(hwnd,LB_GETSELCOUNT,0,0) > 0 )
			ShowAdvancedProps();
		return TRUE;
	case WM_RBUTTONUP:
		CallWindowProc ( (WNDPROC)GetWindowLong(hwnd,GWL_USERDATA), hwnd, msg, wParam, lParam );
		ShowListMenu ();
	default:
		return CallWindowProc ( (WNDPROC)GetWindowLong(hwnd,GWL_USERDATA), hwnd, msg, wParam, lParam );
	}
}



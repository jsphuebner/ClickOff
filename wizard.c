#include "wizard.h"

LIST newList;
int *wizFlags;
//static HINSTANCE 0x400000;

void resetList ( void ){
	freeList(*newList,wiDestructor);
	LPWININFO wi = WI_newItem();
	push_back(*newList,wi);
}

static int getNextPage (UINT current){
	switch (current) {
	case DLG_WIZADDBUTTON:
		if (*wizFlags & WF_BUTTON2) return DLG_WIZCLKMTD;
		else if (*wizFlags & WF_SUMMARY) return DLG_WIZBUTTONFINISH;
		else return -1;
	case DLG_WIZCLKMTD:
		if (*wizFlags & WF_SUMMARY) return DLG_WIZBUTTONFINISH;
		else return -1;
	case DLG_WIZADDWINDOW:
		if (*wizFlags & WF_WINDOW2) return DLG_WIZWINCLKMTD;
		else if (!(*wizFlags & WF_WINDOW2) && *wizFlags & WF_SUMMARY) return DLG_WIZWINFINISH;
		else return -1;
	case DLG_WIZWINCLKMTD:
		if (*wizFlags & WF_SUMMARY) return DLG_WIZWINFINISH;
		else return -1;
	case DLG_WIZSELLIST:
		if (*wizFlags & WF_SUMMARY) return DLG_WIZLISTFINISH;
		else return -1;
	}
	return -1;
}

static int getPrevPage (UINT current){
	switch (current) {
	case DLG_WIZADDBUTTON:
	case DLG_WIZADDWINDOW:
	case DLG_WIZSELLIST:
		if (*wizFlags & WF_INTRO) return DLG_BASEWIZ;
		else return -1;
	case DLG_WIZCLKMTD:
		return DLG_WIZADDBUTTON;
	case DLG_WIZWINCLKMTD:
		return DLG_WIZADDWINDOW;
	case DLG_WIZBUTTONFINISH:
		if (*wizFlags & WF_BUTTON2) return DLG_WIZCLKMTD;
		else return DLG_WIZADDBUTTON;
	case DLG_WIZWINFINISH:
		if (*wizFlags & WF_WINDOW2) return DLG_WIZWINCLKMTD;
		else return DLG_WIZADDWINDOW;
	case DLG_WIZLISTFINISH:
		return DLG_WIZSELLIST;
	}
	return -1;
}

static void setButtons(HWND hwnd, UINT page){
	UINT bflags=0;
	if (getNextPage(page)==-1) bflags=PSWIZB_FINISH;
	else bflags=PSWIZB_NEXT;
	if (getPrevPage(page)!=-1) bflags|=PSWIZB_BACK;

	PropSheet_SetWizButtons(hwnd, bflags);
}

static void AddWindowToWizard ( HWND hwnd, HWND hWizList ){
	if ( IsWindowVisible ( hwnd ) && hwnd != GetParent(GetParent (hWizList))) {
		LPSTR lpWindowTitle=GC_malloc ( GetWindowTextLength (hwnd)+1 );

		GetWindowText ( hwnd, lpWindowTitle, 4096 );
		if ( strcmp ( lpWindowTitle,"" ) != 0 && strcmp ( lpWindowTitle,"Program Manager" ) != 0 )			SendMessage ( hWizList, LB_ADDSTRING, 0, (LPARAM) lpWindowTitle );
	}
}

static BOOL CALLBACK WizEnumWindowsProc(HWND hWindow, LPARAM  hWizList){
	AddWindowToWizard ( hWindow, (HWND) hWizList );
	return TRUE;
}

static int InitializeAddButtonDlg(HWND hDlg,WPARAM wParam, LPARAM lParam){
	LPPROPSHEETPAGE ppsp = (LPPROPSHEETPAGE) lParam;

	SetWindowLong(hDlg,GWL_ID,(LONG)ppsp->pszTemplate);

	SendDlgItemMessage ( hDlg, ID_WIZMTDMSG, BM_SETCHECK, BST_CHECKED, 0 );
	SendDlgItemMessage ( hDlg, ID_WIZCLOSEWND, BM_SETCHECK, BST_CHECKED, 0 );
	SendDlgItemMessage ( hDlg, ID_WIZADDBUTTON, BM_SETCHECK, BST_CHECKED, 0 );

	TranslateDialog(hDlg);
	return 1;
}

static BOOL OnIdWizUseText ( HWND hwnd ){
	BOOL usetext=SendDlgItemMessage ( hwnd, ID_WIZUSETEXT, BM_GETCHECK, 0, 0 );
	EnableWindow ( GetDlgItem ( hwnd, ID_WIZMSGTEXT ), usetext );
	EnableWindow ( GetDlgItem ( hwnd, ID_WIZTEXTSTATIC ), usetext );
	EnableWindow ( GetDlgItem ( hwnd, ID_WIZDETECTTEXT ), usetext );

	return usetext ;
}

static void OnIdWizWinList ( HWND hwnd ){
	DWORD dwCurSel=SendDlgItemMessage ( hwnd, ID_WIZWINLIST, LB_GETCURSEL, 0, 0 );
	LPSTR lpBuffer;
	if ( dwCurSel != -1 ) {
		lpBuffer=GC_malloc ( LB_GetTextLen (hwnd, ID_WIZWINLIST, dwCurSel)+1 );
		LB_GetText ( hwnd, ID_WIZWINLIST, dwCurSel, lpBuffer );
		SetDlgItemText ( hwnd, ID_WIZWINTITLE, lpBuffer );
	}
}

static void OnWizBrowse ( HWND hwnd ){
	char filename[2048];
	OPENFILENAME ofn;

	memset(&filename,0,sizeof(filename));
	memset(&ofn,0,sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = "List Files\0*.lst\0All Files\0*.*\0\0";
	ofn.lpstrFile = filename;
	ofn.nMaxFile = sizeof(filename);
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST;

	GetOpenFileName(&ofn);

	if ( filename[0] )	SetDlgItemText(hwnd,ID_WIZLISTPATH,ofn.lpstrFile);
}

static void OnIdWizDetectButton ( HWND hPage, HWND hWiz ){
	LPWININFO lpTemp = getData(first(*newList));
	ShowWindow ( hWiz, SW_HIDE );
	if (DetectWindow((HINSTANCE)0x400000,hWiz,lpTemp)) {
		SetDlgItemText ( hPage, ID_WIZWINTITLE, lpTemp->WindowTitle );
		SetDlgItemText ( hPage, ID_WIZBUTTONTEXT, lpTemp->ButtonText );
		SetDlgItemText ( hPage, ID_WIZMSGTEXT, lpTemp->MsgText );
	}
	ShowWindow ( hWiz, SW_SHOW );
}

/*static void OnIdWizDetectText ( HWND hWiz )
{
ShowWindow ( hWiz, SW_HIDE );
DialogBoxParam ( 0x400000, MAKEINTRESOURCE ( DLG_DETECT ), hWiz, DetectWindowFunc, TM_DETECTTEXT );
ShowWindow ( hWiz, SW_SHOW );
}*/

static void OnSetActive ( HWND hwnd,NMHDR *lpnmhdr ){
	LPWININFO lpTemp=getData(first(*newList));
	char buffer[2048],clkmtd[32];

	switch ( lpnmhdr->idFrom ) {
	case DLG_WIZBUTTONFINISH:
		LoadString ( (HINSTANCE)0x400000, MSG_METHODSTRING+lpTemp->clkmethod, clkmtd, sizeof(clkmtd) );
		wsprintf ( buffer, TranslateText(ID_BUTTONSUM,MSG_BUTTONSUM), lpTemp->WindowTitle,
			lpTemp->ButtonText,
			(lpTemp->clkevent & 0xF)==CLK_WHENTEXT?lpTemp->MsgText:"-",
			TranslateText(MSG_METHODSTRING+lpTemp->clkmethod,clkmtd) );
		SetDlgItemText ( hwnd, ID_WIZSUMMARY, buffer );
		break;
	case DLG_WIZADDBUTTON:
		resetList();
		lpTemp=getData(first(*newList));
		lpTemp->clkmethod=MTD_CLKMSG;
		break;
	case DLG_WIZADDWINDOW:
		resetList();
		lpTemp=getData(first(*newList));
		lpTemp->clkmethod=MTD_CLOSE;
		//PropSheet_SetWizButtons(lpnmhdr->hwndFrom, PSWIZB_BACK | PSWIZB_NEXT);
		SendDlgItemMessage ( hwnd, ID_WIZWINLIST, LB_RESETCONTENT, 0, 0 );
		EnumWindows ( WizEnumWindowsProc, (LPARAM)GetDlgItem(hwnd, ID_WIZWINLIST) );
		break;
	case DLG_WIZSELLIST:
		resetList();
		//PropSheet_SetWizButtons(lpnmhdr->hwndFrom, PSWIZB_BACK | PSWIZB_NEXT);
		break;
	case DLG_WIZWINFINISH:
		LoadString ( (HINSTANCE)0x400000, MSG_METHODSTRING+lpTemp->clkmethod, clkmtd, sizeof(clkmtd) );
		wsprintf ( buffer, TranslateText(ID_WINDOWSUM,MSG_WINDOWSUM),
			lpTemp->WindowTitle, TranslateText(MSG_METHODSTRING+lpTemp->clkmethod,clkmtd) );
		SetDlgItemText ( hwnd, ID_WIZSUMMARY, buffer );
		//PropSheet_SetWizButtons(lpnmhdr->hwndFrom, PSWIZB_BACK | PSWIZB_FINISH);
		break;
	case DLG_WIZLISTFINISH:
		//PropSheet_SetWizButtons(lpnmhdr->hwndFrom, PSWIZB_BACK | PSWIZB_FINISH);
		SendDlgItemMessage ( hwnd, ID_WIZIMPLIST, LB_RESETCONTENT, 0, 0 );
		for ( ITEM i=first(*newList); !isLast(i); i=next(i) )
			SendDlgItemMessage(hwnd,ID_WIZIMPLIST,LB_ADDSTRING,0,(LPARAM)((LPWININFO)getData(i))->WindowTitle);
		break;
	case DLG_BASEWIZ:
		//PropSheet_SetWizButtons(lpnmhdr->hwndFrom, PSWIZB_NEXT);
		break;
	default:
		//PropSheet_SetWizButtons(lpnmhdr->hwndFrom, PSWIZB_BACK | PSWIZB_NEXT);
		break;
	}
	setButtons(lpnmhdr->hwndFrom,lpnmhdr->idFrom);
}

static DWORD buttonToStruct(HWND hwnd, LPWININFO lpTemp)
{
	DWORD err=0;

	lpTemp->WindowTitle=realloc(lpTemp->WindowTitle,
		GetWindowTextLength(GetDlgItem(hwnd,ID_WIZWINTITLE))+1);
	lpTemp->ButtonText=realloc(lpTemp->ButtonText,
		GetWindowTextLength(GetDlgItem(hwnd,ID_WIZBUTTONTEXT))+1);
	lpTemp->MsgText=realloc(lpTemp->MsgText,
		GetWindowTextLength(GetDlgItem(hwnd,ID_WIZMSGTEXT))+1);
	GetDlgItemText ( hwnd, ID_WIZWINTITLE, lpTemp->WindowTitle, 4096 );
	GetDlgItemText ( hwnd, ID_WIZBUTTONTEXT, lpTemp->ButtonText, 4096 );
	GetDlgItemText ( hwnd, ID_WIZMSGTEXT, lpTemp->MsgText, 4096 );
	if ( CB_isChecked ( hwnd, ID_WIZUSETEXT ) ) {
		lpTemp->clkevent=CLK_WHENTEXT;
		if ( !strcmp ( lpTemp->MsgText, "" ) )			err=MSG_UENOTEXT;
	}
	else
	    lpTemp->clkevent=CLK_ALWAYS;
	if ( !strcmp ( lpTemp->WindowTitle, "" ) || !strcmp ( lpTemp->ButtonText, "" ) )		err=MSG_UENOTITLE;

	return err;
}

static DWORD windowToStruct(HWND hwnd, LPWININFO lpTemp)
{
	lpTemp->WindowTitle=realloc(lpTemp->WindowTitle,
		GetWindowTextLength(GetDlgItem(hwnd,ID_WIZWINTITLE))+1);
	GetDlgItemText ( hwnd, ID_WIZWINTITLE, lpTemp->WindowTitle, 4096 );
	lpTemp->ButtonText=realloc(lpTemp->ButtonText,1);
	lpTemp->MsgText=realloc(lpTemp->MsgText,1);
	*lpTemp->ButtonText=0;
	*lpTemp->MsgText=0;

	lpTemp->clkevent = CLK_ALWAYS;
	if ( !strcmp ( lpTemp->WindowTitle, "" ) )
		return MSG_UENOTITLE2;

	return 0;
}

static void readToList(HWND hwnd)
{
	int plen=GetWindowTextLength(GetDlgItem(hwnd,ID_WIZLISTPATH))+1;
	char *lpBuffer = GC_malloc ( plen );
	GetDlgItemText ( hwnd, ID_WIZLISTPATH, lpBuffer, plen );
	freeList(*newList,wiDestructor);
	ReadFromFile ( *newList, lpBuffer );
}

static void OnWizNext (HWND hwnd,NMHDR FAR *lpnmhdr){
	LPSTR lpBuffer;
	DWORD ResID,err=(DWORD)0;
	LPWININFO lpTemp=getData(first(*newList));

	switch ( lpnmhdr->idFrom ) {
	case DLG_BASEWIZ:
		if (CB_isChecked(hwnd,ID_WIZADDBUTTON))
			ResID = DLG_WIZADDBUTTON;
		else if (CB_isChecked(hwnd,ID_WIZADDWINDOW))
			ResID = DLG_WIZADDWINDOW;
		else if (CB_isChecked(hwnd,ID_WIZIMPORT))
			ResID = DLG_WIZSELLIST;
		if (CB_isChecked(hwnd,ID_WIZSKIP)) {
			switch (ResID) {
			case DLG_WIZADDBUTTON:
				*wizFlags=(*wizFlags ^ WF_WINDOW1 ^ WF_IMPORT ^ WF_INTRO) | WF_BUTTON1;
				break;
			case DLG_WIZADDWINDOW:
				*wizFlags=(*wizFlags ^ WF_BUTTON1 ^ WF_IMPORT ^ WF_INTRO) | WF_WINDOW1;
				break;
			case DLG_WIZSELLIST:
				*wizFlags=(*wizFlags ^ WF_WINDOW1 ^ WF_BUTTON1 ^ WF_INTRO) | WF_IMPORT;
				break;
			}
		}
		break;
	case DLG_WIZADDBUTTON:
		err=buttonToStruct(hwnd,lpTemp);
		if (!err) ResID=getNextPage(DLG_WIZADDBUTTON);
		break;
	case DLG_WIZCLKMTD:
		if ( CB_isChecked ( hwnd, ID_WIZMTDMSG ) )			lpTemp->clkmethod = MTD_CLKMSG;
		else if ( CB_isChecked ( hwnd, ID_WIZLEFTCLK ) )			lpTemp->clkmethod = MTD_LEFTCLICK;
		else if ( CB_isChecked ( hwnd, ID_WIZALTTAB ) )			lpTemp->clkmethod = MTD_MSGACT;
		else if ( CB_isChecked ( hwnd, ID_WIZRIGHTCLK ) )		    lpTemp->clkmethod = MTD_RIGHTCLICK;
		ResID = getNextPage(DLG_WIZCLKMTD);
		if (CB_isChecked(hwnd,ID_WIZSKIP))			*wizFlags^=WF_BUTTON2;
		break;
	case DLG_WIZADDWINDOW:
		err=windowToStruct(hwnd, lpTemp);
		if (!err) ResID=getNextPage(DLG_WIZADDWINDOW);
		break;
	case DLG_WIZWINCLKMTD:
		if ( SendDlgItemMessage ( hwnd, ID_WIZCLOSEWND, BM_GETCHECK, 0, 0 ) )			lpTemp->clkmethod = MTD_CLOSE;
		else if ( SendDlgItemMessage ( hwnd, ID_WIZMINWND, BM_GETCHECK, 0, 0 ) )			lpTemp->clkmethod = MTD_MINIMIZE;
		else
		    lpTemp->clkmethod = MTD_MAXIMIZE;
		ResID = getNextPage(DLG_WIZWINCLKMTD);
		if (CB_isChecked(hwnd,ID_WIZSKIP))			*wizFlags^=WF_WINDOW2;
		break;
	case DLG_WIZSELLIST:
		readToList(hwnd);
		ResID = getNextPage(DLG_WIZSELLIST);
	}
	if (!err) {
		PropSheet_SetCurSelByID(lpnmhdr->hwndFrom, ResID);
	}
	else {
		lpBuffer=GC_malloc(200);
		LoadString ( (HINSTANCE)0x400000, err, lpBuffer, 200 );
		MessageBox ( hwnd,TranslateText(err,lpBuffer),TranslateText(ID_USERERROR,MSG_USERERROR), MB_OK | MB_ICONASTERISK );
	}
	SetWindowLong ( hwnd, DWL_MSGRESULT, -1 );
}

static void OnWizBack (HWND hwnd,NMHDR *lpnmhdr){
	DWORD ResID;

	if ( lpnmhdr->idFrom==DLG_WIZSELLIST
		    || lpnmhdr->idFrom==DLG_WIZADDWINDOW
		    || lpnmhdr->idFrom==DLG_WIZADDBUTTON ) {
		freeList(*newList,wiDestructor);
	}
	ResID=getPrevPage(lpnmhdr->idFrom);
	PropSheet_SetCurSelByID(lpnmhdr->hwndFrom, ResID);
	SetWindowLong ( hwnd, DWL_MSGRESULT, -1 );
}

static void OnWizFinish (HWND hwnd,NMHDR *lpnmhdr)
{
	LPWININFO lpTemp=getData(first(*newList));
	DWORD err=0;

	switch ( lpnmhdr->idFrom ) {
	case DLG_WIZADDBUTTON:
		err=buttonToStruct(hwnd,lpTemp);
		break;
	case DLG_WIZWINCLKMTD:
		err=windowToStruct(hwnd,lpTemp);
		break;
	case DLG_WIZSELLIST:
		readToList(hwnd);
		break;
	}
	if (err) {
		char buffer[200];
		LoadString ( (HINSTANCE)0x400000, err, buffer, sizeof(buffer) );
		MessageBox ( hwnd,TranslateText(err,buffer),TranslateText(ID_USERERROR,MSG_USERERROR), MB_OK | MB_ICONASTERISK );
		SetWindowLong ( hwnd, DWL_MSGRESULT, TRUE );
	}
	else
		SetWindowLong ( hwnd, DWL_MSGRESULT, FALSE );
}

static int OnWmNotify ( HWND hwnd, LPARAM lParam ){
	NMHDR *lpnmhdr=(NMHDR *)lParam;
	lpnmhdr->idFrom = GetWindowLong(hwnd, GWL_ID);
	switch ( (int) lpnmhdr->code ) {
	case PSN_SETACTIVE:
		OnSetActive (hwnd,lpnmhdr);
		break;
	case PSN_QUERYCANCEL:
		freeList(*newList,wiDestructor);
		SetWindowLong ( hwnd, DWL_MSGRESULT, FALSE );
		break;
	case PSN_WIZFINISH:
		OnWizFinish(hwnd,lpnmhdr);
		if (CB_isChecked(hwnd,ID_WIZSKIP))			*wizFlags^=WF_SUMMARY;
		break;
	case PSN_WIZNEXT:
		OnWizNext (hwnd,lpnmhdr);
		break;
	case PSN_WIZBACK:
		OnWizBack (hwnd,lpnmhdr);
		break;
	}
	return 1;
}

static BOOL CALLBACK AddButtonDlgFunc(HWND hwndAdd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch (msg) {
	case WM_INITDIALOG:
		InitializeAddButtonDlg ( hwndAdd, wParam, lParam );
		return TRUE;
	case WM_COMMAND:
		switch ( LOWORD ( wParam ) ) {
		case ID_WIZUSETEXT:
			OnIdWizUseText ( hwndAdd );
			break;
		case ID_WIZWINLIST:
			OnIdWizWinList ( hwndAdd );
			break;
		case ID_WIZDETECTBUTTON:
			OnIdWizDetectButton ( hwndAdd, GetParent(hwndAdd) );
			break;
		case ID_WIZBROWSE:
			OnWizBrowse ( hwndAdd );
			break;
		case ID_WIZMORE:
			AdvPropsDlg(getData(first(*newList)),hwndAdd);
		}
		return TRUE;
	case WM_NOTIFY:
		return OnWmNotify (hwndAdd,lParam) ;
	}
	return FALSE;
}

static HPROPSHEETPAGE CreateSheetFromRes ( UINT ResID ){
	PROPSHEETPAGE ppsp;

	memset ( &ppsp, 0, sizeof(PROPSHEETPAGE) );
	ppsp.dwSize=sizeof(PROPSHEETPAGE);
	ppsp.pszTitle=TranslateText(ResID,"");
	ppsp.dwFlags=(*ppsp.pszTitle!='\0'?PSP_USETITLE:0); //only use translated title if there is one
	ppsp.hInstance=(HINSTANCE)0x400000;
	ppsp.pszTemplate=MAKEINTRESOURCE(ResID);
	//ppsp.lParam = ResID;
	ppsp.pfnDlgProc=AddButtonDlgFunc;

	return CreatePropertySheetPage ( &ppsp );
}

int getStartPage(void){
	if (*wizFlags & WF_INTRO) return 0;
	else if (*wizFlags & WF_BUTTON1) return 1;
	else if (*wizFlags & WF_WINDOW1) return 4;
	else if (*wizFlags & WF_SUMMARY) return 7;
	else return 0;
}

int AddButton( HWND hwnd, HINSTANCE hinst, LIST list, int startPage, int *flags ){
	PROPSHEETHEADER psph;
	HPROPSHEETPAGE phpage[9];

	memset ( &psph, 0, sizeof(PROPSHEETHEADER) );
	newList=list;
	wizFlags=flags;

	if (!startPage) startPage=getStartPage();

	for ( int i=0; i<9; i++ )
		phpage[i]=CreateSheetFromRes ( DLG_BASEWIZ+i*20 );
	psph.dwSize=sizeof(PROPSHEETHEADER);
	psph.dwFlags=PSH_USEHICON | PSH_WIZARD | PSH_USECALLBACK;
	psph.hwndParent=hwnd;
	psph.hInstance=(HINSTANCE)0x400000;
	psph.hIcon=LoadIcon((HINSTANCE)0x400000, MAKEINTRESOURCE(ID_MAINICON));
	psph.nPages=9;
	psph.nStartPage=startPage;
	psph.phpage=(HPROPSHEETPAGE *)&phpage;
	PropertySheet(&psph);

	return 1;
}

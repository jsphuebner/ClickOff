#include "detect.h"

static int  DetectClickPoint(LPWININFO lpItem, HWND hWindow, POINT *lpPoint);
static void DetectEdit(LPWININFO lpItem, HWND hWindow);
static void DetectStandardButton(LPWININFO lpItem, HWND hWindow);
static void DetectWindowCaption(LPWININFO lpItem, HWND hWindow);
static void FindDialogProps (LPWININFO wi, HWND hwnd);
static void DetectStaticText(LPWININFO lpItem, HWND hWindow);
static BOOL IsButtonClassName(const char *className);
static BOOL IsPointingOnCaption(HWND hwnd, POINT *lpPoint);


BOOL DetectControl ( LPWININFO lpItem, BOOL detectOnlyText )
{
	char className[16];
	HWND hwnd;
	POINT point;
	LONG wl;

	GetCursorPos ( &point );

	hwnd=WindowFromPoint ( point );
	if ( IsWindow ( hwnd ) )
	{
		if ( detectOnlyText )
		{
			DetectStaticText(lpItem, hwnd);
		}
		else {
			wl=GetWindowLong(hwnd, GWL_STYLE);
			GetClassName(hwnd, className, sizeof(className));
			//User is possibly pointing on a window caption bar
			if (wl & WS_POPUP)
			{
				if (IsPointingOnCaption(hwnd, &point))
					//-> add a "close window" entry
					DetectWindowCaption(lpItem, hwnd);
				else
					return DetectClickPoint(lpItem, hwnd, &point);
			}
			else if (IsButtonClassName(className))
			{
				DetectStandardButton(lpItem, hwnd);
			}
			else if (!stricmp(className, "EDIT"))
			{
				DetectEdit(lpItem, hwnd);
			}
			//User might be pointing on an unsupported button
			else
			{
				return DetectClickPoint(lpItem, hwnd, &point);
			}
		}
		return 1;
	}
	return 0;
}

static BOOL IsPointingOnCaption(HWND hwnd, POINT *lpPoint)
{
	RECT windowRect;

	GetWindowRect(hwnd, &windowRect);

	if (lpPoint->y < (windowRect.top + 20))
		return TRUE;

	return FALSE;
}

static BOOL IsButtonClassName(const char *className)
{
    if (!stricmp(className, "BUTTON")    ||
	    !stricmp(className, "TBUTTON")   ||
   		!stricmp(className, "TCHECKBOX") ||
	    !stricmp(className, "TRADIOBUTTON") )
	{
		return TRUE;
	}
	return FALSE;
}

static int DetectClickPoint(LPWININFO lpItem, HWND hWindow, POINT *lpPoint)
{
	RECT windowRect;
	LONG wl;

	FindDialogProps(lpItem, hWindow);
	if (strlen(lpItem->WindowTitle)>0)
	{
		wl=GetWindowLong(hWindow, GWL_STYLE);
		if (!(wl & WS_CAPTION || wl & WS_POPUP))
			hWindow = GetParent(hWindow);
		GetWindowRect(hWindow, &windowRect);
		lpItem->bposx = lpPoint->x - windowRect.left;
		lpItem->bposy = lpPoint->y - windowRect.top;
		lpItem->clkmethod = MTD_CLICKPOS;
		lpItem->clkevent  = CLK_ALWAYS | CLK_ONCE;
	}
	else
	{
		return 0;
	}
	return 1;
}

static void DetectEdit(LPWININFO lpItem, HWND hWindow)
{
	char className[16];

	GetClassName(GetParent(hWindow), className, sizeof(className));
	//Comboboxes need special treatment
	if (!stricmp(className, "COMBOBOX"))
		FindDialogProps(lpItem, GetParent(hWindow));
	else
		FindDialogProps(lpItem, hWindow);
	lpItem->clkmethod=MTD_ENTERTEXT;
	lpItem->clkevent|=CLK_ONCE; //Only enter text once
}

static void DetectStandardButton(LPWININFO lpItem, HWND hWindow)
{
	LONG wl;

	FindDialogProps(lpItem, hWindow);
	//Check whether its a button or checkbox
	wl=GetWindowLong(hWindow, GWL_STYLE) & 0xF;

	if (wl == BS_3STATE || wl == BS_AUTO3STATE || wl == BS_AUTOCHECKBOX ||
		wl == BS_AUTORADIOBUTTON || wl == BS_CHECKBOX || wl == BS_RADIOBUTTON)
	{
		lpItem->clkevent |= CLK_ONCE; //Only click checkboxes once
	}
	lpItem->clkmethod=MTD_CLKMSG;
}

static void DetectStaticText(LPWININFO lpItem, HWND hWindow)
{
	int textlen;

	textlen=GetWindowTextLength(hWindow)+1;
	lpItem->mdlgitem=GetDlgCtrlID ( hWindow );
	lpItem->MsgText=realloc(lpItem->MsgText,textlen);
	GetWindowText ( hWindow, lpItem->MsgText, textlen );
}

static void DetectWindowCaption(LPWININFO lpItem, HWND hWindow)
{
	int textlen;

	textlen=GetWindowTextLength(hWindow)+1;
	lpItem->WindowTitle=realloc(lpItem->WindowTitle,textlen);
	GetWindowText ( hWindow, lpItem->WindowTitle, textlen );
	lpItem->clkevent=CLK_ALWAYS;
	lpItem->clkmethod=MTD_CLOSE;
}

static void FindDialogProps (LPWININFO wi, HWND hwnd)
{
	LONG wl;
	HWND hParent=hwnd, hCurrent=NULL;
	int textlen;
	char buffer[2];

	wl=GetWindowLong(hParent, GWL_STYLE);
	if (!(wl & WS_CAPTION || wl & WS_POPUP))
	{
		hParent = GetParent(hParent);
		//Find top window
		wl=GetWindowLong(hParent, GWL_STYLE);
		if (!(wl & WS_CAPTION || wl & WS_POPUP)) {
			wi->ddlgitem=1;
			hParent=GetParent(hParent);
		}
	}

	//Get caption of parent window
	textlen=GetWindowTextLength(hParent)+1;
	wi->WindowTitle=realloc(wi->WindowTitle,textlen);
	GetWindowText ( hParent, wi->WindowTitle, textlen );

	if (hwnd != hParent)
	{
		//Get caption of button itself
		textlen=GetWindowTextLength(hwnd)+1;
		wi->ButtonText=realloc(wi->ButtonText,textlen);
		GetWindowText ( hwnd, wi->ButtonText, textlen );
		wi->bdlgitem=GetDlgCtrlID ( hwnd );
	}


	wi->clkevent=CLK_ALWAYS;

	//Get a text element of the dialog
	do {
		hCurrent=FindWindowEx ( GetParent (hwnd), hCurrent, "STATIC", NULL );

		if ( GetWindowText ( hCurrent, buffer, 2 ) ) {
			textlen=GetWindowTextLength(hCurrent)+1;
			wi->mdlgitem=GetDlgCtrlID ( hCurrent );
			wi->MsgText=realloc(wi->MsgText,textlen);
			GetWindowText ( hCurrent, wi->MsgText, textlen );
			wi->clkevent=CLK_WHENTEXT;
			hCurrent=NULL;
		}
	}
	while ( hCurrent );
}


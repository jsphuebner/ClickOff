/*------------------------------------------------------------------------
 Module:        d:\dokumente\c\clickoff1.7\checkwindows.c
 Author:        Johannes Hbner
 Project:       ClickOff
 State:         testing
 Creation Date: 22.07.03
 Description:   Provides functions to match currently open
                windows against the ClickOff-list
------------------------------------------------------------------------*/
#include "checkwindows.h"

static ITEM item=NULL;
static HWND hwnd=0, hbutton=0;
static unsigned int prevCheckSum=0;
static const unsigned int multipliers[10]={2,3,5,7,11,13,17,19,23,29};

/*------------------------------------------------------------------------
 Procedure:     FindWindowWC ID:1
 Purpose:       Search for a child window containing text owned by
                hParent
 Input:         hParent: Handle of window owning children to be
                searched for
                text: Text (maybe containing wildcards) to match
 Output:        Handle of child window matching text
                0 if not found
 Errors:
------------------------------------------------------------------------*/
int FindWindowWC ( HWND hParent, char* text )
{
	char* lpbuffer=NULL;
	int textlen;

	for ( HWND hwnd=GetWindow(hParent,GW_CHILD); hwnd; hwnd=GetWindow(hwnd,GW_HWNDNEXT) ) {
		textlen=GetWindowTextLength(hwnd)+1;
		lpbuffer=realloc(lpbuffer,textlen);
		GetWindowText(hwnd,lpbuffer,textlen);
		if ( wildcardfit(text,lpbuffer) ) {
			free(lpbuffer);
			return GetDlgCtrlID(hwnd);
		}
	}
	free(lpbuffer);
	return 0;
}

/*------------------------------------------------------------------------
 Procedure:     _cmp ID:1
 Purpose:       Compare a wininfo structure to a window
 Input:         w1, w2: Pointers to WININFO structures to compare
 Output:        1 if w1=w2 (regarding wildcards), 0 otherwise
 Errors:
------------------------------------------------------------------------*/
int _cmp ( LPWININFO w1, char* windowTitle )
{
	int found=0;
	unsigned int id=0;
	char *buffer=NULL;
	HWND hdialog;

	found = ((w1->clkevent & 0xF) != CLK_NEVER) && wildcardfit ( w1->WindowTitle, windowTitle );
	hdialog=hwnd;
	if (w1->clkevent & CLK_ONCE && w1->hCurrent == hwnd) return 0;
	if (w1->ddlgitem) hdialog=GetWindow(hwnd, GW_CHILD);
	if (found && w1->ddlgitem) resetCheckSum();
	if ( found && w1->clkmethod!=MTD_ENTERTEXT && strlen(w1->ButtonText)) {
		int len = GetWindowTextLength ( GetDlgItem(hdialog,w1->bdlgitem) ) +1;
		found = 0;
		if ( len>1 && w1->bdlgitem ) {
			buffer = malloc ( len );
			GetDlgItemText(hdialog,w1->bdlgitem,buffer,len);
			found = wildcardfit ( w1->ButtonText, buffer );
			free(buffer);
		}
		if (!found) {
			id = FindWindowWC(hdialog, w1->ButtonText);
			found=(id>0);
			if ( found ) w1->bdlgitem=id;
		}
	}
	if ( found && (w1->clkevent & 0xF) == CLK_WHENTEXT ) {
		int len = GetWindowTextLength ( GetDlgItem(hdialog,w1->mdlgitem) ) +1;
		found = 0;
		if ( len>1 && w1->mdlgitem ) {
			buffer = malloc ( len );
			GetDlgItemText(hdialog,w1->mdlgitem,buffer,len);
			found = wildcardfit ( w1->MsgText, buffer );
			free(buffer);
		}
		if (!found) {
			id = FindWindowWC(hdialog,w1->MsgText);
			found=(id>0);
			if ( found ) w1->mdlgitem=id;
		}
	}
	if (found) {
		hbutton=GetDlgItem(hdialog,w1->bdlgitem);
		w1->hCurrent=hwnd;
	}
	return found;
}


/*------------------------------------------------------------------------
 Procedure:     CheckWindow ID:1
 Purpose:       Callback function for EnumWindows. It compares the
                window title of hWindow with all items in the
                ClickOff list regarding wildcards.
 Input:         hWindow: Handle of a window
                lParam:  Points to the ClickOff list
 Output:        FALSE if found, TRUE otherwise (EnumWindows exits
                once CheckWindow returns FALSE)
 Errors:
------------------------------------------------------------------------*/
static BOOL CALLBACK CheckWindow(HWND hWindow, LPARAM lParam)
{
	list& lst = *((LIST) lParam);

	if ( hWindow == hwndMain )
		return TRUE;

	int len=GetWindowTextLength(hWindow)+1;

	hwnd = hWindow;

	if ( len>1 && IsWindowVisible(hWindow) ) {
		char* windowTitle=malloc(len);
		GetWindowText(hWindow, windowTitle, len);
		item=search(lst,windowTitle,(CMPPROC)_cmp);
		free(windowTitle);
		return (item==NULL);
	}

	return TRUE; //damit EnumWindows weiterläuft
}

static BOOL CALLBACK CheckSumWindow(HWND hwnd, LPARAM lParam)
{
	if (IsWindowVisible(hwnd))
		//checkSum+=hwnd*"Zufall"
		*((int*)lParam)+=(int)hwnd*multipliers[(int)hwnd % 10];

	return TRUE;
}

void resetCheckSum (void)
{
    prevCheckSum=0;
}

/*------------------------------------------------------------------------
 Procedure:     CheckWindows ID:1
 Purpose:       Check if any new windows have been opened. If yes,
 				match all open windows against the ClickOff list
 Input:         winlist: Reference to ClickOff list
 Output:        Windowhandle and pointer to the matching item in the
                ClickOff list if found
 Errors:
------------------------------------------------------------------------*/
fwitem CheckWindows ( list& winlist )
{
	fwitem fi;
	unsigned int checkSum=0;

	hwnd=hbutton=item=0;
	EnumWindows(CheckSumWindow, (LPARAM)&checkSum);
	if ( checkSum != prevCheckSum ) {
		prevCheckSum=checkSum;
		EnumWindows(CheckWindow, (LPARAM) &winlist);
	}

	fi.hwnd=hwnd;
	fi.hbutton=hbutton;
	fi.item=item;
	return fi;
}

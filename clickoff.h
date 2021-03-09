#ifndef clickoff_h
#define clickoff_h

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <winuser.h>
#include <SHELLAPI.H>
#include <MALLOC.H>
#include <process.h>
#include "clickoffres.h"
#include "fileio.h"
#include "windowinfo.h"
#include "checkwindows.h"
#include "taskbar.h"
#include "detect.h"
#include "maindlg.h"
#include "list.h"
#include "config.h"
#include "settingsdlg.h"
#include "translate.h"
#include "winstuff.h"
#include "version.h"

//Unique Identifier for ClickOff-window, ANSI-Code for CLKO
#define UID 0x434C4B6F

#define	MSG_CLICKOFFDISABLED	"ClickOff - deaktiviert"
#define	MSG_BUTTONSUM	"Fenstertitel: %s\r\nButtontext: %s\r\nMeldungstext: %s\r\nClickOff-Methode: %s"
#define	MSG_WINDOWSUM	"Fenstertitel: %s\r\nClickOff-Methode: %s"
#define	MSG_USERERROR	"Eingabefehler"
#define	MSG_DETECTALL	"Fenstertitel: %s\nButtontext: %s\nMeldungstext: %s"
#define MSG_IOENOTEXIST "Kann nicht auf die ClickOff-Liste zugreifen"
#define MSG_IOEUNEXPEOF "ClickOff Liste unvollständig, möglicherweise wegen einem vorhergehenden Absturz"
#define MSG_IOEFORMAT	"Formatfehler in ClickOff-Liste. Haben Sie die Datei von Hand bearbeitet?"
#define MSG_IOEUNKNTAG	"Unbekannter Tag in ClickOff-Liste.\n-Haben Sie die Datei von Hand bearbeitet?\n-Stammt die Datei von einer neueren Version?\n\nBeim erneuten Speichern werden u.U. Informationen verloren gehen."
#define MSG_IOERR	"E/A-Fehler"

#define ID_CLICKOFFDISABLED 10001
#define ID_BUTTONSUM 10002
#define ID_WINDOWSUM 10003
#define ID_USERERROR 10004
#define ID_DETECTALL 10005
#define ID_HELPTEXT  10006
#define ID_IOENOTEXIST 10007
#define ID_IOEUNEXPEOF 10008
#define ID_IOEFORMAT   10009
#define ID_IOEUNKNTAG  10010
#define ID_IOERR	   10011

#define findfirst _findfirst
#define findnext _findnext
#define MN_SUBSORT	MN_LIST

#define CLK_NEVER 0
#define CLK_ALWAYS 1
#define CLK_WHENTEXT 2
#define CLK_ONCE 0x10

enum _clkmethods
{
	MTD_LEFTCLICK,
	MTD_RIGHTCLICK,
	MTD_CLOSE,
	MTD_CLKMSG,
	MTD_MAXIMIZE,
	MTD_MINIMIZE,
	MTD_MSGACT,
	MTD_ENTERTEXT,
	MTD_CLICKPOS,
	MTD_LAST
};

#define FI_WINDOWTITLE 1
#define FI_BUTTONTEXT 2
#define FI_MSGTEXT 4

#define NUMMTD MTD_LAST
#define HOTKEY 1

#define LB_GetTextLen(h,i,n) SendDlgItemMessage ( h, (DWORD)i, LB_GETTEXTLEN, (WPARAM)n, 0 )
#define LB_GetText(h,i,n,l) SendDlgItemMessage ( h, (DWORD)i, LB_GETTEXT, (WPARAM)n, (LPARAM)(LPSTR)l )
#define HOTKEYFMOD(m) 	(m&HOTKEYF_SHIFT?MOD_SHIFT:0) | \
						(m&HOTKEYF_CONTROL?MOD_CONTROL:0) | \
						(m&HOTKEYF_ALT?MOD_ALT:0) | \
						(m&HOTKEYF_EXT?MOD_WIN:0)
#define IOERR2ID(i)  i==IOERR_UNKNTAG?ID_IOEUNKNTAG: \
					(i==IOERR_FORMAT?ID_IOEFORMAT: \
					(i==IOERR_UNEXPEOF?ID_IOEUNEXPEOF: \
					(i==IOERR_NOTEXIST?ID_IOENOTEXIST:0xFFFFFFFF)))

#define IOERR2MSG(i) i==IOERR_UNKNTAG?MSG_IOEUNKNTAG: \
					(i==IOERR_FORMAT?MSG_IOEFORMAT: \
					(i==IOERR_UNEXPEOF?MSG_IOEUNEXPEOF: \
					(i==IOERR_NOTEXIST?MSG_IOENOTEXIST:"")))

HWND hCfgDlg, hwndMain, hwndAdv;
list clickoffList;

void SaveToMem (void);
void ExitClickOff(void);
void ToggleActive (HWND);

#endif //clickoff_h

#ifndef taskbar_h
#define taskbar_h

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <SHELLAPI.H>
#include "clickoffres.h"
#include "clickoff.h"

void ModifyTrayIcon (HWND, int, LPSTR );
void CreateTrayIcon(HWND,HINSTANCE,BOOL);
BOOL MyTaskBarDeleteIcon(HWND hwnd, UINT uID);
int ShowMenu(HWND, int);
void LoadTrayMenu(HWND mMsgDlg);

HMENU hmenu;				//Handle of tray icon menu

#endif //taskbar_h

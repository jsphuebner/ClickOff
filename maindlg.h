#ifndef maindlg_h
#define maindlg_h

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <windows.h>
#include "clickoffres.h"
#include "clickoff.h"
#include "config.h"
#include "translate.h"
#include "checkwindows.h"
#include "advanceddlg.h"

void HideCfgDlg (void);
void ShowCfgDlg (void);
void InitCfgDlg (LPCLKCFG);
BOOL ResizeControls ( HWND, LPRECT );
void RefreshList (HWND);
void OnAdd ( HWND );

#endif //maindlg_h

#ifndef wizard_h
#define wizard_h

#include "clickoff.h"
#include "windowinfo.h"
#include "list.h"
#include "fileio.h"
#include "clickoffres.h"
#include "detect.h"
#include "advanceddlg.h"

#define CB_isChecked(h,i) SendDlgItemMessage ( h, (DWORD)i, BM_GETCHECK, 0, 0 )

int AddButton( HWND, HINSTANCE, LIST, int, int* );

#endif //wizard_h

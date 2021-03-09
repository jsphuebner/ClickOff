#ifndef translate_h
#define translate_h

#include <windows.h>
#include <stdio.h>
#include "list.h"
#include "fileio.h"

#define CLM_LNGCHANGE WM_USER+3

int LoadLngFile(char*);
const char* TranslateText(int, const char*);
int TranslateDialog(HWND);
int TranslateMenu(HMENU);
int AddToBCQueue(HWND);
int RemoveFromBCQueue(HWND);

#endif //translate_h

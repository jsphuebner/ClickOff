#ifndef config_h
#define config_h

#include <stdio.h>
#include <windows.h>
#include "winstuff.h"

#define WF_ALL 0x7F

typedef struct {
	int nScanTime;
	int bActive;
	char lngFile[64];
	RECT windowRect;
	unsigned int showCmd;
	int trayIcon;
	loc cfgloc;
	WORD wHotkey;
	char logFile[512];
} CLKCFG,*LPCLKCFG;

void ReadConfig (LPCLKCFG config);
void SaveConfig (LPCLKCFG config);

#endif //config_h

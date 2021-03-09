#include "config.h"

/*------------------------------------------------------------------------
 Procedure:     ReadConfig ID:1
 Purpose:       Reads a few configuration variables from clickoff.cfg
 Input:         none
 Output:        none
 Errors:
------------------------------------------------------------------------*/
void ReadConfig (LPCLKCFG config)
{
	FILE *file = fopen(".\\ALL_USERS","r");

	file=fopen ( PrependAppData("clickoff.cfg",CURRENTUSER), "r" );
	if (file) config->cfgloc=CURRENTUSER;
	else {
		file=fopen ( PrependAppData("clickoff.cfg",ALLUSERS), "r" );
		if (file) config->cfgloc=ALLUSERS;
		else {
			file=fopen ( "clickoff.cfg", "r" );
			if (file) config->cfgloc=PROGDIR;
		}
	}

	if (file) {
		fscanf ( file, "%d,%d %s %d,%d,%d,%d,%d,%d %s",
			&config->bActive, &config->nScanTime,
			&config->lngFile, &config->wHotkey,
			&config->windowRect.left,&config->windowRect.top, &config->windowRect.right,
			&config->windowRect.bottom, &config->showCmd, &config->logFile);
		if (config->nScanTime<1) config->nScanTime=1;
		fclose ( file );
	}
	else {
		config->bActive=TRUE;
		config->nScanTime=100;
		config->wHotkey=1604;
	}
}

/*------------------------------------------------------------------------
 Procedure:     ReadConfig ID:1
 Purpose:       Saves a few configuration variables to clickoff.cfg
 Input:         none
 Output:        none
 Errors:
------------------------------------------------------------------------*/
void SaveConfig (LPCLKCFG config)
{
	FILE *file = fopen ( PrependAppData("clickoff.cfg",config->cfgloc), "w+" );

	if (file) {
		fprintf ( file,"%d,%d %s %d,%d,%d,%d,%d,%d %s",
			config->bActive, config->nScanTime,
			config->lngFile, config->wHotkey,
			config->windowRect.left,config->windowRect.top, config->windowRect.right,
			config->windowRect.bottom, config->showCmd, config->logFile );
		fclose ( file );
	}
}



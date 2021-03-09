#ifndef checkwindows_h
#define checkwindows_h
#include <windows.h>
#include "list.h"
#include "windowinfo.h"
#include "wildcards.h"
#include "clickoff.h"

#define CLK_NEVER 0
#define CLK_ALWAYS 1
#define CLK_WHENTEXT 2

typedef struct Tagfwitem {
	HWND hwnd;
	HWND hbutton;
	ITEM item;
} fwitem;

fwitem CheckWindows ( list& );
void resetCheckSum (void);

#endif //checkwindows_h

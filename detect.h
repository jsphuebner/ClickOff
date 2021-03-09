#ifndef detect_h
#define detect_h

#define T_POINTTIME 2000
#define T_DETECTINTERVAL 50
#define TM_DETECTALL 7
#define TM_DETECTTEXT 4

#include <windows.h>
#include "windowinfo.h"
#include "clickoffres.h"
#include "clickoff.h"

BOOL DetectControl (LPWININFO, BOOL);

#endif //detect_h

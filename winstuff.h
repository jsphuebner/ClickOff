#ifndef winstuff_h
#define winstuff_h

#include <windows.h>
#include <shlobj.h>
#include <string.h>

typedef enum _loc { PROGDIR, ALLUSERS, CURRENTUSER } loc;
char* PrependAppData(const char*, loc);


#endif //winstuff_h


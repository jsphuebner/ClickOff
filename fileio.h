#ifndef fileio_h
#define fileio_h

#define IOERR_NOERR (int)0x8000000A
#define IOERR_NOTEXIST (int)0x80000004
#define IOERR_UNEXPEOF (int)0x80000003
#define IOERR_FORMAT (int)0x80000002
#define IOERR_UNKNTAG (int)0x80000001

#include <stdio.h>
#include <string.h>
#include "list.h"
#include "windowinfo.h"
#include "clickoff.h"
#include "winstuff.h"

int ReadFromFile ( list&, char* );
int SaveToFile(list&, char* );
unsigned int gsd ( FILE*, char, char*, int, int ); //GetStringData
int gid ( FILE*, char ); //GetIntData

#endif //fileio_h

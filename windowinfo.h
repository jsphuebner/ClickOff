#ifndef windowinfo_h
#define windowinfo_h

#include <malloc.h>
#include <stdlib.h>
#include <string.h>

typedef struct s_wininfo {
	char* WindowTitle;
	char* ButtonText;
	char* MsgText;
	unsigned int bdlgitem;
	unsigned int mdlgitem;
	unsigned int ddlgitem;
	unsigned int clkevent;
	unsigned int clkmethod;
	unsigned int timesclicked;
	int wait;
	int timecount;
	int bposx;
	int bposy;
	void* hCurrent;
} WININFO,*LPWININFO;

LPWININFO WI_newItem ( void );
void setWindowTitle ( LPWININFO, char* );
void setButtonText ( LPWININFO, char* );
void setMsgText ( LPWININFO, char* );
void wiDestructor ( void* );

#endif //windowinfo_h

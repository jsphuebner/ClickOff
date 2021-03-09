#include "windowinfo.h"

LPWININFO WI_newItem ( void )
{
	LPWININFO new = malloc(sizeof(WININFO));
	memset(new,0,sizeof(WININFO));
	setWindowTitle ( new, "" );
	setButtonText ( new, "" );
	setMsgText ( new, "" );

	return new;
}

void setWindowTitle ( LPWININFO wi, char* wt )
{
	if (!wt) return;
	wi->WindowTitle = realloc(wi->WindowTitle, strlen (wt)+1);
	strcpy(wi->WindowTitle, wt);
}

void setButtonText ( LPWININFO wi, char* bt )
{
	if (!bt) return;
	wi->ButtonText = realloc(wi->ButtonText, strlen (bt)+1);
	strcpy(wi->ButtonText, bt);
}

void setMsgText ( LPWININFO wi, char* mt )
{
	if (!mt) return;
	wi->MsgText = realloc(wi->MsgText, strlen (mt)+1);
	strcpy(wi->MsgText, mt);
}

void wiDestructor ( void* wi )
{
	free(((LPWININFO)wi)->WindowTitle);
	free(((LPWININFO)wi)->ButtonText);
	free(((LPWININFO)wi)->MsgText);
	free(wi);
}


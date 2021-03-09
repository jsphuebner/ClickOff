#include "translate.h"

typedef struct _dictionary
{
	int id;
	char* text;
} DICT,*LPDICT;

#define STATE_INIT 0
#define STATE_ID 1
#define STATE_FOREIGN 2
#define STATE_LNGERR 4
#define STATE_LNGOK 8
#define LNGHEADER "CLICKOFF LNG V1"

int state=STATE_INIT;
list dictnry;
list bclist;

void dict_dest ( void* dict )
{
	free(((LPDICT)dict)->text);
	free(dict);
}

void BroadcastLngChange (void)
{
	for ( ITEM i=first(bclist); !isLast(i); i=next(i) )
		SendMessage((HWND)getData(i),CLM_LNGCHANGE,0,0);
}

void LngError(char* filename, int line)
{
	char buffer[64];
	memset(&buffer,0,sizeof(buffer));
	sprintf(buffer,"Error in language file %s, line %d",filename,line);
	MessageBox(HWND_DESKTOP,buffer,"ClickOff language module", MB_OK | MB_ICONEXCLAMATION );
	state=STATE_LNGERR;
}

/*
[text] Holds text ID (usually ID of control its on)
"translation" one line below holds translated text
; is a comment
*/
int LoadLngFile(char* filename)
{
	FILE *lngFile;

	//I don't expect texts to be longer than 512 bytes
	char c,buffer[512];
	int length=0,line=1;
	LPDICT current;

	if (state==STATE_INIT)
		clearList(dictnry);
	else
		freeList(dictnry,dict_dest);
	memset(&buffer,0,sizeof(buffer));

	lngFile=fopen(filename,"r");

	if (lngFile) {
		gsd(lngFile,'\n',buffer,sizeof(buffer),0);
		if (strcmp(buffer,LNGHEADER)) {
			state=STATE_LNGERR;
		    return 0;
		}
		state=STATE_ID; //we are waiting for the first german string
		c=(char)getc(lngFile);
		while ( c!=EOF ) {
			switch (c) {
			case '[':
				if ( state == STATE_ID ) {
					current = malloc(sizeof(DICT));
					current->id = gid(lngFile, ']');
					state=STATE_FOREIGN;
				}
				else {
					LngError(filename,line);
					fclose(lngFile);
					return 0;
				}
				break;
			case ';':
				gsd(lngFile,'\n',NULL,0,0);
			case '\n':
				line++;
			case ' ':
			case '\t':
				break;
			case '"':
				if ( state == STATE_FOREIGN ) {
					length=gsd(lngFile,'"',buffer,sizeof(buffer),1);
					current->text=malloc(length+1);
					memcpy(current->text,buffer,length+1);
					push_back(dictnry,current);
					state=STATE_ID;
				}
				else {
					LngError(filename,line);
					fclose(lngFile);
					return 0;
				}
				break;
			default:
				LngError(filename,line);
				fclose(lngFile);
				return 0;
			}
			c=(char)getc(lngFile);
		}
		fclose(lngFile);
		state=STATE_LNGOK;
		BroadcastLngChange();
		return 1;
	}
	state=STATE_LNGERR;
	return 0;
}

int dict_cmp(LPDICT d1, int* id)
{
	return (d1->id == *id);
}

int hwnd_cmp(HWND h1, HWND h2)
{
	return (h1==h2);
}

const char* TranslateText(int id, const char* altText)
{
	if (state==STATE_LNGERR) return altText;
	ITEM it=search(dictnry,&id,(CMPPROC)dict_cmp);
	if (it) return ((LPDICT)getData(it))->text;
	return altText;
}

int TranslateDialog(HWND hwnd)
{
	if (state==STATE_LNGERR) return 0;

	char buffer[512];

	GetWindowText(hwnd,buffer,sizeof(buffer));
	SetWindowText(hwnd,TranslateText(GetWindowLong(hwnd,DWL_USER),buffer));

	for (hwnd=GetWindow(hwnd,GW_CHILD); hwnd; hwnd=GetWindow(hwnd,GW_HWNDNEXT)) {
		GetWindowText(hwnd,buffer,sizeof(buffer));
		SetWindowText(hwnd,TranslateText(GetDlgCtrlID(hwnd),buffer));
	}
	return 1;
}

int TranslateMenu(HMENU hmenu)
{
	int n=GetMenuItemCount(hmenu);
	MENUITEMINFO minfo;

	memset(&minfo,0,sizeof(minfo));
	minfo.cbSize=sizeof(minfo);
	minfo.fMask=MIIM_ID	| MIIM_DATA | MIIM_SUBMENU;
	for (int i=0; i<n; i++) {
		GetMenuItemInfo(hmenu,i,TRUE,&minfo);
		if (minfo.hSubMenu) TranslateMenu(minfo.hSubMenu);
		if ( !minfo.wID ) continue;
		minfo.dwTypeData=(char *)TranslateText(minfo.wID,NULL);
		if (minfo.dwTypeData) minfo.fMask|= MIIM_TYPE;
		SetMenuItemInfo(hmenu,i,TRUE,&minfo);
		minfo.fMask=MIIM_ID	| MIIM_DATA | MIIM_SUBMENU;
	}
	return 1;
}

int AddToBCQueue(HWND hwnd)
{
	//if (!bclist.next) clearList(bclist);
	push_back(bclist,hwnd);

	return 1;
}

int RemoveFromBCQueue(HWND hwnd)
{
	ITEM it=search(bclist,hwnd,hwnd_cmp);
	if (it) delete(bclist,it);
	else return 0;

	return 1;
}

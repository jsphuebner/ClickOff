#include "winstuff.h"

static char buffer[512];

char* PrependAppData(const char* filename, loc loc)
{
	//char *buffer=GC_malloc(512);
	int size;
	ITEMIDLIST idl;
	LPITEMIDLIST pidl=&idl;

	switch (loc) {
		case PROGDIR:
			return (char*)filename;
		case ALLUSERS:
			SHGetSpecialFolderLocation(NULL,CSIDL_COMMON_APPDATA,&pidl);
			break;
		case CURRENTUSER:
			SHGetSpecialFolderLocation(NULL,CSIDL_APPDATA,&pidl);
			break;
	}
	memset(buffer,0,512);
	SHGetPathFromIDList(pidl,buffer);
	size=strlen(buffer);
	strcpy(&buffer[size],"\\ClickOff\\");
	strcpy(&buffer[size+10],filename);

	return buffer;
}




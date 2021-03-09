#include "fileio.h"

#define ST_WAITTAG 1
#define ST_WAITEQ 2
#define ST_WAITDATA 3
#define ST_DONE 4
#define EOL "end of list"

#define getc (char)getc

#define TAG_LIST\
	TAG_ENTRY(WINDOWTITLE,	%s)	\
	TAG_ENTRY(BUTTONTEXT,	%s)	\
	TAG_ENTRY(MSGTEXT,		%s)	\
	TAG_ENTRY(BUTTONID,		%d)	\
	TAG_ENTRY(MSGID,		%d)	\
	TAG_ENTRY(DLGID,		%d)	\
	TAG_ENTRY(CLKEVENT,		%d)	\
	TAG_ENTRY(CLKMETHOD,	%d)	\
	TAG_ENTRY(TIMESCLICKED,	%d)	\
	TAG_ENTRY(WAIT,			%d)	\
	TAG_ENTRY(BPOSX,		%d)	\
	TAG_ENTRY(BPOSY,		%d)

#define TAG_ENTRY(a,b) a,
typedef enum _enumtag
{
	TAGERR,
	TAG_LIST
	UNKNOWN
} tag;
#undef TAG_ENTRY

#define NUM_TAGS (UNKNOWN)

#define TAG_ENTRY(a,b) #a,

const char* tagnames[]=
{
	"xxx",
	TAG_LIST
	"xxx"
};
#undef TAG_ENTRY

#define TAG_ENTRY(a,b) #a"=\""#b"\" "

//const char* formatString = { TAG_LIST ";\n"};
#define FORMAT_STRING TAG_LIST ";\n"

//don't undef, it is expanded later on
//this enables compiler warning on missing arguments
//in the format string
//#undef TAG_ENTRY

char TranslateEsc(char c)
{
	switch(c) {
		case 'n': return '\n';
		case 'r': return '\r';
		case 't': return '\t';
		case '\\':return '\\';
		case '"': return '"';
		default : return c-'0';
	}
}

char* InsertEsc ( char* buffer, char* s)
{
	int n=strlen(s),offset=0;

	for (int i=0; i<n; i++) {
		switch (s[i]) {
			case '\n':
				buffer[i+offset++]='\\';
				buffer[i+offset]='n';
				break;
			case '\r':
				buffer[i+offset++]='\\';
				buffer[i+offset]='r';
				break;
			case '\t':
				buffer[i+offset++]='\\';
				buffer[i+offset]='t';
				break;
			case '\\':
				buffer[i+offset++]='\\';
				buffer[i+offset]='\\';
				break;
			case '"':
				buffer[i+offset++]='\\';
				buffer[i+offset]='"';
				break;
			default: buffer[i+offset]=s[i];
		}
	}
	return buffer;
}

unsigned int gsd ( FILE *file, char delim, char* buffer, int len, int trEsc ) //GetStringData
{
	int i=0;
	for ( char c=getc (file); c!=delim; c=getc (file) ) {
		if ( c==EOF ) return IOERR_UNEXPEOF;
		if ( c=='\\' && trEsc ) c=TranslateEsc(getc(file));
		if (i<len) buffer[i++]=c;  //allow blank reads for skipping comments
	}
	if (i<len) buffer[i]=0;
	return i;
}

int gid ( FILE *file, char delim ) //GetIntData
{
	int value=0,sign=1;
	for ( char c=getc (file); c!=delim; c=getc (file) ) {
		if ( c==EOF ) return IOERR_UNEXPEOF;
		if ( c=='-' ) sign=-1;
		else value=value*10+c-'0';
	}
	return value*sign;
}

static int OldReadSettings( LIST list, char* lpFilename )
{
	char buffer[512],*lpstr;
	FILE *file;
	fpos_t pos=6;
	float fVersion = 0;
	LPWININFO wi;
	//CLKCFG config = *lpconfig;

	file = fopen ( lpFilename, "r");
	if (file != NULL) {
		fscanf ( file, "%f", &fVersion );
		if ( fVersion == 0 ) { //alte 1.4x Leseprozedur
			char line[512];
			pos=0;
			fsetpos ( file, &pos );
			while ( 1 ){
				wi=WI_newItem();
				if (!fgets ( line,sizeof(line),file )) break;
				lpstr=strchr(line,'\n');
				if (!lpstr) break;
				*lpstr=0;
				if ( !strcmp ( line, EOL ) )
					break;
				setWindowTitle(wi, strtok(line, ";"));
				setButtonText(wi, strtok(NULL, ";"));
				setMsgText(wi,"");
				wi->clkevent=CLK_ALWAYS;
				wi->clkmethod=MTD_CLKMSG;
				wi->timesclicked=0;
				push_back(*list,wi);
			}
		}

		if ( fVersion >= 1.5 ) {
			while ( 1 ) {
				wi=WI_newItem();
				fsetpos ( file, &pos );
				//Fenstertitel lesen, bei Dateiende abbrechen
				if(!fgets(buffer,sizeof(buffer),file)) break;
				buffer[strlen(buffer)-1]=0;
				if ( !strcmp ( buffer, EOL ) ) break;
				setWindowTitle(wi,buffer);
				//Buttontext
				if(!fgets(buffer,sizeof(buffer),file)) break;
				buffer[strlen(buffer)-1]=0;
				setButtonText(wi,buffer);
				//Nachrichtentext
				if(!fgets(buffer,sizeof(buffer),file)) break;
				buffer[strlen(buffer)-1]=0;
				setMsgText(wi,buffer);

				fscanf ( file, "%d", &wi->clkevent );
				fscanf ( file, "%d", &wi->clkmethod );
				fscanf ( file, "%d", &wi->timesclicked );
				push_back(*list,wi);
				pos=ftell ( file )+2; //weil fscanf nicht weiterzählt+cr/lf
			}
		}
	}
	fclose ( file );
	return 1;
}

tag GetTag(FILE *file, char first)
{
	char buffer[16];
	fpos_t pos;
	memset(buffer,0,sizeof(buffer));
	buffer[0]=first;

	for ( int i=1; i<sizeof(buffer); i++ ) {
		buffer[i]=getc(file);
		for (int j = 1; j < NUM_TAGS; j++)
		{
			if (!stricmp(buffer, tagnames[j]))
				return j;
		}
		if ( buffer[i]==' ' || buffer[i]=='=' || buffer[i]=='\t' || buffer[i]=='\n' ) {
			fgetpos(file,&pos);
			pos--;
			fsetpos(file,&pos);
			return UNKNOWN;
		}
	}
	return TAGERR;
}

int ReadFromFile ( list& lst, char* lpFilename )
{
	FILE *file;
	double version;
	int trEsc=1;
	// Maximum size of a tag is 16 bytes, maximum data length is 4 kb
	char buffer[16],strValues[4096],*ptr=strValues;
	int err=0, state=ST_WAITTAG, unknownPresent=0;
	tag tag=0;

	file = fopen ( lpFilename, "r" );
	if (!file) return IOERR_NOTEXIST;
	// For compatability with older list-formats
	memset(buffer, 0, sizeof(buffer));
	fgets ( buffer, 10, file );
	if (strlen(buffer) == 0)
		return IOERR_UNEXPEOF;
	version=strtod ( buffer, NULL );
	if ( version < 1.65 ) {
		fclose (file);
		return OldReadSettings ( &lst, lpFilename );
	}
	if ( version < 1.7709 ) trEsc=0;
	LPWININFO wi = WI_newItem();
	memset ( buffer,0,sizeof(buffer) );
	memset ( strValues,0,sizeof(strValues));
	for ( char c=getc ( file ); state!=ST_DONE; c=getc ( file ) ) {
		switch (c) {
			// Skip blanks, tabs and newlines
			case ' ':
			case '\t':
			case '\r':
			case '\n':
				break;
			case EOF:
				if(state==ST_WAITTAG) state=ST_DONE;
				else err=IOERR_UNEXPEOF;
				break;
			case '"':
				if (state==ST_WAITDATA) {
					switch (tag) {
						case WINDOWTITLE:
							err=gsd ( file, '"', ptr, sizeof(strValues), trEsc );
							setWindowTitle(wi,ptr);
							break;
						case BUTTONTEXT:
							err=gsd ( file, '"', ptr, sizeof(strValues), trEsc );
							setButtonText(wi,ptr);
							break;
						case MSGTEXT:
							err=gsd ( file, '"', ptr, sizeof(strValues), trEsc );
							setMsgText(wi,ptr);
							break;
						case BUTTONID:
							err=wi->bdlgitem=gid ( file, '"' );
							break;
						case MSGID:
							err=wi->mdlgitem=gid ( file, '"' );
							break;
						case DLGID:
							err=wi->ddlgitem=gid ( file, '"' );
							break;
						case CLKEVENT:
							err=wi->clkevent=gid ( file, '"' );
							break;
						case CLKMETHOD:
							err=wi->clkmethod=gid ( file, '"' );
							break;
						case TIMESCLICKED:
							err=wi->timesclicked=gid ( file, '"' );
							break;
						case WAIT:
							err=wi->wait=gid( file, '"' );
							break;
						case BPOSX:
							err=wi->bposx=gid(file, '"');
							break;
						case BPOSY:
							err=wi->bposy=gid(file, '"');
							break;
						case UNKNOWN:
							//We encountered some unknown tag.
							//We try to keep consistent by reading its data and then continuing normally
							err=gsd(file, '"', NULL, 0, 0);
							unknownPresent=1;
							break;
						default:
							err=IOERR_FORMAT;
					}
					state=ST_WAITTAG;
				}
				else err=IOERR_FORMAT;
				break;
			case '=':
				if (state==ST_WAITEQ) state=ST_WAITDATA;
				else err=IOERR_FORMAT;
				break;
			case ';':
				if (state==ST_WAITTAG) {
					push_back(lst, wi);
					wi = WI_newItem();
					state=ST_WAITTAG;
				}
				else err=IOERR_FORMAT;
				break;
			default:
				if(state==ST_WAITTAG) {
					tag=GetTag(file,c);
					state=ST_WAITEQ;
				}
				else err=IOERR_FORMAT;
		}
		if (err<IOERR_NOERR) break;
	}
	fclose (file);
	if (unknownPresent) err=IOERR_UNKNTAG;
	if (err>=IOERR_NOERR) err=IOERR_NOERR;

	return err;
}

int SaveToFile(list& lst, char* lpFilename)
{
	FILE *file;
	char wintitle[4096], buttontext[4096], msgtext[4096];

	file = fopen(lpFilename,"w+");

	if (file != NULL) {
		if ( fprintf ( file, "%.3f\n", VERSION ) < 0 ) {
			fclose (file);
			return 0;
		}
		for ( ITEM lpCurrent=first(lst); !isLast(lpCurrent); lpCurrent=next(lpCurrent) ) {
			memset(wintitle,0,sizeof(wintitle));
			memset(buttontext,0,sizeof(buttontext));
			memset(msgtext,0,sizeof(msgtext));
			InsertEsc(wintitle,((LPWININFO)getData(lpCurrent))->WindowTitle);
			InsertEsc(buttontext,((LPWININFO)getData(lpCurrent))->ButtonText);
			InsertEsc(msgtext,((LPWININFO)getData(lpCurrent))->MsgText);

			fprintf (   file,
					    FORMAT_STRING,
						wintitle,
						buttontext,
						msgtext,
						((LPWININFO)getData(lpCurrent))->bdlgitem,
						((LPWININFO)getData(lpCurrent))->mdlgitem,
						((LPWININFO)getData(lpCurrent))->ddlgitem,
						((LPWININFO)getData(lpCurrent))->clkevent,
						((LPWININFO)getData(lpCurrent))->clkmethod,
						((LPWININFO)getData(lpCurrent))->timesclicked,
					    ((LPWININFO)getData(lpCurrent))->wait,
					    ((LPWININFO)getData(lpCurrent))->bposx,
						((LPWININFO)getData(lpCurrent))->bposy);
		}
		fclose (file);
		return 1;
	}
	return 0;
}


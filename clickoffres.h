/* Weditres generated include file. Do NOT edit */
#include <windows.h>
#include <lfc.h>
#define	MSG_METHODSTRING	0
#define	IDD_MAINDIALOG	100
#define	ID_EXIT	101
#define	ID_LIST	102
#define	ID_CLOSE	103
#define	ID_ITEXT	105
#define	ID_REMOVE	112
#define	ID_ADVANCED	113
#define	ID_ACTIVE	114
#define	DLG_SETUP	200
#define	ID_COMMON	202
#define	DLG_INFO	300
#define	ID_HOMEPAGE	304
#define	ID_SHOWDOC	304
#define	ID_EMAIL	305
#define	ID_LOGO	306
#define	ID_CLICKSUM	313
#define	DLG_EMPTY	400
#define	DLG_ADVANCED	500
#define	ID_NODELAY	502
#define	ID_CANCEL	503
#define	ID_OK	504
#define	ID_EXTACTIVE	505
#define	ID_MSGTEXT	506
#define	ID_NEVERCLICK	508
#define	ID_ALWAYSCLICK	509
#define	ID_CLICKWHENTEXT	510
#define	ID_GETTEXT	511
#define	ID_CLICKOFFMETHOD	512
#define	ID_CLICKONCE	514
#define	ID_CLICKNUMBER	516
#define	ID_WINDOWTITLE	519
#define	ID_PREDELAY	520
#define	ID_STATICBTEXT	521
#define	ID_BUTTONTEXT	522
#define	ID_DELAY	523
#define	ID_POSTDELAY	525
#define	DLG_BASEWIZ	600
#define	ID_WIZADDBUTTON	603
#define	ID_WIZADDWINDOW	604
#define	ID_WIZIMPORT	605
#define	DLG_WIZADDBUTTON	620
#define	ID_WIZBUTTONTEXT	627
#define	ID_WIZDETECTBUTTON	628
#define	ID_WIZUSETEXT	630
#define	ID_WIZTEXTSTATIC	631
#define	ID_WIZMSGTEXT	632
#define	ID_WIZDETECTTEXT	633
#define	DLG_WIZCLKMTD	640
#define	ID_WIZMTDMSG	645
#define	ID_WIZLEFTCLK	646
#define	ID_WIZRIGHTCLK	647
#define	ID_WIZALTTAB	648
#define	ID_WIZMORE	649
#define	DLG_WIZBUTTONFINISH	660
#define	ID_WIZSUMMARY	663
#define	ID_WIZSKIP	664
#define	DLG_WIZADDWINDOW	680
#define	ID_WIZWINLIST	684
#define	ID_WIZWINTITLE	687
#define	DLG_WIZWINCLKMTD	700
#define	ID_WIZCLOSEWND	703
#define	ID_WIZMINWND	704
#define	ID_WIZMAXWND	705
#define	DLG_WIZWINFINISH	720
#define	DLG_WIZSELLIST	740
#define	ID_WIZFILENAME	741
#define	ID_WIZLISTPATH	744
#define	ID_WIZBROWSE	745
#define	DLG_WIZLISTFINISH	760
#define	ID_WIZIMPLIST	763
#define	DLG_DETECT	800
#define	ID_DETECTTEXT	803
#define	DLG_SETTINGS	900
#define	ID_USEHOTKEY	901
#define	ID_HOTKEY	903
#define	ID_LANG	905
#define	ID_SHOWTRAYICON	906
#define ID_LOGFILE      909
#define ID_BROWSELOGFILE 910
#define	ID_SCANTIME	915
#define	MN_POPUP	1000
#define	MN_CONFIG	1001
#define	MN_SETTINGS	1002
#define	MN_ACTIVE	1003
#define	MN_SUBADD	1004
#define	MN_INFO	1005
#define	MN_HELP	1006
#define	MN_EXIT	1008
#define	MN_LIST	1010
#define	MN_SORT_BYWIN_DES	1011
#define	MN_SORT_BYWIN_ASC	1012
#define	MN_SORT_BYBUT_DES	1013
#define	MN_SORT_BYBUT_ASC	1014
#define	MN_SORT_BYMSG_DES	1015
#define	MN_SORT_BYMSG_ASC	1016
#define	MN_SORT_BYCL_ASC	1017
#define	MN_SUBEXPORT	1020
#define	MN_EXPORT_ALL	1021
#define	MN_EXPORT_SEL	1022
#define	MN_SEL_ALL	1031
#define	MN_INV_SEL	1032
#define	MN_DEL	1033
#define	MN_IMPORT	1034
#define	MN_REMDUPL	1036
#define	ID_MAINICON	4000
#define	ID_TRAYICON	4001
#define	ID_ICODISABLED	4002
#define	MN_ADDWIN	16385
/*@ Prototypes @*/
#ifndef WEDIT_PROTOTYPES
#define WEDIT_PROTOTYPES
/*
 * Structure for dialog Dlg100
 */
struct _Dlg100 {
	ST_STATIC *id_itext;
	ST_CHECKBOX *id_active;
	ST_BUTTON *id_remove;
	ST_STATIC *ctrl110;
	ST_LISTBOX *id_list;
	ST_BUTTON *id_advanced;
	ST_BUTTON *id_close;
	ST_BUTTON *id_exit;
	HWND hwnd;
	WPARAM wParam;
	LPARAM lParam;
};


/*
 * Structure for dialog Dlg300
 */
struct _Dlg300 {
	ST_STATIC *ctrl302;
	ST_STATIC *ctrl303;
	ST_STATIC *ctrl314;
	ST_STATIC *ctrl308;
	ST_BUTTON *id_showdoc;
	ST_BUTTON *id_email;
	ST_STATIC *ctrl307;
	ST_STATIC *ctrl309;
	ST_STATIC *ctrl310;
	ST_STATIC *ctrl311;
	ST_STATIC *ctrl312;
	ST_STATIC *id_clicksum;
	HWND hwnd;
	WPARAM wParam;
	LPARAM lParam;
};


/*
 * Structure for dialog Dlg500
 */
struct _Dlg500 {
	ST_STATIC *ctrl518;
	ST_EDIT *id_windowtitle;
	ST_STATIC *id_staticbtext;
	ST_EDIT *id_buttontext;
	ST_STATIC *ctrl507;
	ST_RADIOBUTTON *id_neverclick;
	ST_RADIOBUTTON *id_alwaysclick;
	ST_RADIOBUTTON *id_clickwhentext;
	ST_MULEDIT *id_msgtext;
	ST_STATIC *ctrl513;
	ST_COMBOBOX *id_clickoffmethod;
	ST_STATIC *ctrl501;
	ST_RADIOBUTTON *id_nodelay;
	ST_RADIOBUTTON *id_predelay;
	ST_RADIOBUTTON *id_postdelay;
	ST_STATIC *ctrl526;
	ST_EDIT *id_delay;
	ST_STATIC *ctrl524;
	ST_STATIC *ctrl515;
	ST_STATIC *id_clicknumber;
	ST_STATIC *ctrl517;
	ST_BUTTON *id_cancel;
	ST_BUTTON *id_ok;
	ST_CHECKBOX *id_clickonce;
	HWND hwnd;
	WPARAM wParam;
	LPARAM lParam;
};


/*
 * Structure for dialog Dlg200
 */
struct _Dlg200 {
	ST_BUTTON *id_ok;
	ST_STATIC *ctrl201;
	ST_COMBOBOX *id_lang;
	ST_CHECKBOX *id_common;
	HWND hwnd;
	WPARAM wParam;
	LPARAM lParam;
};


/*
 * Structure for dialog Dlg900
 */
struct _Dlg900 {
	ST_STATIC *ctrl902;
	ST_STATIC *ctrl904;
	ST_STATIC *ctrl911;
	ST_STATIC *ctrl912;
	ST_EDIT *id_scantime;
	ST_STATIC *ctrl907;
	ST_STATIC *ctrl908;
	ST_COMBOBOX *id_lang;
	ST_BUTTON *id_ok;
	ST_BUTTON *id_cancel;
	HWND hwnd;
	WPARAM wParam;
	LPARAM lParam;
};


/*
 * Structure for dialog Dlg400
 */
struct _Dlg400 {
	HWND hwnd;
	WPARAM wParam;
	LPARAM lParam;
};


#endif
void SetDlgBkColor(HWND,COLORREF);
BOOL APIENTRY HandleCtlColor(UINT,DWORD);
/*
 * Callbacks for dialog Dlg100
 */
HWND StartDlg100(HWND parent);
int RunDlg100(HWND parent);
void AddGdiObject(HWND,HANDLE);
BOOL WINAPI HandleDefaultMessages(HWND hwnd,UINT msg,WPARAM wParam,DWORD lParam);
/* Control: ID_EXIT*/
long Dlg100Push_button101Selected(ST_BUTTON *,struct _Dlg100 *);
/* Control: ID_CLOSE*/
long Dlg100Push_button103Selected(ST_BUTTON *,struct _Dlg100 *);
/* Control: ID_ADVANCED*/
long Dlg100Push_button113Selected(ST_BUTTON *,struct _Dlg100 *);
/* Control: ID_REMOVE*/
long Dlg100Push_button112Selected(ST_BUTTON *,struct _Dlg100 *);
long Dlg100Init(ST_DIALOGBOX *,struct _Dlg100 *);
void CenterWindow(HWND,int);
HFONT SetDialogFont(HWND hwnd,char *name,int size,int type);
BOOL APIENTRY Dlg100(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
/*
 * Callbacks for dialog Dlg300
 */
HWND StartDlg300(HWND parent);
int RunDlg300(HWND parent);
/* Control: ID_EMAIL*/
long Dlg300Push_button305Selected(ST_BUTTON *,struct _Dlg300 *);
/* Control: ID_SHOWDOC*/
long Dlg300Push_button304Selected(ST_BUTTON *,struct _Dlg300 *);
long Dlg300Init(ST_DIALOGBOX *,struct _Dlg300 *);
BOOL APIENTRY Dlg300(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
/*
 * Callbacks for dialog Dlg500
 */
HWND StartDlg500(HWND parent);
int RunDlg500(HWND parent);
/* Control: ID_OK*/
long Dlg500Push_button504Selected(ST_BUTTON *,struct _Dlg500 *);
/* Control: ID_CANCEL*/
long Dlg500Push_button503Selected(ST_BUTTON *,struct _Dlg500 *);
long Dlg500Init(ST_DIALOGBOX *,struct _Dlg500 *);
BOOL APIENTRY Dlg500(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
/*
 * Callbacks for dialog Dlg200
 */
HWND StartDlg200(HWND parent);
int RunDlg200(HWND parent);
/* Control: ID_OK*/
long Dlg200Push_button504Selected(ST_BUTTON *,struct _Dlg200 *);
long Dlg200Init(ST_DIALOGBOX *,struct _Dlg200 *);
BOOL APIENTRY Dlg200(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
/*
 * Callbacks for dialog Dlg900
 */
HWND StartDlg900(HWND parent);
int RunDlg900(HWND parent);
/* Control: ID_CANCEL*/
long Dlg900Push_button503Selected(ST_BUTTON *,struct _Dlg900 *);
/* Control: ID_OK*/
long Dlg900Push_button504Selected(ST_BUTTON *,struct _Dlg900 *);
long Dlg900Init(ST_DIALOGBOX *,struct _Dlg900 *);
BOOL APIENTRY Dlg900(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
/*
 * Callbacks for dialog Dlg400
 */
HWND StartDlg400(HWND parent);
int RunDlg400(HWND parent);
long Dlg400Init(ST_DIALOGBOX *,struct _Dlg400 *);
BOOL APIENTRY Dlg400(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
extern void *GetDialogArguments(HWND);
extern char *GetDico(int,long);
/*@@ End Prototypes @@*/

#ifndef list_h
#define list_h

#include <gc.h>
#include "windowinfo.h"

//#define DATATYPE WININFO

typedef struct _list {
	union datalast {
		void* data;
		struct _list* last;
	};
	struct _list* next;
} *ITEM,_ITEM,*LIST,list,list_item;

//comparison function needed in search and sort
//for search: return 1 if equal, 0 otherwise
//for sort: return 1 is less or equal, 0 otherwise
typedef int (__stdcall *CMPPROC) (void*, void*);
typedef void (__stdcall *DESTRUCTOR ) (void*);

list& append ( list&, list& );
void push_back ( list&, void* );
void* delete ( list&, ITEM );
ITEM next (ITEM);
ITEM first (list&);
int isLast (ITEM);
ITEM operator[] (list&, int);
ITEM search ( list&, void*, CMPPROC );
void* getData ( ITEM );
void clearList ( list& list );
void freeList ( list& list, DESTRUCTOR dest );
void sort (list&, CMPPROC);
int deleteDuplicates ( list&, CMPPROC, DESTRUCTOR );

#endif //list_h

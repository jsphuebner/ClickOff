/*------------------------------------------------------------------------
Module:        d:\dokumente\c\zeitgui\list.c
Author:        Johannes Hübner, Manuel Caroli
Project:
State:
Creation Date: 27. Juli 2003
Description:   A singly linked list for a type declared as void
in a compiler constant. All functions returning an
item return the previous item, e.g. list[4] returns
list[3]. getData fixes this by returning the next item,
so getData(list[4]) actually returns the 4th(or 5th) item
in the list. This is useful for deleting, because we don't
have to write delete_next(list[3]) for deleting the 4th
item, but we can write delete(list[4]). This is rather
important for the search function because it would
otherwise be impossible to delete all items containing
the number 42 for example. delete_next(search(list,42))
would delete the item that follows.
------------------------------------------------------------------------*/
#include <windows.h>
#include "list.h"

CMPPROC cmp=NULL;

//Initialize a newly declared list
void clearList ( list& list )
{
	list.last=NULL;
	list.next=NULL;
}

//free the memory for all items
void freeList ( list& list, DESTRUCTOR dest )
{
	ITEM help;
	for ( ITEM item=list.next; item; item=help ) {
		help = item->next;
		if (dest) dest (item->data);
		free (item);
	}
	list.next=NULL;
	list.last=NULL;
}

//Append an element to the list
void push_back ( list& list, void* data )
{
	LIST new=malloc(sizeof(list));
	new->data=data;
	new->next = NULL; //points to the former first element
	if ( list.last ) list.last->next = new;
	else list.next = new;
	list.last=new;
}

//this should actually be called delete_next,
//but we were smart enough to shift the item index
void* delete ( list& list, ITEM item )
{
	ITEM help = item->next; //we need this to be able to release memory after we
	void* data = item->next->data; //deleted the element from the list

	item->next = item->next->next;
	if ( !item->next ) list.last = item;
	free ( help );
	return data;
}

list& append ( list& list1, list& list2 )
{
	if ( list2.next ) {
		if ( list1.last )
			list1.last->next = list2.next;
		else
			list1.next = list2.next;
		list1.last=list2.last;
	}

	return list1;
}

ITEM next (ITEM item)
{
	if (item->next && item->next->next ) return item->next;
	return NULL;
}

ITEM first (list& list)
{
	return &list;
}

ITEM last ( list& list )
{
	ITEM i;
	for ( i=&list; i->next && i->next->next; i=i->next ) {}

	return i;
}

BOOL isLast (ITEM item)
{
	return !(item && item->next);
}

ITEM operator[] (list& list, int index)
{
	ITEM help=&list;
	for ( int i=0; i<index; i++ )
		if ( !( help=help->next ) ) return NULL;
	return help;
}

//Nur test
list_item& operator++ (list_item &item)
{
	if (item.next)
		item = *item.next;
	return item;
}

//Nur test
list_item& operator+ (list_item &item, int offset)
{
	ITEM help=&item;
	for ( int i=0; i<offset; i++ )
		if ( !( help=help->next ) ) break;
	return *help;
}

int list_size(list& list)
{
	int i=0;
	for ( ITEM it=first(list); !isLast(it); it=next(it) ) i++;
	return i;
}

ITEM search ( list& list, void* data, CMPPROC cmp )
{
	ITEM help = &list;
	for ( ITEM item=list.next; item; item=item->next ) {
		if ( cmp (item->data, data ) ) return help;
		help = item;
	}
	return NULL;
}

int deleteDuplicates ( list& list, CMPPROC cmp, DESTRUCTOR dest )
{
	int nrdel=0;
	for ( ITEM item=first(list); !isLast(item); item=next(item) ) {
		for ( ITEM item2=next(item),help=item; !isLast(item2); item2=next(item2) ) {
			if ( cmp(getData(item),getData(item2)) ) {
				dest ( delete ( list, item2 ) );
				item2=help;
				nrdel++;
			}
			help = item2;
		}
	}
	return nrdel;
}

void* getData ( ITEM item )
{
	if ( item && item->next )
		return item->next->data;
	return NULL;
}

list& merge(list& a, list& b)
{
	list res;
	ITEM ita=a.next,itb=b.next;
	clearList(res);

	while ( ita && itb )
	{
		if(cmp(ita->data,itb->data)) {
			if(!res.last) res.next=res.last=ita;
			else res.last=res.last->next=ita;
			ita=ita->next;
		}
		else {
			if(!res.last) res.next=res.last=itb;
			else res.last=res.last->next=itb;
			itb=itb->next;
		}
	}
	if (!ita) {
		res.last->next=itb;
		res.last=b.last;
	}
	else {
		res.last->next=ita;
		res.last=a.last;
	}
	return res;
}

list& mergesort(list& l, int size)
{
	list a=l,b=l;
	ITEM it;
	if (l.next==l.last) return l;
	else {
		it=l[size/2];
		b.next=it->next;
		it->next=NULL;
		a.last=it;
		b.last->next=NULL;
		a=mergesort(a,size/2);
		b=mergesort(b,(size+1)/2);
		return merge(a,b);
	}
}

void sort (list &l, CMPPROC cmpproc)
{
	cmp = cmpproc; //set global cmp
	l=mergesort(l,list_size(l));
}

#include "StdAfx.h"
#include "sortedintlist.h"

CSortedIntList::CSortedIntList(void)
{
	list.RemoveAll();
}

CSortedIntList::~CSortedIntList(void)
{
	list.RemoveAll();
}

BOOL CSortedIntList::Add(const CString &dir)
{
	int l = dir.GetLength();
	if (l == 0)
		return FALSE;		//EMPTY STRING
	for (int i=0; i<l; i++) {
		if (! isdigit(dir.GetAt(i)))
			return FALSE;	// NOT A NUMBER CHARACTER
	}
	// IT IS AN INTEGER, SO INSERT IT
	int value = atoi(dir);
	POSITION pos = list.GetHeadPosition();
	while (pos) {
		if (list.GetAt(pos) > value) {
			list.InsertBefore(pos, value);
			return TRUE;
		}
		list.GetNext(pos);
	}
	// i IS BIGGER THAN EVERYTHING IN THE LIST
	list.AddTail(value);
	return TRUE;
}

BOOL CSortedIntList::IsEmpty(void)
{
	return list.IsEmpty();
}

int CSortedIntList::RemoveHead(void)
{
	return list.RemoveHead();
}

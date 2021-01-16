#pragma once

class CSortedIntList
{
public:
	CSortedIntList(void);
	~CSortedIntList(void);
private:
	CList<int, int> list;
public:
	BOOL Add(const CString &dir);
	BOOL IsEmpty(void);
	int RemoveHead(void);
};

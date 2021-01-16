#pragma once

#define DATACLASSREGKEY "software\\NMR Lab\\DataClass"

class CDataClass
{
public:
	CDataClass(void);
	~CDataClass(void);
	void Initialize(void);
	int Count(void);
	LPCTSTR Lookup(LPCTSTR key);
	int Index(LPCTSTR key);
	LPCTSTR GetAt(int i);
private:
	int count;
	int defaultvalue;
	struct SDataClass {
		CString key, value;
	};
	CTypedPtrArray<CPtrArray, struct SDataClass *> pDataClass;
};

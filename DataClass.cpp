// DataClass.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "DataClass.h"


CDataClass::CDataClass()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// CDataClassApp initialization

void CDataClass::Initialize()
{	
	HKEY datalist;
	DWORD len, l;
	RegCreateKey(HKEY_CURRENT_USER, DATACLASSREGKEY, &datalist);
	if (NULL == datalist)
		return;
	RegQueryInfoKey(datalist, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &len, NULL, NULL);
	for  (int i=1; ; i++) {
		CString key, name, keystr, valstr;
		name.Format("Key%d", i);
		l = len + 1;
		long gotkey = RegQueryValueEx(datalist, name, NULL, NULL, (LPBYTE)keystr.GetBuffer(l), &l);
		name.Format("Value%d", i);
		l = len + 1;
		long gotval = RegQueryValueEx(datalist, name, NULL, NULL, (LPBYTE)valstr.GetBuffer(l), &l);
		keystr.ReleaseBuffer();
		valstr.ReleaseBuffer();
		if (ERROR_SUCCESS!=gotkey || ERROR_SUCCESS!=gotval)
			break;
		struct SDataClass *newclass = new struct SDataClass;
		newclass->key = keystr;
		newclass->value = valstr;
		INT_PTR test = pDataClass.Add(newclass);
	}
	count = (int)pDataClass.GetCount();
	if (count <= 0)
		return;
	l = sizeof(int);
	long gotdefault = RegQueryValueEx(datalist, "DefaultValue", NULL, NULL, (LPBYTE)&defaultvalue, &l);
	if (ERROR_SUCCESS != gotdefault)
		defaultvalue = count - 1;
	else {
		defaultvalue--;
		if (0>defaultvalue || defaultvalue>=count)
			defaultvalue = count - 1;
	}
	RegCloseKey(datalist);

	return;
}


CDataClass::~CDataClass()
{
	// TODO: Add your specialized code here and/or call the base class
	for (INT_PTR i=0; i<pDataClass.GetCount(); i++)
		delete pDataClass.GetAt(i);
	pDataClass.RemoveAll();
	return;
}

LPCTSTR CDataClass::Lookup(LPCTSTR key)
{
	if (strlen(key) > 0) {
		for (INT_PTR i=0; i<pDataClass.GetCount(); i++) {
			struct SDataClass *pdc = pDataClass.GetAt(i);
			if (0==_strnicmp(pdc->key, key, strlen(pdc->key)) || 0==_strnicmp(pdc->value, key, strlen(pdc->value)))
				return pdc->value;
		}
	}
	return pDataClass.GetAt(defaultvalue)->value;
}

int CDataClass::Count()
{
	return count;
}

int CDataClass::Index(LPCTSTR key) {
	if (strlen(key) > 0) {
		for (int i=0; i<pDataClass.GetCount(); i++) {
			struct SDataClass *pdc = pDataClass.GetAt(i);
			if (0 == _strnicmp(pdc->key, key, strlen(pdc->key)) || 0==_strnicmp(pdc->value, key, strlen(pdc->value)))
				return i;
		}
	}
	return (int)pDataClass.GetCount();
}

LPCTSTR CDataClass::GetAt(int i)
{
	if (0<=i && i<count)
		return pDataClass.GetAt(i)->value;
	else
		return pDataClass.GetAt(defaultvalue)->value;
}
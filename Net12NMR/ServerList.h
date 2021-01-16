#pragma once

#define NMRSERVERNOTFOUND "Not_Found"
#define NMRSERVERREGKEY "software\\NMR Lab\\Servers"

typedef struct servertag {
	CString name, host, user, pass, home;
} SServer, * LPServer;

class CServerList
{
private:
	CTypedPtrArray <CPtrArray, LPServer> serverarray;
public:
	CServerList(void);
	~CServerList(void);
	bool Initialize(void);
	void EnDecrypt(CString & str);
	LPCTSTR Name(int i);
	LPCTSTR Host(int i);
	LPCTSTR User(int i);
	LPCTSTR Home(int i);
	LPCTSTR Pass(int i);
	INT_PTR GetCount(void);
};
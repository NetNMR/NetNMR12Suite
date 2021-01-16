#pragma once

#define NMRSERVERNOTFOUND "Not_Found"
#define NMRSERVERREGKEY "software\\NMR Lab\\Servers"

class CServerList
{
private:
	int servercount;
	struct NMRSERVER {
		CString name, host, user, pass, home, path;
	} *nmrserver;
public:
	CServerList(void);
	~CServerList(void);
	bool Initialize(void);
	void EnDecrypt(CString & str);
	int Find(LPCTSTR name);
	LPCTSTR Name(int i);
	LPCTSTR Host(int i);
	LPCTSTR User(int i);
	LPCTSTR Home(int i);
	LPCTSTR Path(int i);
	LPCTSTR Pass(int i);
	void SetUser(int i, LPCTSTR newuser);
	void SetPass(int i, LPCTSTR newpass);
	void SetHome(int i, LPCTSTR newhome);
	void SetPath(int i, LPCTSTR newpath);
};
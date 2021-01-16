
#include "stdafx.h"
#include "serverlist.h"

CServerList::CServerList(void)
{
	servercount = 0;
	nmrserver = NULL;
}

CServerList::~CServerList(void)
{
	if (nmrserver)
		delete[] nmrserver;
}

bool CServerList::Initialize(void)
{
	HKEY srvrlist;
	DWORD nhost, hostlen;
	RegCreateKey(HKEY_CURRENT_USER, NMRSERVERREGKEY, &srvrlist);
	if (NULL == srvrlist)
		return true;
	RegQueryInfoKey( srvrlist, NULL, NULL, NULL, &nhost, &hostlen,
		NULL, NULL, NULL, NULL, NULL, NULL);
	if ( nhost == 0 ) {
		RegCloseKey( srvrlist );
		return true;
	}
	hostlen++;
	servercount = nhost;
	nmrserver = new NMRSERVER[nhost];
	if ( nmrserver == NULL ) {
		RegCloseKey( srvrlist );
		return true;
	}
	while ( nhost-- ) {
		RegEnumKey( srvrlist, nhost,
			nmrserver[nhost].name.GetBuffer( hostlen ), hostlen );
		nmrserver[nhost].name.ReleaseBuffer();
		DWORD len;
		HKEY hostkey;
		RegOpenKey( srvrlist, nmrserver[nhost].name, &hostkey );
		RegQueryInfoKey( hostkey, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
			NULL, &len, NULL, NULL );
		DWORD l = len++;
		RegQueryValueEx( hostkey, "host", NULL, NULL,
			(LPBYTE)nmrserver[nhost].host.GetBuffer( l ), &l );
		nmrserver[nhost].host.ReleaseBuffer();
		l = len;
		RegQueryValueEx( hostkey, "home", NULL, NULL,
			(LPBYTE)nmrserver[nhost].home.GetBuffer( l ), &l );
		nmrserver[nhost].home.ReleaseBuffer();
		nmrserver[nhost].path = nmrserver[nhost].home;
		l = len;
		RegQueryValueEx( hostkey, "user", NULL, NULL,
			(LPBYTE)nmrserver[nhost].user.GetBuffer( l ), &l );
		nmrserver[nhost].user.ReleaseBuffer();
		l = len;
		RegQueryValueEx( hostkey, "password", NULL, NULL,
			(LPBYTE)nmrserver[nhost].pass.GetBuffer(l), &l );
		nmrserver[nhost].pass.ReleaseBuffer();
		if (nmrserver[nhost].pass.GetLength())
			EnDecrypt(nmrserver[nhost].pass);
		RegCloseKey( hostkey );
	}
	RegCloseKey( srvrlist );

	return false;
}

void CServerList::EnDecrypt(CString & str)
{
	HCRYPTPROV hCryptProv;
	if (CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
		HCRYPTHASH hHash;
		if (CryptCreateHash(hCryptProv, CALG_MD5, 0, 0, &hHash)) {
			char hostname[64];
			DWORD size=64;
			if (NULL == GetComputerName(hostname, &size))
				strcpy_s(hostname, 64, "SOM88Ost");
			if (CryptHashData(hHash, (BYTE *)hostname, (DWORD)strlen(hostname), 0)) {
				HCRYPTKEY hKey; 
				if (CryptDeriveKey(hCryptProv, CALG_RC4, hHash, 0x00800000, &hKey)) { 
					DWORD dwBufferLen = (DWORD)str.GetLength();
					DWORD dwCount = dwBufferLen;
					PBYTE pbuf = (BYTE *)str.GetBuffer(dwBufferLen+1);
					CryptDecrypt(hKey, 0, 1, 0, pbuf, &dwCount);
					str.ReleaseBuffer();
					if (dwCount != dwBufferLen)
						str.Empty();
				}
				CryptDestroyKey(hKey); 
			}
			CryptDestroyHash(hHash); 
		} 
		CryptReleaseContext(hCryptProv, 0);
	}
}

int CServerList::Find(LPCTSTR name)
{
	int i;
	for (i=0; i<servercount; i++) {
		if (! nmrserver[i].name.Compare(name))
			break;
	}
	if (i>=servercount)
		i = -1;
	return i;
}

LPCTSTR CServerList::Name(int i)
{
	if (i<0 || i>=servercount)
		return NMRSERVERNOTFOUND;
	return nmrserver[i].name;
}

LPCTSTR CServerList::Host(int i){
	if (i<0 || i>=servercount)
		return NMRSERVERNOTFOUND;
	return nmrserver[i].host;
}

LPCTSTR CServerList::User(int i){
	if (i<0 || i>=servercount)
		return NMRSERVERNOTFOUND;
	return nmrserver[i].user;
}

LPCTSTR CServerList::Home(int i)
{
	if (i<0 || i>=servercount)
		return NMRSERVERNOTFOUND;
	return nmrserver[i].home;
}

LPCTSTR CServerList::Path(int i)
{
	if (i<0 || i>=servercount)
		return NMRSERVERNOTFOUND;
	return nmrserver[i].path;
}

LPCTSTR CServerList::Pass(int i)
{
	if (i<0 || i>=servercount)
		return NMRSERVERNOTFOUND;
	return nmrserver[i].pass;
}

void CServerList::SetUser(int i, LPCTSTR newuser)
{
	CString str = newuser;
	str = str.Trim();
	if (str.IsEmpty() || i<0 || i>=servercount)
		return;
	nmrserver[i].user = str;
}

void CServerList::SetPass(int i, LPCTSTR newpass)
{
	CString str = newpass;
	str = str.Trim();
	if (str.IsEmpty() || i<0 || i>=servercount)
		return;
	nmrserver[i].pass = str;
}

void CServerList::SetHome(int i, LPCTSTR newhome)
{
	CString str = newhome;
	str = str.Trim();
	if (str.IsEmpty() || i<0 || i>=servercount)
		return;
	nmrserver[i].path = nmrserver[i].home = str;
}

void CServerList::SetPath(int i, LPCTSTR newpath)
{
	CString str = newpath;
	str = str.Trim();
	if (str.IsEmpty() || i<0 || i>=servercount)
		return;
	nmrserver[i].path = str;
}

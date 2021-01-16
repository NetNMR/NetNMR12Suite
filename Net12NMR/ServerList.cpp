
#include "stdafx.h"
#include "serverlist.h"

CServerList::CServerList(void)
{
}

CServerList::~CServerList(void)
{
	for (int i=0; i<serverarray.GetCount(); i++) {
		if (serverarray[i])
			delete serverarray[i];
	}
	serverarray.RemoveAll();
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
	while ( nhost-- ) {
		LPServer newserver = new SServer;
		if (newserver) {
			RegEnumKey( srvrlist, nhost,
				newserver->name.GetBuffer( hostlen ), hostlen );
			newserver->name.ReleaseBuffer();
			DWORD len;
			HKEY hostkey;
			RegOpenKey( srvrlist, newserver->name, &hostkey );
			RegQueryInfoKey( hostkey, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
				NULL, &len, NULL, NULL );
			DWORD l = len++;
			RegQueryValueEx( hostkey, "host", NULL, NULL,
				(LPBYTE)newserver->host.GetBuffer( l ), &l );
			newserver->host.ReleaseBuffer();
			l = len;
			RegQueryValueEx( hostkey, "home", NULL, NULL,
				(LPBYTE)newserver->home.GetBuffer( l ), &l );
			newserver->home.ReleaseBuffer();
			l = len;
			RegQueryValueEx( hostkey, "user", NULL, NULL,
				(LPBYTE)newserver->user.GetBuffer( l ), &l );
			newserver->user.ReleaseBuffer();
			l = len;
			RegQueryValueEx( hostkey, "password", NULL, NULL,
				(LPBYTE)newserver->pass.GetBuffer(l), &l );
			newserver->pass.ReleaseBuffer();
			if (newserver->pass.GetLength())
				EnDecrypt(newserver->pass);
			RegCloseKey( hostkey );
			int i=0;
			for (; i<serverarray.GetCount(); i++) {
				if (0 > newserver->name.CompareNoCase(serverarray[i]->name))
					break;
			}
			serverarray.InsertAt(i, newserver);
		}
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

LPCTSTR CServerList::Name(int i){
	if (0<=i && i<serverarray.GetCount())
		return serverarray[i]->name;
	else
		return NMRSERVERNOTFOUND;
}

LPCTSTR CServerList::Host(int i){
	if (0<=i && i<serverarray.GetCount())
		return serverarray[i]->host;
	else
		return NMRSERVERNOTFOUND;
}

LPCTSTR CServerList::User(int i){
	if (0<=i && i<serverarray.GetCount())
		return serverarray[i]->user;
	else
		return NMRSERVERNOTFOUND;
}

LPCTSTR CServerList::Home(int i)
{
	if (0<=i && i<serverarray.GetCount())
		return serverarray[i]->home;
	else
		return NMRSERVERNOTFOUND;
}

LPCTSTR CServerList::Pass(int i)
{
	if (0<=i && i<serverarray.GetCount())
		return serverarray[i]->pass;
	else
		return NMRSERVERNOTFOUND;
}

INT_PTR CServerList::GetCount()
{
	return serverarray.GetCount();
}
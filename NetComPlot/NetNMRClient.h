#pragma once



// CNetNMRServer command target

class CNetNMRClient : public CSocket
{
public:
	CNetNMRClient();
	virtual ~CNetNMRClient();
	BOOL bConnected;
public:
	BOOL MakeConnection();
	void GetSockString(CString &str);
	void ReadSockString(void *lpBuf, int nCount);
	void SendString(LPCTSTR str);
	void ByteReverseData(int *fBuf, int nCount);
	void BreakConnection(void);
	void ReadSocket(void * lpBuf, int count);
};



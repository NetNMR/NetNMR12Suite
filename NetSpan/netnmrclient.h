#pragma once

// CNetNMRClient command target

#include "..\NMRDataTypes.h"

class CNetNMRClient : public CSocket
{
public:
	CNetNMRClient();
	virtual ~CNetNMRClient();
	BOOL bConnected;
	BOOL MakeConnection();
	void GetSockString(CString &str);
	void ReadSockString(void *lpBuf, int nCount);
	void SendString(LPCTSTR str);
	void ByteReverseData(vector<complex<float>> &data, int nCount);
	void BreakConnection(void);
	void ReadSocket(void * lpBuf, int count);
	void ByteReverseHeader(HD *pHD);
};



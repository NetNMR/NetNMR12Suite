#pragma once

/////////////////////////////////////////////////////////////////////////////
// CNMRServer command target

class CNMRServer
{
// Attributes
public:
	bool IsConnected();
private:
	bool connected;

// Operations
public:
	CNMRServer();
	virtual ~CNMRServer();

// Overrides
public:
	void SendString(CString s);
	void ReadSocket(void *lpBuf, int nCount);
	void ByteReverseData(vector<complex<float>>&);
	void ByteReverseHeader(HD *pHD);
	void Close();
	char *GetSockString();
	bool MakeConnection(LPCTSTR name);

// Implementation
protected:
	CSocket sock;
};

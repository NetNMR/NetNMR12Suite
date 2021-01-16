#pragma once

// CServerSocket command target

class CServerSocket : public CSocket
{
public:
	CServerSocket();
	virtual ~CServerSocket();
	bool Open(int server_index);
	CString GetSockString(void);
	void SendCommand(char c, LPCTSTR param=NULL);
	int ReadSocket(void * lpbuf, int count);
};



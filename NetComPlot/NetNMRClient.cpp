// NetNMRServer.cpp : implementation file
//

#include "stdafx.h"
#include "NetComPlot.h"
#include "NetNMRClient.h"


// CNetNMRServer

CNetNMRClient::CNetNMRClient()
: bConnected(FALSE)
{
}

CNetNMRClient::~CNetNMRClient()
{
}


// CNetNMRClient member functions

BOOL CNetNMRClient::MakeConnection()
{
	CWaitCursor wait;
	CString buf;

	// Create the CSocket
	if (Create()) {
		// Connect to Netfid Server
		while (IsBlocking())
			;
		if (Connect(theApp.Server.Host(theApp.nserv), 1666)) {
			while (IsBlocking())
				;
			// Check initialization response
			GetSockString(buf);
			if (buf.Compare("Tefid Server") && buf.Compare("NetNMR Server")) {
				Close();
				buf.Format("'%s' unexpected nmrserver response!", buf);
				AfxMessageBox(buf);
			} else {
				// Send login information ...
				buf.Format("umswinuser,%s\n", theApp.Server.User(theApp.nserv));
				SendString(buf);
				// ... and check for ok
				GetSockString(buf);
				if (buf.Compare("ok")) {
					if (buf.Compare("Password?")) {
						Close();
						AfxMessageBox( "User not accepted!" );
					} else {
						buf.Format("%s\n", theApp.Server.Pass(theApp.nserv));
						SendString(buf);
						GetSockString(buf);
						if (buf.Compare("ok")) {
							Close();
							AfxMessageBox("Password not accepted!");
						} else
							bConnected = TRUE;
					}
				} else
					bConnected = TRUE; // Ah, the life is good...
			}
		} else {
			Close();
			buf.Format("Could not connect to %s!", theApp.Server.Name(theApp.nserv));
			AfxMessageBox(buf);
		}
	} else
		AfxMessageBox("Cannot create Winsock!");
	return bConnected;
}

void CNetNMRClient::GetSockString(CString &str)
{
	LPTSTR p = str.GetBuffer(256);
	char c = NULL;
	while ( c != '\n' ) {
		while ( 1 != Receive( &c, 1 ) );
		*p++ = c;
	}
	*--p = NULL;
	str.ReleaseBuffer();
}

void CNetNMRClient::ReadSockString(void * lpBuf, int nCount)
{
	CWaitCursor wait;
	int nRead;
	int nLeft = nCount;
	PBYTE pBuf = (PBYTE)lpBuf;

	while(nLeft > 0) {
		nRead = Receive(pBuf, nLeft);
		if (nRead == SOCKET_ERROR) {
			CString str;
			str.Format("Socket Error: %d", GetLastError());
			AfxMessageBox(str, MB_OK|MB_ICONEXCLAMATION);
		}
		ASSERT(nRead!=0);
		nLeft -= nRead;
		pBuf += nRead;
	}
}

void CNetNMRClient::SendString(LPCTSTR str)
{
	int length = (int)strlen(str);
	int bytes_sent = 0;
	while ( bytes_sent < length ) {
		int this_write = Send(str, length);
		bytes_sent += this_write;
		str += this_write;
	}
}

void CNetNMRClient::ByteReverseData(int *fBuf, int nCount)
{
	union uBuf {
		int i;
		u_long u;
		//char c[4];
	};
	for (int i=0; i<nCount; i++) {
		uBuf u;
		u.i = fBuf[i];
		u.u = ntohl(u.u);
		//char ch = u.c[0];
		//u.c[0] = u.c[3];
		//u.c[3] = ch;
		//ch = u.c[1];
		//u.c[1] = u.c[2];
		//u.c[2] = ch;
		fBuf[i] = u.i;
	}		
}


void CNetNMRClient::BreakConnection(void)
{
	if (bConnected) {
		SendString("x\n");
		ShutDown(2);
		Close();
		bConnected = FALSE;
		theApp.nserv = -1;
	}
}

void CNetNMRClient::ReadSocket(void *lpBuf, int count)
{
	int nLeft = count;
	PBYTE pBuf = (PBYTE)lpBuf;

	while(nLeft > 0) {
		int nRead = Receive(pBuf, nLeft);
		//if (nRead == 0) return;
		nLeft -= nRead;
		pBuf += nRead;
	}
}

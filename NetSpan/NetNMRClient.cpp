// NetNMRClient.cpp : implementation file
//

#include "stdafx.h"
#include "NetSpan.h"
#include "NetNMRClient.h"


// CNetNMRClient

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

void CNetNMRClient::ByteReverseData(vector<complex<float>> &data, int nCount)
{
	union uBuf {
		float f;
		u_long u;
	};
	for (int i=0; i<nCount; i++) {
		uBuf u;
		u.f = data[i].real();
		u.u = ntohl(u.u);
		data[i] = complex<float>(u.f, data[i].imag());
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
void CNetNMRClient::ByteReverseHeader(HD * pHD)
{
	// Swap bytes
	// First: Swap all 32-bit values (int and float)
	char *ch = (char *)pHD;
	for ( int i=0; i<sizeof(HD); i+=4 ) {
		// Don't reverse character strings
		switch (i) {
		case 508:	// f1
		case 512:
		case 600:	// f2
		case 604:
		case 608:
		case 696:	// f3
		case 700:
		case 704:
			break;		// do nothing
		default:
			{
				char c = ch[i];
				ch[i] = ch[i+3];
				ch[i+3] = c;
				c = ch[i+1];
				ch[i+1] = ch[i+2];
				ch[i+2] = c;
			}
			break;
		}
	}
	// Then: take care of all doubles.
	int *in = (int *)pHD;
	for (int i=0; i<144; i+=8 ) {		// at the begining
		int j = in[i/4];
		in[i/4] = in[i/4+1];
		in[i/4+1] = j;
	}
	for (int i=464; i<488; i+=8 ) {	// f1 struct
		int j = in[i/4];
		in[i/4] = in[i/4+1];
		in[i/4+1] = j;
	}
	for (int i=528; i<552; i+=8 ) {	// f2 struct
		int j = in[i/4];
		in[i/4] = in[i/4+1];
		in[i/4+1] = j;
	}
	for (int i=624; i<648; i+=8 ) {	// f3 struct
		int j = in[i/4];
		in[i/4] = in[i/4+1];
		in[i/4+1] = j;
	}
	for (int i=776; i<800; i+=8 ) {	// lock struct
		int j = in[i/4];
		in[i/4] = in[i/4+1];
		in[i/4+1] = j;
	}
}

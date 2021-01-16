// NMRServer.cpp : implementation file
//

#include "stdafx.h"
#include "..\NMRDataTypes.h"
#include "Spectrum.h"
#include "netplot.h"
#include "NMRServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNMRServer

CNMRServer::CNMRServer()
{
	connected = false;
}

CNMRServer::~CNMRServer()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CNMRServer, CSocket)
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CNMRServer member functions

bool CNMRServer::MakeConnection(LPCTSTR name)
{
	// Change to the Wait Cursor
	CWaitCursor wait;

	CString buf;

	// Create the CSocket
	if ( ! sock.Create() ) {
		AfxMessageBox( "Cannot create Winsock!" );
		return false;
	}

	// Connect to Netfid Server
	int nserv = theApp.Server.Find(name);
	while ( sock.IsBlocking() )
		;
	if ( sock.Connect( theApp.Server.Host(nserv), 1666 ) ) {
		while ( sock.IsBlocking() )
			;
		// Check initialization response
		char *p = GetSockString();
		if (strcmp(p, "Tefid Server") && strcmp(p, "NetNMR Server")) {
			buf.Format( "'%s' unexpected nmrserver response!", p );
			Close();
			AfxMessageBox( buf );
			return false;
		}

		// Send login information ...
		buf.Format( "umswinuser,%s\n", theApp.Server.User(nserv) );
		//Send( buf, buf.GetLength() );
		SendString( buf );

		// ... and check for ok
		p = GetSockString();
		if (strcmp(p, "ok")) {
			if (strcmp(p, "Password?")) {
				Close();
				AfxMessageBox( "Login not accepted!" );
				return false;
			} else {
				buf.Format("%s\n", theApp.Server.Pass(nserv));
				SendString(buf);
				if (strcmp(GetSockString(), "ok")) {
					Close();
					AfxMessageBox("Password not accepted!");
					return false;
				}
			}
		}
		connected = true;
		return true;
	} else {
		sock.Close();
		buf.Format( "Could not connect to %s!", theApp.Server.Name(nserv) );
		AfxMessageBox( buf );
		return false;
	}
}

char * CNMRServer::GetSockString()
{
	static char str[256];
	char *p = str;
	char c = NULL;
	while ( c != '\n' ) {
		while ( 1 != sock.Receive( &c, 1 ) );
		*p++ = c;
	}
	*--p = NULL;
	return str;
}

void CNMRServer::Close()
{
	CString x( "x\n" );
	SendString( x );
	sock.Close();
	connected = false;
}

bool CNMRServer::IsConnected()
{
	return connected;
}

void CNMRServer::ByteReverseHeader(HD *pHD)
{
	CWaitCursor wait;
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

void CNMRServer::ByteReverseData(vector<complex<float>> &dt)
{
	CWaitCursor wait;
	char *ch = (char *)dt.data();
	for (UINT i=0; i<2*dt.size(); i++) {
		char c = ch[i*4];
		ch[i*4] = ch[i*4+3];
		ch[i*4+3] = c;
		c = ch[i*4+1];
		ch[i*4+1] = ch[i*4+2];
		ch[i*4+2] = c;
	}
}

void CNMRServer::ReadSocket(void *lpBuf, int nCount)
{
	CWaitCursor wait;
	int nRead;
	int nLeft = nCount;
	PBYTE pBuf = (PBYTE)lpBuf;

	while(nLeft > 0) {
		nRead = sock.Receive(pBuf, nLeft);
		if ( nRead == SOCKET_ERROR ) {
			CString str;
			str.Format( "Socket Error: %d", GetLastError() );
			AfxMessageBox( str, MB_OK|MB_ICONEXCLAMATION );
		}
		ASSERT( nRead!=0 );
		nLeft -= nRead;
		pBuf += nRead;
	}
}

void CNMRServer::SendString(CString s)
{
	int length = s.GetLength();
	LPTSTR pstr = s.LockBuffer();
	int bytes_sent = 0;
	while ( bytes_sent < length ) {
		int this_write = sock.Send( pstr, length );
		bytes_sent += this_write;
		pstr += this_write;
	}
	s.UnlockBuffer();
}

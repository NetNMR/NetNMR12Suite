// ServerSocket.cpp : implementation file
//

#include "stdafx.h"
#include "Net12NMR.h"
#include "ServerSocket.h"


// CServerSocket

CServerSocket::CServerSocket()
{
}

CServerSocket::~CServerSocket()
{
}


// CServerSocket member functions


bool CServerSocket::Open(int server_index)
{
	if (! Create())
		return false;
	if (Connect(theApp.servers.Host(server_index), 1666)) {
		CString sockstr = GetSockString();
		if (sockstr.Compare("Tefid Server")) {
			// "Tefid Server" not returned.
			if (sockstr.Compare("NetNMR Server")) {
				// "NetNMR Server" not returned.
				CString err;
				err.Format("Unexpected nmrserver response: '%s'", sockstr);
				AfxMessageBox(err);
				return false;
			} else {
				// "NetNMR Server" returned. Is there a password?
				if (0 == strlen(theApp.servers.Pass(server_index))) {
					CString err;
					err.Format("No password for %s has been specified. Use the Connection Mgr.", theApp.servers.Name(server_index));
					AfxMessageBox(err);
					return false;
				}
			}
		}
		// Set login cmd...
		sockstr.Format("umswinuser,%s\n", theApp.servers.User(server_index));
		Send(sockstr, sockstr.GetLength());
		sockstr = GetSockString();
		if (sockstr.Compare("ok")) {
			// Not "ok"!
			if (sockstr.Compare("Password?")) {
				// Not "Password?"
				AfxMessageBox("User not accepted!");
				return false;
			} else {
				// "Password?" returned.
				sockstr.Format("%s\n", theApp.servers.Pass(server_index));
				Send(sockstr, sockstr.GetLength());
				sockstr = GetSockString();
				if (sockstr.Compare("ok")) {
					AfxMessageBox("Password not accepted!");
					return false;
				} else
					return true;
			}
		} else
			return true;
	}
	return false;
}

CString CServerSocket::GetSockString(void)
{
	CString str;
	char c = NULL;
	while ( c != '\n' ) {
		int i;
		do {
			i = Receive( &c, 1 );
			if (i < 0)
				return "";
		} while (i < 1);
		str += c;
	}
	return str.Trim();
}

void CServerSocket::SendCommand(char command, LPCTSTR param)
{
	CString cmd;
	cmd += command;
	if (param)
		cmd += param;
	cmd += '\n';
	Send(cmd, cmd.GetLength());
}

int CServerSocket::ReadSocket(void * lpbuf, int count)
{
	int nRead;
	int nLeft = count;
	PBYTE pBuf = (PBYTE)lpbuf;

	while(nLeft > 0) {
		nRead = Receive(pBuf, nLeft);
		if (nRead < 0)
			return nRead;
		nLeft -= nRead;
		pBuf += nRead;
	}
	return nRead;
}

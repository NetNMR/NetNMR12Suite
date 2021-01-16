// NetComPlot.h : main header file for the NetComPlot application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include "ServerList.h"
#include "DataClass.h"

struct COLOR {
	CString name;
	COLORREF value;
};

// CNetComPlotApp:
// See NetComPlot.cpp for the implementation of this class
//

class CNetComPlotApp : public CWinApp
{
public:
	CNetComPlotApp();
	CServerList Server;
	CDataClass DataClass;
// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
public:
	CSize csPageSize;
	void GetPageSize(short dmPageSize);
	virtual int ExitInstance();
	int nserv;
	int nColors;
	COLOR *arrColors;
private:
	void InitColors(void);
public:
	afx_msg void OnFilePrintSetup();
	void Message(LPCTSTR str);
};

extern CNetComPlotApp theApp;
// NetSpan.h : main header file for the NetSpan application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include "ServerList.h"

// CNetSpanApp:
// See NetSpan.cpp for the implementation of this class
//

class CLine;

struct COLOR {
	CString name;
	COLORREF value;
};

enum RegistryAction {ReadInt, WriteInt};
enum RegistryVariable {BkgrColor, SpecColor, CalcColor, SingColor, DiffColor, LablColor, ZoomColor, CrsrColor};

class CNetSpanApp : public CWinApp
{
public:
	CNetSpanApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
private:
	void InitColors(void);
public:
	CLine *pCurrentLine;
	UINT lineindex;
	int nColors;
	CServerList Server;
	int nserv;
	COLOR *arrColors;
	CSize csPageSize;
	virtual int ExitInstance();
	afx_msg void OnFilePrintSetup();
	afx_msg void OnFileOpen();
	void SetPaneText(int pane, LPCTSTR text);
	int RegistryInt(RegistryAction action, RegistryVariable name, int value);
	afx_msg void OnEditColors();
	afx_msg void OnUpdateFilePrint(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFileSave(CCmdUI *pCmdUI);
};

extern CNetSpanApp theApp;
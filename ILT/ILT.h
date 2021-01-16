// ILT.h : main header file for the ILT application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#define NCOLORS 18

typedef enum kerneltag {typeDosy=0, typeT2=1, typeT1=2} EKernelType;

//class CDecay;

// CILTApp:
// See ILT.cpp for the implementation of this class
//

class CILTApp : public CWinApp
{
public:
	CILTApp();
	POSITION pDecay;
	COLORREF Colors[NCOLORS];

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	void SetDefaultColors(COLORREF *colors);
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
public:
	void SetPaneText(int pane, LPCTSTR text);
	virtual int ExitInstance();
	afx_msg void OnFileOpen();
private:
	void ReadRegistryColors(void);
public:
	afx_msg void OnEditColors();
public:
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
};

extern CILTApp theApp;
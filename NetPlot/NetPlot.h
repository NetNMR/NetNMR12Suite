// NetPlot.h : main header file for the NetPlot application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include "ServerList.h"
#include "DataClass.h"
#include "atltypes.h"

struct COLOR {
	CString name;
	COLORREF value;
};

enum RegistryAction {ReadInt, WriteInt};
enum RegistryVariable {BackgroundColor, AxisColor, SpectraColor, IntegralColor, TitleColor, 
	LabelsColor, MainCursorColor, ZoomCursorColor, BaselineColor,
	TitleFontSize, LabelFontSize, AxisFontSize};

// CNetPlotApp:
// See NetPlot.cpp for the implementation of this class
//

class CNetPlotApp : public CWinApp
{
public:
	CNetPlotApp();
	CServerList Server;
	CDataClass DataClass;
	int nColors;
	COLOR *arrColors;
	int RegistryInt(RegistryAction action, RegistryVariable name, int value);
	float gphA, gphB;
	vector<bool> spare;

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	void InitColors(void);
	virtual int ExitInstance();
	afx_msg void OnEditParameters();
	afx_msg void OnUpdateEditParameters(CCmdUI *pCmdUI);
	afx_msg void OnEditColors();
	afx_msg void OnEditLabels();
	afx_msg void OnUpdateEditLabels(CCmdUI *pCmdUI);
	afx_msg void OnFileAdd();
	afx_msg void OnUpdateFileAdd(CCmdUI *pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
	afx_msg void OnViewLandscape();
	afx_msg void OnUpdateViewLandscape(CCmdUI *pCmdUI);
	afx_msg void OnViewPort();
	afx_msg void OnUpdateViewPort(CCmdUI *pCmdUI);
	afx_msg void OnFileMysaveas();
	afx_msg void OnUpdateFileMysaveas(CCmdUI *pCmdUI);
};

extern CNetPlotApp theApp;
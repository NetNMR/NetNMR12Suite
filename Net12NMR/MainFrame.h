// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://msdn.microsoft.com/officeui.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// MainFrame.h : interface of the CMainFrame class
//

#pragma once
#include "FileView.h"
#include "OutputWnd.h"
#include "PropertiesWnd.h"
#include "MyRibbonBar.h"

class CNet12NMRDoc;

class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:
	CPoint midscreen;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	void MessageToBar(LPCTSTR msg, UINT id);

protected:  // control bar embedded members
	//CMFCRibbonBar     m_wndRibbonBar;
	//CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages m_PanelImages;
	CMFCRibbonStatusBar m_wndStatusBar;
	CMFCRibbonStatusBarPane *m_wndXHairPane, *m_wndDiffPane;
	CFileView m_wndFileView;
public:
	COutputWnd m_wndOutput;
	CMyRibbonBar m_wndRibbonBar;

protected:
	CPropertiesWnd    m_wndProperties;
	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowManager();
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);

public:
	afx_msg void OnViewFileExplorer();
	afx_msg void OnUpdateViewFileExplorer(CCmdUI *pCmdUI);
	afx_msg void OnViewProperties();
	afx_msg void OnUpdateViewProperties(CCmdUI *pCmdUI);
	afx_msg void OnViewOutput();
	afx_msg void OnUpdateViewOutput(CCmdUI *pCmdUI);
};



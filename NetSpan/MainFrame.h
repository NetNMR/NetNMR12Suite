// MainFrame.h : interface of the CMainFrame class
//


#pragma once

#include "TabbedDlg.h"

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	CPropertySheet *propSheet;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
	CTabbedDlg *tabDlg;
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
public:
	void SetText(int pane, LPCTSTR text);
	void ShowTabbedDialog(int page);
	afx_msg void OnEditLines();
	afx_msg void OnEditSpectra();
	afx_msg void OnUpdateEditLines(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditSpectra(CCmdUI *pCmdUI);
	afx_msg void OnEditFit();
	afx_msg void OnUpdateEditFit(CCmdUI *pCmdUI);
	afx_msg void OnEditSpecfrequency();
	void RefillSpecList(void);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};



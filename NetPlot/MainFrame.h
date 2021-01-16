// MainFrame.h : interface of the CMainFrame class
//


#pragma once
class CProcessWnd;

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:
	int windowCount;
	CPoint midscreen;
	CProcessWnd *dialogFrame;

// Operations
public:
	void HideProcessSheet(void);

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
	void MessagetoBar(LPCTSTR msg);

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnProcess(UINT uID);
	afx_msg void OnUpdateProcess( CCmdUI *pCmdUI );
	afx_msg void OnUpdateCursorDifference(CCmdUI *pCmdUI);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
};



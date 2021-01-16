#pragma once


// CProcessWnd frame

class CProcSheet;

class CProcessWnd : public CMiniFrameWnd
{
	friend class CMainFrame;
	DECLARE_DYNCREATE(CProcessWnd)
protected:
	CProcessWnd();           // protected constructor used by dynamic creation
	virtual ~CProcessWnd();

protected:
	DECLARE_MESSAGE_MAP()
public:
	void UpdateContent();
	CProcSheet *procSheet;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
};



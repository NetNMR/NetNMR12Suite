#pragma once
#include "afxcmn.h"
#include "afxwin.h"

// CFitTextDlg dialog

class CMainFrame;
class CNetSpanDoc;
class CNetSpanView;

class CFitTextDlg : public CDialog
{
	DECLARE_DYNAMIC(CFitTextDlg)

public:
	CFitTextDlg(bool bShowErr, CWnd* pParent = NULL);   // standard constructor
	virtual ~CFitTextDlg();

// Dialog Data
	enum { IDD = IDD_FITTXTDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CRichEditCtrl m_fitRichEditCtrl;
	afx_msg void OnBnClickedCopybutton();
	afx_msg void OnBnClickedPrintbutton();
	afx_msg void OnBnClickedCurvefitbutton();
	afx_msg void OnBnClickedCurveiltbutton();
private:
	bool m_boolShowErrors;
	CFont m_richFont;
	CMainFrame *pMain;
	CNetSpanDoc *pDoc;
	CNetSpanView *pView;
	void FillRichText(void);
};

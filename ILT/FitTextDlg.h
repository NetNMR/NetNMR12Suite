#pragma once
#include "afxcmn.h"

// CFitTextDlg dialog

class CILTDoc;
class CDecay;

class CFitTextDlg : public CDialog
{
	DECLARE_DYNAMIC(CFitTextDlg)

public:
	CFitTextDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFitTextDlg();

// Dialog Data
	enum { IDD = IDD_RICHTEXTDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	double chisq;
	afx_msg void OnBnClickedPrinttextbutton();
	afx_msg void OnBnClickedCopytextbutton();
	CRichEditCtrl m_textRichEditCtrl;
	virtual BOOL OnInitDialog();
private:
	CILTDoc *pDoc;
	CFont m_richFont;
	double constant;
	void fillrichtext(void);
	void getdecaytext(CDecay *pDecay, CString &str);
	void gettermstext(CDecay *pDecay, CString &str);
public:
	BOOL m_boolTermsOnly;
	BOOL m_boolAllExpon;
	afx_msg void OnBnClickedTermsonlycheck();
};

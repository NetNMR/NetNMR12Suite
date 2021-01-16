#pragma once


// CSetIntegralDlg dialog

class CSetIntegralDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetIntegralDlg)

public:
	CSetIntegralDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSetIntegralDlg();

// Dialog Data
	enum { IDD = IDD_SETINTEGRALDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strIntegralPrompt;
	CEdit m_editIntegralValue;
	virtual BOOL OnInitDialog();
	CString m_strIntegralValue;
};

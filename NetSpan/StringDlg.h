#pragma once
#include "afxwin.h"


// CStringDlg dialog

class CStringDlg : public CDialog
{
	DECLARE_DYNAMIC(CStringDlg)

public:
	CStringDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CStringDlg();

// Dialog Data
	enum { IDD = IDD_STRINGDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_string;
	virtual BOOL OnInitDialog();
	CEdit m_stringEdit;
protected:
	virtual void OnOK();
};

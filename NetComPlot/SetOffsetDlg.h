#pragma once
#include "afxwin.h"


// CSetOffsetDlg dialog

class CSetOffsetDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetOffsetDlg)

public:
	CSetOffsetDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSetOffsetDlg();

// Dialog Data
	enum { IDD = IDD_SETOFFSETDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strHUnits;
	CString m_strVUnits;
	virtual BOOL OnInitDialog();
	CString m_strHValue;
	CString m_strVValue;
};

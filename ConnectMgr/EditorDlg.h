#pragma once
#include "afxwin.h"


// CEditorDlg dialog

class CEditorDlg : public CDialog
{
	DECLARE_DYNAMIC(CEditorDlg)

public:
	CEditorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEditorDlg();

// Dialog Data
	enum { IDD = IDD_EDITORDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_userStr;
	CString m_homeStr;
	CString m_pw1Str;
	CString m_pw2Str;
protected:
	virtual void OnOK();
public:
	CString m_staticServerName;
};

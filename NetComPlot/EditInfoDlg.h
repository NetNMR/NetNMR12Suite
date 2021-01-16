#pragma once


// CEditInfoDlg dialog

class CEditInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CEditInfoDlg)

public:
	CEditInfoDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEditInfoDlg();

// Dialog Data
	enum { IDD = IDD_INFODLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strInfo;
};

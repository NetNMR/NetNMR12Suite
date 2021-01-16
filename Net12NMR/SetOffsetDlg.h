#pragma once


// CSetOffsetDlg dialog

class CSetOffsetDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSetOffsetDlg)

public:
	CSetOffsetDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSetOffsetDlg();

// Dialog Data
	enum { IDD = IDD_SETOFFSETDIALOG };
	CEdit	m_conValue;
	int		m_Units;
	CString	m_Value;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
};

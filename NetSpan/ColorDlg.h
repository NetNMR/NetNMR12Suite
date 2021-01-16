#pragma once
#include "afxwin.h"


// CColorDlg dialog

class CColorDlg : public CDialog
{
	DECLARE_DYNAMIC(CColorDlg)

public:
	CColorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CColorDlg();

// Dialog Data
	enum { IDD = IDD_COLORDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CComboBox m_bkgrCbox;
	CComboBox m_specCbox;
	CComboBox m_calcCbox;
	CComboBox m_singCbox;
	CComboBox m_diffCbox;
	CComboBox m_lablCbox;
protected:
	virtual void OnOK();
};

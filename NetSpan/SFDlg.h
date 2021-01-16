#pragma once

// CSFDlg dialog

class CSFDlg : public CDialog
{
	DECLARE_DYNAMIC(CSFDlg)

public:
	CSFDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSFDlg();

// Dialog Data
	enum { IDD = IDD_SFDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	double m_sfDouble;
	virtual BOOL OnInitDialog();
};

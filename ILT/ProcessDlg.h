#pragma once


// CProcessDlg dialog

class CProcessDlg : public CDialog
{
	DECLARE_DYNAMIC(CProcessDlg)

public:
	CProcessDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProcessDlg();

// Dialog Data
	enum { IDD = IDD_ILTDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	double m_dblILTFrom;
	double m_dblILTTo;
	int m_intILTCount;
	int m_intSIRTCount;
	double m_dblILTalpha;
	double m_dblSIRTTolerance;
	BOOL m_boolGlobalCheck;
	BOOL m_boolSimplifyCheck;
	BOOL m_boolOptimizeCheck;
	int m_intMethod;
	int m_intConfidence;
	afx_msg void OnBnClickedSimplifycheck();
	afx_msg void OnBnClickedOptimizecheck();
	afx_msg void OnBnClickedIlthelpbutton();
	afx_msg void OnBnClickedMethod(UINT nID);
	afx_msg void OnBnClickedConfidence(UINT nID);
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};

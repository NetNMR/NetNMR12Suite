#pragma once
#include "afxwin.h"


// CKernelDlg dialog

class CKernelDlg : public CDialog
{
	DECLARE_DYNAMIC(CKernelDlg)

public:
	CKernelDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CKernelDlg();

// Dialog Data
	enum { IDD = IDD_KERNELDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	bool isrealdata;
	CString m_strSolvent;
	EKernelType kernelType;
	afx_msg void OnBnClickedkernelradio();
	virtual BOOL OnInitDialog();
	double m_dblDACtoG;
	double m_dblTimeCubed;
	double m_dblGamma;
	double m_dblViscosity;
	double m_dblTemperature;
	afx_msg void OnBnClickedKernelhelpbutton();
	afx_msg void OnCbnSelchangeSolventcombo();
	CComboBox m_comboSolvent;
};

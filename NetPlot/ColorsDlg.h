#pragma once
#include "afxwin.h"


// CColorsDlg dialog

class CColorsDlg : public CDialog
{
	DECLARE_DYNAMIC(CColorsDlg)

public:
	CColorsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CColorsDlg();

// Dialog Data
	enum { IDD = IDD_COLORDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_cboxCSpectrum;
	CComboBox m_cboxSColor;
	CComboBox m_cboxBkgrndColor;
	CComboBox m_cboxIntegralColor;
	CComboBox m_cboxAxisColor;
	CComboBox m_cboxTitleColor;
	CComboBox m_cboxMainCursorColor;
	CComboBox m_cboxZoomCursorColor;
	CComboBox m_cboxBaselineColor;
	CButton m_buttonAllSpectra;
	afx_msg void OnBnClickedBlkonwhtbutton();
	afx_msg void OnBnClickedWhtonblkbutton();
	afx_msg void OnBnClickedOk();
	afx_msg void OnCbnSelchangeCspectrumcombo();
	afx_msg void OnCbnSelchangeScolorcombo();
	afx_msg void OnBnClickedAllcolorcheck();
	virtual BOOL OnInitDialog();
};

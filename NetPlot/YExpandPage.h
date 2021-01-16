#pragma once
#include "afxwin.h"

class CNetPlotDoc;
class CNetPlotView;
class CMainFrame;

// CYExpandPage dialog

class CYExpandPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CYExpandPage)

public:
	CYExpandPage();
	virtual ~CYExpandPage();

// Dialog Data
	enum { IDD = IDD_YEXPAND_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnSetActive();
	BOOL m_globalBOOL;
	float m_yexpandFloat;
	CComboBox m_yexpandselectCombo;
	CEdit m_yexpandEdit;
	afx_msg void OnBnClickedYexpandresetbutton();
	afx_msg void OnCbnSelchangeYexpandselectcombo();
	afx_msg void OnCbnSetfocusYexpandselectcombo();
	afx_msg void OnBnClickedYexpandupdatebutton();
	afx_msg void OnBnClickedGlobalcheck();
};

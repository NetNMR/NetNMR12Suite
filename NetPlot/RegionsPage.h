#pragma once
#include "afxwin.h"

class CNetPlotDoc;
class CNetPlotView;
class CMainFrame;

// CRegionsPage dialog

class CRegionsPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CRegionsPage)

public:
	CRegionsPage();
	virtual ~CRegionsPage();

// Dialog Data
	enum { IDD = IDD_REGIONS_PAGE };

private:
	CNetPlotView *GetView();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_baselineorderInt;
	CButton m_calculatebaselineCheck;
	CButton m_baselinequitButton;
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	afx_msg void OnBnClickedAddregionbutton();
	afx_msg void OnBnClickedClearregionbutton();
	afx_msg void OnBnClickedClearallregionbutton();
	afx_msg void OnBnClickedFitintegralbutton();
	afx_msg void OnBnClickedCalculatebaselinecheck();
	afx_msg void OnBnClickedBaselinequitbutton();
	afx_msg void OnCbnSelchangeBaselineordercombo();
public:
	afx_msg void OnBnClickedIltintegralbutton();
};

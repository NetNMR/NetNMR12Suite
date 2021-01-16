#pragma once

class CNetPlotDoc;

// CProcessPage dialog

class CProcessPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CProcessPage)

public:
	CProcessPage();
	virtual ~CProcessPage();

// Dialog Data
	enum { IDD = IDD_PROCESS_PAGE };

private:
	CNetPlotDoc *GetDocument();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	int m_apoType, m_apoUnits;
	CString m_apoValue;
	virtual BOOL OnSetActive();
	afx_msg void OnBnClickedDCLevel();
	afx_msg void OnBnClickedTruncate();
	afx_msg void OnBnClickedApodize();
	afx_msg void OnBnClickedFT();
	afx_msg void OnBnClickedZerofill();
};

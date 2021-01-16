#pragma once


// CFitTab dialog
class CMainFrame;
class CNetSpanDoc;
class CNetSpanView;

class CFitTab : public CPropertyPage
{
	DECLARE_DYNAMIC(CFitTab)

public:
	CFitTab();
	virtual ~CFitTab();
private:
	CMainFrame *pMain;
	CNetSpanDoc *pDoc;
	CNetSpanView *pView;

// Dialog Data
	enum { IDD = IDD_FITDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	double m_toleranceDouble;
	int m_iterationInt;
	afx_msg void OnBnClickedResetBaseline();
	afx_msg void OnBnClickedFitAction();
	afx_msg void OnBnClickedLorencheck();
	afx_msg void OnBnClickedGausscheck();
	afx_msg void OnBnClickedResetshapebutton();
	afx_msg void OnBnClickedLeastSquaresbutton();
	afx_msg void OnBnClickedSimplexbutton();
};

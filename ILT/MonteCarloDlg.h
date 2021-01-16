#pragma once
#include "afxwin.h"

// CMonteCarloDlg dialog

class CILTDoc;
class CDecay;
class CTerm;

class CMonteCarloDlg : public CDialog
{
	DECLARE_DYNAMIC(CMonteCarloDlg)

public:
	CMonteCarloDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMonteCarloDlg();

// Dialog Data
	enum { IDD = IDD_MONTECARLODIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	CILTDoc *pDoc;
	CDecay *pDecay;
	CTypedPtrList<CPtrList, CTerm *> dlgTerms;

protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	CListBox m_termsList;
	CButton m_buttonSimplify;
	CButton m_buttonOptimize;
	BOOL m_boolSimplify;
	BOOL m_boolOptimize;
	int m_intIterations;
	int m_iObsCount;
	int m_iBasisCount;
	int m_iSpacing;
	int m_iConfidenceInterval;
	double m_dblAmplitude;
	double m_dblRate;
	double m_dblStdErr;
	double m_dObsFrom;
	double m_dObsTo;
	double m_dBasisFrom;
	double m_dBasisTo;
	double m_dblAlpha;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedAddtermbutton();
	afx_msg void OnBnClickedDeletetermbutton();
	afx_msg void OnLbnSelchangeTermslist();
	afx_msg void OnBnClickedMchelpbutton();
	afx_msg void OnBnClickedPastetermsbutton();
	afx_msg void OnBnClickedMcsimplifycheck();
	afx_msg void OnBnClickedMcoptimizecheck();
	afx_msg void OnBnClickedDeeleteallbutton();
};

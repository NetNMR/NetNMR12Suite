#pragma once
#include "afxwin.h"

class CNetPlotDoc;
// CLabelsDlg dialog

class CLabelsDlg : public CDialog
{
	DECLARE_DYNAMIC(CLabelsDlg)

public:
	CLabelsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLabelsDlg();

// Dialog Data
	enum { IDD = IDD_LABELS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_labelsEdit;
	CComboBox m_labelsizeCombo;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
private:
	CNetPlotDoc *pDoc;
};

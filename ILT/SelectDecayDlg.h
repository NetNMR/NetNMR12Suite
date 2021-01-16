#pragma once
#include "afxwin.h"

class CILTDoc;
class CILTView;
// CSelectDecayDlg dialog

class CSelectDecayDlg : public CDialog
{
	DECLARE_DYNAMIC(CSelectDecayDlg)

public:
	CSelectDecayDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelectDecayDlg();

// Dialog Data
	enum { IDD = IDD_SELECTDECAYDIALOG };

private:
	CILTDoc *pDoc;
	CILTView *pView;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_cboxSelectDecay;
protected:
	virtual void OnOK();
public:
	virtual BOOL OnInitDialog();
};

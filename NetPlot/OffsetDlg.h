#pragma once
#include "afxwin.h"

class CNetPlotDoc;
class CNetPlotView;

// COffsetDlg dialog

class COffsetDlg : public CDialog
{
	DECLARE_DYNAMIC(COffsetDlg)

public:
	COffsetDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COffsetDlg();

// Dialog Data
	enum { IDD = IDD_OFFSETDIALOG };

private:
	CNetPlotDoc *pDoc;
	CNetPlotView *pView;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_offsetEdit;
	CString m_strStatic;
	float m_floatOffset;
	virtual BOOL OnInitDialog();
};

#pragma once
#include "afxwin.h"


// CIntegralEdit dialog

class CIntegralEdit : public CDialog
{
	DECLARE_DYNAMIC(CIntegralEdit)

public:
	CIntegralEdit(CWnd* pParent = NULL);   // standard constructor
	virtual ~CIntegralEdit();

// Dialog Data
	enum { IDD = IDD_INTEGRALDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_integralEdit;
	afx_msg void OnBnClickedIntcopybutton();
};

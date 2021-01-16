#pragma once
#include "afxwin.h"


// CProjectionDlg dialog

class CProjectionDlg : public CDialog
{
	DECLARE_DYNAMIC(CProjectionDlg)

public:
	CProjectionDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProjectionDlg();

// Dialog Data
	enum { IDD = IDD_PROJECTIONDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bcolmaxCheck;
	BOOL m_bcolminCheck;
	BOOL m_browmaxCheck;
	BOOL m_browminCheck;
};

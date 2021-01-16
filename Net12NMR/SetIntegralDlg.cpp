// SetIntegralDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Net12NMR.h"
#include "SetIntegralDlg.h"
#include "afxdialogex.h"


// CSetIntegralDlg dialog

IMPLEMENT_DYNAMIC(CSetIntegralDlg, CDialog)

CSetIntegralDlg::CSetIntegralDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetIntegralDlg::IDD, pParent)
{

	m_strIntegralPrompt = _T("");
	m_strIntegralValue = _T("");
}

CSetIntegralDlg::~CSetIntegralDlg()
{
}

void CSetIntegralDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_INTEGRALPROMPTSTATIC, m_strIntegralPrompt);
	DDX_Control(pDX, IDC_SETINTEGRALEDIT, m_editIntegralValue);
	DDX_Text(pDX, IDC_SETINTEGRALEDIT, m_strIntegralValue);
}


BEGIN_MESSAGE_MAP(CSetIntegralDlg, CDialog)
END_MESSAGE_MAP()


// CSetIntegralDlg message handlers


BOOL CSetIntegralDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_editIntegralValue.SetSel(0, -1);
	m_editIntegralValue.SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

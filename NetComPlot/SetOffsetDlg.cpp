// SetOffsetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NetComPlot.h"
#include "SetOffsetDlg.h"


// CSetOffsetDlg dialog

IMPLEMENT_DYNAMIC(CSetOffsetDlg, CDialog)
CSetOffsetDlg::CSetOffsetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetOffsetDlg::IDD, pParent)
	, m_strHUnits(_T(""))
	, m_strVUnits(_T(""))
	, m_strHValue(_T(""))
	, m_strVValue(_T(""))
{
}

CSetOffsetDlg::~CSetOffsetDlg()
{
}

void CSetOffsetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_HSOSTATIC, m_strHUnits);
	DDX_Text(pDX, IDC_VSOSTATIC, m_strVUnits);
	DDX_Text(pDX, IDC_HSOEDIT, m_strHValue);
	DDX_Text(pDX, IDC_VSOEDIT, m_strVValue);
}


BEGIN_MESSAGE_MAP(CSetOffsetDlg, CDialog)
END_MESSAGE_MAP()


// CSetOffsetDlg message handlers

BOOL CSetOffsetDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	if (0 == m_strHUnits.GetLength()) {
		GetDlgItem(IDC_HTITLESTATIC)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_HSOEDIT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_HSOSTATIC)->ShowWindow(SW_HIDE);
	}
	if (0 == m_strVUnits.GetLength()) {
		GetDlgItem(IDC_VTITLESTATIC)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_VSOEDIT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_VSOSTATIC)->ShowWindow(SW_HIDE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

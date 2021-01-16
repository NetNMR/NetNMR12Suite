// SetOffsetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Net12NMR.h"
#include "SetOffsetDlg.h"
#include "afxdialogex.h"


// CSetOffsetDlg dialog

IMPLEMENT_DYNAMIC(CSetOffsetDlg, CDialogEx)

CSetOffsetDlg::CSetOffsetDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSetOffsetDlg::IDD, pParent)
{
	m_Units = 1;
	m_Value = "";
}

CSetOffsetDlg::~CSetOffsetDlg()
{
}

void CSetOffsetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SETOFFSETEDIT, m_conValue);
	DDX_Radio(pDX, IDC_HZOFFSETRADIO, m_Units);
	DDX_Text(pDX, IDC_SETOFFSETEDIT, m_Value);
	DDV_MaxChars(pDX, m_Value, 8);
}


BEGIN_MESSAGE_MAP(CSetOffsetDlg, CDialogEx)
END_MESSAGE_MAP()


// CSetOffsetDlg message handlers

BOOL CSetOffsetDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_conValue.SetSel( 0, -1 );
	m_conValue.SetFocus();
	return FALSE;
}
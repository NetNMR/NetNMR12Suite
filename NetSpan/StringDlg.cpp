// StringDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NetSpan.h"
#include "StringDlg.h"
#include ".\stringdlg.h"


// CStringDlg dialog

IMPLEMENT_DYNAMIC(CStringDlg, CDialog)
CStringDlg::CStringDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStringDlg::IDD, pParent)
{
}

CStringDlg::~CStringDlg()
{
}

void CStringDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STRINGEDIT, m_stringEdit);
}


BEGIN_MESSAGE_MAP(CStringDlg, CDialog)
END_MESSAGE_MAP()


// CStringDlg message handlers

BOOL CStringDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_stringEdit.SetWindowText(m_string);
	m_stringEdit.SetSel(0, -1);
	m_stringEdit.SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CStringDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	m_stringEdit.GetWindowText(m_string);
	CDialog::OnOK();
}

// SFDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NetSpan.h"
#include "SFDlg.h"
#include "MainFrame.h"
#include "NetSpanDoc.h"


// CSFDlg dialog

IMPLEMENT_DYNAMIC(CSFDlg, CDialog)
CSFDlg::CSFDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSFDlg::IDD, pParent)
	, m_sfDouble(0)
{
}

CSFDlg::~CSFDlg()
{
}

void CSFDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SFEDIT, m_sfDouble);
	DDV_MinMaxDouble(pDX, m_sfDouble, 1.0, 1000.0);
}


BEGIN_MESSAGE_MAP(CSFDlg, CDialog)
END_MESSAGE_MAP()


// CSFDlg message handlers

BOOL CSFDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, theApp.m_pMainWnd);
	CNetSpanDoc *pDoc = STATIC_DOWNCAST(CNetSpanDoc, pMain->GetActiveDocument());
	m_sfDouble = pDoc->sf;
	UpdateData(FALSE);
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_SFEDIT);
	pEdit->SetSel(0, -1, FALSE);
	pEdit->SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

// SelectDecayDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ILT.h"
#include "SelectDecayDlg.h"
#include "MainFrm.h"
#include "ILTDoc.h"
#include "ILTView.h"
#include "Decay.h"


// CSelectDecayDlg dialog

IMPLEMENT_DYNAMIC(CSelectDecayDlg, CDialog)

CSelectDecayDlg::CSelectDecayDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectDecayDlg::IDD, pParent)
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, theApp.m_pMainWnd);
	pDoc = STATIC_DOWNCAST(CILTDoc, pMain->GetActiveDocument());
	pView = STATIC_DOWNCAST(CILTView, pMain->GetActiveView());
}

CSelectDecayDlg::~CSelectDecayDlg()
{
}

void CSelectDecayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DECAYSELECTCOMBO, m_cboxSelectDecay);
}


BEGIN_MESSAGE_MAP(CSelectDecayDlg, CDialog)
END_MESSAGE_MAP()

BOOL CSelectDecayDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	int ip = -1;
	POSITION pos = pDoc->Decays.GetHeadPosition();
	while (pos) {
		CDecay *decay = pDoc->Decays.GetAt(pos);
		int i = m_cboxSelectDecay.AddString(decay->strTitle);
		m_cboxSelectDecay.SetItemDataPtr(i, pos);
		if (pos == theApp.pDecay)
			ip = i;
		pDoc->Decays.GetNext(pos);
	}
	if (ip >= 0)
		m_cboxSelectDecay.SetCurSel(ip);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSelectDecayDlg::OnOK()
{
	theApp.pDecay = (POSITION)m_cboxSelectDecay.GetItemDataPtr(m_cboxSelectDecay.GetCurSel());

	pView->Invalidate();

	CDialog::OnOK();
}

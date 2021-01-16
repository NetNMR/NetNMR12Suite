// TabbedDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NetSpan.h"
#include "TabbedDlg.h"

#include "LineTab.h"
#include "SpectraTab.h"
#include "FitTab.h"
#include "MainFrame.h"
#include "NetSpanDoc.h"

// CTabbedDlg

IMPLEMENT_DYNAMIC(CTabbedDlg, CPropertySheet)
CTabbedDlg::CTabbedDlg(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CTabbedDlg::CTabbedDlg(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	lineTab = new CLineTab;
	specTab = new CSpectraTab;
	fitTab = new CFitTab;
	AddPage(specTab);
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, theApp.m_pMainWnd);
	pDoc = STATIC_DOWNCAST(CNetSpanDoc, pMain->GetActiveDocument());
	if (pDoc->Lines.GetCount()) {
		AddPage(lineTab);
		AddPage(fitTab);
	}
}

CTabbedDlg::~CTabbedDlg()
{
	if (lineTab)
		delete lineTab;
	if (specTab)
		delete specTab;
	if (fitTab)
		delete fitTab;
}


BEGIN_MESSAGE_MAP(CTabbedDlg, CPropertySheet)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CTabbedDlg message handlers

void CTabbedDlg::OnClose()
{
	theApp.pCurrentLine = NULL;
	ShowWindow(SW_HIDE);
	pDoc->UpdateAllViews(NULL);
	//CPropertySheet::OnClose();
}

void CTabbedDlg::ShowAllPages(bool bShow)
{
	if (this) {
		int count = GetPageCount();
		if (count < 2) {
			if (bShow) {
				AddPage(lineTab);
				AddPage(fitTab);
			}
		} else {
			if (! bShow) {
				RemovePage(fitTab);
				RemovePage(lineTab);
			}
		}
	}
}

void CTabbedDlg::RefillSpecList(void)
{
	if (specTab)
		specTab->RefillSpecList();
}

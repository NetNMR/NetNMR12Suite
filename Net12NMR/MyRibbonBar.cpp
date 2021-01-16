// MyRibbonBar.cpp : implementation file
//

#include "stdafx.h"
#include "Net12NMR.h"
#include "MyRibbonBar.h"
#include "MainFrame.h"
#include "ChildFrame.h"
#include "Net12NMRDoc.h"


// CMyRibbonBar

IMPLEMENT_DYNAMIC(CMyRibbonBar, CMFCRibbonBar)

CMyRibbonBar::CMyRibbonBar()
{

}

CMyRibbonBar::~CMyRibbonBar()
{
}


BEGIN_MESSAGE_MAP(CMyRibbonBar, CMFCRibbonBar)
	ON_WM_ACTIVATE()
	ON_WM_CHANGEUISTATE()
	ON_WM_UPDATEUISTATE()
	ON_WM_CHILDACTIVATE()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// CMyRibbonBar message handlers


void CMyRibbonBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	CString s_was = GetActiveCategory()->GetName();
	CMFCRibbonBar::OnLButtonDown(nFlags, point);
	CString s_now = GetActiveCategory()->GetName();
	s_was = s_was.Trim();
	s_now = s_now.Trim();
	if (s_was.Compare(s_now)) {
		CNet12NMRDoc *pDoc = NULL;
		CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, theApp.m_pMainWnd);
		if (pMain != pMain->GetActiveFrame()) {
			CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->GetActiveFrame());
			pDoc = STATIC_DOWNCAST(CNet12NMRDoc, pChild->GetActiveDocument());
		}
		// save the bvector and fvector state back in the appropraite ?REGION
		// reuse the allocated ?REGION objects, if possible
		if (pDoc)
			pDoc->SaveState();
		// switch to new mode and set up fvector and bvector, if needed
		if (0==s_now.Compare("Home") || 0==s_now.Compare("Process")) {
			theApp.disMode = normalMode;
		} else if (0 == s_now.Compare("Baseline")) {
			theApp.disMode = baselineMode;
			if (pDoc) {
				UINT size = pDoc->dt.size();
				pDoc->InitializeBaseline();
			}
		} else if (0 == s_now.Compare("Peaks")) {
			theApp.disMode = peakpickMode;
		} else if (0 == s_now.Compare("Integral")) {
			theApp.disMode = integralMode;
		} else if (0 == s_now.Compare("Header")) {
			theApp.disMode = normalMode;
			if (pDoc)
				pDoc->UpdateRibbon(this);
		} else if (0 == s_now.Compare("Shelf")) {
			theApp.disMode = shelfMode;
		} else {
			CString msg = "Unknown ribbon tab name: ";
			AfxMessageBox(msg+s_now, MB_ICONEXCLAMATION);
			theApp.disMode = normalMode;
		}
		// update the display
		if (pDoc) {
			pDoc->LoadState();
			pDoc->UpdateAllViews(NULL);
		}
		// show the correct outputlist
		pMain->m_wndOutput.ShowOutputList(theApp.disMode);
	}
}

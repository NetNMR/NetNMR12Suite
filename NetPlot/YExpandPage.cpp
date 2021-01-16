// YExpandPage.cpp : implementation file
//

#include "stdafx.h"
#include "..\NMRDataTypes.h"
#include "NetPlot.h"
#include "YExpandPage.h"
#include "NetPlotDoc.h"
#include "NetPlotView.h"
#include "MainFrame.h"
#include "ChildFrame.h"
#include "Spectrum.h"


// CYExpandPage dialog

IMPLEMENT_DYNAMIC(CYExpandPage, CPropertyPage)

CYExpandPage::CYExpandPage()
	: CPropertyPage(CYExpandPage::IDD)
	, m_yexpandFloat(1.f)
	, m_globalBOOL(FALSE)
{

}

CYExpandPage::~CYExpandPage()
{
}

void CYExpandPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_YEXPANDSELECTCOMBO, m_yexpandselectCombo);
	DDX_Control(pDX, IDC_YEXPANDEDIT, m_yexpandEdit);
	DDX_Text(pDX, IDC_YEXPANDEDIT, m_yexpandFloat);
	DDX_Check(pDX, IDC_GLOBALCHECK, m_globalBOOL);
}


BEGIN_MESSAGE_MAP(CYExpandPage, CPropertyPage)
	ON_BN_CLICKED(IDC_YEXPANDRESETBUTTON, &CYExpandPage::OnBnClickedYexpandresetbutton)
	ON_CBN_SELCHANGE(IDC_YEXPANDSELECTCOMBO, &CYExpandPage::OnCbnSelchangeYexpandselectcombo)
	ON_CBN_SETFOCUS(IDC_YEXPANDSELECTCOMBO, &CYExpandPage::OnCbnSetfocusYexpandselectcombo)
	ON_BN_CLICKED(IDC_YEXPANDUPDATEBUTTON, &CYExpandPage::OnBnClickedYexpandupdatebutton)
	ON_BN_CLICKED(IDC_GLOBALCHECK, &CYExpandPage::OnBnClickedGlobalcheck)
END_MESSAGE_MAP()


// CYExpandPage message handlers

void CYExpandPage::OnBnClickedYexpandresetbutton()
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CNetPlotDoc *pDoc = STATIC_DOWNCAST(CNetPlotDoc, pMain->MDIGetActive()->GetActiveDocument());
	POSITION pos = pDoc->Spectra.GetHeadPosition();
	while (pos) {
		CSpectrum *pSpec = pDoc->Spectra.GetNext(pos);
		pSpec->SetYExpansion(1.f);
	}
	m_yexpandFloat = 1.f;
	UpdateData(FALSE);
	pDoc->UpdateAllViews(NULL);
}

void CYExpandPage::OnCbnSelchangeYexpandselectcombo()
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CNetPlotView *pView = STATIC_DOWNCAST(CNetPlotView, pMain->MDIGetActive()->GetActiveView());
	pView->oneSpec = (CSpectrum *)m_yexpandselectCombo.GetItemDataPtr(m_yexpandselectCombo.GetCurSel());
	m_yexpandFloat = pView->oneSpec->GetYExpansion();
	UpdateData(FALSE);
}

BOOL CYExpandPage::OnSetActive()
{
	CNetPlotDoc *pDoc = NULL;
	CNetPlotView *pView = NULL;
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->MDIGetActive());
	if (pChild) {
		pDoc = STATIC_DOWNCAST(CNetPlotDoc, pChild->GetActiveDocument());
		pView = STATIC_DOWNCAST(CNetPlotView, pChild->GetActiveView());
		if (pView) {
			pView->disMode = yexpandMode;
			pView->Invalidate();
		}
	}
	OnCbnSetfocusYexpandselectcombo();
	m_globalBOOL = pDoc->GlobalScaling;
	UpdateData(FALSE);
	return CPropertyPage::OnSetActive();
}

void CYExpandPage::OnCbnSetfocusYexpandselectcombo()
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->MDIGetActive());
	CNetPlotDoc *pDoc = STATIC_DOWNCAST(CNetPlotDoc, pChild->GetActiveDocument());
	CNetPlotView *pView = STATIC_DOWNCAST(CNetPlotView, pChild->GetActiveView());
	int selected = 0;
	m_yexpandselectCombo.ResetContent();
	POSITION pos = pDoc->Spectra.GetHeadPosition();
	while (pos) {
		CSpectrum *pSpec = pDoc->Spectra.GetNext(pos);
		int added;
		if (pSpec->label.IsEmpty()) {
			CString str = pSpec->pathname;
			str.MakeReverse();
			int i = str.FindOneOf("/\\");
			if (i > 1)
				str = str.Left(i);
			else {
				i = str.FindOneOf(":");
				if (i > 1)
					str = str.Left(i);
			}
			str.MakeReverse();
			added = m_yexpandselectCombo.AddString(str);
		} else
			added = m_yexpandselectCombo.AddString(pSpec->label);
		m_yexpandselectCombo.SetItemDataPtr(added, pSpec);
		if (pView->oneSpec == pSpec)
			selected = added;
	}
	m_yexpandselectCombo.SetCurSel(selected);
	if (0 == selected)
		pView->oneSpec = (CSpectrum *)m_yexpandselectCombo.GetItemDataPtr(0);
	m_yexpandFloat = pView->oneSpec->GetYExpansion();
	m_globalBOOL = pDoc->GlobalScaling;
	UpdateData(FALSE);
}

void CYExpandPage::OnBnClickedYexpandupdatebutton()
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->MDIGetActive());
	CNetPlotDoc *pDoc = STATIC_DOWNCAST(CNetPlotDoc, pChild->GetActiveDocument());
	CNetPlotView *pView = STATIC_DOWNCAST(CNetPlotView, pChild->GetActiveView());
	UpdateData(TRUE);
	if (m_yexpandFloat <= 0.f) {
		AfxMessageBox("Expansion factor must be > 0.0", MB_ICONERROR);
		m_yexpandFloat = pView->oneSpec->GetYExpansion();
		UpdateData(FALSE);
		return;
	}
	pView->oneSpec->SetYExpansion(m_yexpandFloat);
	pDoc->UpdateAllViews(NULL);
}

void CYExpandPage::OnBnClickedGlobalcheck()
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CNetPlotDoc *pDoc = STATIC_DOWNCAST(CNetPlotDoc, pMain->MDIGetActive()->GetActiveDocument());
	pDoc->GlobalScaling = !pDoc->GlobalScaling;
	m_globalBOOL = pDoc->GlobalScaling;
	UpdateData(FALSE);
	pDoc->UpdateAllViews(NULL);
}

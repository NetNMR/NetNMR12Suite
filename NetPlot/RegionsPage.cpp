// RegionsPage.cpp : implementation file
//

#include "stdafx.h"
#include "NetPlot.h"
#include "MainFrame.h"
#include "ChildFrame.h"
#include "NetPlotDoc.h"
#include "NetPlotView.h"
#include "RegionsPage.h"


// CRegionsPage dialog

IMPLEMENT_DYNAMIC(CRegionsPage, CPropertyPage)

CRegionsPage::CRegionsPage()
	: CPropertyPage(CRegionsPage::IDD)
	, m_baselineorderInt(8)
{

}

CRegionsPage::~CRegionsPage()
{
}

void CRegionsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_CBIndex(pDX, IDC_BASELINEORDERCOMBO, m_baselineorderInt);
	DDV_MinMaxInt(pDX, m_baselineorderInt, 0, 8);
	DDX_Control(pDX, IDC_CALCULATEBASELINECHECK, m_calculatebaselineCheck);
	DDX_Control(pDX, IDC_BASELINEQUITBUTTON, m_baselinequitButton);
}


BEGIN_MESSAGE_MAP(CRegionsPage, CPropertyPage)
	ON_BN_CLICKED(IDC_ADDREGIONBUTTON, &CRegionsPage::OnBnClickedAddregionbutton)
	ON_BN_CLICKED(IDC_CLEARREGIONBUTTON, &CRegionsPage::OnBnClickedClearregionbutton)
	ON_BN_CLICKED(IDC_CLEARALLREGIONBUTTON, &CRegionsPage::OnBnClickedClearallregionbutton)
	ON_BN_CLICKED(IDC_FITINTEGRALBUTTON, &CRegionsPage::OnBnClickedFitintegralbutton)
	ON_BN_CLICKED(IDC_CALCULATEBASELINECHECK, &CRegionsPage::OnBnClickedCalculatebaselinecheck)
	ON_BN_CLICKED(IDC_BASELINEQUITBUTTON, &CRegionsPage::OnBnClickedBaselinequitbutton)
	ON_CBN_SELCHANGE(IDC_BASELINEORDERCOMBO, &CRegionsPage::OnCbnSelchangeBaselineordercombo)
	ON_BN_CLICKED(IDC_ILTINTEGRALBUTTON, &CRegionsPage::OnBnClickedIltintegralbutton)
END_MESSAGE_MAP()


// CRegionsPage message handlers

CNetPlotView * CRegionsPage::GetView()
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	return STATIC_DOWNCAST(CNetPlotView, pMain->MDIGetActive()->GetActiveView());
}

void CRegionsPage::OnBnClickedAddregionbutton()
{
	GetView()->OnIntegralAddintegral();
}

void CRegionsPage::OnBnClickedClearregionbutton()
{
	GetView()->OnIntegralRemoveintegral();
}

void CRegionsPage::OnBnClickedClearallregionbutton()
{
	GetView()->OnIntegralClearall();
}

void CRegionsPage::OnBnClickedFitintegralbutton()
{
	GetView()->OnIntegralFitintegrals();
}

BOOL CRegionsPage::OnSetActive()
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->MDIGetActive());
	if (pChild) {
		CNetPlotView *pView = STATIC_DOWNCAST(CNetPlotView, pChild->GetActiveView());
		if (pView) {
			pView->disMode = normalMode;
			pView->Invalidate();
		}
		CNetPlotDoc *pDoc = STATIC_DOWNCAST(CNetPlotDoc, pChild->GetActiveDocument());
		if (pDoc)
			m_baselineorderInt = pDoc->nBaseOrder - 2;
	}

	return CPropertyPage::OnSetActive();
}

BOOL CRegionsPage::OnKillActive()
{
	OnBnClickedBaselinequitbutton();

	return CPropertyPage::OnKillActive();
}

void CRegionsPage::OnBnClickedCalculatebaselinecheck()
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->MDIGetActive());
	CNetPlotView *pView = STATIC_DOWNCAST(CNetPlotView, pChild->GetActiveView());
	CNetPlotDoc *pDoc = STATIC_DOWNCAST(CNetPlotDoc, pChild->GetActiveDocument());
	if (m_calculatebaselineCheck.GetCheck()) {
		UpdateData();
		m_baselinequitButton.ShowWindow(SW_SHOW);
		m_calculatebaselineCheck.SetWindowText("Apply");
		pView->disMode = baseMode;
		pDoc->CalculateBaseline();
		pView->Invalidate();
	} else {
		pDoc->SubtractBaseline();
		m_baselinequitButton.ShowWindow(SW_HIDE);
		m_calculatebaselineCheck.SetWindowTextA("Calculate");
		pView->disMode = normalMode;
		pView->Invalidate();
	}
}

void CRegionsPage::OnBnClickedBaselinequitbutton()
{
	CNetPlotView *pView = GetView();
	if (baseMode == pView->disMode) {
		m_calculatebaselineCheck.SetCheck(0);
		m_baselinequitButton.ShowWindow(SW_HIDE);
		m_calculatebaselineCheck.SetWindowText("Calculate");
		pView->disMode = normalMode;
		pView->Invalidate();
	}
}

void CRegionsPage::OnCbnSelchangeBaselineordercombo()
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->MDIGetActive());
	CNetPlotView *pView = STATIC_DOWNCAST(CNetPlotView, pChild->GetActiveView());
	CNetPlotDoc *pDoc = STATIC_DOWNCAST(CNetPlotDoc, pChild->GetActiveDocument());
	UpdateData();
	pDoc->nBaseOrder = m_baselineorderInt + 2;
	if (baseMode == pView->disMode)
		OnBnClickedCalculatebaselinecheck();
}

void CRegionsPage::OnBnClickedIltintegralbutton()
{
	GetView()->OnIntegralILTintegrals();
}

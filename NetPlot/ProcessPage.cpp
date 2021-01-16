// ProcessPage.cpp : implementation file
//

#include "stdafx.h"
#include "NetPlot.h"
#include "MainFrame.h"
#include "ChildFrame.h"
#include "NetPlotDoc.h"
#include "NetPlotView.h"
#include "ProcessPage.h"


// CProcessPage dialog

IMPLEMENT_DYNAMIC(CProcessPage, CPropertyPage)

CProcessPage::CProcessPage()
	: CPropertyPage(CProcessPage::IDD)
	, m_apoType(0)
	, m_apoValue("0.5")
	, m_apoUnits(0)
{

}

CProcessPage::~CProcessPage()
{
}

void CProcessPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_CBIndex(pDX, IDC_APOSHAPECOMBO, m_apoType);
	DDX_CBString(pDX, IDC_APOVALUECOMBO, m_apoValue);
	DDX_CBIndex(pDX, IDC_APOUNITCOMBO, m_apoUnits);
}


BEGIN_MESSAGE_MAP(CProcessPage, CPropertyPage)
	ON_BN_CLICKED(IDC_DCLEVELBUTTON, &CProcessPage::OnBnClickedDCLevel)
	ON_BN_CLICKED(IDC_TRUNCATEBUTTON, &CProcessPage::OnBnClickedTruncate)
	ON_BN_CLICKED(IDC_APODIZEBUTTON, &CProcessPage::OnBnClickedApodize)
	ON_BN_CLICKED(IDC_ZEROBUTTON, &CProcessPage::OnBnClickedZerofill)
	ON_BN_CLICKED(IDC_FTBUTTON, &CProcessPage::OnBnClickedFT)
END_MESSAGE_MAP()


// CProcessPage message handlers


BOOL CProcessPage::OnSetActive()
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->MDIGetActive());
	if (pChild) {
		CNetPlotView *pView = STATIC_DOWNCAST(CNetPlotView, pChild->GetActiveView());
		if (pView) {
			pView->disMode = normalMode;
			pView->Invalidate();
		}
	}

	return CPropertyPage::OnSetActive();
}

CNetPlotDoc *CProcessPage::GetDocument()
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	return STATIC_DOWNCAST(CNetPlotDoc, pMain->MDIGetActive()->GetActiveDocument());
}

void CProcessPage::OnBnClickedDCLevel()
{
	GetDocument()->DCLevel();
}

void CProcessPage::OnBnClickedTruncate()
{
	GetDocument()->Zerofill(false);
}

void CProcessPage::OnBnClickedZerofill()
{
	GetDocument()->Zerofill(true);
}

void CProcessPage::OnBnClickedFT()
{
	GetDocument()->FT();
}

void CProcessPage::OnBnClickedApodize()
{
	UpdateData();
	double value = atof(m_apoValue);
	if (value)
		GetDocument()->Apodize(m_apoType, value, m_apoUnits);
}
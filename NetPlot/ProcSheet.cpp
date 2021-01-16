// ProcSheet.cpp : implementation file
//

#include "stdafx.h"
#include "..\NMRDataTypes.h"
#include "NetPlot.h"
#include "ProcSheet.h"
#include "ProcessPage.h"
#include "PhasePage.h"
#include "RegionsPage.h"
#include "YExpandPage.h"
#include "SubtractPage.h"


// CProcSheet

IMPLEMENT_DYNAMIC(CProcSheet, CPropertySheet)

CProcSheet::CProcSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{

}

CProcSheet::CProcSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	if (processPage = new CProcessPage)
		AddPage(processPage);
	if (phasePage = new CPhasePage)
		AddPage(phasePage);
	if (subtractPage = new CSubtractPage)
		AddPage(subtractPage);
	if (regionsPage = new CRegionsPage)
		AddPage(regionsPage);
	if (yexpandPage = new CYExpandPage)
		AddPage(yexpandPage);
}

CProcSheet::~CProcSheet()
{
	if (processPage)
		delete processPage;
	if (phasePage)
		delete phasePage;
	if (regionsPage)
		delete regionsPage;
	if (yexpandPage)
		delete yexpandPage;
	if (subtractPage)
		delete subtractPage;
}


BEGIN_MESSAGE_MAP(CProcSheet, CPropertySheet)
END_MESSAGE_MAP()


// CProcSheet message handlers

void CProcSheet::PostNcDestroy()
{
	CPropertySheet::PostNcDestroy();
	delete this;
}

void CProcSheet::UpdateContent()
{
	if (phasePage && phasePage->m_hWnd && phasePage->IsWindowVisible())
		phasePage->OnCbnSetfocusPhaseselectcombo();	
	else if (yexpandPage && yexpandPage->m_hWnd && yexpandPage->IsWindowVisible())
		yexpandPage->OnCbnSetfocusYexpandselectcombo();
	else if (subtractPage && subtractPage->m_hWnd && subtractPage->IsWindowVisible())
		subtractPage->OnSetActive();
}
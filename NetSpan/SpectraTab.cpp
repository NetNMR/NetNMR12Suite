// SpectraTab.cpp : implementation file
//

#include "stdafx.h"
#include "NetSpan.h"
#include "SpectraTab.h"
#include "MainFrame.h"
#include "NetSpanView.h"
#include "NetSpanDoc.h"
#include "StringDlg.h"


// CSpectraTab dialog

IMPLEMENT_DYNAMIC(CSpectraTab, CPropertyPage)
CSpectraTab::CSpectraTab()
	: CPropertyPage(CSpectraTab::IDD)
{
	pMain = STATIC_DOWNCAST(CMainFrame, theApp.m_pMainWnd);
	pView = STATIC_DOWNCAST(CNetSpanView, pMain->GetActiveView());
	pDoc = STATIC_DOWNCAST(CNetSpanDoc, pMain->GetActiveDocument());
}

CSpectraTab::~CSpectraTab()
{
}

void CSpectraTab::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SPECTRALIST, m_spectraList);
	DDX_Control(pDX, IDC_CHANGEBUTTON, m_changeButton);
	DDX_Control(pDX, IDC_REMOVEBUTTON, m_removeButton);
	DDX_Control(pDX, IDC_MOVEUPBUTTON, m_moveupButton);
	DDX_Control(pDX, IDC_MOVEDOWNBUTTON, m_movedownButton);
}


BEGIN_MESSAGE_MAP(CSpectraTab, CPropertyPage)
	ON_BN_CLICKED(IDC_CHANGEBUTTON, OnBnClickedChangebutton)
	ON_BN_CLICKED(IDC_REMOVEBUTTON, OnBnClickedRemovebutton)
	ON_BN_CLICKED(IDC_MOVEUPBUTTON, OnBnClickedMoveupbutton)
	ON_BN_CLICKED(IDC_VIEWALLRADIO, OnBnClickedViewbutton)
	ON_BN_CLICKED(IDC_VIEWTOPRADIO, OnBnClickedViewbutton)
	ON_BN_CLICKED(IDC_VIEWBOTTOMRADIO, OnBnClickedViewbutton)
	ON_BN_CLICKED(IDC_VIEWSPACEDRADIO, OnBnClickedViewbutton)
	ON_BN_CLICKED(IDC_MOVEDOWNBUTTON, OnBnClickedMovedownbutton)
	ON_LBN_SELCHANGE(IDC_SPECTRALIST, OnLbnSelchangeSpectralist)
END_MESSAGE_MAP()


// CSpectraTab message handlers

void CSpectraTab::OnBnClickedViewbutton()
{
	pView->myView = (NetSpanView)GetCheckedRadioButton(IDC_VIEWALLRADIO, IDC_VIEWSPACEDRADIO);
	pView->Invalidate();
}

void CSpectraTab::OnBnClickedChangebutton()
{
	if (arraySelection.GetCount() != 1)
		return;
	int i = arraySelection[0];
	CSpectrum *p = (CSpectrum *)m_spectraList.GetItemDataPtr(i);
	CStringDlg dlg;
	dlg.m_string = p->label;
	if (IDOK == dlg.DoModal()) {
		pDoc->SetModifiedFlag();
		p->label = dlg.m_string.Trim();
		pView->Invalidate();
		m_spectraList.DeleteString(i);
		m_spectraList.InsertString(i, p->label);
		m_spectraList.SetSel(i);
		m_spectraList.SetItemDataPtr(i, p);
	}
}

void CSpectraTab::OnBnClickedRemovebutton()
{
	pDoc->SetModifiedFlag();
	pView->serr = 0.0;
	for (INT_PTR i=arraySelection.GetCount()-1; i>=0; i--) {
		m_spectraList.DeleteString(arraySelection[i]);
	}
	OnLbnSelchangeSpectralist();
}

void CSpectraTab::OnLbnSelchangeSpectralist()
{
	arraySelection.RemoveAll();
	int listcount = m_spectraList.GetCount();
	int selcount = m_spectraList.GetSelCount();
	arraySelection.SetSize(selcount);
	m_spectraList.GetSelItems(selcount, arraySelection.GetData());
	m_removeButton.EnableWindow(0<selcount);
	m_changeButton.EnableWindow(1==selcount);
	if (selcount > 0) {
		if (arraySelection[selcount-1]-arraySelection[0]+1 == selcount) {
			// THE SELECTION IS CONTIGIUOUS
			if (0 < arraySelection[0])
				m_moveupButton.EnableWindow(TRUE);
			else
				m_moveupButton.EnableWindow(FALSE);
			if (listcount-1 > arraySelection[selcount-1])
				m_movedownButton.EnableWindow(TRUE);
			else
				m_movedownButton.EnableWindow(FALSE);
		} else {
			// THE SELECTION CONTAINS GAPS (NON-CONTIGIOUS)
			m_moveupButton.EnableWindow(FALSE);
			m_movedownButton.EnableWindow(FALSE);
		}
	} else {
		m_moveupButton.EnableWindow(FALSE);
		m_movedownButton.EnableWindow(FALSE);
	}

	// HAS THE LIST CHANGED (BY DELETING A SPECTRUM)?
	// OR WAS THE LIST REARRANGED?
	BOOL changed = FALSE;
	int speccount = (int)pDoc->Spectra.GetCount();
	if (listcount == speccount) {
		int i = 0;
		POSITION pos = pDoc->Spectra.GetTailPosition();
		while (pos) {
			CSpectrum *pSpec = (CSpectrum *)m_spectraList.GetItemDataPtr(i++);
			if (pSpec != pDoc->Spectra.GetPrev(pos)) {
				changed = TRUE;
				break;
			}
		}
	} else
		changed = TRUE;
	if (changed) {
		if (listcount != speccount) {
			// A SPECTRUM WAS DELETED
			while (pDoc->Spectra.GetCount()) {
				int i;
				CSpectrum *pSpec = pDoc->Spectra.RemoveHead();
				for (i=0; i<speccount; i++) {
					if ((CSpectrum *)m_spectraList.GetItemDataPtr(i) == pSpec)
						break;
				}
				if (i >= speccount)
					delete pSpec;
			}
			pDoc->FindMinMax();
		} else
			// THE SPECTRA WERE MOVED, REBUILD THE Spectra LIST
			pDoc->Spectra.RemoveAll();
		for (speccount=0; speccount<listcount; speccount++) {
			CSpectrum *pSpec = (CSpectrum *)m_spectraList.GetItemDataPtr(speccount);
			if (speccount) {
				if (pSpec->GetMax() > pDoc->ymax)
					pDoc->ymax = pSpec->GetMax();
				if (pSpec->GetMin() < pDoc->ymin)
					pDoc->ymin = pSpec->GetMin();
			} else {
				pDoc->ymax = pSpec->GetMax();
				pDoc->ymin = pSpec->GetMin();
			}
			pDoc->Spectra.AddHead(pSpec);
		}
		pDoc->cols = listcount;
		if (listcount < 1) {
			pView->ClearZoomCursor();
			pMain->tabDlg->ShowWindow(SW_HIDE);
			pDoc->Lines.RemoveAll();
			int i = pMain->tabDlg->GetPageCount();
			while (--i)
				pMain->tabDlg->RemovePage(i);
		}
		pView->Invalidate();
	}
}

BOOL CSpectraTab::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	RefillSpecList();
	UpdateData(FALSE);
	CheckRadioButton(IDC_VIEWALLRADIO, IDC_VIEWSPACEDRADIO, pView->myView);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSpectraTab::OnBnClickedMoveupbutton()
{
	int i = arraySelection[0] - 1;
	CString str;
	m_spectraList.GetText(i, str);
	CSpectrum *pSpec = (CSpectrum *)m_spectraList.GetItemDataPtr(i);
	m_spectraList.DeleteString(i);
	i = arraySelection[arraySelection.GetCount()-1];
	if (i >= m_spectraList.GetCount())
		i = -1;
	m_spectraList.SetItemDataPtr(m_spectraList.InsertString(i, str), pSpec);
	OnLbnSelchangeSpectralist();
}

void CSpectraTab::OnBnClickedMovedownbutton()
{
	int i = arraySelection[arraySelection.GetCount() - 1] + 1;
	CString str;
	m_spectraList.GetText(i, str);
	CSpectrum *pSpec = (CSpectrum *)m_spectraList.GetItemDataPtr(i);
	m_spectraList.DeleteString(i);
	i = arraySelection[0];
	m_spectraList.SetItemDataPtr(m_spectraList.InsertString(i, str), pSpec);
	OnLbnSelchangeSpectralist();
}

void CSpectraTab::RefillSpecList(void)
{
	m_spectraList.SelItemRange(FALSE, 0, m_spectraList.GetCount()-1);
	m_spectraList.ResetContent();
	POSITION pos = pDoc->Spectra.GetTailPosition();
	while (pos) {
		CSpectrum *pSpec = pDoc->Spectra.GetPrev(pos);
		m_spectraList.SetItemDataPtr(m_spectraList.AddString(pSpec->label), pSpec);
	}
}

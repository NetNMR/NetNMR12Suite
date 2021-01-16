// FitTab.cpp : implementation file
//

#include "stdafx.h"
#include "NetSpan.h"
#include "FitTab.h"
#include "MainFrame.h"
#include "NetSpanDoc.h"
#include "NetSpanView.h"
#include "LSquare.h"
#include "Amoeba.h"
#include "FitTextDlg.h"

// CFitTab dialog

IMPLEMENT_DYNAMIC(CFitTab, CPropertyPage)
CFitTab::CFitTab()
	: CPropertyPage(CFitTab::IDD)
	, m_toleranceDouble(0.0001)
	, m_iterationInt(100)
{
	pMain = STATIC_DOWNCAST(CMainFrame, theApp.m_pMainWnd);
	pDoc = STATIC_DOWNCAST(CNetSpanDoc, pMain->GetActiveDocument());
	pView = STATIC_DOWNCAST(CNetSpanView, pMain->GetActiveView());
}

CFitTab::~CFitTab()
{
}

void CFitTab::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TOLERANCEEDIT, m_toleranceDouble);
	DDV_MinMaxDouble(pDX, m_toleranceDouble, 1.0e-6, 0.1);
	DDX_Text(pDX, IDC_ITERATIONEDIT, m_iterationInt);
	DDV_MinMaxInt(pDX, m_iterationInt, 1, 5000000);
}


BEGIN_MESSAGE_MAP(CFitTab, CPropertyPage)
	ON_BN_CLICKED(IDC_RESETBASELINEBUTTON, OnBnClickedResetBaseline)
	ON_BN_CLICKED(IDC_FITACTIONBUTTON, OnBnClickedFitAction)
	ON_BN_CLICKED(IDC_LORENCHECK, OnBnClickedLorencheck)
	ON_BN_CLICKED(IDC_GAUSSCHECK, OnBnClickedGausscheck)
	ON_BN_CLICKED(IDC_RESETSHAPEBUTTON, OnBnClickedResetshapebutton)
	ON_BN_CLICKED(IDC_LSQRADIO, OnBnClickedLeastSquaresbutton)
	ON_BN_CLICKED(IDC_SMPRADIO, OnBnClickedSimplexbutton)
END_MESSAGE_MAP()


// CFitTab message handlers

BOOL CFitTab::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	CheckRadioButton(IDC_1BPRADIO, IDC_4BPRADIO, IDC_1BPRADIO+pDoc->nbase-1);
	CheckRadioButton(IDC_LSQRADIO, IDC_SMPRADIO, IDC_LSQRADIO);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CFitTab::OnBnClickedLeastSquaresbutton()
{
	m_iterationInt = 100;
	UpdateData(FALSE);
}

void CFitTab::OnBnClickedSimplexbutton()
{
	m_iterationInt = 200000;
	UpdateData(FALSE);
}

void CFitTab::OnBnClickedResetBaseline()
{
	CheckRadioButton(IDC_1BPRADIO, IDC_4BPRADIO, IDC_1BPRADIO);
	POSITION pos = pDoc->Spectra.GetHeadPosition();
	while (pos) {
		CSpectrum *pSpec = pDoc->Spectra.GetNext(pos);
		pSpec->SetBaseline(0.0, 0.0, 0.0, 0.0);
	}
	pView->serr = 0.0;
	pDoc->ReCalculate();
	pView->Invalidate();
}

void CFitTab::OnBnClickedFitAction()
{
	CWaitCursor wait;
	pDoc->SetModifiedFlag();
	UpdateData(TRUE);
	if (IsDlgButtonChecked(IDC_LORENCHECK) || IsDlgButtonChecked(IDC_GAUSSCHECK)) {
		pView->serr = 0.0;
		POSITION pos = pDoc->Lines.GetHeadPosition();
		while (pos) {
			CLine *pLine = pDoc->Lines.GetNext(pos);
			pLine->ChangeShape(IsDlgButtonChecked(IDC_LORENCHECK)?1.0:0.0);
			pLine->bshape = FALSE;
		}
		pDoc->ReCalculate();
	}
	pDoc->nbase = GetCheckedRadioButton(IDC_1BPRADIO, IDC_4BPRADIO) - IDC_1BPRADIO + 1;
	if (IDC_LSQRADIO == GetCheckedRadioButton(IDC_LSQRADIO, IDC_SMPRADIO)) {
		CLSquare leastsq;
		if (leastsq.Create(true)) {
			if (leastsq.Optimize(m_toleranceDouble, m_iterationInt, true)) {
				CFitTextDlg dlg(true);
				dlg.DoModal();
			}
		} else
			AfxMessageBox("Trouble creating Least Sq. data!", MB_ICONERROR);
	} else {
		CAmoeba amoeba;
		if (amoeba.Create()) {
			if (amoeba.Optimize(m_toleranceDouble, m_iterationInt)) {
				CFitTextDlg dlg(false);
				dlg.DoModal();
			}
		} else
			AfxMessageBox("Trouble initializing Simplex!");
	}
}

void CFitTab::OnBnClickedLorencheck()
{
	pDoc->SetModifiedFlag();
	pView->serr = 0.0;
	CheckDlgButton(IDC_GAUSSCHECK, BST_UNCHECKED);
	if (IsDlgButtonChecked(IDC_LORENCHECK)) {
		POSITION pos = pDoc->Lines.GetHeadPosition();
		while (pos) {
			CLine *pLine = pDoc->Lines.GetNext(pos);
			pLine->ChangeShape(1.0);
			pLine->bshape = FALSE;
		}
		pDoc->ReCalculate();
		pView->Invalidate(TRUE);
	}
}

void CFitTab::OnBnClickedGausscheck()
{
	pDoc->SetModifiedFlag();
	pView->serr = 0.0;
	CheckDlgButton(IDC_LORENCHECK, BST_UNCHECKED);
	if (IsDlgButtonChecked(IDC_GAUSSCHECK)) {
		POSITION pos = pDoc->Lines.GetHeadPosition();
		while (pos) {
			CLine *pLine = pDoc->Lines.GetNext(pos);
			pLine->ChangeShape(0.0);
			pLine->bshape = FALSE;
		}
		pDoc->ReCalculate();
		pView->Invalidate(TRUE);
	}
}

void CFitTab::OnBnClickedResetshapebutton()
{
	pDoc->SetModifiedFlag();
	pView->serr = 0.0;
	CheckDlgButton(IDC_LORENCHECK, BST_UNCHECKED);
	CheckDlgButton(IDC_GAUSSCHECK, BST_UNCHECKED);
	POSITION pos = pDoc->Lines.GetHeadPosition();
	while (pos) {
		CLine *pLine = pDoc->Lines.GetNext(pos);
		pLine->ChangeShape(0.5);
		pLine->bshape = TRUE;
	}
	pDoc->ReCalculate();
	pView->Invalidate(TRUE);
}

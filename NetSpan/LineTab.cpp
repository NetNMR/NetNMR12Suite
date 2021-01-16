// LineTab.cpp : implementation file
//

#include "stdafx.h"
#include "NetSpan.h"
#include "LineTab.h"
#include "MainFrame.h"
#include "NetSpanView.h"
#include "NetSpanDoc.h"
#include "StringDlg.h"
#include "LSquare.h"
#include ".\linetab.h"

// CLineTab dialog

IMPLEMENT_DYNAMIC(CLineTab, CPropertyPage)
CLineTab::CLineTab()
	: CPropertyPage(CLineTab::IDD)
{
	pMain = STATIC_DOWNCAST(CMainFrame, theApp.m_pMainWnd);
	pView = STATIC_DOWNCAST(CNetSpanView, pMain->GetActiveView());
	pDoc = STATIC_DOWNCAST(CNetSpanDoc, pMain->GetActiveDocument());
}

CLineTab::~CLineTab()
{
}

void CLineTab::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NAMECOMBO, m_nameCombo);
	DDX_Control(pDX, IDC_SHAPESLIDER, m_shapeSlider);
	DDX_Control(pDX, IDC_WIDTHSLIDER, m_widthSlider);
	DDX_Control(pDX, IDC_SHIFTSLIDER, m_shiftSlider);
	DDX_Control(pDX, IDC_SHAPEEDIT, m_shapeEdit);
	DDX_Control(pDX, IDC_WIDTHEDIT, m_widthEdit);
	DDX_Control(pDX, IDC_SHIFTEDIT, m_shiftEdit);
}

BEGIN_MESSAGE_MAP(CLineTab, CPropertyPage)
	ON_CBN_SELCHANGE(IDC_NAMECOMBO, OnCbnSelchangeNameCombo)
	ON_BN_CLICKED(IDC_RENAMEBUTTON, OnBnClickedRenameButton)
	ON_BN_CLICKED(IDC_DELETEBUTTON, OnBnClickedDeleteButton)
	ON_CBN_DROPDOWN(IDC_NAMECOMBO, OnCbnDropDownNameCombo)
	ON_BN_CLICKED(IDC_SHAPECHECK, OnBnClickedShapecheck)
	ON_BN_CLICKED(IDC_WIDTHCHECK, OnBnClickedWidthcheck)
	ON_BN_CLICKED(IDC_SHIFTCHECK, OnBnClickedShiftcheck)
	ON_EN_KILLFOCUS(IDC_SHAPEEDIT, OnEnKillfocusShapeedit)
	ON_EN_KILLFOCUS(IDC_WIDTHEDIT, OnEnKillfocusWidthedit)
	ON_EN_KILLFOCUS(IDC_SHIFTEDIT, OnEnKillfocusShiftedit)
	ON_WM_ACTIVATE()
	ON_WM_HSCROLL()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

// CLineTab message handlers

BOOL CLineTab::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	POSITION pos = pDoc->Lines.GetHeadPosition();
	while (pos) {
		CLine *pLine = pDoc->Lines.GetNext(pos);
		m_nameCombo.SetItemDataPtr(m_nameCombo.AddString(pLine->name), pLine);
	}
	m_shapeSlider.SetRange(0, 1000);
	m_shapeSlider.SetPageSize(10);
	m_widthSlider.SetRange(0, 1000);
	m_widthSlider.SetPageSize(10);
	m_shiftSlider.SetRange(0, 1000);
	m_shiftSlider.SetPageSize(10);
	OnCbnSelchangeNameCombo();
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CLineTab::OnBnClickedRenameButton()
{
	int i = m_nameCombo.GetCurSel();
	if (i < 0)
		return;
	CLine *pLine = (CLine *)m_nameCombo.GetItemDataPtr(i);
	CStringDlg dlg;
	dlg.m_string = pLine->name;
	if (IDOK == dlg.DoModal()) {
		pDoc->SetModifiedFlag();
		pLine->name = dlg.m_string.Trim();
		m_nameCombo.DeleteString(i);
		m_nameCombo.InsertString(i, pLine->name);
		m_nameCombo.SetItemDataPtr(i, pLine);
		m_nameCombo.SetCurSel(i);
	}
}

void CLineTab::OnCbnSelchangeNameCombo()
{
	if (m_nameCombo.GetCurSel() < 0)
		m_nameCombo.SetCurSel(0);
	CLine *pLine = (CLine *)m_nameCombo.GetItemDataPtr(m_nameCombo.GetCurSel());
	theApp.pCurrentLine = pLine;
	CString str;
	str.Format("%.1f", 100.0*pLine->GetShape());
	m_shapeEdit.SetWindowText(str);
	str.Format("%.2f", pLine->GetWidth()*pDoc->sf*pDoc->xsw);
	m_widthEdit.SetWindowText(str);
	str.Format("%.3f", pDoc->xoffset-pDoc->xsw*pLine->GetShift());
	m_shiftEdit.SetWindowText(str);
	m_shapeSlider.SetPos(int(1000.0 * pLine->GetShape() + 0.5));
	double of = pDoc->xnorm[pDoc->from];
	double sw = pDoc->xnorm[pDoc->to] - of;
	m_shiftSlider.SetPos(int(1000.0 * (pLine->GetShift()-of)/sw + 0.5));
	sw *= 0.5;				// max linewidth is half the display width
	sw -= pDoc->xnorm[2];	// min linewidth is 2 points
	m_widthSlider.SetPos(int(1000.0 * (pLine->GetWidth() - pDoc->xnorm[2])/sw + 0.5));
	CheckDlgButton(IDC_SHAPECHECK, pLine->bshape?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_WIDTHCHECK, pLine->bwidth?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_SHIFTCHECK, pLine->bshift?BST_CHECKED:BST_UNCHECKED);
	UpdateData(FALSE);
	pDoc->ReCalculate();
	pView->Invalidate();
}

void CLineTab::OnCbnDropDownNameCombo()
{
	if (pDoc->Lines.GetCount() != m_nameCombo.GetCount()) {
		// LINES HAVE BEEN ADDED SINCE COMBOBOX WAS LAST DISPLAYED
		// SO I MUST RECONSTRUCT IT, RESELECT IT AND REDISPLAY LINE PARAMETERS
		int selected = m_nameCombo.GetCurSel();
		CLine *pSelected = (CLine *)m_nameCombo.GetItemDataPtr(selected);
		selected = -1;
		m_nameCombo.ResetContent();
		POSITION pos = pDoc->Lines.GetHeadPosition();
		while (pos) {
			CLine *pLine = pDoc->Lines.GetNext(pos);
			int i = m_nameCombo.AddString(pLine->name);
			m_nameCombo.SetItemDataPtr(i, pLine);
			if (pLine == pSelected) {
				selected = i;
			}
		}
		if (selected < 0) {
			selected = 0;
		}
		m_nameCombo.SetCurSel(selected);
	}
}

void CLineTab::OnBnClickedDeleteButton()
{
	pDoc->SetModifiedFlag();
	pView->serr = 0.0;
	// FIRST MAKE SURE THE m_nameCombo IS UP TO DATE
	OnCbnDropDownNameCombo();
	int selected = m_nameCombo.GetCurSel();
	CLine *pSelected = (CLine *)m_nameCombo.GetItemDataPtr(selected);
	// REMOVE THE LINE'S INTENSITIES FROM EACH SPECTRUM
	POSITION pos = pDoc->Spectra.GetHeadPosition();
	while (pos) {
		CSpectrum *pSpec = pDoc->Spectra.GetNext(pos);
		pSpec->intensity.RemoveKey(pSelected->id);
	}
	// REMOVE THE STRING FROM THE DROPDOWN, DELETE THE CLine
	// AND REMOVE pDoc->Lines ENTRY
	m_nameCombo.DeleteString(selected);
	delete pSelected;
	pDoc->Lines.RemoveAt(pDoc->Lines.FindIndex(selected));
	// THEN SELECT ANOTHER LINE, OR
	// HIDE THE LINES DIALOG IF THERE ARE NO MORE
	int count = m_nameCombo.GetCount();
	if (count > 0) {
		if (selected>=count)
			selected--;
		m_nameCombo.SetCurSel(selected);
		OnCbnSelchangeNameCombo();	// THIS WILL SET theApp.pCurrentLine
	} else {
		pMain->tabDlg->ShowAllPages(FALSE);
		theApp.pCurrentLine = NULL;
	}
	// FINALLY RECALCULATE THE SPECTRA AND SHOW THEM
	pDoc->ReCalculate();
	pView->Invalidate();
}


void CLineTab::OnBnClickedShapecheck()
{
	pDoc->SetModifiedFlag();
	pView->serr = 0.0;
	CLine *pLine = (CLine *)m_nameCombo.GetItemDataPtr(m_nameCombo.GetCurSel());
	pLine->bshape = !pLine->bshape;
}

void CLineTab::OnBnClickedWidthcheck()
{
	pDoc->SetModifiedFlag();
	pView->serr = 0.0;
	CLine *pLine = (CLine *)m_nameCombo.GetItemDataPtr(m_nameCombo.GetCurSel());
	pLine->bwidth = !pLine->bwidth;
}

void CLineTab::OnBnClickedShiftcheck()
{
	pDoc->SetModifiedFlag();
	pView->serr = 0.0;
	CLine *pLine = (CLine *)m_nameCombo.GetItemDataPtr(m_nameCombo.GetCurSel());
	pLine->bshift = !pLine->bshift;
}

void CLineTab::OnEnKillfocusShapeedit()
{
	UpdateData(TRUE);
	CLine *pLine = (CLine *)m_nameCombo.GetItemDataPtr(m_nameCombo.GetCurSel());
	CString str;
	m_shapeEdit.GetWindowText(str);
	pLine->ChangeShape(0.01 * atof(str));
	OnCbnSelchangeNameCombo();
	//pDoc->ReCalculate();
	//pView->Invalidate(TRUE);
	CLSquare lsquare;
	if (lsquare.Create(false))
		lsquare.Optimize(0.01, 0, false);
}

void CLineTab::OnEnKillfocusWidthedit()
{
	UpdateData(TRUE);
	CLine *pLine = (CLine *)m_nameCombo.GetItemDataPtr(m_nameCombo.GetCurSel());
	CString str;
	m_widthEdit.GetWindowText(str);
	pLine->ChangeWidth(atof(str)/(pDoc->sf*pDoc->xsw));
	OnCbnSelchangeNameCombo();
	//pDoc->ReCalculate();
	//pView->Invalidate();
	CLSquare lsquare;
	if (lsquare.Create(false))
		lsquare.Optimize(0.01, 0, false);
}

void CLineTab::OnEnKillfocusShiftedit()
{
	UpdateData(TRUE);
	CLine *pLine = (CLine *)m_nameCombo.GetItemDataPtr(m_nameCombo.GetCurSel());
	CString str;
	m_shiftEdit.GetWindowText(str);
	pLine->ChangeShift((pDoc->xoffset-atof(str))/pDoc->xsw);
	OnCbnSelchangeNameCombo();
	//pDoc->ReCalculate();
	//pView->Invalidate();
	CLSquare lsquare;
	if (lsquare.Create(false))
		lsquare.Optimize(0.01, 0, false);
}

void CLineTab::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (TB_ENDTRACK==nSBCode)
		return;
	CLine *pLine = (CLine *)m_nameCombo.GetItemDataPtr(m_nameCombo.GetCurSel());
	CSliderCtrl *pSlider = (CSliderCtrl *)pScrollBar;
	switch (nSBCode) {
		case TB_TOP:
			nPos = 0;
			break;
		case TB_BOTTOM:
			nPos = 1000;
			break;
		case TB_LINEDOWN:
		case TB_LINEUP:
		case TB_PAGEDOWN:
		case TB_PAGEUP:
			nPos = pSlider->GetPos();
			break;
	}
	double of = pDoc->xnorm[pDoc->from];
	double sw = pDoc->xnorm[pDoc->to] - of;
	CString str;
	if (&m_shapeSlider == pSlider) {
		double sh = nPos / 1000.0;
		str.Format("%.1f", 100.0 * pLine->ChangeShape(sh));
		m_shapeEdit.SetWindowText(str);
	} else if (&m_widthSlider == pSlider) {
		sw *= 0.5;
		sw -= pDoc->xnorm[2];
		double w = nPos * sw / 1000.0 + pDoc->xnorm[2];
		str.Format("%.2f", pDoc->sf * pDoc->xsw * pLine->ChangeWidth(w));
		m_widthEdit.SetWindowText(str);
	} else if (&m_shiftSlider == pSlider) {
		double sh = nPos * sw / 1000.0 + of;
		str.Format("%.3f", pDoc->xoffset - pDoc->xsw * pLine->ChangeShift(sh));
		m_shiftEdit.SetWindowText(str);
	}
	//pDoc->ReCalculate();
	//pView->Invalidate();
	CLSquare lsquare;
	if (lsquare.Create(false))
		lsquare.Optimize(0.01, 0, false);
}

void CLineTab::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CPropertyPage::OnShowWindow(bShow, nStatus);
	if (TRUE == bShow)
		theApp.pCurrentLine = (CLine *)m_nameCombo.GetItemDataPtr(m_nameCombo.GetCurSel());
	else
		theApp.pCurrentLine = NULL;
	pView->Invalidate();
}

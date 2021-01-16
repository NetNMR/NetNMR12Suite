// PhasePage.cpp : implementation file
//

#include "stdafx.h"
#include "..\NMRDataTypes.h"
#include "NetPlot.h"
#include "NetPlotDoc.h"
#include "NetPlotView.h"
#include "MainFrame.h"
#include "ChildFrame.h"
#include "PhasePage.h"
#include "Spectrum.h"


// CPhasePage dialog

IMPLEMENT_DYNAMIC(CPhasePage, CPropertyPage)

CPhasePage::CPhasePage()
	: CPropertyPage(CPhasePage::IDD)
	, allcheck(FALSE)
{
}

CPhasePage::~CPhasePage()
{
}

void CPhasePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PHASESELECTCOMBO, m_phaseselectCombo);
	DDX_Control(pDX, IDC_AllCHECK, m_allButton);
	DDX_Control(pDX, IDC_CLONEBUTTON, m_cloneButton);
	DDX_Control(pDX, IDC_ATCURSORCHECK, m_atcursorCheck);
	DDX_Control(pDX, IDC_CLONEUPBUTTON, m_cloneupButton);
	DDX_Control(pDX, IDC_CLONEDOWNBUTTON, m_clonedownButton);
	DDX_Control(pDX, IDC_PHASEPROMPTSTATIC, m_phasepromptStatic);
}


BEGIN_MESSAGE_MAP(CPhasePage, CPropertyPage)
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_CONTEXTMENU()
	ON_CBN_SELCHANGE(IDC_PHASESELECTCOMBO, &CPhasePage::OnCbnSelchangePhaseselectcombo)
	ON_BN_CLICKED(IDC_PHASEAUTOABUTTON, &CPhasePage::OnBnClickedPhaseautoabutton)
	ON_BN_CLICKED(IDC_PHASEAUTOABBUTTON, &CPhasePage::OnBnClickedPhaseautoabbutton)
	ON_BN_CLICKED(IDC_MAGNITUDEBUTTON, &CPhasePage::OnBnClickedMagnitudebutton)
	ON_BN_CLICKED(IDC_AllCHECK, &CPhasePage::OnBnClickedAllcheck)
	ON_BN_CLICKED(IDC_CLONEBUTTON, &CPhasePage::OnBnClickedClonebutton)
	ON_BN_CLICKED(IDC_ATCURSORCHECK, &CPhasePage::OnBnClickedAtcursorcheck)
	ON_BN_CLICKED(IDC_UPBUTTON, &CPhasePage::OnBnClickedUpbutton)
	ON_BN_CLICKED(IDC_DOWNBUTTON, &CPhasePage::OnBnClickedDownbutton)
	ON_BN_CLICKED(IDC_LEFTPHASEBUTTON, &CPhasePage::OnBnClickedLeftphasebutton)
	ON_BN_CLICKED(IDC_RIGHTPHASEBUTTON, &CPhasePage::OnBnClickedRightphasebutton)
	ON_BN_CLICKED(IDC_CLONEUPBUTTON, &CPhasePage::OnBnClickedCloneupbutton)
	ON_BN_CLICKED(IDC_CLONEDOWNBUTTON, &CPhasePage::OnBnClickedClonedownbutton)
	ON_BN_CLICKED(IDC_UNDOPHASEBUTTON, &CPhasePage::OnBnClickedUndophasebutton)
	ON_CBN_SETFOCUS(IDC_PHASESELECTCOMBO, &CPhasePage::OnCbnSetfocusPhaseselectcombo)
	ON_BN_CLICKED(IDC_SPECUPBUTTON, &CPhasePage::OnBnClickedSpecupbutton)
	ON_BN_CLICKED(IDC_SPECDOWNBUTTON, &CPhasePage::OnBnClickedSpecdownbutton)
END_MESSAGE_MAP()


// CPhasePage message handlers

BOOL CPhasePage::OnSetActive()
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
	AllCheckClicked();
	if (! allcheck)
		OnCbnSetfocusPhaseselectcombo();
	return CPropertyPage::OnSetActive();
}

CNetPlotDoc *CPhasePage::GetDocument()
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	return STATIC_DOWNCAST(CNetPlotDoc, pMain->MDIGetActive()->GetActiveDocument());
}

CNetPlotView *CPhasePage::GetView()
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	return STATIC_DOWNCAST(CNetPlotView, pMain->MDIGetActive()->GetActiveView());
}

void CPhasePage::OnBnClickedPhaseautoabutton()
{
	CSpectrum *p = NULL;
	if (! allcheck)
		p = (CSpectrum *)m_phaseselectCombo.GetItemDataPtr(m_phaseselectCombo.GetCurSel());
	GetDocument()->AutoPhaseA(p);
}

void CPhasePage::OnBnClickedPhaseautoabbutton()
{
	CSpectrum *p = NULL;
	if (! allcheck)
		p = (CSpectrum *)m_phaseselectCombo.GetItemDataPtr(m_phaseselectCombo.GetCurSel());
	GetDocument()->AutoPhaseAB(p);
}

void CPhasePage::OnBnClickedMagnitudebutton()
{
	CSpectrum *p = NULL;
	if (! allcheck)
		p = (CSpectrum *)m_phaseselectCombo.GetItemDataPtr(m_phaseselectCombo.GetCurSel());
	GetDocument()->Magnitude(p);
}

void CPhasePage::OnBnClickedAllcheck()
{
	allcheck = (! allcheck);
	AllCheckClicked();
	if (! allcheck)
		OnCbnSetfocusPhaseselectcombo();
}

void CPhasePage::AllCheckClicked()
{
	m_phaseselectCombo.EnableWindow(! allcheck);
	m_cloneButton.EnableWindow(! allcheck);
	m_cloneupButton.EnableWindow(! allcheck);
	m_clonedownButton.EnableWindow(! allcheck);
	m_atcursorCheck.EnableWindow(! allcheck);
	if (! allcheck) {
		GetView()->oneSpec = (CSpectrum *)m_phaseselectCombo.GetItemDataPtr(0);
	}
}

void CPhasePage::OnBnClickedClonebutton()
{
	GetDocument()->ClonePhase((CSpectrum *)m_phaseselectCombo.GetItemDataPtr(m_phaseselectCombo.GetCurSel()));
}

void CPhasePage::OnCbnSelchangePhaseselectcombo()
{
	GetView()->oneSpec = (CSpectrum *)m_phaseselectCombo.GetItemDataPtr(m_phaseselectCombo.GetCurSel());
}

void CPhasePage::OnBnClickedAtcursorcheck()
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->MDIGetActive());
	CNetPlotDoc *pDoc = STATIC_DOWNCAST(CNetPlotDoc, pChild->GetActiveDocument());
	CNetPlotView *pView = STATIC_DOWNCAST(CNetPlotView, pChild->GetActiveView());
	if (m_atcursorCheck.GetCheck()) {
		pView->GetZoomCursors(zleft, zright);
		if (zleft<0 || zright <0) {
			m_atcursorCheck.SetCheck(FALSE);
			AfxMessageBox("Please set the zoom cursors first!", MB_ICONEXCLAMATION);
			return;
		}
		savedA = theApp.gphA;
		savedB = theApp.gphB;
		pView->oneSpec->GlobalPhase();
		pView->oneSpec->FindMinMax();
		float w = 1000.0f * pDoc->Width;
		// l and r are fractions of the zoom cursor positions in the window widths
		float l = float(zleft) / w;
		float r = float(zright) / w;
		// d is the number of total points in the current displayed region
		float d = float(pView->oneSpec->Stop() - pView->oneSpec->Start());
		zleft = pView->oneSpec->Start() + int(l * d);
		zright = pView->oneSpec->Start() + int(r * d);

		float max = abs(pView->oneSpec->Ymin());
		if (abs(pView->oneSpec->Ymax()) > max)
			max = abs(pView->oneSpec->Ymax());
		pView->oneRange = 2.0f * max;
		pView->oneMin = 0.0f - max;
		pView->disMode = phaseMode;
		pView->Invalidate();
		m_phasepromptStatic.ShowWindow(SW_SHOW);
		SetCapture();
		ShowCursor(FALSE);
		CRect wr;
		pMain->GetWindowRect(&wr);
		pMain->midscreen = wr.CenterPoint();
		SetCursorPos(pMain->midscreen.x, pMain->midscreen.y);
	} else {
		pView->oneSpec->FindMinMax();
		ReleaseCapture();
		ShowCursor(TRUE);
		pView->disMode = normalMode;
		pDoc->SetMinMax();
		pView->Invalidate();
		m_phasepromptStatic.ShowWindow(SW_HIDE);
	}
}

void CPhasePage::OnBnClickedUpbutton()
{
	CNetPlotView *pView = GetView();
	if (phaseMode == pView->disMode) {
		pView->oneMin *= 0.5f;
		pView->oneRange *= 0.5f;
		pView->Invalidate();
	}
}

void CPhasePage::OnBnClickedDownbutton()
{
	CNetPlotView *pView = GetView();
	if (phaseMode == pView->disMode) {
		pView->oneMin *= 2.0f;
		pView->oneRange *= 2.0f;
		pView->Invalidate();
	}
}

BOOL CPhasePage::OnKillActive()
{
	if (phaseMode == GetView()->disMode) {
		m_atcursorCheck.SetCheck(FALSE);
		OnBnClickedAtcursorcheck();
	}

	return CPropertyPage::OnKillActive();
}

void CPhasePage::OnMouseMove(UINT nFlags, CPoint point)
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->MDIGetActive());
	CNetPlotView *pView = STATIC_DOWNCAST(CNetPlotView, pChild->GetActiveView());
	if (phaseMode==pView->disMode || this==GetCapture()) {
		ClientToScreen(&point);
		float inc = pView->oneRange / (pView->oneSpec->Ymax() - pView->oneSpec->Ymin());
		if (inc > 1.0f)
			inc = 1.0f;
		else if (inc < 0.0625f)
			inc = 0.0625f;
		inc *= float(pMain->midscreen.y - point.y);
		if (nFlags & MK_CONTROL)
			inc *= 0.1f;
		else if (nFlags & MK_SHIFT)
			inc *= 10.0f;
		if (inc) {
			float a;
			int cb = pView->oneSpec->Size();
			float b = inc * float(cb) / float(zleft - zright);
			if (IDC_LEFTPHASEBUTTON == pView->iLeft) {
				b *= -1.0f;
				a = float(cb/2 - zright) * b / float(cb);
			} else
				a = float(cb/2 - zleft) * b / float(cb);
			theApp.gphA += a;
			theApp.gphB += b;
			pView->oneSpec->GlobalPhase();
			pView->Invalidate();
			ReleaseCapture();
			SetCursorPos(pMain->midscreen.x, pMain->midscreen.y);
			SetCapture();
		}
	}

	CPropertyPage::OnMouseMove(nFlags, point);
}

BOOL CPhasePage::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (zDelta >= 0)
		OnBnClickedUpbutton();
	else
		OnBnClickedDownbutton();

	return CPropertyPage::OnMouseWheel(nFlags, zDelta, pt);
}

void CPhasePage::OnLButtonDown(UINT nFlags, CPoint point)
{
	CNetPlotView *pView = GetView();
	if (phaseMode==pView->disMode && this==GetCapture()) {
		if (IDC_LEFTPHASEBUTTON==pView->iLeft)
			OnBnClickedRightphasebutton();
		else
			OnBnClickedLeftphasebutton();
	}
	pView->Invalidate();

	CPropertyPage::OnLButtonDown(nFlags, point);
}

void CPhasePage::OnBnClickedLeftphasebutton()
{
	CNetPlotView *pView = GetView();
	if (phaseMode==pView->disMode && this==GetCapture())
		pView->iLeft = IDC_LEFTPHASEBUTTON;
}

void CPhasePage::OnBnClickedRightphasebutton()
{
	CNetPlotView *pView = GetView();
	if (phaseMode==pView->disMode && this==GetCapture())
		pView->iLeft = IDC_RIGHTPHASEBUTTON;
}

void CPhasePage::OnBnClickedCloneupbutton()
{
	CNetPlotView *pView = GetView();
	CNetPlotDoc *pDoc = GetDocument();
	POSITION pos = pDoc->Spectra.GetHeadPosition();
	while (pos) {
		CSpectrum *p = pDoc->Spectra.GetNext(pos);
		if (p == pView->oneSpec)
			break;
		p->GlobalPhase();
		p->FindMinMax();
	}
	pView->Invalidate();
}

void CPhasePage::OnBnClickedClonedownbutton()
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->MDIGetActive());
	CNetPlotDoc *pDoc = STATIC_DOWNCAST(CNetPlotDoc, pChild->GetActiveDocument());
	CNetPlotView *pView = STATIC_DOWNCAST(CNetPlotView, pChild->GetActiveView());
	POSITION pos = pDoc->Spectra.GetHeadPosition();
	bool there = false;
	while (pos) {
		CSpectrum *p = pDoc->Spectra.GetNext(pos);
		if (! there) {
			if (p == pView->oneSpec)
				there = true;
		} else {
			p->GlobalPhase();
			p->FindMinMax();
		}
	}
	pView->Invalidate();
}

void CPhasePage::OnBnClickedUndophasebutton()
{
	CNetPlotView *pView = GetView();
	if (phaseMode==pView->disMode && this==GetCapture()) {
		theApp.gphA = savedA;
		theApp.gphB = savedB;
		pView->oneSpec->GlobalPhase();
		pView->Invalidate();
	}
}

void CPhasePage::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->MDIGetActive());
	CNetPlotView *pView = STATIC_DOWNCAST(CNetPlotView, pChild->GetActiveView());
	ReleaseCapture();
	ShowCursor( true );
	CMenu menu;
	menu.LoadMenu( IDR_CONTEXTMENU );
	CMenu *popup = NULL;
	if (IDC_LEFTPHASEBUTTON == pView->iLeft)
		popup = menu.GetSubMenu( 1 );
	else if (IDC_RIGHTPHASEBUTTON == pView->iLeft)
		popup = menu.GetSubMenu( 2 );
	if (popup)
		popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this );
	ShowCursor( false );
	SetCursorPos( pMain->midscreen.x, pMain->midscreen.y );
	SetCapture();
}

void CPhasePage::OnCbnSetfocusPhaseselectcombo()
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->MDIGetActive());
	CNetPlotDoc *pDoc = STATIC_DOWNCAST(CNetPlotDoc, pChild->GetActiveDocument());
	CNetPlotView *pView = STATIC_DOWNCAST(CNetPlotView, pChild->GetActiveView());
	int selected = 0;
	m_phaseselectCombo.ResetContent();
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
			str.MakeReverse();
			added = m_phaseselectCombo.AddString(str);
		} else
			added = m_phaseselectCombo.AddString(pSpec->label);
		m_phaseselectCombo.SetItemDataPtr(added, pSpec);
		if (pView->oneSpec == pSpec)
			selected = added;
	}
	m_phaseselectCombo.SetCurSel(selected);
	if (0 == selected)
		pView->oneSpec = (CSpectrum *)m_phaseselectCombo.GetItemDataPtr(0);
}

void CPhasePage::OnBnClickedSpecupbutton()
{
	OnBnClickedSpecbutton(true);
}

void CPhasePage::OnBnClickedSpecdownbutton()
{
	OnBnClickedSpecbutton(false);
}

void CPhasePage::OnBnClickedSpecbutton(bool go_up)
{
	CNetPlotView *pView = GetView();
	if (phaseMode==pView->disMode && this==GetCapture()) {
		int cursel = m_phaseselectCombo.GetCurSel();
		if (cursel < 0)
			return;	// nothing selected
		if (go_up) { // increment or decrement as needed
			if (--cursel < 0)
				return;
		} else {
			if (++cursel >= m_phaseselectCombo.GetCount())
				return;
		}
		// capture the new mix and max before moving on...
		pView->oneSpec->FindMinMax();
		// change the combobox
		m_phaseselectCombo.SetCurSel(cursel);
		// Get the new pointer
		OnCbnSelchangePhaseselectcombo();
		// apply the global phase and repaint
		pView->oneSpec->GlobalPhase();
		pView->Invalidate();
	}
}

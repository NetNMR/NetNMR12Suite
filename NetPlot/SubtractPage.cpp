// SubtractPage.cpp : implementation file
//

#include "stdafx.h"
#include "..\NMRDataTypes.h"
#include "NetPlot.h"
#include "MainFrame.h"
#include "ChildFrame.h"
#include "NetPlotDoc.h"
#include "NetPlotView.h"
#include "Spectrum.h"
#include "SubtractPage.h"


// CSubtractPage dialog

IMPLEMENT_DYNAMIC(CSubtractPage, CPropertyPage)

CSubtractPage::CSubtractPage()
	: CPropertyPage(CSubtractPage::IDD)
	, m_leftFloat(0.9f)
	, m_rightFloat(1.1f)
	, m_subtractFloat(1.0f)
{

}

CSubtractPage::~CSubtractPage()
{
}

CNetPlotDoc *CSubtractPage::GetDocument()
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	return STATIC_DOWNCAST(CNetPlotDoc, pMain->MDIGetActive()->GetActiveDocument());
}

void CSubtractPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_LEFTEDIT, m_leftFloat);
	DDX_Text(pDX, IDC_RIGHTEDIT, m_rightFloat);
	DDX_Text(pDX, IDC_SUBTRACTSTATIC, m_subtractFloat);
	DDX_Control(pDX, IDC_SUBTRACTLIST, m_subtractListBox);
	DDX_Control(pDX, IDC_SUBTRACTCHECK, m_subtractCheck);
	DDX_Control(pDX, IDC_SUBTRACTCANCELBUTTON, m_subtractcancelButton);
	DDX_Control(pDX, IDC_SUBTRACTSLIDER, m_subtractSlider);
	DDX_Control(pDX, IDC_SUBTRACTGROUPSTATIC, m_subtractgroupStatic);
	DDX_Control(pDX, IDC_LEFTEDIT, m_subtractfromEdit);
	DDX_Control(pDX, IDC_RIGHTEDIT, m_subtracttoEdit);
	DDX_Control(pDX, IDC_SUBTRACTSTATIC, m_subtractStatic);
	DDX_Control(pDX, IDC_SUBTRACTFROMSTATIC, m_subtractfromStatic);
	DDX_Control(pDX, IDC_SUBTRACTTOSTATIC, m_subtracttoStatic);
}


BEGIN_MESSAGE_MAP(CSubtractPage, CPropertyPage)
	ON_BN_CLICKED(IDC_SUBTRACTCHECK, &CSubtractPage::OnBnClickedSubtractcheck)
	ON_BN_CLICKED(IDC_SUBTRACTCANCELBUTTON, &CSubtractPage::OnBnClickedSubtractcancelbutton)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_LEFTEDIT, &CSubtractPage::OnEnChangeLeftedit)
	ON_EN_CHANGE(IDC_RIGHTEDIT, &CSubtractPage::OnEnChangeRightedit)
END_MESSAGE_MAP()


// CSubtractPage message handlers

void CSubtractPage::ShowSubtract(int showcmd)
{
	m_subtractCheck.SetCheck(SW_SHOW==showcmd?BST_CHECKED:BST_UNCHECKED);
	m_subtractCheck.SetWindowText(SW_SHOW==showcmd?"Apply":"Calculate");
	m_subtractcancelButton.ShowWindow(showcmd);
	m_subtractgroupStatic.ShowWindow(showcmd);
	m_subtractfromEdit.ShowWindow(showcmd);
	m_subtractfromStatic.ShowWindow(showcmd);
	m_subtractStatic.ShowWindow(showcmd);
	m_subtracttoStatic.ShowWindow(showcmd);
	m_subtracttoEdit.ShowWindow(showcmd);
	m_subtractSlider.ShowWindow(showcmd);
}

BOOL CSubtractPage::OnSetActive()
{
	m_subtractSlider.SetRange(0, 100);
	m_subtractSlider.SetPos(50);
	m_subtractSlider.SetTicFreq(10);
	ShowSubtract(SW_HIDE);
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->MDIGetActive());
	if (pChild) {
		CNetPlotDoc *pDoc = STATIC_DOWNCAST(CNetPlotDoc, pChild->GetActiveDocument());
		if (pDoc) {
			m_subtractListBox.ResetContent();
			POSITION pos = pDoc->Spectra.GetHeadPosition();
			while (pos) {
				CSpectrum *pSpec = pDoc->Spectra.GetNext(pos);
				m_subtractListBox.SetItemDataPtr(m_subtractListBox.AddString(pSpec->label), pSpec);
			}
		}
		CNetPlotView *pView = STATIC_DOWNCAST(CNetPlotView, pChild->GetActiveView());
		if (pView) {
			pView->disMode = normalMode;
			pView->Invalidate();
		}
	}
	return CPropertyPage::OnSetActive();
}


void CSubtractPage::OnBnClickedSubtractcheck()
{
	CArray<int,int> selectionArray;
	CTypedPtrArray<CPtrArray, CSpectrum *> spectrumArray;
	
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->MDIGetActive());
	CNetPlotView *pView = STATIC_DOWNCAST(CNetPlotView, pChild->GetActiveView());
	CNetPlotDoc *pDoc = STATIC_DOWNCAST(CNetPlotDoc, pChild->GetActiveDocument());
	if (! pDoc->AllTheSame)
		return;
	int selec = m_subtractListBox.GetSelCount();
	if (selec < 1) {
		m_subtractCheck.SetCheck(0);
		return;
	}
	// CALCULATE THE AVERAGE OF THE SELECTED SPECTRA
	selectionArray.SetSize(selec);	// integer indicies of the listbox selections
	spectrumArray.SetSize(selec);	// pointers to the CSpectrum spectra of the listbox selections
	m_subtractListBox.GetSelItems(selec, selectionArray.GetData());
	for (int i=0; i<selec; i++)
		spectrumArray.SetAt(i, (CSpectrum *)m_subtractListBox.GetItemDataPtr(selectionArray.GetAt(i)));
	int size = spectrumArray[0]->Size();
	sum.resize(size);
	for (int i=0; i<size; i++)
		sum[i] = complex<float>(0.0f, 0.0f);
	for (int i=0; i<selec; i++)
		spectrumArray[i]->Add(sum);
	if (selec > 1) {
		float f = 1.0f / float(selec);
		for (int i=0; i<size; i++)
			sum[i] *= f;
	}

	// convert the slider to a float scale based on the right and left values
	float scale = m_rightFloat - m_leftFloat;
	scale = 0.01f * float(m_subtractSlider.GetPos()) * scale + m_leftFloat;
	m_subtractFloat = scale;
	UpdateData(FALSE);

	if (m_subtractCheck.GetCheck()) {	// SUBTRACT THE AVERAGE FROM ALL THE DATA
		UpdateData();
		ShowSubtract(SW_SHOW);
		pView->disMode = subtractMode;
		for (int i=0; i<m_subtractListBox.GetCount(); i++) {
			CSpectrum *pSpec = (CSpectrum *)m_subtractListBox.GetItemDataPtr(i);
			pSpec->SubtractData(sum, scale);
		}
		pDoc->SetMinMax();
		pView->Invalidate();
	} else {							// SUBTRACT (AND COMIT) THE AVERAGE FROM ALL THE RAW

		if (2==m_subtractListBox.GetCount() && 1==selectionArray.GetCount()) {
			// special case, subtracting one spectrum from another: keep both and create a new difference
			CString sublabel;
			// base the new spectrum on the one NOT seltected
			CSpectrum *newSpec = new CSpectrum;
			for (int i=0; i<2; i++) {
				CSpectrum *pSpec = (CSpectrum *)m_subtractListBox.GetItemDataPtr(i);
				if (i != selectionArray.GetAt(0))
					newSpec->Copy(pSpec);
				else
					sublabel = pSpec->label;
			}
			newSpec->label += '-';
			if (1.0f != scale) {
				CString s;
				s.Format("%g*", scale);
				newSpec->label += s;
			}
			// now subtract the selected spectrum
			newSpec->SubtractRaw(sum, scale);
			newSpec->label += sublabel;	// finish up the new label
			newSpec->pathname = newSpec->label;
			pDoc->Spectra.AddTail(newSpec);
		} else {
			for (int i=0; i<m_subtractListBox.GetCount(); i++) {
				CSpectrum *pSpec = (CSpectrum *)m_subtractListBox.GetItemDataPtr(i);
				pSpec->SubtractRaw(sum, scale);
			}
			if (1==selectionArray.GetCount() && 1.0f==scale) {
				// Special case - remove the subtracted spectrum from the doucment!
				// Fist get the pointer to the spectrum being subtracted
				int selected = selectionArray.GetAt(0);
				CSpectrum *selectedSpec = (CSpectrum *)m_subtractListBox.GetItemDataPtr(selected);
				// reset the ListBox
				m_subtractListBox.ResetContent();

				// find the selected spectrum, delete it and remove it from the pDoc->Spectra list.
				POSITION pos = pDoc->Spectra.GetHeadPosition();
				while (pos) {
					if (selectedSpec == pDoc->Spectra.GetAt(pos)) {
						pDoc->Spectra.RemoveAt(pos);
						delete selectedSpec;
						break;
					}
					(void)pDoc->Spectra.GetNext(pos);
				}
				// finaly, rebuild the ListBox
				pos = pDoc->Spectra.GetHeadPosition();
				while (pos) {
					CSpectrum *pSpec = pDoc->Spectra.GetNext(pos);
					m_subtractListBox.SetItemDataPtr(m_subtractListBox.AddString(pSpec->label), pSpec);
				}
			}
		}
		ShowSubtract(SW_HIDE);
		pView->disMode = normalMode;
		pDoc->SetMinMax();
		pDoc->SetModifiedFlag();
		pView->Invalidate();
	}
}


void CSubtractPage::OnBnClickedSubtractcancelbutton()
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->MDIGetActive());
	CNetPlotView *pView = STATIC_DOWNCAST(CNetPlotView, pChild->GetActiveView());
	CNetPlotDoc *pDoc = STATIC_DOWNCAST(CNetPlotDoc, pChild->GetActiveDocument());
	if (subtractMode == pView->disMode) {
		ShowSubtract(SW_HIDE);
		pView->disMode = normalMode;
		POSITION pos = pDoc->Spectra.GetHeadPosition();
		while (pos) {
			CSpectrum *pSpec = pDoc->Spectra.GetNext(pos);
			pSpec->FindMinMax();
		}
		pDoc->SetMinMax();
		pView->Invalidate();
	}
}


BOOL CSubtractPage::OnKillActive()
{
	OnBnClickedSubtractcancelbutton();

	return CPropertyPage::OnKillActive();
}


void CSubtractPage::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->MDIGetActive());
	CNetPlotView *pView = STATIC_DOWNCAST(CNetPlotView, pChild->GetActiveView());
	CNetPlotDoc *pDoc = STATIC_DOWNCAST(CNetPlotDoc, pChild->GetActiveDocument());

	// only respond to slider if it is calling the event and we are in subtract mode!
	if (IDC_SUBTRACTSLIDER==pScrollBar->GetDlgCtrlID() && subtractMode==pView->disMode && m_subtractListBox.GetSelCount()>0) {
		float scale = m_rightFloat - m_leftFloat;
		scale = 0.01f * float(m_subtractSlider.GetPos()) * scale + m_leftFloat;
		m_subtractFloat = scale;
		UpdateData(FALSE);
		POSITION pos = pDoc->Spectra.GetHeadPosition();
		while (pos) {
			CSpectrum *pSpec = pDoc->Spectra.GetNext(pos);
			pSpec->SubtractData(sum, scale);
			pSpec->FindMinMax(false);
		}
		pDoc->SetMinMax();
		pView->Invalidate();
	}

	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CSubtractPage::OnEnChangeLeftedit()
{
	UpdateData();
}


void CSubtractPage::OnEnChangeRightedit()
{
	UpdateData();
}


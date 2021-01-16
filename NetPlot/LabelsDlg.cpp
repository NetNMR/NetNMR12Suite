// LabelsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "..\NMRDataTypes.h"
#include "Spectrum.h"
#include "NetPlot.h"
#include "MainFrame.h"
#include "ChildFrame.h"
#include "NetPlotDoc.h"
#include "LabelsDlg.h"

#define FONTNUM 10
static int fontsize[FONTNUM] = {10, 12, 14, 16, 18, 20, 24, 28, 36, 44};
// CLabelsDlg dialog

IMPLEMENT_DYNAMIC(CLabelsDlg, CDialog)
CLabelsDlg::CLabelsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLabelsDlg::IDD, pParent)
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->MDIGetActive());
	pDoc = STATIC_DOWNCAST(CNetPlotDoc, pChild->GetActiveDocument() );
}

CLabelsDlg::~CLabelsDlg()
{
}

void CLabelsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LABELSEDIT, m_labelsEdit);
	DDX_Control(pDX, IDC_LABELSIZECOMBO, m_labelsizeCombo);
}


BEGIN_MESSAGE_MAP(CLabelsDlg, CDialog)
END_MESSAGE_MAP()


// CLabelsDlg message handlers

BOOL CLabelsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CString txt = "";
	POSITION pos = pDoc->Spectra.GetHeadPosition();
	while (pos) {
		CSpectrum *pSpec = pDoc->Spectra.GetNext(pos);
		txt += pSpec->label + "\r\n";
	}
	m_labelsEdit.SetWindowText(txt);
	int selection = 1;
	for (int i=0; i<FONTNUM; i++) {
		CString str;
		str.Format("%d", fontsize[i]);
		m_labelsizeCombo.AddString(str);
		if (pDoc->nLabelSize == fontsize[i])
			selection =i;
	}
	m_labelsizeCombo.SetCurSel(selection);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CLabelsDlg::OnOK()
{
	CString txt;
	m_labelsEdit.GetWindowText(txt);
	int index = 0;
	POSITION pos = pDoc->Spectra.GetHeadPosition();
	while (pos) {
		CSpectrum *pSpec = pDoc->Spectra.GetNext(pos);
		if (index >= 0)
			pSpec->label = txt.Tokenize("\r", index).Trim();
		else
			pSpec->label = "";
	}
	pDoc->nLabelSize = fontsize[m_labelsizeCombo.GetCurSel()];
	CDialog::OnOK();
}

// ParamDlg.cpp : implementation file
//

#include "stdafx.h"
#include "..\NMRDataTypes.h"
#include "Spectrum.h"
#include "netplot.h"
#include "ParamDlg.h"
#include "MainFrame.h"
#include "ChildFrame.h"
#include "NetPlotDoc.h"
#include "NetPlotView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CParamDlg dialog

#define FONTNUM 10
static int fontsize[FONTNUM] = {10, 12, 14, 16, 18, 20, 24, 28, 36, 44};

CParamDlg::CParamDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CParamDlg::IDD, pParent)
{
	m_Range = IDC_RANGEFULL;
	m_GlobalScaling = FALSE;
	m_LabelX = 0.0f;
	m_LabelY = 0.0f;
	m_OffsetX = 0.0f;
	m_OffsetY = 0.0f;
	m_RangeFrom = 0.0f;
	m_RangeTo = 0.0f;
	m_Width = 10.0f;
	m_Height = 10.0f;
	m_Title = _T("");
	m_YExpansion = 1.0f;
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->MDIGetActive());
	pDoc = STATIC_DOWNCAST(CNetPlotDoc, pChild->GetActiveDocument() );
}


void CParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_GLOBALSCALING, m_GlobalScaling);
//	DDX_Text(pDX, IDC_LABELNAME, m_Label);
//	DDV_MaxChars(pDX, m_Label, 32);
	DDX_Text(pDX, IDC_LABELX, m_LabelX);
	DDV_MinMaxFloat(pDX, m_LabelX, -30.f, 30.f);
	DDX_Text(pDX, IDC_LABELY, m_LabelY);
	DDV_MinMaxFloat(pDX, m_LabelY, -30.f, 30.f);
	DDX_Text(pDX, IDC_OFFSETX, m_OffsetX);
	DDV_MinMaxFloat(pDX, m_OffsetX, -30.f, 30.f);
	DDX_Text(pDX, IDC_OFFSETY, m_OffsetY);
	DDV_MinMaxFloat(pDX, m_OffsetY, -30.f, 30.f);
	DDX_Text(pDX, IDC_RANGEFROM, m_RangeFrom);
	DDX_Text(pDX, IDC_RANGETO, m_RangeTo);
	DDX_Text(pDX, IDC_SIZECX, m_Width);
	DDV_MinMaxFloat(pDX, m_Width, 0.1f, 30.f);
	DDX_Text(pDX, IDC_SIZECY, m_Height);
	DDV_MinMaxFloat(pDX, m_Height, 0.1f, 30.f);
	DDX_Text(pDX, IDC_TITLE, m_Title);
	DDV_MaxChars(pDX, m_Title, 256);
	DDX_Text(pDX, IDC_YEXPANSION, m_YExpansion);
	DDV_MinMaxFloat(pDX, m_YExpansion, 1.e-009f, 1.e+009f);
	DDX_Control(pDX, IDC_TITLEFONTCOMBO, m_titlefontCombo);
	DDX_Control(pDX, IDC_LABELFONTCOMBO, m_labelfontCombo);
	DDX_Control(pDX, IDC_AXISFONTCOMBO, m_axisfontCombo);
	DDX_Control(pDX, IDC_LABELCOMBO, m_labelCombo);
}


BEGIN_MESSAGE_MAP(CParamDlg, CDialog)
	ON_BN_CLICKED(IDC_RANGEFULL, OnRangefull)
	ON_BN_CLICKED(IDC_RANGEHERTZ, OnRangehertz)
	ON_BN_CLICKED(IDC_RANGEPPM, OnRangeppm)
	ON_EN_CHANGE(IDC_LABELCOMBO, OnChangeLabelname)
	ON_BN_CLICKED(IDC_AUTOARRANGE, OnAutoarrange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParamDlg message handlers

BOOL CParamDlg::OnInitDialog() 
{
	CWaitCursor wait;
	CDialog::OnInitDialog();
	m_GlobalScaling = pDoc->GlobalScaling;
//	m_Label = pDoc->Label;
	m_LabelX = pDoc->LabelX;
	m_LabelY = pDoc->LabelY;
	m_OffsetX = pDoc->OffsetX;
	m_OffsetY = pDoc->OffsetY;
	m_RangeFrom = pDoc->RangeFrom;
	m_RangeTo = pDoc->RangeTo;
	m_Width = pDoc->Width;
	m_Height = pDoc->Height;
	m_Title = pDoc->Title;
	m_YExpansion = pDoc->YExpansion;
	m_Range = pDoc->Range;
	OnRange(m_Range);
//	ShowLabelControls();
	GetDlgItem(IDC_AUTOARRANGE)->ShowWindow(pDoc->Spectra.GetCount()?SW_SHOW:SW_HIDE);
	CheckRadioButton(IDC_RANGEFULL, IDC_RANGEPPM, m_Range);
	UpdateData(FALSE);
	int seltitle=5, sellabel=2, selaxis=4;
	for (int i=0; i<FONTNUM; i++) {
		CString str;
		str.Format("%d", fontsize[i]);
		m_titlefontCombo.AddString(str);
		if (pDoc->nTitleSize == fontsize[i])
			seltitle = i;
		m_labelfontCombo.AddString(str);
		if (pDoc->nLabelSize == fontsize[i])
			sellabel = i;
		m_axisfontCombo.AddString(str);
		if (pDoc->nAxisSize == fontsize[i])
			selaxis = i;
	}
	m_titlefontCombo.SetCurSel(seltitle);
	m_labelfontCombo.SetCurSel(sellabel);
	m_axisfontCombo.SetCurSel(selaxis);
	CSpectrum *pSpec = pDoc->Spectra.GetHead();
	CStringList names;
	pSpec->FHGetNames(names);
	names.AddHead("<EMPTY>");
	names.AddHead("filename");
	while (names.GetCount())
		m_labelCombo.AddString(names.RemoveHead());
	if (pDoc->Label.IsEmpty())
		m_labelCombo.SelectString(-1, "<EMPTY>");
	else {
		//m_labelCombo.SelectString(-1, pDoc->Label);
		//we can't use SelectString because it isn't case sensitive!
		for (int i=0; i<m_labelCombo.GetCount(); i++) {
			CString str;
			m_labelCombo.GetLBText(i, str);
			if (0==pDoc->Label.Compare(str)) {
				m_labelCombo.SetCurSel(i);
				break;
			}
		}
	}
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CParamDlg::OnOK() 
{
	CWaitCursor wait;
	UpdateData( TRUE );
	if ( pDoc->Range != m_Range || 
			IsDifferent( pDoc->RangeFrom, m_RangeFrom ) ||
			IsDifferent( pDoc->RangeTo, m_RangeTo ) ) {
		pDoc->Range = (ViewRange)m_Range;
		if (m_RangeTo > m_RangeFrom) {
			float x = m_RangeFrom;
			m_RangeFrom = m_RangeTo;
			m_RangeTo = x;
		}
		pDoc->RangeFrom = m_RangeFrom;
		pDoc->RangeTo = m_RangeTo;
		pDoc->WhatChanged |= RANGECHANGE;
	}
	if ( pDoc->GlobalScaling != m_GlobalScaling ) {
		pDoc->GlobalScaling = m_GlobalScaling;
		pDoc->WhatChanged |= GLOBALCHANGE;
	}
	CString selected;
	m_labelCombo.GetLBText(m_labelCombo.GetCurSel(), selected);
	if (selected == "<EMPTY>")
		selected.Empty();
	if ( pDoc->Label != selected) {
		pDoc->Label = selected;
		pDoc->WhatChanged |= LABELCHANGE;
	}
	if ( IsDifferent( pDoc->LabelX, m_LabelX ) || 
			IsDifferent( pDoc->LabelY, m_LabelY ) ) {
		pDoc->LabelX = m_LabelX;
		pDoc->LabelY = m_LabelY;
		pDoc->WhatChanged |= POSITIONCHANGE;
	}
	if ( IsDifferent( pDoc->OffsetX, m_OffsetX ) ||
			IsDifferent( pDoc->OffsetY, m_OffsetY ) || 
			IsDifferent( pDoc->Width, m_Width ) || 
			IsDifferent( pDoc->Height, m_Height ) ) {
		pDoc->OffsetX = m_OffsetX;
		pDoc->OffsetY = m_OffsetY;
		pDoc->Width = m_Width;
		pDoc->Height = m_Height;
		pDoc->WhatChanged |= SIZECHANGE;
	}
	if ( pDoc->Title != m_Title) {
		pDoc->Title = m_Title;
		pDoc->WhatChanged |= TITLECHANGE;
	}
	if ( IsDifferent( pDoc->YExpansion, m_YExpansion ) ) {
		pDoc->YExpansion = m_YExpansion;
		pDoc->WhatChanged |= EXPANSIONCHANGE;
	}
	int nSize = fontsize[m_titlefontCombo.GetCurSel()];
	if (theApp.RegistryInt(ReadInt, TitleFontSize, 20)!=nSize ) {
		pDoc->nTitleSize = nSize;
		theApp.RegistryInt(WriteInt, TitleFontSize, nSize);
		pDoc->WhatChanged |= FONTCHANGE;
	}
	nSize = fontsize[m_labelfontCombo.GetCurSel()];
	if (theApp.RegistryInt(ReadInt, LabelFontSize, 14)!=nSize ) {
		pDoc->nLabelSize = nSize;
		theApp.RegistryInt(WriteInt, LabelFontSize, nSize);
		pDoc->WhatChanged |= FONTCHANGE;
	}
	nSize = fontsize[m_axisfontCombo.GetCurSel()];
	if (theApp.RegistryInt(ReadInt, AxisFontSize, 18)!=nSize ) {
		pDoc->nAxisSize = nSize;
		theApp.RegistryInt(WriteInt, AxisFontSize, nSize);
		pDoc->WhatChanged |= FONTCHANGE;
	}
	CDialog::OnOK();
}

void CParamDlg::OnRange(int rangeid)
{
	switch (m_Range) {
		case IDC_RANGEFULL:
			{
				if (rangeid == IDC_RANGEPPM) {
					m_RangeFrom = pDoc->PPMFrom;
					m_RangeTo = pDoc->PPMTo;
				} else if (rangeid == IDC_RANGEHERTZ) {
					m_RangeFrom = pDoc->HertzFrom;
					m_RangeTo = pDoc->HertzTo;
				}
			}
			break;
		case IDC_RANGEHERTZ:
			if (rangeid == IDC_RANGEPPM) {
				CSpectrum *pSpec = pDoc->Spectra.GetHead();
				double sf = pSpec->Frequency();
				if (sf) {
					m_RangeFrom /= (float)sf;
					m_RangeTo /= (float)sf;
				}
			}
			break;
		case IDC_RANGEPPM:
			if (rangeid == IDC_RANGEHERTZ) {
				CSpectrum *pSpec = pDoc->Spectra.GetHead();
				double sf = pSpec->Frequency();
				if (sf) {
					m_RangeFrom *= (float)sf;
					m_RangeTo *= (float)sf;
				}
			}
			break;
	}
	m_Range = rangeid;
	GetDlgItem( IDC_FROMPROMPT )->ShowWindow((m_Range!=IDC_RANGEFULL) ? SW_SHOW : SW_HIDE);
	GetDlgItem( IDC_TOPROMPT   )->ShowWindow((m_Range!=IDC_RANGEFULL) ? SW_SHOW : SW_HIDE);
	GetDlgItem( IDC_RANGEFROM  )->ShowWindow((m_Range!=IDC_RANGEFULL) ? SW_SHOW : SW_HIDE);
	GetDlgItem( IDC_RANGETO    )->ShowWindow((m_Range!=IDC_RANGEFULL) ? SW_SHOW : SW_HIDE);
//	CheckRadioButton(IDC_RANGEFULL, IDC_RANGEPPM, rangeid);
	UpdateData(FALSE);
}

void CParamDlg::OnRangefull() 
{
	m_RangeFrom = m_RangeTo = 0.0f;
	OnRange(IDC_RANGEFULL);
}

void CParamDlg::OnRangehertz() 
{
	OnRange(IDC_RANGEHERTZ);
}

void CParamDlg::OnRangeppm() 
{
	OnRange(IDC_RANGEPPM);	
}

void CParamDlg::OnChangeLabelname() 
{
	UpdateData( TRUE );
//	ShowLabelControls();
}

//void CParamDlg::ShowLabelControls()
//{
//	int labelisempty = m_Label.IsEmpty();
//	GetDlgItem( IDC_LABELX )->ShowWindow( labelisempty?SW_HIDE:SW_SHOW );
//	GetDlgItem( IDC_LABELXPROMPT )->ShowWindow( labelisempty?SW_HIDE:SW_SHOW );
//	GetDlgItem( IDC_LABELY )->ShowWindow( labelisempty?SW_HIDE:SW_SHOW );
//	GetDlgItem( IDC_LABELYPROMPT )->ShowWindow( labelisempty?SW_HIDE:SW_SHOW );
//}

void CParamDlg::OnAutoarrange() 
{
	UpdateData( true );
	if (0==m_Range)
		OnRange(IDC_RANGEPPM);

	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->MDIGetActive());
	CNetPlotView *pView = STATIC_DOWNCAST(CNetPlotView, pChild->GetActiveView());

	pView->AutoArrange(m_Width, m_Height, m_OffsetX, m_OffsetY, m_LabelX, m_LabelY);

	UpdateData(FALSE);
}

bool CParamDlg::IsDifferent(float a, float b)
{
	CString A, B;
	A.Format( "%g", a );
	B.Format( "%g", b );
	return A != B;
}

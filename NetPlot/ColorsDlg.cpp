// ColorsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "..\NMRDataTypes.h"
#include "Spectrum.h"
#include "NetPlot.h"
#include "MainFrame.h"
#include "ChildFrame.h"
#include "NetPlotDoc.h"
#include "ColorsDlg.h"


// CColorsDlg dialog

IMPLEMENT_DYNAMIC(CColorsDlg, CDialog)
CColorsDlg::CColorsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CColorsDlg::IDD, pParent)
{
}

CColorsDlg::~CColorsDlg()
{
}

void CColorsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BACKGROUNDCOLORCOMBO, m_cboxBkgrndColor);
	DDX_Control(pDX, IDC_BASELINECOLORCOMBO, m_cboxBaselineColor);
	DDX_Control(pDX, IDC_INTEGRALCOLORCOMBO, m_cboxIntegralColor);
	DDX_Control(pDX, IDC_AXISCOLORCOMBO, m_cboxAxisColor);
	DDX_Control(pDX, IDC_TITLECOLORCOMBO, m_cboxTitleColor);
	DDX_Control(pDX, IDC_MAINCURSORCOLORCOMBO, m_cboxMainCursorColor);
	DDX_Control(pDX, IDC_ZOOMCURSORCOLORCOMBO, m_cboxZoomCursorColor);
	DDX_Control(pDX, IDC_CSPECTRUMCOMBO, m_cboxCSpectrum);
	DDX_Control(pDX, IDC_SCOLORCOMBO, m_cboxSColor);
	DDX_Control(pDX, IDC_ALLCOLORCHECK, m_buttonAllSpectra);
}


BEGIN_MESSAGE_MAP(CColorsDlg, CDialog)
	ON_BN_CLICKED(IDC_BLKONWHTBUTTON, OnBnClickedBlkonwhtbutton)
	ON_BN_CLICKED(IDC_WHTONBLKBUTTON, OnBnClickedWhtonblkbutton)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_CSPECTRUMCOMBO, &CColorsDlg::OnCbnSelchangeCspectrumcombo)
	ON_CBN_SELCHANGE(IDC_SCOLORCOMBO, &CColorsDlg::OnCbnSelchangeScolorcombo)
	ON_BN_CLICKED(IDC_ALLCOLORCHECK, &CColorsDlg::OnBnClickedAllcolorcheck)
END_MESSAGE_MAP()


// CColorsDlg message handlers

void CColorsDlg::OnBnClickedBlkonwhtbutton()
{
	m_buttonAllSpectra.SetCheck(BST_CHECKED);
	m_cboxCSpectrum.EnableWindow(FALSE);
	m_cboxBkgrndColor.SetCurSel(8);
	m_cboxSColor.SetCurSel(0);
	m_cboxIntegralColor.SetCurSel(5);
	m_cboxAxisColor.SetCurSel(0);
	m_cboxTitleColor.SetCurSel(0);
	m_cboxMainCursorColor.SetCurSel(1);
	m_cboxZoomCursorColor.SetCurSel(3);
	m_cboxBaselineColor.SetCurSel(6);
	for (int i=0; i<m_cboxCSpectrum.GetCount(); i++)
		m_cboxCSpectrum.SetItemData(i, 0);
}

void CColorsDlg::OnBnClickedWhtonblkbutton()
{
	m_buttonAllSpectra.SetCheck(BST_CHECKED);
	m_cboxCSpectrum.EnableWindow(FALSE);
	m_cboxBkgrndColor.SetCurSel(0);
	m_cboxSColor.SetCurSel(8);
	m_cboxIntegralColor.SetCurSel(5);
	m_cboxAxisColor.SetCurSel(8);
	m_cboxTitleColor.SetCurSel(8);
	m_cboxMainCursorColor.SetCurSel(1);
	m_cboxZoomCursorColor.SetCurSel(3);
	m_cboxBaselineColor.SetCurSel(6);
	for (int i=0; i<m_cboxCSpectrum.GetCount(); i++)
		m_cboxCSpectrum.SetItemData(i, 8);
}

void CColorsDlg::OnBnClickedOk()
{
	bool restart = false;
	if (theApp.RegistryInt(ReadInt, BackgroundColor, 8) != m_cboxBkgrndColor.GetCurSel())
		restart = true;
	if (theApp.RegistryInt(ReadInt, MainCursorColor, 8) != m_cboxMainCursorColor.GetCurSel())
		restart = true;
	if (theApp.RegistryInt(ReadInt, ZoomCursorColor, 8) != m_cboxZoomCursorColor.GetCurSel())
		restart = true;
	if (restart)
		AfxMessageBox("You must restart NetPlot before the new colors will be correct.", MB_ICONINFORMATION);
	theApp.RegistryInt(WriteInt, BackgroundColor, m_cboxBkgrndColor.GetCurSel());
	if (BST_CHECKED == m_buttonAllSpectra.GetCheck())
		theApp.RegistryInt(WriteInt, SpectraColor, m_cboxSColor.GetCurSel());
	theApp.RegistryInt(WriteInt, IntegralColor, m_cboxIntegralColor.GetCurSel());
	theApp.RegistryInt(WriteInt, AxisColor, m_cboxAxisColor.GetCurSel());
	theApp.RegistryInt(WriteInt, TitleColor, m_cboxTitleColor.GetCurSel());
	theApp.RegistryInt(WriteInt, MainCursorColor, m_cboxMainCursorColor.GetCurSel());
	theApp.RegistryInt(WriteInt, ZoomCursorColor, m_cboxZoomCursorColor.GetCurSel());
	theApp.RegistryInt(WriteInt, BaselineColor, m_cboxBaselineColor.GetCurSel());
	// store new color selections in Spectra.color, if appropriate
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, theApp.m_pMainWnd);
	CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->MDIGetActive());
	if (pChild) {
		CNetPlotDoc *pDoc = STATIC_DOWNCAST(CNetPlotDoc, pChild->GetActiveDocument());
		POSITION pos = pDoc->Spectra.GetHeadPosition();
		int i = 0;
		while (pos) {
			CSpectrum *pSpec = pDoc->Spectra.GetNext(pos);
			int newcolor = (int)m_cboxCSpectrum.GetItemData(i++);
			if (newcolor != pSpec->color) {
				pSpec->color = newcolor;
				pDoc->SetModifiedFlag();
			}
		}
	}
	OnOK();
}

BOOL CColorsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	// fill all the color selection comboboxes
	for (int i=0; i<theApp.nColors; i++) {
		m_cboxSColor.AddString(theApp.arrColors[i].name);
		m_cboxBkgrndColor.AddString(theApp.arrColors[i].name);
		m_cboxIntegralColor.AddString(theApp.arrColors[i].name);
		m_cboxAxisColor.AddString(theApp.arrColors[i].name);
		m_cboxTitleColor.AddString(theApp.arrColors[i].name);
		m_cboxMainCursorColor.AddString(theApp.arrColors[i].name);
		m_cboxZoomCursorColor.AddString(theApp.arrColors[i].name);
		m_cboxBaselineColor.AddString(theApp.arrColors[i].name);
	}
	// initialize the color selection comboboxes
	m_cboxBkgrndColor.SetCurSel(theApp.RegistryInt(ReadInt, BackgroundColor, 8));
	m_cboxSColor.SetCurSel(theApp.RegistryInt(ReadInt, SpectraColor, 0));
	m_cboxIntegralColor.SetCurSel(theApp.RegistryInt(ReadInt, IntegralColor, 5));
	m_cboxAxisColor.SetCurSel(theApp.RegistryInt(ReadInt, AxisColor, 0));
	m_cboxTitleColor.SetCurSel(theApp.RegistryInt(ReadInt, TitleColor, 0));
	m_cboxMainCursorColor.SetCurSel(theApp.RegistryInt(ReadInt, MainCursorColor, 1));
	m_cboxZoomCursorColor.SetCurSel(theApp.RegistryInt(ReadInt, ZoomCursorColor, 3));
	m_cboxBaselineColor.SetCurSel(theApp.RegistryInt(ReadInt, BaselineColor, 6));
	// what are the current colors of each spectrum?
	// are they all the same?
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, theApp.m_pMainWnd);
	CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->MDIGetActive());
	if (pChild) {
		bool thesame = true;
		CNetPlotDoc *pDoc = STATIC_DOWNCAST(CNetPlotDoc, pChild->GetActiveDocument());
		if (pDoc->Spectra.GetCount()) {
			int color = pDoc->Spectra.GetHead()->color;
			POSITION pos = pDoc->Spectra.GetHeadPosition();
			while (pos) {
				CSpectrum *pSpec = pDoc->Spectra.GetNext(pos);
				if (pSpec->label.IsEmpty()) {
					CString str = pSpec->pathname;
					str.MakeReverse();
					int i = str.FindOneOf("/\\");
					if (i > 1)
						str = str.Left(i);
					else {
						i = str.FindOneOf(":");
						if (i > 1)
							str = str.Left(i);
					}
					str.MakeReverse();
					m_cboxCSpectrum.SetItemData(m_cboxCSpectrum.AddString(str), pSpec->color);
				} else
					m_cboxCSpectrum.SetItemData(m_cboxCSpectrum.AddString(pSpec->label), pSpec->color);
				if (color != pSpec->color)
					thesame = false;
			}
			m_cboxCSpectrum.SetCurSel(0);
			m_cboxSColor.SetCurSel(color);
		} else
			m_buttonAllSpectra.EnableWindow(FALSE);
		m_buttonAllSpectra.SetCheck(thesame ? BST_CHECKED : BST_UNCHECKED);
		m_cboxCSpectrum.EnableWindow(thesame ? FALSE : TRUE);
	} else {
		m_buttonAllSpectra.SetCheck(BST_CHECKED);
		m_buttonAllSpectra.EnableWindow(FALSE);
		m_cboxCSpectrum.EnableWindow(FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CColorsDlg::OnCbnSelchangeCspectrumcombo()
{
	int color = (int)m_cboxCSpectrum.GetItemData(m_cboxCSpectrum.GetCurSel());
	m_cboxSColor.SetCurSel(color);
}

void CColorsDlg::OnCbnSelchangeScolorcombo()
{
	int color = m_cboxSColor.GetCurSel();
	if (m_cboxCSpectrum.IsWindowEnabled())
		m_cboxCSpectrum.SetItemData(m_cboxCSpectrum.GetCurSel(), color);
	else if (m_buttonAllSpectra.IsWindowEnabled() && BST_CHECKED == m_buttonAllSpectra.GetCheck())
		for (int i=0, color=m_cboxSColor.GetCurSel(); i<m_cboxCSpectrum.GetCount(); i++)
			m_cboxCSpectrum.SetItemData(i, color);
}

void CColorsDlg::OnBnClickedAllcolorcheck()
{
	if (BST_CHECKED == m_buttonAllSpectra.GetCheck()) {
		// SET ALL SPECTRA COLORS
		for (int i=0, color=m_cboxSColor.GetCurSel(); i<m_cboxCSpectrum.GetCount(); i++)
			m_cboxCSpectrum.SetItemData(i, color);
	}
	m_cboxCSpectrum.EnableWindow(BST_UNCHECKED == m_buttonAllSpectra.GetCheck());
}

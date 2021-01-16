// ColorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ILT.h"
#include "ColorDlg.h"
#include "MainFrm.h"

// CStaticRect

IMPLEMENT_DYNAMIC(CStaticRect, CStatic)
CStaticRect::CStaticRect()
{
	EnableAutomation();
}

CStaticRect::~CStaticRect()
{
}

BEGIN_MESSAGE_MAP(CStaticRect, CStatic)
END_MESSAGE_MAP()

// CStaticRect message handlers


void CStaticRect::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	CBrush brush(color);
	FillRect(lpDIS->hDC, &lpDIS->rcItem, HBRUSH(brush));
}


// CColorDlg dialog

IMPLEMENT_DYNAMIC(CColorDlg, CDialog)

CColorDlg::CColorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CColorDlg::IDD, pParent)
{
	memcpy(colors, theApp.Colors, NCOLORS*sizeof(COLORREF));
}

CColorDlg::~CColorDlg()
{
}

void CColorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COLORCOMBO, m_cboxName);
	DDX_Control(pDX, IDC_RECTSTATIC, m_staticRect);
}


BEGIN_MESSAGE_MAP(CColorDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_COLORCOMBO, &CColorDlg::OnCbnSelchangeColorcombo)
	ON_BN_CLICKED(IDC_CHANGEBUTTON, &CColorDlg::OnBnClickedChangebutton)
	ON_BN_CLICKED(IDC_BONWBUTTON, &CColorDlg::OnBnClickedBonwbutton)
	ON_BN_CLICKED(IDC_WONBBUTTON, &CColorDlg::OnBnClickedWonbbutton)
	ON_BN_CLICKED(IDC_DEFAULTBUTTON, &CColorDlg::OnBnClickedDefaultbutton)
END_MESSAGE_MAP()


// CColorDlg message handlers

BOOL CColorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_cboxName.AddString("Observed Data");
	m_cboxName.AddString("Calculated");
	m_cboxName.AddString("Difference");
	m_cboxName.AddString("Spectrum");
	m_cboxName.AddString("Error Ellipse");
	m_cboxName.AddString("Text");
	m_cboxName.AddString("Axis");
	m_cboxName.AddString("Background");
	m_cboxName.AddString("MonteCarlo#1");
	m_cboxName.AddString("MonteCarlo#2");
	m_cboxName.AddString("MonteCarlo#3");
	m_cboxName.AddString("MonteCarlo#4");
	m_cboxName.AddString("MonteCarlo#5");
	m_cboxName.AddString("MonteCarlo#6");
	m_cboxName.AddString("MonteCarlo#7");
	m_cboxName.AddString("MonteCarlo#8");
	m_cboxName.AddString("MonteCarlo#9");
	m_cboxName.AddString("MonteCarlo#10+");
	m_cboxName.SetCurSel(0);
	OnCbnSelchangeColorcombo();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CColorDlg::OnOK()
{
	memcpy(theApp.Colors, colors, NCOLORS*sizeof(COLORREF));
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, theApp.m_pMainWnd);
	pMain->GetActiveDocument()->UpdateAllViews(NULL);

	CDialog::OnOK();
}

void CColorDlg::OnCbnSelchangeColorcombo()
{
	m_staticRect.color = colors[m_cboxName.GetCurSel()];
	m_staticRect.Invalidate();
}

void CColorDlg::OnBnClickedChangebutton()
{
	CColorDialog dlg(m_staticRect.color, CC_ANYCOLOR);
	if (IDOK == dlg.DoModal()) {
		m_staticRect.color = colors[m_cboxName.GetCurSel()] = dlg.GetColor();
		m_staticRect.Invalidate();
	}
}

void CColorDlg::OnBnClickedBonwbutton()
{
	colors[0]   = RGB(  0,   0,   0);	//  0 Data
	colors[1]   = RGB(127, 127, 127);	//  1 Calc
	colors[2]   = RGB(  0,   0,   0);	//  2 Diff
	colors[3]   = RGB(  0,   0,   0);	//  3 Spec
	colors[4]   = RGB(127, 127, 127);	//  4 Error
	colors[5]   = RGB(  0,   0,   0);	//  5 Text
	colors[6]   = RGB(  0,   0,   0);	//  6 Axis
	colors[7]   = RGB(255, 255, 255);	//  7 Bkgrnd
	colors[8]   = RGB(  0,   0,   0);	//  8 MC1
	colors[9]   = RGB(  0,   0,   0);	//  9 MC2
	colors[10]  = RGB(  0,   0,   0);	// 10 MC3
	colors[11]  = RGB(  0,   0,   0);	// 11 MC4
	colors[12]  = RGB(  0,   0,   0);	// 12 MC5
	colors[13]  = RGB(  0,   0,   0);	// 13 MC6
	colors[14]  = RGB(  0,   0,   0);	// 14 MC7
	colors[15]  = RGB(  0,   0,   0);	// 15 MC8
	colors[16]  = RGB(  0,   0,   0);	// 16 MC9
	colors[17]  = RGB(  0,   0,   0);	// 17 MC10+
	m_staticRect.color = colors[m_cboxName.GetCurSel()];
	m_staticRect.Invalidate();
}

void CColorDlg::OnBnClickedWonbbutton()
{
	colors[0]  = RGB(255, 255, 255);	//  0 Data
	colors[1]  = RGB(127, 127, 127);	//  1 Calc
	colors[2]  = RGB(255, 255, 255);	//  2 Diff
	colors[3]  = RGB(255, 255, 255);	//  3 Spec
	colors[4]  = RGB(127, 127, 127);	//  4 Error
	colors[5]  = RGB(255, 255, 255);	//  5 Text
	colors[6]  = RGB(255, 255, 255);	//  6 Axis
	colors[7]  = RGB(  0,   0,   0);	//  7 Bkgrnd
	colors[6]  = RGB(255, 255, 255);	//  8 MC1
	colors[6]  = RGB(255, 255, 255);	//  9 MC2
	colors[6]  = RGB(255, 255, 255);	// 10 MC3
	colors[6]  = RGB(255, 255, 255);	// 11 MC4
	colors[6]  = RGB(255, 255, 255);	// 12 MC5
	colors[6]  = RGB(255, 255, 255);	// 13 MC6
	colors[6]  = RGB(255, 255, 255);	// 14 MC7
	colors[6]  = RGB(255, 255, 255);	// 15 MC8
	colors[6]  = RGB(255, 255, 255);	// 16 MC9
	colors[6]  = RGB(255, 255, 255);	// 17 MC10+
	m_staticRect.color = colors[m_cboxName.GetCurSel()];
	m_staticRect.Invalidate();
}

void CColorDlg::OnBnClickedDefaultbutton()
{
	theApp.SetDefaultColors(colors);
	m_staticRect.color = colors[m_cboxName.GetCurSel()];
	m_staticRect.Invalidate();
}

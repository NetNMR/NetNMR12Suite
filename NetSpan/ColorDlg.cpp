// ColorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NetSpan.h"
#include "ColorDlg.h"
#include ".\colordlg.h"


// CColorDlg dialog

IMPLEMENT_DYNAMIC(CColorDlg, CDialog)
CColorDlg::CColorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CColorDlg::IDD, pParent)
{
}

CColorDlg::~CColorDlg()
{
}

void CColorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BKGR_CBOX, m_bkgrCbox);
	DDX_Control(pDX, IDC_SPEC_CBOX, m_specCbox);
	DDX_Control(pDX, IDC_CALC_CBOX, m_calcCbox);
	DDX_Control(pDX, IDC_SING_CBOX, m_singCbox);
	DDX_Control(pDX, IDC_DIFF_CBOX, m_diffCbox);
	DDX_Control(pDX, IDC_LABL_CBOX, m_lablCbox);
}


BEGIN_MESSAGE_MAP(CColorDlg, CDialog)
END_MESSAGE_MAP()


// CColorDlg message handlers

BOOL CColorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	for (int i=0; i<theApp.nColors; i++) {
		m_bkgrCbox.AddString(theApp.arrColors[i].name);
		m_specCbox.AddString(theApp.arrColors[i].name);
		m_calcCbox.AddString(theApp.arrColors[i].name);
		m_singCbox.AddString(theApp.arrColors[i].name);
		m_diffCbox.AddString(theApp.arrColors[i].name);
		m_lablCbox.AddString(theApp.arrColors[i].name);
	}
	m_bkgrCbox.SetCurSel(theApp.RegistryInt(ReadInt, BkgrColor, 0));
	m_specCbox.SetCurSel(theApp.RegistryInt(ReadInt, SpecColor, 2));
	m_calcCbox.SetCurSel(theApp.RegistryInt(ReadInt, CalcColor, 8));
	m_singCbox.SetCurSel(theApp.RegistryInt(ReadInt, SingColor, 4));
	m_diffCbox.SetCurSel(theApp.RegistryInt(ReadInt, DiffColor, 1));
	m_lablCbox.SetCurSel(theApp.RegistryInt(ReadInt, LablColor, 7));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CColorDlg::OnOK()
{
	theApp.RegistryInt(WriteInt, BkgrColor, m_bkgrCbox.GetCurSel());
	theApp.RegistryInt(WriteInt, SpecColor, m_specCbox.GetCurSel());
	theApp.RegistryInt(WriteInt, CalcColor, m_calcCbox.GetCurSel());
	theApp.RegistryInt(WriteInt, SingColor, m_singCbox.GetCurSel());
	theApp.RegistryInt(WriteInt, DiffColor, m_diffCbox.GetCurSel());
	theApp.RegistryInt(WriteInt, LablColor, m_lablCbox.GetCurSel());
	CDialog::OnOK();
}

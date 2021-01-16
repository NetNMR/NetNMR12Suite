// KernelDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ILT.h"
#include "KernelDlg.h"


// CKernelDlg dialog

IMPLEMENT_DYNAMIC(CKernelDlg, CDialog)

CKernelDlg::CKernelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKernelDlg::IDD, pParent)
	, m_dblDACtoG(0.0)
	, m_dblTimeCubed(0.0)
	, m_dblGamma(0.0)
	, m_dblViscosity(0.1)
	, m_dblTemperature(25.0)
	, isrealdata(true)
{
	kernelType = typeT2;
}

CKernelDlg::~CKernelDlg()
{
}

void CKernelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_DACTOGEDIT, m_dblDACtoG);
	DDV_MinMaxDouble(pDX, m_dblDACtoG, 1.0e-6, 1.0e6);
	DDX_Text(pDX, IDC_TIMECUBEDEDIT, m_dblTimeCubed);
	DDV_MinMaxDouble(pDX, m_dblTimeCubed, 1.0e-10, 1.0e10);
	DDX_Text(pDX, IDC_GAMMAEDIT, m_dblGamma);
	DDV_MinMaxDouble(pDX, m_dblGamma, 1.0e-6, 1.0e15);
	DDX_Text(pDX, IDC_VISCOSITYEDIT, m_dblViscosity);
	DDV_MinMaxDouble(pDX, m_dblViscosity, 1.0e-6, 1.0e15);
	DDX_Text(pDX, IDC_TEMPERATUREEDIT, m_dblTemperature);
	DDV_MinMaxDouble(pDX, m_dblTemperature, -200.0, 200.0);
	DDX_Control(pDX, IDC_SOLVENTCOMBO, m_comboSolvent);
}


BEGIN_MESSAGE_MAP(CKernelDlg, CDialog)
	ON_BN_CLICKED(IDC_T2KERNELRADIO, &CKernelDlg::OnBnClickedkernelradio)
	ON_BN_CLICKED(IDC_T1KERNELRADIO, &CKernelDlg::OnBnClickedkernelradio)
	ON_BN_CLICKED(IDC_DOSYKERNELRADIO, &CKernelDlg::OnBnClickedkernelradio)
	ON_BN_CLICKED(IDC_KERNELHELPBUTTON, &CKernelDlg::OnBnClickedKernelhelpbutton)
	ON_CBN_SELCHANGE(IDC_SOLVENTCOMBO, &CKernelDlg::OnCbnSelchangeSolventcombo)
END_MESSAGE_MAP()


// CKernelDlg message handlers

void CKernelDlg::OnBnClickedkernelradio()
{
	int id = GetCheckedRadioButton(IDC_T2KERNELRADIO, IDC_DOSYKERNELRADIO);
	GetDlgItem(IDC_DACTOGEDIT)->EnableWindow(id==IDC_DOSYKERNELRADIO);
	GetDlgItem(IDC_TIMECUBEDEDIT)->EnableWindow(id==IDC_DOSYKERNELRADIO);
	GetDlgItem(IDC_GAMMAEDIT)->EnableWindow(id==IDC_DOSYKERNELRADIO);
	GetDlgItem(IDC_SOLVENTCOMBO)->EnableWindow(id==IDC_DOSYKERNELRADIO && isrealdata);
	GetDlgItem(IDC_VISCOSITYEDIT)->EnableWindow(id==IDC_DOSYKERNELRADIO && isrealdata);
	GetDlgItem(IDC_TEMPERATUREEDIT)->EnableWindow(id==IDC_DOSYKERNELRADIO && isrealdata);
	switch (id) {
		case IDC_T2KERNELRADIO:
			kernelType = typeT2;
			break;
		case IDC_T1KERNELRADIO:
			kernelType = typeT1;
			break;
		case IDC_DOSYKERNELRADIO:
			kernelType = typeDosy;
			break;
	}
}

BOOL CKernelDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_comboSolvent.AddString("Unknown");
	m_comboSolvent.SetItemData( m_comboSolvent.AddString("Acetone"), 306);
	m_comboSolvent.SetItemData( m_comboSolvent.AddString("Benzene"), 604);
	m_comboSolvent.SetItemData( m_comboSolvent.AddString("Chloroform"), 380);
	m_comboSolvent.SetItemData( m_comboSolvent.AddString("DMSO"), 1996);
	m_comboSolvent.SetItemData( m_comboSolvent.AddString("Ethanol"), 1074);
	m_comboSolvent.SetItemData( m_comboSolvent.AddString("Ethylene Glycol"), 16100);
	m_comboSolvent.SetItemData( m_comboSolvent.AddString("Methanol"), 544);
	m_comboSolvent.SetItemData( m_comboSolvent.AddString("Methylene Chloride"), 440);
	m_comboSolvent.SetItemData( m_comboSolvent.AddString("Nitro Benezene"), 18630);
	m_comboSolvent.SetItemData( m_comboSolvent.AddString("Propanol"), 1945);
	m_comboSolvent.SetItemData( m_comboSolvent.AddString("Olive Oil"), 81000);
	m_comboSolvent.SetItemData( m_comboSolvent.AddString("Water"), 894);

	int id = m_comboSolvent.FindStringExact(1, m_strSolvent);
	if (CB_ERR == id) id = 0;
	m_comboSolvent.SetCurSel(id);
	OnCbnSelchangeSolventcombo();

	switch (kernelType) {
		case typeT2:
			id = IDC_T2KERNELRADIO;
			break;
		case typeT1:
			id = IDC_T1KERNELRADIO;
			break;
		case typeDosy:
			id = IDC_DOSYKERNELRADIO;
			break;
	}

	CheckRadioButton(IDC_T2KERNELRADIO, IDC_DOSYKERNELRADIO, id);
	OnBnClickedkernelradio();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CKernelDlg::OnBnClickedKernelhelpbutton()
{
	theApp.HtmlHelp(0x18007);
}

void CKernelDlg::OnCbnSelchangeSolventcombo()
{
	int sel = m_comboSolvent.GetCurSel();
	m_comboSolvent.GetLBText(sel, m_strSolvent);
	if (sel > 0) {
		m_dblViscosity = 1.0e-6 * double(m_comboSolvent.GetItemData(sel));
		UpdateData(FALSE);
	}
}

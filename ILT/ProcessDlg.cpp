// ProcessDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ILT.h"
#include "ProcessDlg.h"
#include "MainFrm.h"
#include "ILTDoc.h"


// CProcessDlg dialog

IMPLEMENT_DYNAMIC(CProcessDlg, CDialog)

CProcessDlg::CProcessDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProcessDlg::IDD, pParent)
	, m_dblILTFrom(0.01)
	, m_dblILTTo(10000.0)
	, m_intILTCount(100)
	, m_dblILTalpha(0.0)
	, m_intSIRTCount(100000)
	, m_dblSIRTTolerance(1.0e-6)
	, m_boolGlobalCheck(FALSE)
	, m_boolSimplifyCheck(FALSE)
	, m_boolOptimizeCheck(FALSE)
	, m_intMethod(IDC_EGLMETHODRADIO)
	, m_intConfidence(0)
{

}

CProcessDlg::~CProcessDlg()
{
}

void CProcessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ILTFROMEDIT, m_dblILTFrom);
	DDV_MinMaxDouble(pDX, m_dblILTFrom, 1.0e-6, 1.0e10);
	DDX_Text(pDX, IDC_ILTTOEDIT, m_dblILTTo);
	DDV_MinMaxDouble(pDX, m_dblILTTo, 1.0e-4, 1.0e12);
	DDX_Text(pDX, IDC_ILTCOUNTEDIT, m_intILTCount);
	DDV_MinMaxInt(pDX, m_intILTCount, 10, 200);
	DDX_Text(pDX, IDC_ILTALPHAEDIT, m_dblILTalpha);
	DDV_MinMaxDouble(pDX, m_dblILTalpha, 0, 1.0e6);
	DDX_Text(pDX, IDC_SIRTCOUNTEDIT, m_intSIRTCount);
	DDV_MinMaxInt(pDX, m_intSIRTCount, 1, 10000000);
	DDX_Text(pDX, IDC_SIRTTOLERANCEEDIT, m_dblSIRTTolerance);
	DDV_MinMaxDouble(pDX, m_dblSIRTTolerance, 1.0e-12, 0.1);
	DDX_Check(pDX, IDC_GLOBALCHECK, m_boolGlobalCheck);
	DDX_Check(pDX, IDC_SIMPLIFYCHECK, m_boolSimplifyCheck);
	DDX_Check(pDX, IDC_OPTIMIZECHECK, m_boolOptimizeCheck);
}


BEGIN_MESSAGE_MAP(CProcessDlg, CDialog)
	ON_BN_CLICKED(IDC_SIMPLIFYCHECK, &CProcessDlg::OnBnClickedSimplifycheck)
	ON_BN_CLICKED(IDC_OPTIMIZECHECK, &CProcessDlg::OnBnClickedOptimizecheck)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_EGLMETHODRADIO, IDC_SIRTMETHODRADIO, &CProcessDlg::OnBnClickedMethod)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CI1RADIO, IDC_CI6RADIO, &CProcessDlg::OnBnClickedConfidence)
	ON_BN_CLICKED(IDC_ILTHELPBUTTON, &CProcessDlg::OnBnClickedIlthelpbutton)
END_MESSAGE_MAP()


// CProcessDlg message handlers

void CProcessDlg::OnBnClickedSimplifycheck()
{
	UpdateData(TRUE);
	m_boolOptimizeCheck = FALSE;
	if (m_boolSimplifyCheck)
		m_dblILTalpha = 0.0;
	UpdateData(FALSE);
	GetDlgItem(IDC_ILTALPHAEDIT)->EnableWindow(!m_boolSimplifyCheck && m_intMethod==IDC_ILTMETHODRADIO);
	GetDlgItem(IDC_OPTIMIZECHECK)->EnableWindow(m_boolSimplifyCheck);
	OnBnClickedOptimizecheck();
}

void CProcessDlg::OnBnClickedOptimizecheck()
{
	UpdateData(TRUE);
	BOOL bCI = m_boolOptimizeCheck ? 1 : 0;
	GetDlgItem(IDC_CI1RADIO)->EnableWindow(bCI);
	GetDlgItem(IDC_CI2RADIO)->EnableWindow(bCI);
	GetDlgItem(IDC_CI3RADIO)->EnableWindow(bCI);
	GetDlgItem(IDC_CI4RADIO)->EnableWindow(bCI);
	GetDlgItem(IDC_CI5RADIO)->EnableWindow(bCI);
	GetDlgItem(IDC_CI6RADIO)->EnableWindow(bCI);
}

void CProcessDlg::OnBnClickedMethod(UINT nID)
{
	m_intMethod = nID;
	GetDlgItem(IDC_ILTFROMEDIT)->EnableWindow(TRUE);
	GetDlgItem(IDC_ILTTOEDIT)->EnableWindow(TRUE);
	GetDlgItem(IDC_ILTCOUNTEDIT)->EnableWindow(m_intMethod!=IDC_EGLMETHODRADIO);
	GetDlgItem(IDC_SIRTCOUNTEDIT)->EnableWindow(m_intMethod==IDC_SIRTMETHODRADIO);
	GetDlgItem(IDC_SIRTTOLERANCEEDIT)->EnableWindow(m_intMethod==IDC_SIRTMETHODRADIO);
	if (IDC_SIRTMETHODRADIO==m_intMethod) {
		m_boolSimplifyCheck = FALSE;
		m_boolOptimizeCheck = FALSE;
		UpdateData(FALSE);
	}
	GetDlgItem(IDC_SIMPLIFYCHECK)->EnableWindow(m_intMethod!=IDC_SIRTMETHODRADIO);
	GetDlgItem(IDC_OPTIMIZECHECK)->EnableWindow(m_intMethod!=IDC_SIRTMETHODRADIO);
	if (m_boolSimplifyCheck || IDC_ILTMETHODRADIO!=m_intMethod) {
		m_dblILTalpha = 0.0;
		UpdateData(FALSE);
	}
	GetDlgItem(IDC_ILTALPHAEDIT)->EnableWindow(IDC_ILTMETHODRADIO==m_intMethod && !m_boolSimplifyCheck);
	OnBnClickedOptimizecheck();
}

void CProcessDlg::OnBnClickedConfidence(UINT nID)
{
	m_intConfidence = nID - IDC_CI1RADIO;
}

BOOL CProcessDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, theApp.m_pMainWnd);
	CILTDoc *pDoc = STATIC_DOWNCAST(CILTDoc, pMain->GetActiveDocument());
	GetDlgItem(IDC_GLOBALCHECK)->EnableWindow((pDoc->Decays.GetCount()>1)?TRUE:FALSE);
	CheckRadioButton(IDC_EGLMETHODRADIO, IDC_SIRTMETHODRADIO, m_intMethod);
	CheckRadioButton(IDC_CI1RADIO, IDC_CI6RADIO, m_intConfidence+IDC_CI1RADIO);
	GetDlgItem(IDC_ILTFROMEDIT)->EnableWindow(TRUE);
	GetDlgItem(IDC_ILTTOEDIT)->EnableWindow(TRUE);
	GetDlgItem(IDC_ILTCOUNTEDIT)->EnableWindow(IDC_EGLMETHODRADIO!=m_intMethod);
	GetDlgItem(IDC_ILTALPHAEDIT)->EnableWindow(IDC_ILTMETHODRADIO==m_intMethod && !m_boolSimplifyCheck);
	GetDlgItem(IDC_SIRTCOUNTEDIT)->EnableWindow(IDC_SIRTMETHODRADIO==m_intMethod);
	GetDlgItem(IDC_SIRTTOLERANCEEDIT)->EnableWindow(IDC_SIRTMETHODRADIO==m_intMethod);
	if (IDC_SIRTMETHODRADIO == m_intMethod) {
		m_boolSimplifyCheck = m_boolOptimizeCheck = FALSE;
	}
	GetDlgItem(IDC_SIMPLIFYCHECK)->EnableWindow(IDC_SIRTMETHODRADIO!=m_intMethod);
	GetDlgItem(IDC_OPTIMIZECHECK)->EnableWindow(IDC_SIRTMETHODRADIO!=m_intMethod && m_boolSimplifyCheck);
	OnBnClickedOptimizecheck();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CProcessDlg::OnOK()
{
	CDialog::OnOK();
	if (m_dblILTFrom > m_dblILTTo) {
		double tmp = m_dblILTFrom;
		m_dblILTFrom = m_dblILTTo;
		m_dblILTTo = tmp;
	}
	if (m_dblILTFrom == m_dblILTTo)
		m_dblILTTo = 10.0 * m_dblILTFrom;
}
void CProcessDlg::OnBnClickedIlthelpbutton()
{
	theApp.HtmlHelp(0x18006);
}

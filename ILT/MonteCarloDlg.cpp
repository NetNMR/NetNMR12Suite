// MonteCarloDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ILT.h"
#include "MonteCarloDlg.h"
#include "MainFrm.h"
#include "ILTDoc.h"
#include "Decay.h"


// CMonteCarloDlg dialog

IMPLEMENT_DYNAMIC(CMonteCarloDlg, CDialog)

CMonteCarloDlg::CMonteCarloDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMonteCarloDlg::IDD, pParent)
	, m_dblAmplitude(1.0)
	, m_dblRate(1.0)
	, m_dblStdErr(1.0)
	, m_intIterations(1000)
	, m_iObsCount(16)
	, m_dObsFrom(0.1)
	, m_dObsTo(100.0)
	, m_iBasisCount(100)
	, m_dBasisFrom(0.01)
	, m_dBasisTo(10000.0)
	, m_dblAlpha(0.0)
	, m_boolSimplify(FALSE)
	, m_boolOptimize(FALSE)
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, theApp.m_pMainWnd);
	pDoc = STATIC_DOWNCAST(CILTDoc, pMain->GetActiveDocument());
}

CMonteCarloDlg::~CMonteCarloDlg()
{
}

void CMonteCarloDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TERMSLIST, m_termsList);
	DDX_Text(pDX, IDC_AMPTERMEDIT, m_dblAmplitude);
	DDV_MinMaxDouble(pDX, m_dblAmplitude, 1.0e-30, 1.0e30);
	DDX_Text(pDX, IDC_RATETERMEDIT, m_dblRate);
	DDV_MinMaxDouble(pDX, m_dblRate, 1.0e-30, 1.0e30);
	DDX_Text(pDX, IDC_STDERROREDIT, m_dblStdErr);
	DDV_MinMaxDouble(pDX, m_dblStdErr, 1.0e-30, 1.0e30);
	DDX_Text(pDX, IDC_ITERATIONSEDIT, m_intIterations);
	DDV_MinMaxInt(pDX, m_intIterations, 1, 10000000);
	DDX_Text(pDX, IDC_OBSERVEDATACOUNTEDIT, m_iObsCount);
	DDV_MinMaxInt(pDX, m_iObsCount, 4, 256);
	DDX_Text(pDX, IDC_OBSERVEDATAFROMEDIT, m_dObsFrom);
	DDX_Text(pDX, IDC_OBSERVEDATATOEDIT, m_dObsTo);
	DDX_Text(pDX, IDC_MCBASISCOUNTEDIT, m_iBasisCount);
	DDX_Text(pDX, IDC_MCBASISFROMEDIT, m_dBasisFrom);
	DDX_Text(pDX, IDC_MCBASISTOEDIT, m_dBasisTo);
	DDX_Control(pDX, IDC_MCSIMPLIFYCHECK, m_buttonSimplify);
	DDX_Control(pDX, IDC_MCOPTIMIZECHECK, m_buttonOptimize);
	DDX_Text(pDX, IDC_MCALPHAEDIT, m_dblAlpha);
	DDV_MinMaxDouble(pDX, m_dblAlpha, 0.0, 1.0e10);
	DDX_Radio(pDX, IDC_LINEARRADIO, m_iSpacing);
	DDX_Radio(pDX, IDC_MCCI1RADIO, m_iConfidenceInterval);
	DDX_Check(pDX, IDC_MCSIMPLIFYCHECK, m_boolSimplify);
	DDX_Check(pDX, IDC_MCOPTIMIZECHECK, m_boolOptimize);
}


BEGIN_MESSAGE_MAP(CMonteCarloDlg, CDialog)
	ON_BN_CLICKED(IDC_ADDTERMBUTTON, &CMonteCarloDlg::OnBnClickedAddtermbutton)
	ON_BN_CLICKED(IDC_DELETETERMBUTTON, &CMonteCarloDlg::OnBnClickedDeletetermbutton)
	ON_LBN_SELCHANGE(IDC_TERMSLIST, &CMonteCarloDlg::OnLbnSelchangeTermslist)
	ON_BN_CLICKED(IDC_MCHELPBUTTON, &CMonteCarloDlg::OnBnClickedMchelpbutton)
	ON_BN_CLICKED(IDC_PASTETERMSBUTTON, &CMonteCarloDlg::OnBnClickedPastetermsbutton)
	ON_BN_CLICKED(IDC_MCSIMPLIFYCHECK, &CMonteCarloDlg::OnBnClickedMcsimplifycheck)
	ON_BN_CLICKED(IDC_MCOPTIMIZECHECK, &CMonteCarloDlg::OnBnClickedMcoptimizecheck)
	ON_BN_CLICKED(IDC_DEELETEALLBUTTON, &CMonteCarloDlg::OnBnClickedDeeleteallbutton)
END_MESSAGE_MAP()


// CMonteCarloDlg message handlers

BOOL CMonteCarloDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString str;
	switch (pDoc->kernelType) {
		case typeT2:
			str = "T2:";
			break;
		case typeT1:
			str = "T1:";
			break;
		case typeDosy:
			str = "D*10^10:";
			break;
	}
	GetDlgItem(IDC_TERMSTATIC)->SetWindowText(str);
	GetDlgItem(IDC_MCMESSAGESTATIC)->SetWindowText((typeDosy==pDoc->kernelType) ?
		"From-To units: DAC values" : "From-To units: Time");

	// refill the model terms
	m_termsList.ResetContent();
	POSITION pos = pDoc->Model.GetHeadPosition();
	while (pos) {
		SMCModel *p = pDoc->Model.GetNext(pos);
		str.Format("%g\t%g", p->a, p->r);
		m_termsList.SetItemDataPtr(m_termsList.AddString(str), p);
	}
	if (m_termsList.GetCount())
		m_termsList.SetCurSel(0);
	
	// check for consistent variables
	if (m_boolOptimize && ! m_boolSimplify)
		m_boolOptimize = FALSE;
	if (m_boolOptimize && 0.0!=m_dblAlpha)
		m_dblAlpha = 0.0;
	OnBnClickedMcsimplifycheck();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CMonteCarloDlg::OnBnClickedAddtermbutton()
{
	int i;
	UpdateData(TRUE);
	SMCModel *p = new SMCModel;
	p->a = m_dblAmplitude;
	p->r = m_dblRate;
	CString str;
	str.Format("%g\t%g", m_dblAmplitude, m_dblRate);
	int n = m_termsList.GetCount();
	for (i=0; i<n; i++) {
		SMCModel *t = (SMCModel *)m_termsList.GetItemDataPtr(i);
		if (p->r > t->r)
			break;
	}
	if (i >= n)
		i = -1;
	i = m_termsList.InsertString(i, str);
	m_termsList.SetItemDataPtr(i, p);
	m_termsList.SetCurSel(i);
	GetDlgItem(IDOK)->EnableWindow(TRUE);
}

void CMonteCarloDlg::OnBnClickedPastetermsbutton()
{
	if (! IsClipboardFormatAvailable(CF_TEXT))
		return;
	if (! OpenClipboard())
		return;
	UpdateData(TRUE);
	HGLOBAL hglb = GetClipboardData(CF_TEXT);
	if (hglb) {
		LPTSTR str = (LPTSTR)GlobalLock(hglb);
		if (str) {
			CString t = str;
			if (! t.IsEmpty()) {
				int iCount = 0;
				while (1) {
					CString line = t.Tokenize("\r\n", iCount);
					if (iCount < 0) break;
					if (line.IsEmpty()) continue;
					int jCount = 0;
					CString astr = line.Tokenize(" \t", jCount).Trim();
					if (jCount > 0) {
						CString rstr = line.Tokenize(" \t", jCount).Trim();
						if (0.0<atof(astr) && 0.0<atof(rstr)) {
							SMCModel *newterm = new SMCModel;
							newterm->a = atof(astr);
							newterm->r = atof(rstr);
							line = astr + '\t' + rstr;
							int n = m_termsList.GetCount();
							int i;
							for (i=0; i<n; i++) {
								SMCModel *term = (SMCModel *)m_termsList.GetItemDataPtr(i);
								if (newterm->r > term->r)
									break;
							}
							if (i >= n)
								i = -1;
							i = m_termsList.InsertString(i, line);
							m_termsList.SetItemDataPtr(i, newterm);
							m_termsList.SetCurSel(i);
						}
					}
				}
				GetDlgItem(IDOK)->EnableWindow(TRUE);
			}
			GlobalUnlock(hglb);
		}
	}
	CloseClipboard();
}

void CMonteCarloDlg::OnBnClickedDeletetermbutton()
{
	int i = m_termsList.GetCurSel();
	delete m_termsList.GetItemDataPtr(i);
	m_termsList.DeleteString(i);
	GetDlgItem(IDOK)->EnableWindow(m_termsList.GetCount()>0);
	GetDlgItem(IDC_DELETETERMBUTTON)->EnableWindow(FALSE);
}

void CMonteCarloDlg::OnBnClickedDeeleteallbutton()
{
	while (m_termsList.GetCount())
		m_termsList.DeleteString(0);
}

void CMonteCarloDlg::OnLbnSelchangeTermslist()
{
	GetDlgItem(IDC_DELETETERMBUTTON)->EnableWindow(m_termsList.GetCurSel()>=0);
}

void CMonteCarloDlg::OnOK()
{
	if (0 == m_termsList.GetCount()) {
		AfxMessageBox("You must specify at least one model term!", MB_ICONEXCLAMATION);
		return;
	}
	// remove pDoc->Model terms that no longer exist
	int limit = m_termsList.GetCount();
	POSITION pos = pDoc->Model.GetHeadPosition();
	while (pos) {
		SMCModel *p = pDoc->Model.GetNext(pos);
		int i;
		for (i=0; i<limit; i++) {
			if (p == m_termsList.GetItemDataPtr(i))
				break;
		}
		if (i >= limit)
			delete p;
	}
	// now reset and build new pDoc->Model
	pDoc->Model.RemoveAll();
	for (int i=0; i<m_termsList.GetCount(); i++)
		pDoc->Model.AddTail((SMCModel *)m_termsList.GetItemDataPtr(i));
	// clear the list
	m_termsList.ResetContent();
	CDialog::OnOK();
}

void CMonteCarloDlg::OnCancel()
{
	// destroy any newly created models!
	for (int i=0; i<m_termsList.GetCount(); i++) {
		bool found = false;
		POSITION pos = pDoc->Model.GetHeadPosition();
		while (pos) {
			SMCModel *p = pDoc->Model.GetNext(pos);
			if (p == m_termsList.GetItemDataPtr(i)) {
				found = true;
				break;
			}
		}
		if (! found)
			delete m_termsList.GetItemDataPtr(i);
	}
	m_termsList.ResetContent();
	CDialog::OnCancel();
}

void CMonteCarloDlg::OnBnClickedMchelpbutton()
{
	theApp.HtmlHelp(0x1800C);
}

void CMonteCarloDlg::OnBnClickedMcsimplifycheck()
{
	UpdateData(TRUE);	// move data from dialog to member variables
	if (m_boolSimplify)
		m_dblAlpha = 0.0;
	else
		m_boolOptimize = FALSE;
	UpdateData(FALSE);	// move data from member variables to dialog
	GetDlgItem(IDC_MCALPHAEDIT)->EnableWindow(!m_boolSimplify);
	m_buttonOptimize.EnableWindow(m_boolSimplify);
	OnBnClickedMcoptimizecheck();
}

void CMonteCarloDlg::OnBnClickedMcoptimizecheck()
{
	UpdateData(TRUE);	// move data from dialog to member variables
	BOOL bCI = m_buttonOptimize.GetCheck();
	GetDlgItem(IDC_MCCI1RADIO)->EnableWindow(bCI);
	GetDlgItem(IDC_MCCI2RADIO)->EnableWindow(bCI);
	GetDlgItem(IDC_MCCI3RADIO)->EnableWindow(bCI);
	GetDlgItem(IDC_MCCI4RADIO)->EnableWindow(bCI);
	GetDlgItem(IDC_MCCI5RADIO)->EnableWindow(bCI);
	GetDlgItem(IDC_MCCI6RADIO)->EnableWindow(bCI);
}

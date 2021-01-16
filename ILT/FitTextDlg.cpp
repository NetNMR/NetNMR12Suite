// FitTextDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ILT.h"
#include "FitTextDlg.h"
#include "MainFrm.h"
#include "ILTDoc.h"
#include "decay.h"


// CFitTextDlg dialog

IMPLEMENT_DYNAMIC(CFitTextDlg, CDialog)

CFitTextDlg::CFitTextDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFitTextDlg::IDD, pParent)
	, m_boolTermsOnly(FALSE)
	, m_boolAllExpon(FALSE)
{
	m_richFont.CreateFont(16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Courier New");

	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, theApp.m_pMainWnd);
	pDoc = STATIC_DOWNCAST(CILTDoc, pMain->GetActiveDocument());
	// 7.32113e-5 = 1e20 * k / (6pi)
	constant = 7.32113e-5 * (273.15 + pDoc->Temperature()) / pDoc->Viscosity();
}

CFitTextDlg::~CFitTextDlg()
{
	m_richFont.DeleteObject();
}

void CFitTextDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RICHTEXTEDIT, m_textRichEditCtrl);
	DDX_Check(pDX, IDC_TERMSONLYCHECK, m_boolTermsOnly);
	DDX_Check(pDX, IDC_ALLEXPONCHECK, m_boolAllExpon);
}


BEGIN_MESSAGE_MAP(CFitTextDlg, CDialog)
	ON_BN_CLICKED(IDC_PRINTTEXTBUTTON, &CFitTextDlg::OnBnClickedPrinttextbutton)
	ON_BN_CLICKED(IDC_COPYTEXTBUTTON, &CFitTextDlg::OnBnClickedCopytextbutton)
	ON_BN_CLICKED(IDC_TERMSONLYCHECK, &CFitTextDlg::OnBnClickedTermsonlycheck)
	ON_BN_CLICKED(IDC_ALLEXPONCHECK, &CFitTextDlg::OnBnClickedTermsonlycheck)
END_MESSAGE_MAP()


// CFitTextDlg message handlers

void CFitTextDlg::OnBnClickedPrinttextbutton()
{
	CPrintDialog dlg(FALSE);
	if (IDOK==dlg.DoModal()) {
		CDC dc;
		dc.Attach(dlg.m_pd.hDC);
		dc.StartDoc("ILT Fit");
		if (dc.StartPage()>=0) {
			int x = 400;
			int y = 400;
			dc.SetMapMode(MM_TEXT);
			CFont font;
			font.CreateFont(80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Courier New");
			CFont *oldFont = dc.SelectObject(&font);
			CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, theApp.m_pMainWnd);
			CILTDoc *pDoc = STATIC_DOWNCAST(CILTDoc, pMain->GetActiveDocument());
			dc.TextOut(x, y, pDoc->GetPathName());
			CString str;
			m_textRichEditCtrl.GetWindowText(str);
			int pos = 0;
			y += 200;
			CString line = str.Tokenize("\n", pos);
			while (line.GetLength() > 0) {
				dc.TextOut(x, y, line.TrimRight());
				y += 100;
				line = str.Tokenize("\n", pos);
			}
			dc.SelectObject(oldFont);
		} else
			AfxMessageBox("StartDoc() failed!");
		dc.EndPage();
		dc.EndDoc();
		DeleteDC(dc.Detach());
	}
}

void CFitTextDlg::OnBnClickedCopytextbutton()
{
	m_textRichEditCtrl.SetSel(0, -1);
	m_textRichEditCtrl.Copy();
}

BOOL CFitTextDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_textRichEditCtrl.SetFont(&m_richFont);

	int ndecays = int(pDoc->Decays.GetCount());
	if (1 == ndecays) {
		CDecay *pDecay = pDoc->Decays.GetHead();
		bool isMC = false;
		for (int i=0; i<10; i++) {
			if (! pDecay->mcterms[i].IsEmpty()) {
				isMC = true;
				break;
			}
		}
		// special processing for MonteCarlo Processing
		if (isMC) {
			CString text, str;
			text.Format("%s\nMonte Carlo simulation", pDecay->strTitle);
			int total = 0;
			for (int i=0; i<10; i++) {
				int count = int(pDecay->mcterms[i].GetCount());
				if (count) {
					total += count;
					str.Format("\n%d %d-term", int(pDecay->mcterms[i].GetCount())/(i+1), i+1);
					text += str;
				}
			}
			str.Format("\n%d total terms", total);
			text += str;
			m_textRichEditCtrl.SetWindowText(text);
			GetDlgItem(IDOK)->SetFocus();
			return FALSE;
		}
	}
	GetDlgItem(IDC_ALLEXPONCHECK)->EnableWindow(ndecays > 1);
	fillrichtext();

	return FALSE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CFitTextDlg::fillrichtext()
{
	CString text;
	if (m_boolTermsOnly) {
		switch (pDoc->kernelType) {
			case typeDosy:
				text = "Expon.\tA\tD*10^10\tRadius(A)\n";
				break;
			case typeT1:
				text = "Expon.\tA\tT1\n";
				break;
			case typeT2:
				text = "Expon.\tA\tT2\n";
				break;
		}
	}
	if (m_boolAllExpon) { // process all decays
		POSITION pos = pDoc->Decays.GetHeadPosition();
		while (pos) {
			CDecay *pDecay = pDoc->Decays.GetNext(pos);
			if (m_boolTermsOnly) {
				if (! pDecay->Terms.IsEmpty()) {
					CString dstr;
					gettermstext(pDecay, dstr);
					text += dstr;
				}
			} else {
				CString dstr;
				getdecaytext(pDecay, dstr);
				text += dstr;
			}
			if (pos)
				text += '\n';
		}
	} else {
		CDecay *pDecay = pDoc->Decays.GetAt(theApp.pDecay);
		if (m_boolTermsOnly) {
			if (! pDecay->Terms.IsEmpty()) {
				CString dstr;
				gettermstext(pDecay, dstr);
				text += dstr;
			}
		} else {
			CString dstr;
			getdecaytext(pDecay, dstr);
			text += dstr;
		}
	}
	m_textRichEditCtrl.SetWindowText(text);
	GetDlgItem(IDOK)->SetFocus();
}

void CFitTextDlg::gettermstext(CDecay *p, CString &msg)
{
	POSITION pos = p->Terms.GetHeadPosition();
	while (pos) {
		CTerm *t = p->Terms.GetNext(pos);
		double ea = sqrt(chisq * t->la);
		double er = sqrt(chisq * t->lc);
		CString tstr;
		if (t->r > 0.0) {
			if (t->la > 0.0) {
				if (pDoc->kernelType == typeDosy)
					tstr.Format("%s\t%g ± %g\t%g ± %g\t%g ± %g\n", p->strTitle, t->a, ea, t->r, er,
						constant/t->r, constant*er/(t->r*t->r));
				else {
					double T = 1.0 / t->r;
					double Tmin = (t->r > er) ? 1.0 / (t->r - er) : DBL_MAX;
					double Tmax = 1.0 / (t->r + er);
					tstr.Format("%s\t%g ± %g\t%g < %g < %g\n", p->strTitle, t->a, ea, Tmin, T, Tmax);
				}
			} else {
				if (pDoc->kernelType==typeDosy)
					tstr.Format("%s\t%g\t%g\t%g\n", p->strTitle, t->a, t->r, constant/t->r);
				else
					tstr.Format("%s\t%g\t%g\n", p->strTitle, t->a, 1.0/t->r);
			}
		} else {
			if (ea > 0.0)
				tstr.Format("%s\t%g ± %g\tconstant\n", p->strTitle, t->a, ea);
			else
				tstr.Format("%s\t%g\tconstant\n", p->strTitle, t->a);
		}
		msg += tstr;
	}
}

void CFitTextDlg::getdecaytext(CDecay *p, CString &msg)
{
	CString line;
	double ssq, rssq;
	EKernelType kType = pDoc->kernelType;
	p->GetStats(ssq, rssq);
	if (p->Terms.IsEmpty()) {
		if (typeDosy == kType)
			msg.Format("G Energy\t%s\n", p->strTitle);
		else
			msg.Format("time(%s)\t%s\n", pDoc->strTime, p->strTitle);
		for (int i=0; i<pDoc->Xvalue.dim(); i++) {
			line.Format("%g\t%g\n", pDoc->Xvalue[i], p->GetAt(i));
			msg += line;
		}
		line.Format("\nSSq\t%g\n", ssq);
		msg += line;
	} else {
		msg.Format("Exponential: %s\n", p->strTitle);
		switch (p->iMethod) {
			case IDC_EGLMETHODRADIO:
				line ="Wiscombe/Evans/Gragg/LaVeque Method\n";
				break;
			case IDC_SIRTMETHODRADIO:
				line = "Sirt Method\n";
				break;
			default:
				line = "NNLS Method\n";
				break;
		}
		msg += line;
		if (p->bSimplify) {
			msg += "With Simplification";
			if (p->bOptimize)
				msg += " and Optimization";
			msg += '\n';
		} else {
			if (IDC_ILTMETHODRADIO == p->iMethod) {
				line.Format("Alpha=%g\n", p->dblAlpha);
				msg += line;
			} else if (IDC_SIRTMETHODRADIO == p->iMethod) {
				line.Format("SIRT limit=%d; SIRT tolerance=%g\n", p->iSIRTCount, p->dblSIRTTolerance);
				msg += line;
			}
		}
		msg += '\n';
		LPCTSTR lp;
		switch (kType) {
			case typeT2:
				lp = "T2";
				break;
			case typeT1:
				lp = "T1";
				break;
			case typeDosy:
				lp = "D*10^10\tRadius(A)";
				break;
		}
		line.Format("Intensity\t%s\n", lp);
		msg += line;
		POSITION pos = p->Terms.GetHeadPosition();
		while (pos) {
			CTerm *term = p->Terms.GetNext(pos);
			double ea = sqrt(chisq * term->la);
			double er = sqrt(chisq * term->lc);
			if (ea != 0.0) {
				if (pos) {
					if (typeDosy == kType) {
						line.Format("%g ± %g\t%g ± %g\t%g ± %g\n", term->a, ea, term->r, er,
							constant/term->r, constant*er/(term->r*term->r));
					} else {
						double tmid = 1.0 / term->r;
						double tlow = 1.0 / (term->r + er);
						double thi;
						if (term->r > er)
							thi  = 1.0 / (term->r - er);
						else
							thi = DBL_MAX;
						line.Format("%g ± %g\t%g < %g < %g\n", term->a, ea, tlow, tmid, thi);
					}
				} else {
					line.Format("%g ± %g\tconstant\n", term->a, ea);
				}
			} else {
				if (pos) {
					if (typeDosy==kType)
						line.Format("%g\t%g\t%g\n", term->a, term->r, constant/term->r);
					else
						line.Format("%g\t%g\n", term->a, 1.0/term->r);
				} else {
					line.Format("%g\tconstant\n", term->a);
				}
			}
			msg += line;
		}
		line.Format("\nSSq\t%g\n", ssq);
		msg += line;
		line.Format("RSSq\t%g\n", rssq);
		msg += line;
		line.Format("RSq\t%g\n", 1.0 - rssq/ssq);
		msg += line;
		int df = pDoc->Xvalue.dim() - (2 * int(p->Terms.GetCount()) - 1);
		if (df > 0) {
			line.Format("StdErr\t%g\n", sqrt(rssq/double(df)));
			msg += line;
		}
		if (typeDosy == kType) {
			line.Format("\ndosygamma = %g\ntimecubed = %g\nDACtoG = %g\n",
										pDoc->Gamma(), pDoc->TimeCubed(), pDoc->DACtoG());
			msg += line;
			line.Format("Temperature = %g\nSolvent = %s\nViscosity = %g\n",
										pDoc->Temperature(), pDoc->strSolvent, pDoc->Viscosity());
			msg += line;
			line.Format("\nG Energy\t%s\tCalcluated\tDifference\n", p->strTitle);
		} else
			line.Format("\ntime(%s)\t%s\tCalculated\tDifference\n", pDoc->strTime, p->strTitle);
		msg += line;
		for (int i=0; i<pDoc->Xvalue.dim(); i++) {
			line.Format("%g\t%g\t%g\t%g\n", pDoc->Xvalue[i], p->GetAt(i), p->GetCalc(i), p->GetDiff(i));
			msg += line;
		}
	}
}

void CFitTextDlg::OnBnClickedTermsonlycheck()
{
	UpdateData(TRUE);
	fillrichtext();
	UpdateData(FALSE);
}

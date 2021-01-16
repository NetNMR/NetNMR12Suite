// FitTextDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NetSpan.h"
#include "MainFrame.h"
#include "NetSpanDoc.h"
#include "NetSpanView.h"
#include "FitTextDlg.h"


// CFitTextDlg dialog

IMPLEMENT_DYNAMIC(CFitTextDlg, CDialog)
CFitTextDlg::CFitTextDlg(bool bShowErr, CWnd* pParent /*=NULL*/)
	: CDialog(CFitTextDlg::IDD, pParent)
{
	m_boolShowErrors = bShowErr;
	pMain = STATIC_DOWNCAST(CMainFrame, theApp.m_pMainWnd);
	pDoc = STATIC_DOWNCAST(CNetSpanDoc, pMain->GetActiveDocument());
	pView = STATIC_DOWNCAST(CNetSpanView, pMain->GetActiveView());
	m_richFont.CreateFont(16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Courier New");
}

CFitTextDlg::~CFitTextDlg()
{
	m_richFont.DeleteObject();
}

void CFitTextDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FITRICHEDIT, m_fitRichEditCtrl);
}


BEGIN_MESSAGE_MAP(CFitTextDlg, CDialog)
	ON_BN_CLICKED(IDC_COPYBUTTON, OnBnClickedCopybutton)
	ON_BN_CLICKED(IDC_PRINTBUTTON, OnBnClickedPrintbutton)
	ON_BN_CLICKED(IDC_CURVEFITBUTTON, OnBnClickedCurvefitbutton)
	ON_BN_CLICKED(IDC_CURVEILTBUTTON, &CFitTextDlg::OnBnClickedCurveiltbutton)
END_MESSAGE_MAP()


// CFitTextDlg message handlers

void CFitTextDlg::OnBnClickedCopybutton()
{
	m_fitRichEditCtrl.SetSel(0, -1);
	m_fitRichEditCtrl.Copy();
}

void CFitTextDlg::OnBnClickedPrintbutton()
{
	CPrintDialog dlg(FALSE);
	if (IDOK==dlg.DoModal()) {
		CDC dc;
		dc.Attach(dlg.m_pd.hDC);
		dc.StartDoc("Best Fit");
		if (dc.StartPage()>=0) {
			int x = 400;
			int y = 400;
			dc.SetMapMode(MM_TEXT);
			CFont font;
			font.CreateFont(80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Courier New");
			CFont *oldFont = dc.SelectObject(&font);
			dc.TextOut(x, y, pDoc->GetPathName());
			CString str;
			m_fitRichEditCtrl.GetWindowText(str);
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

BOOL CFitTextDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_fitRichEditCtrl.SetFont(&m_richFont);
	FillRichText();
	if (pDoc->cols < 3) {
		GetDlgItem(IDC_CURVEFITBUTTON)->EnableWindow(FALSE);
		GetDlgItem(IDC_CURVEILTBUTTON)->EnableWindow(FALSE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CFitTextDlg::FillRichText(void)
{
	CString msg;
	msg.Format("Standard Error=%g Y-Scale=%g\n", 100.0*pView->serr, 0.01*pDoc->ynorm);
	if (pDoc->Lines.GetCount()+1 > pDoc->Spectra.GetCount()+4) {
		CString str = "        Line";
		POSITION pos = pDoc->Spectra.GetHeadPosition();
		while (pos) {
			CString tmp = str;
			CSpectrum *pSpec = pDoc->Spectra.GetNext(pos);
			str.Format("%s%-12.12s", tmp, pSpec->label);
		}
		if (pDoc->sf == 1.0)
			str += "     %-Loren  Width(PPM)       Shift\n";
		else
			str += "     %-Loren  Width(Hz)        Shift\n";
		msg += str;
		pos = pDoc->Lines.GetHeadPosition();
		while (pos) {
			CLine *pLine = pDoc->Lines.GetNext(pos);
			str.Format("%12.12s", pLine->name);
			POSITION spos = pDoc->Spectra.GetHeadPosition();
			while (spos) {
				CString tmp = str;
				CSpectrum *pSpec = pDoc->Spectra.GetNext(spos);
				str.Format("%s%12.2f", tmp, 100.0*pSpec->intensity[pLine->id]);
			}
			CString tmp = str;
			str.Format("%s%12.1f%12.2f%12.4f\n", tmp, 
				100.0*pLine->GetShape(), 
				pLine->GetWidth() * pDoc->sf * pDoc->xsw, 
				pDoc->xoffset - pDoc->xsw * pLine->GetShift());
			msg += str;
			if (m_boolShowErrors) {
				str = "            ";
				spos = pDoc->Spectra.GetHeadPosition();
				while (spos) {
					tmp = str;
					CSpectrum *pSpec = pDoc->Spectra.GetNext(spos);
					str.Format("%s%12.2f", tmp, 100.0*pSpec->eintensity[pLine->id]);
				}
				if (pLine->bshape) {
					tmp = str;
					str.Format("%s%12.1f", tmp, 100.0*pLine->eshape);
				} else
					str += "            ";
				if (pLine->bwidth) {
					tmp = str;
					str.Format("%s%12.2f", tmp, pLine->ewidth * pDoc->xsw * pDoc->sf);
				} else
					str += "            ";
				if (pLine->bshift) {
					tmp = str;
					str.Format("%s%12.4f\n", tmp, pDoc->xsw * pLine->eshift);
				} else
					str += "\n";
				msg += str;
			}
		}
	} else {
		CString str = "Line        ";
		POSITION pos = pDoc->Lines.GetHeadPosition();
		while (pos) {
			CLine *pLine = pDoc->Lines.GetNext(pos);
			CString tmp = str;
			str.Format("%s%12.12s", tmp, pLine->name);
		}
		msg += str + "\n";
		pos = pDoc->Spectra.GetHeadPosition();
		while (pos) {
			CSpectrum *pSpec = pDoc->Spectra.GetNext(pos);
			str.Format("%-12.12s", pSpec->label);
			POSITION lpos = pDoc->Lines.GetHeadPosition();
			while (lpos) {
				CLine *pLine = pDoc->Lines.GetNext(lpos);
				CString tmp = str;
				str.Format("%s%12.2f", tmp, 100.0*pSpec->intensity[pLine->id]);
			}
			msg += str + "\n";
			if (m_boolShowErrors) {
				str = "            ";
				lpos = pDoc->Lines.GetHeadPosition();
				while (lpos) {
					CLine *pLine = pDoc->Lines.GetNext(lpos);
					CString tmp = str;
					str.Format("%s%12.2f", tmp, 100.0*pSpec->eintensity[pLine->id]);
				}
				msg += str + "\n";
			}
		}
		str = "%-Loren     ";
		pos = pDoc->Lines.GetHeadPosition();
		while (pos) {
			CLine *pLine = pDoc->Lines.GetNext(pos);
			CString tmp = str;
			str.Format("%s%12.1f", tmp, 100.0*pLine->GetShape());
		}
		msg += str + "\n";
		if (m_boolShowErrors) {
			str = "            ";
			pos = pDoc->Lines.GetHeadPosition();
			while (pos) {
				CLine *pLine = pDoc->Lines.GetNext(pos);
				if (pLine->bshape) {
					CString tmp = str;
					str.Format("%s%12.1f", tmp, 100.0*pLine->eshape);
				} else
					str += "            ";
			}
			msg += str + "\n";
		}
		if (pDoc->sf == 1.0)
			str = "Width(PPM)  ";
		else
			str = "Width(Hz)   ";
		pos = pDoc->Lines.GetHeadPosition();
		while (pos) {
			CLine *pLine = pDoc->Lines.GetNext(pos);
			CString tmp = str;
			str.Format("%s%12.2f", tmp, pDoc->xsw*pDoc->sf*pLine->GetWidth());
		}
		msg += str + "\n";
		if (m_boolShowErrors) {
			str = "            ";
			pos = pDoc->Lines.GetHeadPosition();
			while (pos) {
				CLine *pLine = pDoc->Lines.GetNext(pos);
				if (pLine->bwidth) {
					CString tmp = str;
					str.Format("%s%12.2f", tmp, pDoc->xsw*pDoc->sf*pLine->ewidth);
				} else
					str += "            ";
			}
			msg += str + "\n";
		}
		str = "Shift       ";
		pos = pDoc->Lines.GetHeadPosition();
		while (pos) {
			CLine *pLine = pDoc->Lines.GetNext(pos);
			CString tmp = str;
			str.Format("%s%12.4f", tmp, pDoc->xoffset - pDoc->xsw * pLine->GetShift());
		}
		msg += str + "\n";
		if (m_boolShowErrors) {
			str = "            ";
			pos = pDoc->Lines.GetHeadPosition();
			while (pos) {
				CLine *pLine = pDoc->Lines.GetNext(pos);
				if (pLine->bshift) {
					CString tmp = str;
					str.Format("%s%12.4f", tmp, pDoc->xsw * pLine->eshift);
				} else
					str += "            ";
			}
			msg += str;
		}
	}
	m_fitRichEditCtrl.SetWindowText(msg);
}

void CFitTextDlg::OnBnClickedCurvefitbutton()
{
	CString cmd = "start excel \"";
	cmd += theApp.m_pszHelpFilePath;
	int i = cmd.ReverseFind('\\') + 1;
	cmd = cmd.Mid(0, i) + "nmr curve fit.xlsm\"";
	pView->OnEditCopy();
	system(cmd);
}

void CFitTextDlg::OnBnClickedCurveiltbutton()
{
	pDoc->MakeILTTextFile();
}

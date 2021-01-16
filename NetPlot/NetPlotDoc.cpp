// NetPlotDoc.cpp : implementation of the CNetPlotDoc class
//

#include "stdafx.h"
#include "..\NMRDataTypes.h"
#include "Spectrum.h"
#include "NetPlot.h"
#include "NMRServer.h"
#include "NetPlotDoc.h"
#include "MainFrame.h"
#include "ProcessWnd.h"
#include "ProcSheet.h"
#include "FileDlg.h"
#include "ParamDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CNetPlotDoc

IMPLEMENT_DYNCREATE(CNetPlotDoc, CDocument)

BEGIN_MESSAGE_MAP(CNetPlotDoc, CDocument)
END_MESSAGE_MAP()


// CNetPlotDoc construction/destruction

CNetPlotDoc::CNetPlotDoc()
{
	IsLandscape = false;
	AllTheSame = false;
	ShowClass = true;
	GlobalScaling = FALSE;
	LabelX = LabelY = OffsetX = RangeFrom = RangeTo = 0.0f;
	OffsetY = Height = 4.0f;
	Width = 16.0f;
	Title.Empty();
	Label.Empty();
	YExpansion = 1.0f;
	Range = RangeFull;
	nTitleSize = theApp.RegistryInt(ReadInt, TitleFontSize, 20);
	nLabelSize = theApp.RegistryInt(ReadInt, LabelFontSize, 14);
	nAxisSize = theApp.RegistryInt(ReadInt, AxisFontSize, 18);
	nBaseOrder = 10;
}

CNetPlotDoc::~CNetPlotDoc()
{
	POSITION pos = Spectra.GetHeadPosition();
	while (pos)
		delete Spectra.GetNext(pos);
	Spectra.RemoveAll();
	bIntegral.resize(0);
}

BOOL CNetPlotDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CNetPlotDoc serialization
void CNetPlotDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ar.WriteString(Title+'\n');
		ar.WriteString(Label+'\n');
		ar << IsLandscape;
		ar << Range;
		ar << nLabelSize << nTitleSize << nAxisSize;
		ar << ymin << ymax << Width << Height << OffsetX << OffsetY;
		ar << LabelX << LabelY << RangeFrom << RangeTo << YExpansion;
		ar << GlobalScaling;
		ar << (int)Spectra.GetCount();
		POSITION pos = Spectra.GetHeadPosition();
		while (pos) {
			CSpectrum *pSpec = Spectra.GetNext(pos);
			pSpec->Serialize(ar);
		}
		if (AllTheSame && bIntegral.size()) {
			for (UINT i=0; i<bIntegral.size(); i++)
				ar << bIntegral[i];
		}
	}
	else
	{
		int n;
		CString vstr;
		ar.ReadString(Title);
		ar.ReadString(Label);
		ar >> IsLandscape;
		ar >> n;
		Range = (ViewRange)n;
		ar >> nLabelSize >> nTitleSize >> nAxisSize;
		ar >> ymin >> ymax >> Width >> Height >> OffsetX >> OffsetY;
		ar >> LabelX >> LabelY >> RangeFrom >> RangeTo >> YExpansion;
		ar >> GlobalScaling;
		ar >> n;
		AllTheSame = true;
		UINT cb=0;
		double sw=0.0, sf=0.0, of=0.0; 
		for (int i=0; i<n; i++) {
			CSpectrum *pSpec = new CSpectrum;
			pSpec->Serialize(ar);
			Spectra.AddTail(pSpec);
			// set span PPM and hertz
			// set AllTheSame (do all of the spectra have the same spectral window?)
			if (i) {
				float pfrom, pto;
				pSpec->GetFromToRange(pfrom, pto);
				float hfrom = pfrom * (float)pSpec->Frequency();
				float hto = pto * (float)pSpec->Frequency();
				if (pfrom > PPMFrom)
					PPMFrom = pfrom;
				if (pto < PPMTo)
					PPMTo = pto;
				if (hfrom > HertzFrom)
					HertzFrom = hfrom;
				if (hto < HertzTo)
					HertzTo = hto;
				if (cb!=pSpec->Size() || sw!=pSpec->SweepWidth() || sf!= pSpec->Frequency() || of!=pSpec->Offset())
					AllTheSame = false;
			} else {
				pSpec->GetFromToRange(PPMFrom, PPMTo);
				cb = pSpec->Size();
				sw = pSpec->SweepWidth();
				sf = pSpec->Frequency();
				of = pSpec->Offset();
				HertzFrom = PPMFrom * (float)sf;
				HertzTo = PPMTo * (float)sf;
			}
		}
		if (AllTheSame) {
			bIntegral.resize(cb);
			ar.Flush();
			CFile *fp = ar.GetFile();
			if (fp->GetLength() >= fp->GetPosition() + cb*sizeof(bool)) {
				for (UINT i=0; i<cb; i++) {
					bool b;
					ar >> b;
					bIntegral[i] = b;
				}
			} else {
				for (UINT i=0; i<cb; i++)
					bIntegral[i] = false;
			}
		}
		ar.Flush();
	}
}


// CNetPlotDoc diagnostics

#ifdef _DEBUG
void CNetPlotDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CNetPlotDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CNetPlotDoc commands

void CNetPlotDoc::AddSpectrum(LPCTSTR str)
{
	CWaitCursor wait;
	CString path = str;
	int colon = path.Find(':');
	if (colon < 1) return;
	CString host = path.Left(colon);
	if (host.GetLength() > 1) {
		// NetNMR Server
		CNMRServer server;
		CString file = path.Right(path.GetLength()-(colon+1));
		if (server.MakeConnection(host)) {
			CString buf;
			buf.Format("f%s\n", file);
			server.SendString(buf);
			bool doagain;
			do {
				doagain = false;
				CSpectrum *pSpec = new CSpectrum;
				if (pSpec) {
					doagain = pSpec->Server(server, str);
					Spectra.AddTail(pSpec);
				}
			} while (doagain);
		}
	} else {
		// LOCAL NetNMR FILE
		CFile file;
		if ( file.Open(path, CFile::modeRead|CFile::shareDenyWrite)) {
			CSpectrum *pSpec = new CSpectrum;
			if (pSpec) {
				pSpec->pathname = path;
				if (0 == path.Right(4).CompareNoCase(".nmr"))
					pSpec->File(file);
				else if (0 == path.Right(5).CompareNoCase(".nmrx"))
					pSpec->FileX(file);
				else {
					delete pSpec;
					file.Close();
					CString msg = "Can't open ";
					msg += path;
					AfxMessageBox(msg, MB_ICONERROR);
					return;
				}
				Spectra.AddTail(pSpec);
			}
			file.Close();
		}
	}
}

void CNetPlotDoc::SetMinMax()
{
	bool first = true;
	POSITION pos = Spectra.GetHeadPosition();
	while (pos) {
		CSpectrum *pSpec = Spectra.GetNext(pos);
		if (first) {
			ymin = pSpec->Ymin();
			ymax = pSpec->Ymax();
			first = false;
		} else {
			if (ymin > pSpec->Ymin())
				ymin = pSpec->Ymin();
			if (ymax < pSpec->Ymax())
				ymax = pSpec->Ymax();
		}
	}
}

void CNetPlotDoc::AddFile()
{
	CFileDlg dlg;
	WhatChanged = 0;
	if (IDOK == dlg.DoModal()) {
		if (WhatChanged) {
			POSITION pos = Spectra.GetHeadPosition();
			bool first = true;
			while (pos) {
				CSpectrum *pSpec = Spectra.GetNext( pos );
				if (first && Label.IsEmpty()) {
					CString astr;
					if (pSpec->FHGet("array", astr))
						Label = astr;
				}
				if (! Label.IsEmpty() && pSpec->label.IsEmpty())
					pSpec->GetLabel();
				if ( first ) {
					ymax = pSpec->Ymax();
					ymin = pSpec->Ymin();
					first = false;
				} else {
					if ( pSpec->Ymax() > ymax )
						ymax = pSpec->Ymax();
					if ( pSpec->Ymin() < ymin )
						ymin = pSpec->Ymin();
				}
			}
			WhatChanged = 0;
			UpdateAllViews(NULL);
			SetModifiedFlag();
		}
	}
}

void CNetPlotDoc::EditParameters()
{
	CParamDlg dlg;
	WhatChanged = 0;
	if ( IDOK==dlg.DoModal() ) {
		if ( WhatChanged ) {
			if ( WhatChanged & RANGECHANGE ) {
				Range = (ViewRange)dlg.m_Range;
				RangeFrom = dlg.m_RangeFrom;
				RangeTo = dlg.m_RangeTo;
				POSITION pos = Spectra.GetHeadPosition();
				while( pos ) {
					CSpectrum *pSpec = Spectra.GetNext( pos );
					pSpec->CalcStartStop();
					pSpec->CalcToFrom();
					pSpec->FindMinMax();
				}
			}
			if ( WhatChanged & LABELCHANGE ) {
				POSITION pos = Spectra.GetHeadPosition();
				while ( pos ) {
					CSpectrum *pSpec = Spectra.GetNext( pos );
					pSpec->GetLabel();
				}
			}
			if ( WhatChanged & GLOBALCHANGE || WhatChanged & RANGECHANGE )
				SetMinMax();
			if ( WhatChanged & GLOBALCHANGE) {
				CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
				if (pMain->dialogFrame && pMain->dialogFrame->procSheet)
					pMain->dialogFrame->procSheet->UpdateContent();
			}
			WhatChanged = 0;
			UpdateAllViews(NULL);
			SetModifiedFlag();
		}
	}
}

void CNetPlotDoc::FileSaveAs()
{
	POSITION pos = Spectra.GetHeadPosition();
	CSpectrum *pSpec = Spectra.GetNext(pos);
	int cb = pSpec->Size();
	int start = pSpec->Start();
	int stop = pSpec->Stop();
	double sw = pSpec->SweepWidth();
	double sf = pSpec->Frequency();
	double of = pSpec->Offset();
	CFileDialog dlg(FALSE, "npl", NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, 
		AllTheSame ? "NetPlot files (*.npl)|*.npl|Text files (*.txt)|*.txt||" : "NetPlot files (*.npl)|*.npl||");
	if (IDOK == dlg.DoModal()) {
		if (dlg.GetFileExt().CompareNoCase("npl")) {	// TXT file
			if (AllTheSame) {
				if (stop-start+2 > 64*1024) {
					CString buf;
					buf.Format("This text file will have %d lines\nand may be too large to open with Excel.\nContinue?", stop-start+2);
					if (IDCANCEL == AfxMessageBox(buf, MB_OKCANCEL))
						return;
				}
				CStdioFile file;
				if (file.Open(dlg.GetPathName(), CFile::modeCreate|CFile::modeWrite|CFile::typeText)) {
					CString buf, value;
					if (pSpec->FHGet("dosygamma", value)) {
						buf.Format("# gamma=%s\n", value);
						file.WriteString(buf);
					}
					if (pSpec->FHGet("timecubed", value)) {
						buf.Format("# tcubed=%s\n", value);
						file.WriteString(buf);
					}
					if (pSpec->FHGet("DAC_to_G", value)) {
						buf.Format("# dactog=%s\n", value);
						file.WriteString(buf);
					}
					buf.Format("PPM");
					file.WriteString(buf);
					int i=0;
					pos = Spectra.GetHeadPosition();
					while (pos) {
						pSpec = Spectra.GetNext(pos);
						if (pSpec->label.GetLength())
							buf.Format("\t%s", pSpec->label);
						else
							buf.Format("\t%d", i);
						file.WriteString(buf);
					}
					file.WriteString("\n");
					for (i=start; i<stop; i++) {
						buf.Format("%g", float((of+0.5*sw-(double(i)*sw/double(cb-1)))/sf));
						file.WriteString(buf);
						pos = Spectra.GetHeadPosition();
						while(pos) {
							CSpectrum *pSpec = Spectra.GetNext(pos);
							buf.Format("\t%g", pSpec->GetAt(i));
							file.WriteString(buf);
						}
						file.WriteString("\n");
					}
				} else {
					AfxMessageBox("Can't open file!", MB_ICONERROR);
				}
			} else {
				AfxMessageBox("All datasets must have the same spectral window!", MB_ICONERROR);
			}
		} else {	// NPL file
			OnSaveDocument(dlg.GetPathName());
			SetPathName(dlg.GetPathName());
		}
	}
}

void CNetPlotDoc::DCLevel()
{
	POSITION pos = Spectra.GetHeadPosition();
	while (pos) {
		CSpectrum *pSpec = Spectra.GetNext(pos);
		pSpec->DCCorrect();
		pSpec->FindMinMax();
	}
	SetModifiedFlag();
	SetMinMax();
	UpdateAllViews(NULL);
}

void CNetPlotDoc::Zerofill(bool bigger)
{
	POSITION pos = Spectra.GetHeadPosition();
	while (pos) {
		CSpectrum *pSpec = Spectra.GetNext(pos);
		pSpec->ZeroFill(bigger);
		pSpec->CalcStartStop();
		pSpec->CalcToFrom();
		if (bIntegral.size() < pSpec->Size()) {
			bIntegral.resize(pSpec->Size());
			for (UINT i=0; i<pSpec->Size(); i++)
				bIntegral[i] = false;
		}
	}
	SetModifiedFlag();
	UpdateAllViews(NULL);
}

void CNetPlotDoc::FT()
{
	POSITION pos = Spectra.GetHeadPosition();
	while (pos) {
		CSpectrum *pSpec = Spectra.GetNext(pos);
		pSpec->FFT();
		pSpec->FindMinMax();
		if (bIntegral.size() < pSpec->Size()) {
			bIntegral.resize(pSpec->Size());
			for (UINT i=0; i<pSpec->Size(); i++)
				bIntegral[i] = false;
		}
	}
	SetMinMax();
	SetModifiedFlag();
	UpdateAllViews(NULL);
}

void CNetPlotDoc::Apodize(int shape, double value, int unit)
{
	POSITION pos = Spectra.GetHeadPosition();
	while (pos) {
		CSpectrum *pSpec = Spectra.GetNext(pos);
		pSpec->Apodize(shape, value, unit);
	}
	SetModifiedFlag();
	SetMinMax();
	UpdateAllViews(NULL);
}

void CNetPlotDoc::Magnitude(CSpectrum *p)
{
	if (p) {
		p->Magnitude();
		p->FindMinMax();
	} else {
		POSITION pos = Spectra.GetHeadPosition();
		while (pos) {
			CSpectrum *pSpec = Spectra.GetNext(pos);
			pSpec->Magnitude();
			pSpec->FindMinMax();
		}
	}
	SetModifiedFlag();
	SetMinMax();
	UpdateAllViews(NULL);
}

void CNetPlotDoc::AutoPhaseA(CSpectrum *p)
{
	if (p) {
		p->AutoPhaseAOnly();
		p->FindMinMax();
	} else {
		POSITION pos = Spectra.GetHeadPosition();
		while (pos) {
			CSpectrum *pSpec = Spectra.GetNext(pos);
			pSpec->AutoPhaseAOnly();
			pSpec->FindMinMax();
		}
	}
	SetModifiedFlag();
	SetMinMax();
	UpdateAllViews(NULL);
}

void CNetPlotDoc::AutoPhaseAB(CSpectrum *p)
{
	if (p) {
		p->AutoPhase();
		p->FindMinMax();
	} else {
		POSITION pos = Spectra.GetHeadPosition();
		while (pos) {
			CSpectrum *pSpec = Spectra.GetNext(pos);
			pSpec->AutoPhase();
			pSpec->FindMinMax();
		}
	}
	SetModifiedFlag();
	SetMinMax();
	UpdateAllViews(NULL);
}

void CNetPlotDoc::ClonePhase(CSpectrum *p)
{
	p->SetGlobalPhase();
	POSITION pos = Spectra.GetHeadPosition();
	while (pos) {
		CSpectrum *pSpec = Spectra.GetNext(pos);
		if (pSpec != p) {
			pSpec->GlobalPhase();
			pSpec->FindMinMax();
		}
	}
	SetModifiedFlag();
	SetMinMax();
	UpdateAllViews(NULL);
}

void CNetPlotDoc::CalculateBaseline()
{
	POSITION pos = Spectra.GetHeadPosition();
	while (pos) {
		CSpectrum *pSpec = Spectra.GetNext(pos);
		pSpec->CalcBaseline(nBaseOrder);
	}
	UpdateAllViews(NULL);
}

void CNetPlotDoc::SubtractBaseline()
{
	POSITION pos = Spectra.GetHeadPosition();
	while (pos) {
		CSpectrum *pSpec = Spectra.GetNext(pos);
		pSpec->SubtractBaseline(nBaseOrder);
		pSpec->FindMinMax();
	}
	SetModifiedFlag();
	SetMinMax();
	UpdateAllViews(NULL);
}
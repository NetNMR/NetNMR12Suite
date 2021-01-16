// NetSpanDoc.cpp : implementation of the CNetSpanDoc class
//

#include "stdafx.h"
#include "NetSpan.h"

#include "NetSpanDoc.h"
#include "MainFrame.h"
#include "NetSpanView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CNetSpanDoc

IMPLEMENT_DYNCREATE(CNetSpanDoc, CDocument)

BEGIN_MESSAGE_MAP(CNetSpanDoc, CDocument)
END_MESSAGE_MAP()


// CNetSpanDoc construction/destruction

CNetSpanDoc::CNetSpanDoc()
{
	cols = rows = 0;
	sf = 1.0;
	nbase = 1;
}

CNetSpanDoc::~CNetSpanDoc()
{
}

BOOL CNetSpanDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	DeleteContents();
	return TRUE;
}




// CNetSpanDoc serialization

void CNetSpanDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ar << cols << rows << from << to;
		ar << sf << ynorm;
		ar << nbase;
		for (int i=0; i<rows; i++)
			ar << ppm[i];
		POSITION pos = Spectra.GetHeadPosition();
		while (pos) {
			CSpectrum *pSpec = Spectra.GetNext(pos);
			ar.WriteString(pSpec->label);
			ar.WriteString("\n");
			double dc, slope, curve, cube;
			pSpec->GetBaseline(dc, slope, curve, cube);
			ar << dc << slope << curve << cube;
			for (int j=0; j<rows; j++) {
				ar << pSpec->obs[j];
			}
		}
		int nlines = (int)Lines.GetCount();
		ar << nlines;
		pos = Lines.GetHeadPosition();
		while (pos) {
			CLine *pLine = Lines.GetNext(pos);
			ar.WriteString(pLine->name);
			ar.WriteString("\n");
			ar << pLine->GetShape();
			ar << pLine->GetWidth();
			ar << pLine->GetShift();
			ar << pLine->bshape << pLine->bwidth << pLine->bshift;
			POSITION spos = Spectra.GetHeadPosition();
			while (spos) {
				CSpectrum *pSpec = Spectra.GetNext(spos);
				ar << pSpec->intensity[pLine->id];
			}
		}
	} else {
		ar >> cols >> rows >> from >> to;
		ar >> sf >> ynorm;
		ar >> nbase;
		ppm.SetSize(rows);
		xnorm.SetSize(rows);
		for (int i=0; i<rows; i++) {
			ar >> ppm[i];
			xnorm[i] = double(i)/double(rows-1);
		}
		for (int i=0 ; i<cols; i++) {
			CSpectrum *pSpec = new CSpectrum;
			ar.ReadString(pSpec->label);
			double dc, slope, curve, cube;
			ar >> dc >> slope >> curve >> cube;
			pSpec->SetBaseline(dc, slope, curve, cube);
			pSpec->obs.SetSize(rows);
			pSpec->calc.SetSize(rows);
			pSpec->diff.SetSize(rows);
			pSpec->singleline.SetSize(rows);
			for (int j=0; j<rows; j++)
				ar >> pSpec->obs[j];
			Spectra.AddTail(pSpec);
		}
		int nlines;
		ar >> nlines;
		while (nlines--) {
			CString name;
			ar.ReadString(name);
			double shape, width, shift;
			ar >> shape >> width >> shift;
			CLine *pLine = new CLine(shape, width, shift, rows, this, name);
			ar >> pLine->bshape >> pLine->bwidth >> pLine->bshift;
			POSITION pos = Spectra.GetHeadPosition();
			while (pos) {
				CSpectrum *pSpec = Spectra.GetNext(pos);
				ar >> pSpec->intensity[pLine->id];
			}
			Lines.AddTail(pLine);
		}
		ReCalculate();
		FindMinMax();
		xoffset = ppm[0];
		xsw = xoffset - ppm[rows-1];
	}
}

void CNetSpanDoc::OpenNetPlotFile(LPCTSTR pathname)
{
	CFile file;
	if (file.Open(pathname, CFile::modeRead|CFile::shareDenyWrite)) {
		CArchive ar(&file, CArchive::load);
		CString sbuf;
		bool bUNUSED;
		float fUNUSED;
		int iUNUSED;
		int nspec;
		BOOL BUNUSED;
		ar.ReadString(sbuf);	//TITLE
		ar.ReadString(sbuf);	//LABEL
		ar >> bUNUSED;			//IsLandscape
		ar >> iUNUSED;			//ViewRange
		ar >> iUNUSED >> iUNUSED >> iUNUSED;	//Font sizes
		for (int i=0; i<11; i++)
			ar >> fUNUSED;	//ymin,ymax,width,height,offsetx,offsety,labelx,labely,rangefrom,rangeto,yexpansion
		ar >> BUNUSED;		//GlobalScaling
		ar >> nspec;
		for (int i=0; i<nspec; i++) {
			CSpectrum *pSpec = new CSpectrum;
			CMapStringToString fh;
			ar.ReadString(sbuf);		// this is the pathname
			ar.ReadString(pSpec->label);	// this is the label
			if (pSpec->label.IsEmpty())
				pSpec->label = sbuf;
			int nfh;
			ar >> nfh;	// the number of flex header items
			while (nfh--) {
				CString key, value;
				ar.ReadString(key);
				ar.ReadString(value);
				if (key.Left(2).Compare("##"))
					fh.SetAt(key, value);
			}
			int size, from, to, start, stop;
			double swlocal, sflocal, of;
			float ymin, ymax;
			ar >> from >> to;
			ar >> swlocal >> sflocal >> of;
			ar >> size;
			ar >> start >> stop;
			ar >> ymax >> ymin;
			pSpec->obs.SetSize(size);
			pSpec->calc.SetSize(size);
			pSpec->diff.SetSize(size);
			pSpec->singleline.SetSize(size);
			pSpec->SetBaseline(0.0, 0.0, 0.0, 0.0);
			for (int j=0; j<size; j++) {
				float yval;
				ar >> yval;
				pSpec->obs[j] = double(yval);
			}
			if (0 == i) {
				rows = size;
				sf = sflocal;
				xsw = swlocal / sflocal;
				xoffset = (0.5 * swlocal + of) / sflocal;
				sfstr.Format("sf=%g", sf);
				if (fh.Lookup("dosygamma", sbuf))
					gammastr.Format("gamma=%s", sbuf);
				if (fh.Lookup("timecubed", sbuf))
					tcubedstr.Format("tcubed=%s", sbuf);
				if (fh.Lookup("DAC_to_G", sbuf))
					dactogstr.Format("dactog=%s", sbuf);
			}
			if (rows!=size || sf!=sflocal || xsw!=swlocal/sflocal || xoffset!=(0.5*swlocal+of)/sflocal)
				delete pSpec;
			else
				Spectra.AddTail(pSpec);
			fh.RemoveAll();
		}
		cols = int(Spectra.GetCount());
		ppm.SetSize(rows);
		xnorm.SetSize(rows);
		for (int row=0; row<rows; row++) {
			double f = double(row)/double(rows);
			ppm[row] = xoffset - (xsw * f);
			xnorm[row] = f;
		}
		from = 0;
		to = rows - 1;
		FindMinMax();
		ynorm = abs(ymax);
		if (abs(ymin) > ynorm)
			ynorm = abs(ymin);
		double normal = 1.0 / ynorm;
		sumsq = 0.0;
		POSITION pos = Spectra.GetHeadPosition();
		while (pos) {
			CSpectrum *pSpec = Spectra.GetNext(pos);
			for (int i=0; i<rows; i++) {
				double y = pSpec->obs[i] * normal;
				sumsq += y*y;
				pSpec->diff[i] = pSpec->obs[i] = y;
				pSpec->calc[i] = 0.0;
			}
		}
		ReCalculate();
		FindMinMax();
		SetModifiedFlag(TRUE);
		UpdateAllViews(NULL);
		nspec -= cols;
		if (nspec) {
			if (nspec > 1)
				sbuf.Format("%d spectra were not imported, they had a different window!", nspec);
			else
				sbuf.Format("One spectrum was not imported, it had a different window!");
			AfxMessageBox(sbuf, MB_ICONEXCLAMATION);
		}
	}
}

void CNetSpanDoc::OpenTextSpanFile(LPCTSTR pathname)
{
	CStdioFile txtfile;
	if (txtfile.Open(pathname, CFile::modeRead|CFile::typeText|CFile::shareDenyWrite)) {
		char *space = "\t";
		UINT size = (UINT)txtfile.GetLength();
		// IS THE FILESIZE REASONABLE
		if (size > 4*1024*1024-1) {
			AfxMessageBox("Text file is too big!", MB_ICONERROR);
			return;
		}
		// READ ALL THE LINES, IGNORING BLANK AND COMMENT LINES
		CStringList lines;
		CString str;
		while (txtfile.ReadString(str)) {
			str = str.Trim();
			if (str.GetLength() > 2) {
				if (str.Left(1).Compare("#"))
					lines.AddTail(str);
				else {
					int i = 0;
					CString key = str.Tokenize(" \t#", i);
					if (0==key.Find("sf="))
						sfstr = key;
					else if (0==key.Find("gamma="))
						gammastr = key;
					else if (0==key.Find("dactog="))
						dactogstr = key;
					else if (0==key.Find("tcubed="))
						tcubedstr = key;
				}
			}
		}
		rows = (int)lines.GetCount();
		// GET THE FIRST LINE AND COUNT THE WORDS (# of WORDS = col+1)
		POSITION pos = lines.GetHeadPosition();
		str = lines.GetAt(pos);
		int lpos = 0;
		CString word = str.Tokenize(space, lpos);
		word = str.Tokenize(space, lpos);
		while (word.GetLength()) {
			cols++;
			word = str.Tokenize(space, lpos);
		}
		if (cols < 1) {
			AfxMessageBox("No Y-values in text file!", MB_ICONERROR);
			lines.RemoveAll();
			return;
		}
		// TRY TO CONVERT THE FIRST WORD OF THE FIRST LINE
		char *pstr = str.GetBuffer();
		char *term;
		double unused = strtod(pstr, &term);
		str.ReleaseBuffer();
		if (! isspace(*term)) {
			// THE FIRST WORD OF THE FIRST LINE IS NOT A NUMBER!
			// READ THE WORDS INTO THE Labels Array.
			lpos = 0;
			SetTitle(str.Tokenize(space, lpos));
			for (int i=0; i<cols; i++) {
				CSpectrum *ps = new CSpectrum;
				ps->label = str.Tokenize(space, lpos);
				Spectra.AddTail(ps);
			}
			// GET NEXT LINE AND DECREMENT NUMBER OF ROWS SINCE THESE ARE LABELS
			rows--;
			str = lines.GetNext(pos);
		} else {
			// CREATE GENERIC LABELS
			for (int i=0; i<cols; i++) {
				CSpectrum *ps = new CSpectrum;
				ps->label.Format("Spectrum %d", i);
				Spectra.AddTail(ps);
			}
		}
		// MAKE DATA STORAGE
		POSITION spos = Spectra.GetHeadPosition();
		while (spos) {
			CSpectrum *pSpec = Spectra.GetNext(spos);
			pSpec->calc.SetSize(rows);
			pSpec->diff.SetSize(rows);
			pSpec->obs.SetSize(rows);
			pSpec->singleline.SetSize(rows);
		}
		ppm.SetSize(rows);
		xnorm.SetSize(rows);
		// FILL X and Ys
		int row = 0;
		while (pos) {
			str = lines.GetNext(pos);
			lpos = 0;
			ppm[row] = atof(str.Tokenize(space, lpos));
			xnorm[row] = double(row)/double(rows-1);
			POSITION spec = Spectra.GetHeadPosition();
			while (spec) {
				CSpectrum *ps = Spectra.GetNext(spec);
				ps->obs[row] = atof(str.Tokenize(space, lpos));
			}
			row++;
		}
		lines.RemoveAll();
		from = 0;
		to = rows - 1;
		FindMinMax();
		ynorm = (abs(ymax)>abs(ymin)) ? abs(ymax) : abs(ymin);
		double normal = 1.0 / ynorm;
		sumsq = 0.0;
		spos = Spectra.GetHeadPosition();
		while (spos) {
			CSpectrum *pSpec = Spectra.GetNext(spos);
			for (int i=0; i<rows; i++) {
				double y = pSpec->obs[i] * normal;
				sumsq += y * y;
				pSpec->diff[i] = pSpec->obs[i] = y;
				pSpec->calc[i] = 0.0;
			}
		}
		ReCalculate();
		FindMinMax();
		xoffset = ppm[0];
		xsw = xoffset - ppm[rows-1];
		SetModifiedFlag(TRUE);
		UpdateAllViews(NULL);
	}
}

// CNetSpanDoc diagnostics

#ifdef _DEBUG
void CNetSpanDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CNetSpanDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CNetSpanDoc commands

void CNetSpanDoc::DeleteContents()
{
	if (theApp.m_pMainWnd) {
		CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, theApp.m_pMainWnd);
		if (pMain->tabDlg) {
			pMain->tabDlg->ShowAllPages(FALSE);
			pMain->tabDlg->ShowWindow(SW_HIDE);
		}
		CNetSpanView *pView = STATIC_DOWNCAST(CNetSpanView, pMain->GetActiveView());
		pView->ClearZoomCursor();
	}
	xnorm.RemoveAll();
	ppm.RemoveAll();
	while (Spectra.GetCount())
		delete Spectra.RemoveHead();
	while (Lines.GetCount()) 
		delete Lines.RemoveHead();
	cols = rows = 0;
	nbase = 1;
	sumsq = 0.0;
	CDocument::DeleteContents();
}

void CNetSpanDoc::FindMinMax()
{
	POSITION pos = Spectra.GetHeadPosition();
	if (pos) {
		CSpectrum *pSpec = Spectra.GetNext(pos);
		pSpec->FindMinMax(from, to);
		ymax = pSpec->GetMax();
		ymin = pSpec->GetMin();
		while (pos) {
			CSpectrum *pSpec = Spectra.GetNext(pos);
			pSpec->FindMinMax(from, to);
			if (pSpec->GetMax() > ymax)
				ymax = pSpec->GetMax();
			if (pSpec->GetMin() < ymin)
				ymin = pSpec->GetMin();
		}
	} else {
		ymin = 0.0;
		ymax = 1.0;
	}
}

// RECALCULATE THE SIMULATION FOR EACH SPECTRUM
void CNetSpanDoc::ReCalculate(void)
{
	if (! Spectra.GetCount())
		return;
	sumsq = 0.0;
	if (Lines.GetCount()) {
		// FIRST: RECALCUATE THE LINE (SHAPE, WIDTH AND SHIFT)
		POSITION pos = Lines.GetHeadPosition();
		while (pos) {
			CLine *pLine = Lines.GetNext(pos);
			pLine->ReCalculate();
		}
		// THEN CALCULATE THE SIMULATION FROM THE INTENSITIES AND BASELINE
		POSITION spos = Spectra.GetHeadPosition();
		while (spos) {
			CSpectrum *pSpec = Spectra.GetNext(spos);
			pSpec->CalcBaseline(from, to);
			POSITION lpos = Lines.GetHeadPosition();
			while (lpos) {
				CLine *pLine = Lines.GetNext(lpos);
				double intensity;
				pSpec->intensity.Lookup(pLine->id, intensity);
				if (pLine == theApp.pCurrentLine) {
					// CALCULATE THE SINGLE, SELECTED LINE, IF NEEDED
					for (int i=from; i<=to; i++) {
						pSpec->singleline[i] = pLine->line[i] * intensity;
						pSpec->calc[i] += pSpec->singleline[i];
					}
				} else {
					for (int i=from; i<=to; i++)
						pSpec->calc[i] += pLine->line[i] * intensity;
				}
			}
			for (int i=from; i<=to; i++) {
				double d = pSpec->obs[i] - pSpec->calc[i];
				sumsq += d * d;
				pSpec->diff[i] = d;
			}
		}
	} else {
		// THERE ARE NO LINES, SO ZERO THE CALCULATED SPECTRUM
		POSITION pos = Spectra.GetHeadPosition();
		while (pos) {
			CSpectrum *pSpec = Spectra.GetNext(pos);
			for (int i=from; i<=to; i++) {
				double d = pSpec->diff[i] = pSpec->obs[i];
				pSpec->calc[i] = 0.0;
				sumsq += d * d;
			}
		}
	}
}

void CNetSpanDoc::MakeILTTextFile()
{
	// Generate the filename
	if (GetPathName().IsEmpty()) {
		AfxMessageBox("You must save this document first!", MB_ICONHAND);
		return;
	}
	CString textpath = GetPathName();
	if (0 == textpath.Right(4).CompareNoCase(".nms"))
		textpath = textpath.Left(textpath.GetLength() - 4);
	textpath += "-deconvolv.txt";
	// write file
	CStdioFile textfile(textpath, CFile::modeCreate|CFile::modeWrite|CFile::typeText);
	if (textfile.m_hFile == CFile::hFileNull) {
		textpath = "Can't open " + textpath;
		AfxMessageBox(textpath, MB_ICONERROR);
		return;
	}
	// write the DOSY comments, if any
	CString line, word;
	if (! gammastr.IsEmpty()) {
		line.Format( "# %s\n", gammastr);
		textfile.WriteString(line);
	}
	if (! tcubedstr.IsEmpty()) {
		line.Format("# %s\n", tcubedstr);
		textfile.WriteString(line);
	}
	if (! dactogstr.IsEmpty()) {
		line.Format("# %s\n", dactogstr);
		textfile.WriteString(line);
	}
	// write the data headers
	textfile.WriteString(gammastr.IsEmpty()?"time":"DAC");
	POSITION pos = Lines.GetHeadPosition();
	while (pos) {
		CLine *pLine = Lines.GetNext(pos);
		if (0 == pLine->name.Find("Line "))
			word.Format("\t%.4f PPM", xoffset - xsw * pLine->GetShift());
		else
			word.Format("\t%s", pLine->name);
		textfile.WriteString(word);
	}
	textfile.WriteString("\n");
	// write the integrals from each line in each spectrum;
	pos = Spectra.GetHeadPosition();
	while (pos) {
		CSpectrum *pSpec = Spectra.GetNext(pos);
		line = pSpec->label;
		POSITION lpos = Lines.GetHeadPosition();
		while (lpos) {
			CLine *pLine = Lines.GetNext(lpos);
			double integral = ynorm * pSpec->intensity[pLine->id] * sf * xsw * pLine->GetWidth() *
				(1.0 + pLine->GetShape() * 0.47566462663560588938881769);
			word.Format("\t%g", integral);
			line += word;
		}
		textfile.WriteString(line + '\n');
	}
	textfile.Close();
	// launch ILT.exe
	line = theApp.m_pszHelpFilePath;
	line = line.Left(line.ReverseFind('\\') + 1);
	line += "ilt.exe";
	if (line.Find(' '))
		line = '\"' + line + '\"';
	line = "start \"Launch ILT\" " + line;
	line += " \"" + textpath + '\"';
	system(line);
}
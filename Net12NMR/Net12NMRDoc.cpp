// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://msdn.microsoft.com/officeui.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// Net12NMRDoc.cpp : implementation of the CNetNMRDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Net12NMR.h"
#endif

#include "MainFrame.h"
#include "Net12NMRDoc.h"
#include "MyRibbonBar.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CNet12NMRDoc

IMPLEMENT_DYNCREATE(CNet12NMRDoc, CDocument)

BEGIN_MESSAGE_MAP(CNet12NMRDoc, CDocument)
	ON_COMMAND(ID_HEADERNAMECOMBO, &CNet12NMRDoc::OnHeadernamecombo)
	ON_UPDATE_COMMAND_UI(ID_HEADERDELETEBUTTON, &CNet12NMRDoc::OnUpdateHeaderdeletebutton)
	ON_UPDATE_COMMAND_UI(ID_HEADERVALUEEDIT, &CNet12NMRDoc::OnUpdateHeaderdeletebutton)
	ON_COMMAND(ID_HEADERDELETEBUTTON, &CNet12NMRDoc::OnHeaderdeletebutton)
	ON_UPDATE_COMMAND_UI(ID_HEADERUPDATEBUTTON, &CNet12NMRDoc::OnUpdateHeaderupdatebutton)
	ON_COMMAND(ID_HEADERUPDATEBUTTON, &CNet12NMRDoc::OnHeaderupdatebutton)
	ON_UPDATE_COMMAND_UI(ID_HEADERNEWEDIT, &CNet12NMRDoc::OnUpdateHeadernewedit)
	ON_UPDATE_COMMAND_UI(ID_HEADERNEWBUTTON, &CNet12NMRDoc::OnUpdateHeadernewbutton)
	ON_COMMAND(ID_HEADERNEWBUTTON, &CNet12NMRDoc::OnHeadernewbutton)
END_MESSAGE_MAP()


// CNet12NMRDoc construction/destruction

CNet12NMRDoc::CNet12NMRDoc()
{
	initsp();
}

void CNet12NMRDoc::initsp()
{
	setintegralindex = -1;
	sp.dunused1 = sp.dunused2 = sp.dunused3 = sp.dunused4 = 0.0;
	sp.funused1 = sp.funused2 = sp.funused3 = 0.0f;
	sp.integralconst = 1.0f;
	sp.qca = sp.qcg = sp.apoValue = 0.0f;
	sp.bunused1 = sp.bunused2 = sp.bunused3 = false;
	sp.bintegralconst = true;
	sp.polybaseline = sp.baseline = sp.filtercomp = sp.level = sp.magnitude = sp.isLoren = sp.isPPM = sp.dclevel = false;
	sp.iunused1 = sp.iunused2 = sp.iunused3 = sp.groupcur = sp.groupnum = 0;
	sp.lrshift = 0;
	sp.ftflag = false;
}

CNet12NMRDoc::~CNet12NMRDoc()
{
	DeleteAllNewData();
}

void CNet12NMRDoc::DeleteAllNewData()
{
	POSITION pos = fh.GetStartPosition();
	while (pos) {
		CString type;
		fh.GetNextAssoc(pos, type, fhmap);
		fhmap->RemoveAll();
		delete fhmap;
	}
	fh.RemoveAll();
	dt.resize(0);
	fbaseline.resize(0);
	bbaseline.resize(0);
	while (baseregions.GetCount())
		delete baseregions.RemoveHead();
	while (peakregions.GetCount())
		delete peakregions.RemoveHead();
	while (intregions.GetCount()) {
		IREGION *p = intregions.RemoveHead();
		p->integral.resize(0);
		delete p;
	}
	while (peaks.GetCount())
		delete peaks.RemoveHead();
	initsp();
}

BOOL CNet12NMRDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

// CNet12NMRDoc serialization
void CNet12NMRDoc::Serialize(CArchive& ar)
{
	CString fileversion;
	if (ar.IsStoring())
	{
		SaveState();	// make sure the <base,int,peak>regions lists are up-to-date
		const char nl('\n');
		ar.WriteString(theApp.AppVersion + nl);
		ar.WriteString("FileVersion=A\n");
		ar.WriteString(fullpathname + nl);
		ar.WriteString(f1nucleus + nl);
		ar.WriteString(f2nucleus + nl);
		ar.WriteString(f3nucleus + nl);
		ar.WriteString(locknucleus + nl);
		ar.Write(&sp, sizeof(SPECTRUMPARAMETERS));
		UINT size = fh.GetCount();
		ar << size;	// number of fh types
		POSITION pos = fh.GetStartPosition();
		while (pos) {
			CString type;
			fh.GetNextAssoc(pos, type, fhmap);
			ar.WriteString(type + nl);
			POSITION tpos = fhmap->GetStartPosition();
			size = fhmap->GetCount();
			ar << size;	// number of key = value pairs
			while (tpos) {
				CString key, value;
				fhmap->GetNextAssoc(tpos, key, value);
				ar.WriteString(key + nl);
				ar.WriteString(value + nl);
			}
		}
		size = dt.size();
		ar << size;	// number of complex points
		for (UINT i=0; i<size; i++)
			ar << dt[i].real() << dt[i].imag();
		if (sp.ftflag) {
			// write baseline regions
			ar << baseregions.GetCount();
			POSITION pos = baseregions.GetHeadPosition();
			while (pos) {
				BREGION *p = baseregions.GetNext(pos);
				REGION r;
				r.start = (int)p->start;
				r.stop = (int)p->stop;
				ar.Write(&r, sizeof(REGION));
			}

			// write peak threshold regions
			ar << peakregions.GetCount();
			pos = peakregions.GetHeadPosition();
			while (pos) {
				PREGION *p = peakregions.GetNext(pos);
				REGION r;
				r.start = (int)p->start;
				r.stop = (int)p->stop;
				r.value = p->threshold;
				ar.Write(&r, sizeof(REGION));
			}

			// write the integral regions
			ar << intregions.GetCount();
			pos = intregions.GetHeadPosition();
			while (pos) {
				IREGION *p = intregions.GetNext(pos);
				REGION r;
				r.start = (int)p->start;
				r.stop = (int)p->stop;
				UINT n = p->stop - p->start;
				r.value = p->total;
				ar.Write(&r, sizeof(REGION));
			}
		}
	} else {
		CString fileversion;
		ar.ReadString(fileversion); // first read the appversion
		UpdateFlexHeader("Saved With", NULL, fileversion);
		ar.ReadString(fileversion);
		ar.ReadString(fullpathname);
		ar.ReadString(f1nucleus);
		ar.ReadString(f2nucleus);
		ar.ReadString(f3nucleus);
		ar.ReadString(locknucleus);
		ar.Read(&sp, sizeof(SPECTRUMPARAMETERS));
		UINT size;
		ar >> size;	// number of flex header types
		while (size--) {
			CString type;
			ar.ReadString(type);
			UINT j;
			ar >> j;	// number of key = value pairs;
			while (j--) {
				CString key, value;
				ar.ReadString(key);
				ar.ReadString(value);
				if (0 == key.Compare("IntegralConst")) {
					sp.integralconst = (float)atof(value);
					if (sp.integralconst <= 0.0f)
						sp.integralconst = 1.0f;
					sp.bintegralconst = true;
				} else
					UpdateFlexHeader(key, type, value);
			}
		}
		ar >> size;	// number of complex points
		dt.resize(size);
		for (UINT i=0; i<size; i++) {
			float re, im;
			ar >> re >> im;
			dt[i] = complex<float>(re, im);
		}
		if (sp.ftflag) {
			UINT n;
			ar >> n;	// number of baseline regions
			while (n--) {
				REGION r;
				ar.Read(&r, sizeof(REGION));
				BREGION *p = new BREGION;
				p->start = (UINT)r.start;
				p->stop = (UINT)r.stop;
				baseregions.AddTail(p);
			}
			ar >> n;	// number of peak regions
			while (n--) {
				REGION r;
				ar.Read(&r, sizeof(REGION));
				PREGION *p = new PREGION;
				p->start = (UINT)r.start;
				p->stop = (UINT)r.stop;
				p->threshold = r.value;
				peakregions.AddTail(p);
			}
			RecalcPeaks();
			ar >> n;	// number of integral regions
			while (n--) {
				REGION r;
				ar.Read(&r, sizeof(REGION));
				IREGION *p = new IREGION;
				p->selected = false;
				p->start = (UINT)r.start;
				p->stop = (UINT)r.stop;
				UINT n = p->stop - p->start;
				p->integral.resize(n);
				intregions.AddTail(p);
			}
			RecalcIntegrals();
		}
	}
}

void CNet12NMRDoc::Integrate(IREGION *p)
{
	p->total = 0.0;
	//p->from = ntoppm(p->start);
	//p->to = ntoppm(p->stop);
	UINT n = p->stop - p->start;
	p->integral.resize(n);
	for (UINT i=0; i<n; i++) {
		p->total += dt[i+p->start].real();
		p->integral[i] = p->total;
	}
}

void CNet12NMRDoc::RecalcIntegrals()
{
	POSITION pos = intregions.GetHeadPosition();	// reuse intregions list, if possible
	while (pos) {
		// first, calculate integrals
		IREGION *p = intregions.GetNext(pos);
		Integrate(p);
	}
}

bool CNet12NMRDoc::IsAPeak(UINT i, UINT size, UINT &from, UINT &to, float &s)
{
	float y = dt[i].real();
	s = (y > 0.0f) ? 1.0f : -1.0f;	// essentially, the sign of the peak
	y *= s;
	if (s*dt[i-1].real()>=y || y<s*dt[i+1].real())
		return false;
	from = (s*dt[i-2].real() <= s*dt[i-1].real()) ? i - 2 : i - 1;
	to = 0;
	while (i<size-2 && dt[i].real()==dt[i+1].real())
		i++;	// accumulate a flat peak!
	if (s*dt[i].real() > s*dt[i+1].real()) {
		// we have a peak!
		to = ++i;
		// where to stop?
		if (s*dt[i].real() >= s*dt[i+1].real())
			to = ++i;
	}
	return to ? true : false;
}

void CNet12NMRDoc::DefinePeak(PEAK *peak, UINT from, UINT to, float sign)
{
	peak->selected = false;
	peak->n = from + (to - from) / 2;
	//peak->ppmfit = peak->ppm = ntoppm(peak->n);
	peak->height = sign * dt[peak->n].real();
	double scale = 3.0 / double(peak->height);
	double A=0.0, B=0.0, C=0.0, E=0.0, F=1.0, G=0.0, H=0.0, I=0.0;
	for (UINT i=from; i<=to; i++) {
		double x = double(int(i) - int(peak->n));
		double y = double(sign * dt[i].real() - peak->height) * scale;
		//TRACE2("%g\t%g\n", x, y);
		double xx = x * x;
		I += y;
		H += x * y;
		G += xx * y;
		F += 1.0;
		E += x;
		C += xx;
		B += xx * x;
		A += xx * xx;
	}
	double D = C;
	double J = 2.0*B*C*E - C*C*D - A*E*E + A*D*F - B*B*F;
	if (F > 2.5 && J!=0.0 && A!=0.0) {
		double a = (C*(E*H-D*I) + B*(E*I-F*H) + G*(D*F-E*E))/J;
		double b = (I*(B*C-A*E) + H*(A*F-C*C) + G*(C*E-B*F))/J;
		double c = (G*(B*E-C*D) + H*(B*C-A*E) + I*(A*D-B*B))/J;
		double x = -0.5 * b / a;
		double y = a*x*x + b*x + c;
		peak->heightfit = sign * ((float)(y / scale) + peak->height);
		peak->height *= sign;
		peak->nfit = x + double(peak->n);
	}
}

void CNet12NMRDoc::RecalcPeaks()
{
	UINT size = dt.size();
	POSITION pos = peaks.GetHeadPosition();
	PEAK *pNew = NULL;
	POSITION ppos = peakregions.GetHeadPosition();
	while (ppos) {
		PREGION *pR = peakregions.GetNext(ppos);
		UINT start = (pR->start > 1) ? pR->start : 2;
		UINT stop = (pR->stop < size-2) ? pR->stop : size - 2;
		for (UINT i=start; i<stop; i++) {
			if (abs(dt[i].real())>abs(pR->threshold) && dt[i].real()*pR->threshold>0.0f) { // find negative and positive peaks
				UINT from, to;
				float s;
				if (IsAPeak(i, size, from, to, s)) {
					PEAK *peak;
					if (pos)
						peak = peaks.GetAt(pos);
					else
						peak = new PEAK;
					DefinePeak(peak, from, to, s);
					if (pos)
						peaks.GetNext(pos);
					else
						peaks.AddTail(peak);
				}
			}
		}
	}

	// delete any leftover peaks
	while (pos) {
		POSITION deadpos = pos;
		delete peaks.GetNext(pos);
		peaks.RemoveAt(deadpos);
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CNet12NMRDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CNet12NMRDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CNet12NMRDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CNet12NMRDoc diagnostics

#ifdef _DEBUG
void CNet12NMRDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CNet12NMRDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CNet12NMRDoc commands


void CNet12NMRDoc::Truncate(void)
{
	UINT size = dt.size();
	UINT newsize = 2;
	while (newsize < size)
		newsize *= 2;
	newsize /= 2;
	if (newsize > 2)
		dt.resize(newsize);
	SetModifiedFlag();
}

void CNet12NMRDoc::DCLevelCorrect(void)
{
	CWaitCursor wait;
	int size = dt.size();
	int n = size / 16;
	if ( n < 8 ) {
		n = size / 4;
		if ( n == 0 )
			return;
	}

	complex<float> dc(0.0f, 0.0f);

	for ( int m=1; m<=n; m++ )
		dc += dt[size-m];

	dc /= float(n);

	while ( size-- )
		dt[size] -= dc;

	sp.dclevel = true;;
	SetModifiedFlag();
}

void CNet12NMRDoc::QuadratureCorrection(void)
{
	CWaitCursor wait;
	int start = dt.size() / 32;
	if ( start <= 2 )
		return;
	double a = 0.0, g = 0.0, t = 0.0;
	for (UINT i=start; i<dt.size(); i++) {
		a += double(dt[i].real()) * double(dt[i].imag());
		g += pow(double(dt[i].imag()), 2);
	}
	a /= g;
	for (UINT i=start; i<dt.size(); i++) {
		double x = double(dt[i].real()) - a * double(dt[i].imag());
		t += x * x;
	}
	g = sqrt( g / t );
	for (UINT i=0; i<dt.size(); i++)
		dt[i] = complex<float>(float(g*double(dt[i].real()) - a*double(dt[i].imag())), dt[i].imag());
	sp.qca = float(a);
	sp.qcg = float(g);
	SetModifiedFlag();
}

void CNet12NMRDoc::LeftShift(void)
{
	dt.erase(dt.begin());
	dt.push_back(complex<float>(0.0f, 0.0f));
	sp.lrshift++;
	SetModifiedFlag();
}

void CNet12NMRDoc::RightShift(void)
{
	dt.insert(dt.begin(), complex<float>(0.0f, 0.0f));
	dt.pop_back();
	sp.lrshift--;
	SetModifiedFlag();
}

void CNet12NMRDoc::Apodize()
{
	CWaitCursor wait;
	bool isPPM = (theApp.apoUnits == ppmApoUnits);
	bool isLoren = (theApp.apoType == lorenApoType);
	double value = isPPM ? theApp.apoValue * sp.sf : theApp.apoValue;
	double a = value * (isLoren ? 1.836896377053 : 1.570796326795) / sp.sw;
	for (UINT i=0; i<dt.size(); i++) {
		double b = double(i) * a;
		if (! isLoren)
			b *= b;
		b = exp(-b);
		dt[i] *= float(b);
	}
	sp.isLoren = isLoren;
	sp.apoValue = float(theApp.apoValue);
	sp.isPPM = isPPM;
	SetModifiedFlag();
}

float CNet12NMRDoc::AmoTry(int ihi, int & nfunk, float fac)
{
	float fac1 = (1.0f-fac)/2.0f;
	float fac2 = fac1 - fac;
	float suma = param[0][0] + param[1][0] + param[2][0];
	float sumb = param[0][1] + param[1][1] + param[2][1];
	theApp.GlobalPhaseA = suma*fac1 - param[ihi][0]*fac2;
	theApp.GlobalPhaseB = sumb*fac1 - param[ihi][1]*fac2;
	GlobalPhase();
	float ytry = EvalPhase();
	nfunk++;
	if (ytry < yvals[ihi]) {
		yvals[ihi] = ytry;
		param[ihi][0] = theApp.GlobalPhaseA;
		param[ihi][1] = theApp.GlobalPhaseB;
	}
	return ytry;
}

void CNet12NMRDoc::AutoPhase(void)
{
	CWaitCursor wait;
	param[0][0] = theApp.GlobalPhaseA;
	param[0][1] = theApp.GlobalPhaseB;
	param[1][0] = theApp.GlobalPhaseA+10.0f;
	param[1][1] = theApp.GlobalPhaseB;
	param[2][0] = theApp.GlobalPhaseA;
	param[2][1] = theApp.GlobalPhaseB+10.0f;
	for (int i=0; i<3; i++) {
		theApp.GlobalPhaseA = param[i][0];
		theApp.GlobalPhaseB = param[i][1];
		GlobalPhase();
		yvals[i] = EvalPhase();
	}
	int nfunk = 0;
	while (1) {
		int ihi, inhi;
		int ilo = 0;
		if (yvals[0] > yvals[1]) {
			inhi = 1;
			ihi = 0;
		} else {
			inhi = 0;
			ihi = 1;
		}
		for (int i=0; i<3; i++) {
			if (yvals[i] < yvals[ilo]) ilo = i;
			if (yvals[i] > yvals[ihi]) {
				inhi = ihi;
				ihi = i;
			} else if (yvals[i] > yvals[inhi]) {
				if ( i != ihi)
					inhi = i;
			}
		}
		float rtol = sqrt(sq(abs(param[ihi][0]-param[ilo][0]))+sq(abs(param[ihi][1]-param[ilo][1])));
		if (rtol<0.5 || nfunk>1000) {
			theApp.GlobalPhaseA = param[ilo][0];
			theApp.GlobalPhaseB = param[ihi][1];
			GlobalPhase();
			float yfinal = EvalPhase();
//			TRACE3("FINAL    A=%.2f B=%.2f y=%g\n", theApp.GlobalPhaseA, theApp.GlobalPhaseB, yfinal);
			break;
		}
		float ytry = AmoTry(ihi, nfunk, -1.0f);
//		TRACE3("REFLECT  A=%.2f B=%.2f y=%g\n", theApp.GlobalPhaseA, theApp.GlobalPhaseB, ytry);
		if (ytry <= yvals[ilo]) {
			ytry = AmoTry(ihi, nfunk, 2.0f);
//			TRACE3("EXPAND   A=%.2f B=%.2f y=%g\n", theApp.GlobalPhaseA, theApp.GlobalPhaseB, ytry);
		} else if (ytry >= yvals[inhi]) {
			float ysave = yvals[ihi];
			ytry = AmoTry(ihi, nfunk, 0.5f);
//			TRACE3("CONTRACT A=%.2f B=%.2f y=%g\n", theApp.GlobalPhaseA, theApp.GlobalPhaseB, ytry);
			if (ytry >= ysave) {
				for (int i=0; i<3; i++) {
					if (i != ilo) {
						theApp.GlobalPhaseA = param[i][0] = 0.5f * (param[i][0] + param[ilo][0]);
						theApp.GlobalPhaseB = param[i][1] = 0.5f * (param[i][1] + param[ilo][1]);
						GlobalPhase();
						yvals[i] = EvalPhase();
//						TRACE3("SHRINK   A=%.2f B=%.2f y=%g\n", theApp.GlobalPhaseA, theApp.GlobalPhaseB, yvals[i]);
					}
				}
				nfunk += 2;
			}
		}
	}
	UINT imax;
	double max = 0.0;
	for (UINT i=0; i<dt.size(); i++) {
		float m = abs(dt[i]);
		if (m > max) {
			max = m;
			imax = i;
		}
	}
	if (0.0f > dt[imax].real()) {
		theApp.GlobalPhaseA += 180.0f;
		GlobalPhase();
	}
}

void CNet12NMRDoc::SHFT(float &a, float &b, float &c, float &d)
{
	a = b;
	b = c;
	c = d;
}

void CNet12NMRDoc::Bracket(float &ax, float &bx, float &cx, float &fa, float &fb, float &fc)
{
	const float GOLD = 1.618034f;
	float dum = 0.0f;
	if (fb > fa) {
		SHFT(dum, ax, bx, dum);
		SHFT(dum, fb, fa, dum);
	}
	theApp.GlobalPhaseA = cx = bx + GOLD * (bx - ax);
	GlobalPhase();
	fc = EvalPhase();
	while (fb > fc) {
		float r = (bx - ax) * (fb - fc);
		float q = (bx - cx) * (fb - fa);
		float fu, u = bx - ((bx-cx)*q - (bx-ax)*r) / (2.0f * (float)_copysign(__max(fabsf(q-r),1.0e-20f), q-r));
		float ulim = bx + 100.0f * (cx-bx);
		if ((bx-u)*(u-cx) > 0.0f) {
			theApp.GlobalPhaseA = u;
			GlobalPhase();
			float fu = EvalPhase();
			if (fu < fc) {
				ax = bx;
				bx = u;
				fa = fb;
				return;
			} else if (fu > fb) {
				cx = u;
				fc = fu;
				return;
			}
			theApp.GlobalPhaseA = u = cx + GOLD * (cx - bx);
			GlobalPhase();
			fu = EvalPhase();
		} else if ((cx-u)*(u-ulim) > 0.0f) {
			theApp.GlobalPhaseA = u;
			GlobalPhase();
			fu = EvalPhase();
			if (fu < fc) {
				theApp.GlobalPhaseA = dum = cx + GOLD * (cx-bx);
				SHFT(bx, cx, u, dum);
				GlobalPhase();
				dum = EvalPhase();
				SHFT(fb, fc, fu, dum);
			}
		} else if ((u-ulim)*(ulim-cx) >= 0.0f) {
			theApp.GlobalPhaseA = u = ulim;
			GlobalPhase();
			fu = EvalPhase();
		} else {
			theApp.GlobalPhaseA = u = cx + GOLD * (cx-bx);
			GlobalPhase();
			fu = EvalPhase();
		}
		SHFT(ax, bx, cx, u);
		SHFT(fa, fb, fc, fu);
	}
}

void CNet12NMRDoc::AutoPhaseAOnly()
{
	const float R = 0.61803399f;
	const float C = 1.0f - R;
	float f0, f1, f2, f3, x0, x1, x2, x3;
	theApp.GlobalPhaseB = 0.0f;
	float ax = theApp.GlobalPhaseA;
	GlobalPhase();
	float fa = EvalPhase();
	float bx = ax + 10.0f;
	theApp.GlobalPhaseA = bx;
	GlobalPhase();
	float fb = EvalPhase();
	float fc, cx;
	Bracket(ax, bx, cx, fa, fb, fc);
	x0 = ax;
	x3 = cx;
	if (fabsf(cx-bx) > fabsf(bx-ax)) {
		x1 = bx;
		x2 = bx + C * (cx-bx);
	} else {
		x2 = bx;
		x1 = bx - C * (bx-ax);
	}
	theApp.GlobalPhaseA = x1;
	GlobalPhase();
	f1 = EvalPhase();
	theApp.GlobalPhaseA = x2;
	GlobalPhase();
	f2 = EvalPhase();
	while (fabsf(x3-x0) > 0.00001f * (fabsf(x1)+fabsf(x2))) {
		if (f2 < f1) {
			float dum = R*x1 + C*x3;
			SHFT(x0, x1, x2, dum);
			theApp.GlobalPhaseA = x2;
			GlobalPhase();
			dum = EvalPhase();
			SHFT(f0, f1, f2, dum);
		} else {
			float dum = R*x2 + C*x0;
			SHFT(x3, x2, x1, dum);
			theApp.GlobalPhaseA = x1;
			GlobalPhase();
			dum = EvalPhase();
			SHFT(f3, f2, f1, dum);
		}
	}
	if (f1 < f2)
		theApp.GlobalPhaseA = x1;
	else
		theApp.GlobalPhaseA = x2;
	GlobalPhase();
	UINT imax;
	float max = 0.0;
	for (UINT i=0; i<dt.size(); i++) {
		float m = abs(dt[i]);
		if (m > max) {
			max = m;
			imax = i;
		}
	}
	if (0.0f > dt[imax].real()) {
		theApp.GlobalPhaseA += 180.0f;
		GlobalPhase();
	}
}

float CNet12NMRDoc::EvalPhase()
{
	UINT i;
	UINT size = dt.size();
	FindBaseline(8, 15.0f, 0, size);
	float A=0.0f, B=0.0f, D=0.0f, E=0.0f, F=0.0f, G=0.0f, H=0.0f, I=0.0f;
	for (i=0; i<dt.size(); i++) {
		if (! bbaseline[i]) {
			float y = dt[i].real();
			float x = float(i)/float(dt.size());
			float x2 = x*x;
			A += x2*x2;
			B += x2*x;
			D += x2;
			E += x;
			F++;
			G += y*x2;
			H += y*x;
			I += y;
		}
	}
	if (0.0f == F) return 1.0e10f;
	float C = D;
	float J = 1.0f/(C*(2.0f*B*E-C*D)-A*E*E+F*(A*D-B*B));
	float a = J*(C*(E*H-D*I)+B*(E*I-F*H)+G*(D*F-E*E));
	float b = J*(B*(C*I-F*G)+H*(A*F-C*C)+E*(C*G-A*I));
	float c = J*(G*(B*E-C*D)+H*(B*C-A*E)+I*(A*D-B*B));
	for (A=0.0f, F=0.0f, i=0; i<dt.size(); i++) {
		if (! bbaseline[i]) {
			float x = float(i)/float(dt.size());
			A += sq(dt[i].real() - (x*(a*x+b)+c));
			F++;
		}
	}
	if (F==1.0) return 1.0e10f;
	return sqrt(A/(F-1.0f));
}

void CNet12NMRDoc::GlobalPhase(void)
{
	CWaitCursor wait;
	while (theApp.GlobalPhaseA < 0.0f)
		theApp.GlobalPhaseA += 360.0f;
	while (theApp.GlobalPhaseA >= 360.0f)
		theApp.GlobalPhaseA -= 360.0f;
	float a = theApp.GlobalPhaseA - sp.pha;
	float b = theApp.GlobalPhaseB - sp.phb;
	if (a==0.0f && b==0.0f)
		return;
	for (UINT i=0; i<dt.size(); i++) {
		float theta = 1.74532925199e-2f * (a + float(int(i)-int(dt.size()/2)) * b / float(dt.size()));
		complex<float> c = polar(abs(dt[i]), arg(dt[i]) - theta);
		dt[i] = c;
	}
	sp.pha = theApp.GlobalPhaseA;
	sp.phb = theApp.GlobalPhaseB;
	SetModifiedFlag();
}

void CNet12NMRDoc::FindBaseline(UINT width, float sn, UINT start, UINT stop)
{
	UINT size = dt.size();
	if (bbaseline.size() != size)
		bbaseline.resize(size);
	for (UINT i=0; i<size; i++) {
		bbaseline[i] = false;
	}
	// FIND SIGMA
	float sigma = 0.0f;
	UINT span = size / 32;
	for (UINT i=0; i<size; i+=span) {
		float ave = 0.0;
		for (UINT j=0; j<span; j++)
			ave += dt[i+j].real();
		ave /= span;
		float tmp = 0.0;
		for (UINT j=0; j<span; j++)
			tmp += sq(dt[i+j].real() - ave);
		tmp = sqrt(tmp/float(span-1));
		if (sigma > 0.0) {
			if (tmp>0.0 && tmp<sigma)
				sigma = tmp;
		} else
			sigma = tmp;
	}
	// IS POINT NOT IN BASELINE?
	if (start < width) {
		for (UINT i=start; i<width; i++)
			bbaseline[i] = true;
		start = width;
	}
	if (stop > size-width) {
		for (UINT i=size-width; i<size; i++)
			bbaseline[i] = true;
		stop = size - width;
	}
	for (UINT i=start; i<stop; i++) {
		float min = dt[i].real();
		float max = min;
		for (UINT j=i-width; j<i+width; j++) {
			float d = dt[j].real();
			if (d > max)
				max = d;
			else if (d < min)
				min = d;
		}
		if (max-min > sn * float(sigma))
			bbaseline[i] = true;
	}
}

void CNet12NMRDoc::InitializeBaseline()
{
	UINT size = dt.size();
	if (bbaseline.size()<size || fbaseline.size()<size) {
		bbaseline.resize(size);
		fbaseline.resize(size);
		for (UINT i=0; i<size; i++) {
			bbaseline[i] = false;
			fbaseline[i] = 0.0f;
		}
	}
	POSITION pos = baseregions.GetHeadPosition();
	while (pos) {
		BREGION *p = baseregions.GetNext(pos);
		for (UINT i=p->start; i<p->stop; i++)
			bbaseline[i] = true;
	}
}

void CNet12NMRDoc::CalcBaseline(int from, int to)
{
	if (theApp.polybaseline) {
		double vv[10], a[10][10];
		int indx[10];
		for (int i=0; i<10; i++) {
			basecoef[i] = 0.0;
			for (int j=0; j<10; j++) a[i][j] = 0.0;
		}
		for (int i=from; i<to; i++ ) {
			double y, d, x;
			if (! bbaseline[i]) {
				d = x = 1.0;
				y = double(dt[i].real());
				a[theApp.m_nPolyorder-1][theApp.m_nPolyorder-1] += 1.0;
				basecoef[theApp.m_nPolyorder-1] += y;
				for (int j=theApp.m_nPolyorder-2; j>=0; j--) {
					x *= double(i);
					a[j][j+1] += x;
					d *= double(i);
					basecoef[j] += d * y;
					x *= double( i );
					a[j][j] += x;
				}
			}
		}
		if ( (int)a[theApp.m_nPolyorder-1][theApp.m_nPolyorder-1] < theApp.m_nPolyorder ) {
			AfxMessageBox("Number of baseline points must exceed polynomial order!");
			return;
		}
		for (int i=0; i<theApp.m_nPolyorder-2; i++) {
			for (int j=i+2; j<theApp.m_nPolyorder; j++) {
				int k = i + j;
				a[i][j] = a[k/2][k/2+k%2];
			}
		}
		for (int i=1; i<theApp.m_nPolyorder; i++)
			for (int j=0; j<i; j++)
				a[i][j] = a[j][i];
		double temp;
		for (int i=0; i<theApp.m_nPolyorder; i++) {
			double big = 0.0f;
			for (int j=0; j<theApp.m_nPolyorder; j++)
				if ((temp = fabs(a[i][j])) > big )
					big = temp;
				if (big == 0.0f) {
					AfxMessageBox("Matrix problem, aborting.");
					return;
				}
			vv[i] = 1.0f / big;
		}
		for (int j=0; j<theApp.m_nPolyorder; j++) {
			for (int i=0; i<j; i++ ) {
				double sum = a[i][j];
				for ( int k=0; k<i; k++ )
					sum -= a[i][k] * a[k][j];
				a[i][j] = sum;
			}
			double big = 0.0;
			int imax;
			for (int i=j; i<theApp.m_nPolyorder; i++) {
				double sum = a[i][j];
				for(int k=0; k<j; k++)
					sum -= a[i][k] * a[k][j];
				a[i][j] = sum;
				double dum = vv[i] * fabs(sum);
				if (dum >= big) {
					big = dum;
					imax = i;
				}
			}
			if (j != imax) {
				for(int k=0; k<theApp.m_nPolyorder; k++) {
					double dum = a[imax][k];
					a[imax][k] = a[j][k];
					a[j][k] = dum;
				}
				vv[imax] = vv[j];
			}
			indx[j] = imax;
			if (a[j][j] == 0.0f)
				a[j][j] = 1.0e-20f;
			if (j != theApp.m_nPolyorder) {
				double dum = 1.0 / a[j][j];
				for(int i=j+1; i<theApp.m_nPolyorder; i++)
					a[i][j] *= dum;
			}
		}
		int	ii = -1, ip;
		for (int i=0; i<theApp.m_nPolyorder; i++) {
			ip = indx[i];
			double sum = basecoef[ip];
			basecoef[ip] = basecoef[i];
			if (ii > -1)
				for (int j=ii; j<=i-1; j++)
					sum -= a[i][j] * basecoef[j];
			else if (sum)
				ii = i;
			basecoef[i] = sum;
		}
		for (int i=theApp.m_nPolyorder-1; i>=0; i--) {
			double sum = basecoef[i];
			for (int j=i+1;j<theApp.m_nPolyorder; j++)
				sum -= a[i][j] * basecoef[j];
			basecoef[i] = sum / a[i][i];
		}
		for (int i=from; i<to; i++) {
			double y = basecoef[0];
			for (int j=1; j<theApp.m_nPolyorder; j++) {
				y *= double(i);
				y += basecoef[j];
			}
			fbaseline[i] = float(y);
		}
	} else {
		int start = from;
		int stop = to;
		if (stop-start <= 0)
			return;
		for (int i=start; i<stop; i++) {
			if (! bbaseline[i]) {
				float ave = 0.0f;
				int count = 0;
				for (int j=i-theApp.m_nMAWidth; j<i+theApp.m_nMAWidth; j++) {
					if (j>=0 && j<int(dt.size())) {
						count++;
						ave += dt[j].real();
					}
				}
				fbaseline[i] = ave/float(count);
			}
		}
		int i = start;
		while (i < stop) {
			if (bbaseline[i]) {
				float ystart, ystop;
				int j;
				for (j=i; j<stop && bbaseline[j]; j++)
					;
				if (j > stop-1) {
					if (i == start) break;
					ystop = fbaseline[i-1];
				} else {
					ystop = fbaseline[j];
				}
				if (i == start) {
					ystart = ystop;
				} else {
					ystart = fbaseline[i-1];
				}
				float slope = (ystop-ystart)/float(j-(i-1));
				float ycept = ystart - slope*float(i-1);
				while (i < j) {
					fbaseline[i] = slope*float(i) + ycept;
					i++;
				}
			} else {
				i++;
			}
		}
	}
	}

float CNet12NMRDoc::sq(float x)
{
	return x * x;
}

void CNet12NMRDoc::ZeroFill(void)
{
	UINT newsize = 2;
	UINT oldsize = dt.size(); 
	while (newsize <= oldsize)
		newsize *= 2;
	dt.resize(newsize);
	for (UINT i=oldsize; i<newsize; i++)
		dt[i] = complex<float>(0.0f, 0.0f);
	SetModifiedFlag();
}

void CNet12NMRDoc::FourierTransform(void)
{
	CWaitCursor wait;
	UINT n = 2;
	while (n < dt.size())
		n *= 2;
	if (n != dt.size())
		ZeroFill();
	n = dt.size();
	// inverse imaginary, if needed
	if (! sp.ssb) {
		for (UINT i=0; i<n; i++)
			dt[i] = conj(dt[i]);
	}
	/* compute power of 2 corresponding to n */
	UINT i = 2;
	UINT m = 1;
	while (i < n) {
		i *= 2;
		m++;
 	}
 	n = i;
	UINT n1 = n - 1;
	UINT n2 = 2 * n;
	for (UINT k=1; k<m; k++) {
		n2 /= 2;
		UINT n4 = n2 / 4;
		float e = 6.28318530717959f / float(n2);
		float a = 0.0f;
		for(UINT j=1; j<=n4; j++) {
			float a3 = 3 * a;
			float cc1 = cos(a);
			float ss1 = sin(a);
			float cc3 = cos(a3);
			float ss3 = sin(a3);
			a = float(j) * e;
			UINT isxx = j;
			UINT id = 2 * n2;
			while (isxx < n) {
				for(UINT i0=isxx-1; i0<n1; i0+=id) {
					UINT i1 = i0 + n4;
					UINT i2 = i1 + n4;
					UINT i3 = i2 + n4;
					float r1 = dt[i0].real() - dt[i2].real();
					float r2 = dt[i1].real() - dt[i3].real();
					float s1 = dt[i0].imag() - dt[i2].imag();
					float s2 = dt[i1].imag() - dt[i3].imag();
					dt[i0] += dt[i2];
					dt[i1] += dt[i3];
					float s3 = r1 - s2;
					r1 += s2;
					s2 = r2 - s1;
					r2 += s1;
					dt[i2] = complex<float>(r1*cc1-s2*ss1, -s2*cc1-r1*ss1);
					dt[i3] = complex<float>(s3*cc3+r2*ss3, r2*cc3-s3*ss3);
				}
				isxx = 2 * id - n2 + j;
				id = 4 * id;
			}
		}
	}
	/* last stage, length = 2 butterfly */
	UINT isxx = 1;
	UINT id = 4;
	while (isxx < n) {
		for (UINT i1=isxx; i1<=n; i1+=id) {
			UINT i0 = i1 - 1;
			complex<float> tmp(dt[i0]);
			dt[i0] = tmp + dt[i1];
			dt[i1] = tmp - dt[i1];
		}
		isxx = 2 * id - 1;
		id = 4 * id;
	}
	/* bit reverse counter */
	UINT j = 1;
	for (i=1; i<=n1; i++) {
		if (i < j) {
			UINT i0 = i - 1;
			UINT i1 = j - 1;
			complex<float> tmp(dt[i0]);
			dt[i0] = dt[i1];
			dt[i1] = tmp;
		}
		UINT k = n / 2;
		while (k>=2 && k<j) {
			j -= k;
			k /= 2;
		}
		j += k;
	}
	UINT k = n / 2;
	float norm = sqrt(1.0f / float(n));
	for (UINT i0=0, i1=k-1, i2=k, i3=n-1; i0<k/2; i0++, i1--, i2++, i3-- ) {
		complex<float> tmp(dt[i0] * norm);
		dt[i0] = dt[i1] * norm;
		dt[i1] = tmp;
		tmp = dt[i2] * norm;
		dt[i2] = dt[i3] * norm;
		dt[i3] = tmp;
	}
	sp.ftflag = true;
	float phtmp = theApp.GlobalPhaseB;
	if (sp.phc)
		theApp.GlobalPhaseB += sp.phc;
	GlobalPhase();
	if (sp.phc) {
		theApp.GlobalPhaseB = phtmp;
		sp.pha = theApp.GlobalPhaseA;
		sp.phb = theApp.GlobalPhaseB;
		sp.phc = 0.0f;
	}
	SetModifiedFlag();
}

double CNet12NMRDoc::ntoppm(UINT n)
{
	double dn = double(n);
	return ntoppm(dn);
}

double CNet12NMRDoc::ntoppm(int n)
{
	double dn = double(n);
	return ntoppm(dn);
}

double CNet12NMRDoc::ntoppm(double n)
{
	return (((0.5 * sp.sw + sp.of) - sp.sw * n / double(dt.size() - 1)) / sp.sf);
}

int CNet12NMRDoc::ppmton(double ppm)
{
	ppm *= sp.sf;
	ppm = 0.5 * sp.sw + sp.of - ppm;
	int s = int(dt.size());
	int i = int(double(s) * ppm / sp.sw);
	if (i < 0)
		i = 0;
	else if (i >= s)
		i = s - 1;
	return i;
}

void CNet12NMRDoc::Magnitude()
{
	for (UINT i=0; i<dt.size(); i++)
		dt[i] = complex<float>(abs(dt[i]), 0.0f);
	sp.magnitude = true;
	SetModifiedFlag();
}

void CNet12NMRDoc::UpdateFlexHeader(LPCTSTR key, LPCTSTR type, LPCTSTR value)
{
	CString t = type;
	if (t.IsEmpty())
		t = "NULL";
	if (! fh.Lookup(t, fhmap)) {
		fhmap = new CMapStringToString();
		fh.SetAt(t, fhmap);
	}
	fhmap->SetAt(key, value);
}

bool CNet12NMRDoc::GetFlexHeaderItem(LPCTSTR key, CString &value)
{
	POSITION pos = fh.GetStartPosition();
	while (pos) {
		CString type;
		fh.GetNextAssoc(pos, type, fhmap);
		if (fhmap->Lookup(key, value))
			return true;
	}
	value.Empty();
	return false;
}

void CNet12NMRDoc::FilterComp(void)
{
	CWaitCursor wait;
	UINT size = dt.size();
	for (UINT i=size/2; i<size; i++) {
		float x = 2.0e4f * float(i-size/2) / float(size);
		x = 1.0f / ((-2.464e-9f * x - 2.146e-8f) * x + 1.0f);
		dt[i] *= x;
		dt[(size-i)-1] *= x;
	}
	sp.filtercomp = true;
	SetModifiedFlag();
}

void CNet12NMRDoc::Level(int from, int to)
{
	CWaitCursor wait;
	int n = (to - from + 1) / 32;
	if (n < 2)
		n = 2;
	float nn = float(n * 2);
	float sx=0.0, sx2=0.0, sy=0.0, sxy=0.0;
	for (int i=0; i<n; i++) {
		float x = (float)(to - (i + 1));
		float y = dt[to-(i+1)].real();
		sx += x;
		sx2 += x * x;
		sy += y;
		sxy += y * x;
		x = (float)(from + i);
		y = dt[from+i].real();
		sx += x;
		sx2 += x * x;
		sy += y;
		sxy += y * x;
	}
	float slope = (nn*sxy - sx*sy) / (nn*sx2 - sx*sx);
	float ycept = (sy - slope*sx) / nn;
	for (int i=from; i<to; i++)
		dt[i] -= complex<float>(float(i) * slope + ycept, 0.0f);
	sp.level = true;
	SetModifiedFlag();
}

void CNet12NMRDoc::GetMinMax(float &min, float &max)
{
	min = dt[0].real();
	max = min;
	for (UINT i=1; i<dt.size(); i++) {
		float y = dt[i].real();
		if (y < min)
			min = y;
		else if (y > max)
			max = y;
	}
}

void CNet12NMRDoc::UpdateRibbon(CMyRibbonBar *pRibbon)
{
	CStringList keys;
	// rebuild the header category.
	CMFCRibbonComboBox *pCombo = DYNAMIC_DOWNCAST(CMFCRibbonComboBox, pRibbon->FindByID(ID_HEADERNAMECOMBO));
	if (! pCombo)
		return;	// no header ribbon tab in the current ribbon (printing perhaps)
	// remember where we are!
	CString curkey;
	if (pCombo->GetCount())
		curkey = pCombo->GetItem(pCombo->GetCurSel());
	// clear it
	pCombo->RemoveAllItems();
	// now load all the values
	POSITION pos = fh.GetStartPosition();
	while (pos) {
		CString type;
		fh.GetNextAssoc(pos, type, fhmap);
		if (type.Compare("ProcPar")) {
			POSITION fpos = fhmap->GetStartPosition();
			while (fpos) {
				CString name, value;
				fhmap->GetNextAssoc(fpos, name, value);
				//TRACE2("Adding %s = %s\n", name, value);
				// now we need to sort the flex header parameter names
				// we'll do that in CStringList key.
				if (keys.IsEmpty())
					// the keys list is empty, add the first value
					keys.AddHead(name);
				else {
					// figure out where to add this new value
					POSITION lpos = keys.GetHeadPosition();
					while (lpos) {
						CString key = keys.GetAt(lpos);
						if (name.CompareNoCase(key) < 0 ) {
							keys.InsertBefore(lpos, name);
							break;
						}
						keys.GetNext(lpos);
					}
					// if we have made it through the entire sorted list without adding this new value,
					// then this new key gets tacked on at the end.
					if (NULL == lpos)
						keys.AddTail(name);
				}
			}
		}
	}
	// now fill the control with the sorted list
	while (! keys.IsEmpty())
		pCombo->AddItem(keys.RemoveHead());
	// set the combo box to an item and also set the value in its item.
	CMFCRibbonEdit *pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, pRibbon->FindByID(ID_HEADERVALUEEDIT));
	if (curkey.IsEmpty()) {
		pCombo->SelectItem(0);
		curkey = pCombo->GetItem(0);
	} else
		pCombo->SelectItem(curkey);
	CString curval;
	GetFlexHeaderItem(curkey, curval);
	pEdit->SetEditText(curval);
}

void CNet12NMRDoc::OnHeadernamecombo()
{
	CMainFrame *pMain = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CMyRibbonBar *pRibbon = DYNAMIC_DOWNCAST(CMyRibbonBar, pMain->GetRibbonBar());
	CMFCRibbonEdit *pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, pRibbon->FindByID(ID_HEADERVALUEEDIT));
	CMFCRibbonComboBox *pCombo = DYNAMIC_DOWNCAST(CMFCRibbonComboBox, pRibbon->FindByID(ID_HEADERNAMECOMBO));
	CString key = pCombo->GetItem(pCombo->GetCurSel());
	CString value;
	GetFlexHeaderItem(key, value);
	pEdit->SetEditText(value);
}

void CNet12NMRDoc::OnUpdateHeaderdeletebutton(CCmdUI *pCmdUI)
{
	CMainFrame *pMain = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CMyRibbonBar *pRibbon = DYNAMIC_DOWNCAST(CMyRibbonBar, pMain->GetRibbonBar());
	CMFCRibbonComboBox *pCombo = DYNAMIC_DOWNCAST(CMFCRibbonComboBox, pRibbon->FindByID(ID_HEADERNAMECOMBO));
	pCmdUI->Enable((pCombo->GetCurSel() >= 0) ? TRUE : FALSE);
}

void CNet12NMRDoc::OnHeaderdeletebutton()
{
	CMainFrame *pMain = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CMyRibbonBar *pRibbon = DYNAMIC_DOWNCAST(CMyRibbonBar, pMain->GetRibbonBar());
	CMFCRibbonComboBox *pCombo = DYNAMIC_DOWNCAST(CMFCRibbonComboBox, pRibbon->FindByID(ID_HEADERNAMECOMBO));
	int i = pCombo->GetCurSel();
	CString key = pCombo->GetItem(i);
	POSITION pos = fh.GetStartPosition();
	while (pos) {
		CString type;
		fh.GetNextAssoc(pos, type, fhmap);
		if (fhmap->RemoveKey(key)) {	// try to remove the current key from a type.
			pCombo->DeleteItem(i);		// Yes!, delete the item in the ComboBox
			if (pCombo->GetCount()) {	// show the next item
				if (i >= pCombo->GetCount())
					i = pCombo->GetCount() - 1;
				pCombo->SelectItem(i);
				key = pCombo->GetItem(i);
				CString value;
				GetFlexHeaderItem(key, value);
				CMFCRibbonEdit *pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, pRibbon->FindByID(ID_HEADERVALUEEDIT));
				pEdit->SetEditText(value);
			}
			if (fhmap->IsEmpty()) {
				// if the type is now empty, remove it
				fh.RemoveKey(type);
				delete fhmap;
			}
		}
	}
}

void CNet12NMRDoc::OnUpdateHeaderupdatebutton(CCmdUI *pCmdUI)
{
	CMainFrame *pMain = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CMyRibbonBar *pRibbon = DYNAMIC_DOWNCAST(CMyRibbonBar, pMain->GetRibbonBar());
	CMFCRibbonComboBox *pCombo = DYNAMIC_DOWNCAST(CMFCRibbonComboBox, pRibbon->FindByID(ID_HEADERNAMECOMBO));
	int enable = FALSE;
	if (pCombo->GetCurSel() >= 0) {
		CString key = pCombo->GetItem(pCombo->GetCurSel());
		CString value;
		if (GetFlexHeaderItem(key, value)) {
			CMFCRibbonEdit *pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, pRibbon->FindByID(ID_HEADERVALUEEDIT));
			enable = (value.Compare(pEdit->GetEditText())) ? TRUE : FALSE;
		}
	}
	pCmdUI->Enable(enable);
}

void CNet12NMRDoc::OnHeaderupdatebutton()
{
	// this button is only enabled if the text in the header value editbox is different form the value in the flex header.
	// so if it's pressed, just do it!
	CMainFrame *pMain = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CMyRibbonBar *pRibbon = DYNAMIC_DOWNCAST(CMyRibbonBar, pMain->GetRibbonBar());
	CMFCRibbonComboBox *pCombo = DYNAMIC_DOWNCAST(CMFCRibbonComboBox, pRibbon->FindByID(ID_HEADERNAMECOMBO));
	CMFCRibbonEdit *pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, pRibbon->FindByID(ID_HEADERVALUEEDIT));
	CString key = pCombo->GetItem(pCombo->GetCurSel());

	// update the key in all the types in the flex header!
	POSITION pos = fh.GetStartPosition();
	while (pos) {
		CString type, dummy;
		fh.GetNextAssoc(pos, type, fhmap);
		if (fhmap->Lookup(key, dummy)) {
			UpdateFlexHeader(key, type, pEdit->GetEditText());
			SetModifiedFlag();
		}
	}
	UpdateAllViews(NULL);
}

void CNet12NMRDoc::OnUpdateHeadernewedit(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);	// always on
}

void CNet12NMRDoc::OnUpdateHeadernewbutton(CCmdUI *pCmdUI)
{
	int enable = FALSE;
	CMainFrame *pMain = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CMyRibbonBar *pRibbon = DYNAMIC_DOWNCAST(CMyRibbonBar, pMain->GetRibbonBar());
	CMFCRibbonEdit *pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, pRibbon->FindByID(ID_HEADERNEWEDIT));
	CString newname = pEdit->GetEditText();
	if (! newname.IsEmpty()) {
		CString value;
		// only enable if we can't find it in the current flex header.
		enable = (GetFlexHeaderItem(newname, value)) ? FALSE : TRUE;
	}
	pCmdUI->Enable(enable);
}

void CNet12NMRDoc::OnHeadernewbutton()
{
	CMainFrame *pMain = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CMyRibbonBar *pRibbon = DYNAMIC_DOWNCAST(CMyRibbonBar, pMain->GetRibbonBar());
	CMFCRibbonComboBox *pCombo = DYNAMIC_DOWNCAST(CMFCRibbonComboBox, pRibbon->FindByID(ID_HEADERNAMECOMBO));
	CMFCRibbonEdit *pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, pRibbon->FindByID(ID_HEADERNEWEDIT));
	CString key = pEdit->GetEditText();
	const CString value = "Update this value!";
	UpdateFlexHeader(key, "NULL", value);
	pCombo->SelectItem(pCombo->AddItem(key));
	pEdit->SetEditText("");
	pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, pRibbon->FindByID(ID_HEADERVALUEEDIT));
	pEdit->SetEditText(value);
	SetModifiedFlag();
	UpdateAllViews(NULL);
}

void CNet12NMRDoc::DeleteContents()
{
	DeleteAllNewData();
	CDocument::DeleteContents();
}


BOOL CNet12NMRDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	leaf.s.location = shared;
	leaf.s.leaftype = net12nmrfile;
	leaf.s.dimension = simple;
	leaf.s.drivenumber = lpszPathName[0] - 'A' + 1;
	fullpathname = lpszPathName;

	return TRUE;
}

void CNet12NMRDoc::SaveState()
{
	UINT n = dt.size();
	if (baselineMode == theApp.disMode) {
		if (bbaseline.size() < n)
			return;
		POSITION pos = baseregions.GetHeadPosition();
		BREGION *p = NULL;
		bool prev = false;
		for (UINT i=0; i<=n; i++) {
			bool current = (i==n) ? false : bbaseline[i];
			if (prev != current) {
				if (current) {
					if (pos)
						p = baseregions.GetAt(pos);
					else
						p = new BREGION;
					p->start = i;
				} else {
					p->stop = i;
					if (pos)
						baseregions.GetNext(pos);
					else
						baseregions.AddTail(p);
				}
				prev = current;
			}
		}
		while (pos) {
			POSITION deadpos = pos;
			delete baseregions.GetNext(pos);
			baseregions.RemoveAt(deadpos);
		}
	}
}

void CNet12NMRDoc::LoadState()
{
	UINT size = dt.size();
	if (normalMode == theApp.disMode) {
		CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
		UpdateRibbon(&pMain->m_wndRibbonBar);
	} else if (baselineMode == theApp.disMode) {
		if (bbaseline.size()<size || fbaseline.size()<size) {
			bbaseline.resize(size);
			fbaseline.resize(size);
		}
		for (UINT i=0; i<size; i++) {
			fbaseline[i] = 0.0f;
			bbaseline[i] = false;
		}
		POSITION pos = baseregions.GetHeadPosition();
		while (pos) {
			BREGION *p = baseregions.GetNext(pos);
			for (UINT i=p->start; i<p->stop; i++)
				bbaseline[i] = true;
		}
	} else if (shelfMode == theApp.disMode) {
		CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
		theApp.shelf.RibbonUpdate(pMain->m_wndRibbonBar);
	}
}

void CNet12NMRDoc::OnCloseDocument()
{
	// base class does its work first
	CDocument::OnCloseDocument();

	// then if there are no more documents, clear two output tabs.
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if (NULL == pMain->MDIGetActive()) {
		pMain->m_wndOutput.ClearIntegrals();
		pMain->m_wndOutput.ClearPeaks();
	}
}

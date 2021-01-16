// Spectrum.cpp: implementation of the CSpectrum class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\NMRDataTypes.h"
#include "Spectrum.h"
#include "NetPlot.h"
#include "MainFrame.h"
#include "ChildFrame.h"
#include "NetPlotDoc.h"
#include "NMRServer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSpectrum::CSpectrum()
{
	pathname.Empty();
	label.Empty();
	raw.resize(0);
	baseline.resize(0);
	data.resize(0);
	POSITION pos = fh.GetStartPosition();
	while (pos) {
		CString unused;
		CMapStringToString *map;
		fh.GetNextAssoc(pos, unused, map);
		map->RemoveAll();
		delete map;
	}
	fh.RemoveAll();
	start = stop = 0;
	from = to = 0;
	ymin = ymax = 0.0f;
	yexpansion = 1.0f;
	color = theApp.RegistryInt(ReadInt, SpectraColor, 0);
	dataclass = -1;
}

CSpectrum::~CSpectrum()
{
	pathname.Empty();
	label.Empty();
	raw.resize(0);
	baseline.resize(0);
	data.resize(0);
	POSITION pos = fh.GetStartPosition();
	while (pos) {
		CString unused;
		CMapStringToString *map;
		fh.GetNextAssoc(pos, unused, map);
		map->RemoveAll();
		delete map;
	}
	fh.RemoveAll();
}

void CSpectrum::Add(vector<complex<float>> &aspec)
{
	if (aspec.size() != data.size())
		return;
	if (raw.size() == aspec.size())
		for (UINT i=0; i<raw.size(); i++) {
			aspec[i] += raw[i];
		}
	else	// else there is no raw data
		for (UINT i=0; i<data.size(); i++)
			aspec[i] = complex<float>(aspec[i].real()+data[i], aspec[i].imag());
}

// start AND stop ARE THE PSUEDO-INDICIES OF THE SPECTRUM
// THAT CORRESPOND TO THE LEFT AND RIGHT EDGE OF THE PLOT REGION
// NOTE THAT START CAN BE LESS THAN 0 AND STOP CAN BE GREATER THAN CB-1
void CSpectrum::CalcStartStop()
{
	CWaitCursor wait;
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->MDIGetActive());
	CNetPlotDoc *pDoc = STATIC_DOWNCAST(CNetPlotDoc, pChild->GetActiveDocument() );
	if ( pDoc->Range == RangeFull ) {
		start = 0;
		stop = npoints - 1;
	} else {
		double dstart = pDoc->RangeFrom;
		double dstop = pDoc->RangeTo;
		if ( pDoc->Range == RangePPM ) {
			dstart *= frequency;
			dstop *= frequency;
		}
		dstart -= 0.5 * sweepwidth + offset;
		dstop -= 0.5 * sweepwidth + offset;
		start = int( -0.5 - npoints * dstart / sweepwidth );
		stop = int( -0.5 - npoints * dstop / sweepwidth);
		if ( start > stop ) {
			int tmp = start;
			start = stop;
			stop = tmp;
		}
	}
}

// GIVEN THE start AND stop FOR THE CURRENT DISPLAY,
// CALCULATE THE ACTUAL BEGINNING AND ENDING INDEX TO BE DISPLAYED
void CSpectrum::CalcToFrom()
{
	to = from = 0;
	if ( start >= 0 ) {
		if ( start < (int)npoints) {
			from = start;
			if ( stop < (int)npoints)
				to = stop;
			else
				to = (int)npoints- 1;
		}
	} else {
		if ( stop > 0 ) {
			if ( stop < (int)npoints)
				to = stop;
			else
				to = (int)npoints- 1;
		}
	}
}

void CSpectrum::FindMinMax(bool usedata)
{
	CWaitCursor wait;
	if ( to > from ) {
		if (usedata)
			ymin = ymax = data[from];
		else
			ymin = ymax = baseline[from];
		for ( int i=from+1; i<=to; i++ ) {
			float pf = (usedata) ? data[i] : baseline[i];
			if ( pf > ymax )
				ymax = pf;
			else if ( pf < ymin )
				ymin = pf;
		}
	} else {
		to = from = 0;
		ymin = ymax = 0.f;
	}
}

UINT CSpectrum::Size()
{
	return npoints;
}

double CSpectrum::Offset()
{
	return offset;
}

double CSpectrum::SweepWidth()
{
	return sweepwidth;
}

double CSpectrum::Frequency()
{
	return frequency;
}

int CSpectrum::Translate(int h, float range, float ym, float yf)
{
	int y = int( (yf-ym) * float(h) / range + 0.5f );
	if ( y < 0 )
		y = 0;
	else if ( y > h )
		y = h;
	return y;
}

float CSpectrum::GetYRange()
{
	return ymax-ymin;
}

float CSpectrum::Ymin()
{
	return ymin;
}

float CSpectrum::Ymax()
{
	return ymax;
}

int CSpectrum::Start()
{
	return start;
}

int CSpectrum::Stop()
{
	return stop;
}

void CSpectrum::Serialize(CArchive &ar)
{
	LPCTSTR ykey = "##Y-EXPANSION";
	LPCTSTR ckey = "SpectrumColor";
	LPCTSTR hidden = "##HIDDEN";
	CString tname = "##TYPE";
	if (ar.IsStoring()) {
		ar.WriteString(pathname + '\n');
		ar.WriteString(label + '\n');
		// How many key-value pairs?
		CString str;
		str.Format("%f", yexpansion);
		FHSet(ykey, hidden, str);
		str.Format("%i", color);
		FHSet(ckey, hidden, str);
		int n = int(fh.GetCount());
		POSITION pos = fh.GetStartPosition();
		while (pos) {
			CString type;
			CMapStringToString *map;
			fh.GetNextAssoc(pos, type, map);
			n += int(map->GetCount());
		}
		// Now write out all the key-value pairs!
		ar << n;
		pos = fh.GetStartPosition();
		while (pos) {
			CString type;
			CMapStringToString *map;
			fh.GetNextAssoc(pos, type, map);
			ar.WriteString(tname + '\n');
			ar.WriteString(type + '\n');
			POSITION mpos = map->GetStartPosition();
			while (mpos) {
				CString key, value;
				map->GetNextAssoc(mpos, key, value);
				ar.WriteString(key + '\n');
				ar.WriteString(value + '\n');
			}
		}
		ar << from << to;
		ar << sweepwidth << frequency << offset;
		ar << npoints;
		ar << start << stop;
		ar << ymax << ymin;
		ar.Write(data.data(), data.size()*sizeof(float));
	} else {
		CString type = "NULL";
		ar.ReadString(pathname);
		ar.ReadString(label);
		int n;
		ar >> n;
		while (n--) {
			CString key, value;
			ar.ReadString(key);
			ar.ReadString(value);
			if (key.Compare(tname))
				FHSet(key, type, value);
			else
				type = value;
		}
		ar >> from >> to;
		ar >> sweepwidth >> frequency >> offset;
		ar >> npoints;
		ar >> start >> stop;
		ar >> ymax >> ymin;
		data.resize(npoints);
		CString value;
		if (FHGet(ykey, value))
			yexpansion = float(atof(value));
		if (yexpansion <= 0.0f)
			yexpansion = 1.0f;
		if (FHGet(ckey, value))
			color = atoi(value);
		ar.Read(&data[0], npoints*sizeof(float));
	}
}

void CSpectrum::GetLabel()
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->MDIGetActive());
	CNetPlotDoc *pDoc = STATIC_DOWNCAST(CNetPlotDoc, pChild->GetActiveDocument() );
	if (pDoc->Label == "filename")
		label = pathname;
	else if (! FHGet(pDoc->Label, label))
		label = "Not Found";			
}

void CSpectrum::GetFromToRange(float &RangeFrom, float &RangeTo)
{
	RangeFrom = float(( 0.5 * sweepwidth + offset ) / frequency);
	RangeTo = float(( offset - 0.5 * sweepwidth ) / frequency);
}

void CSpectrum::Initialize()
{
	ftflag = raw.size() ? true : false;
	CalcStartStop();
	CalcToFrom();
	FindMinMax();
}

void CSpectrum::FHSet(LPCTSTR key, LPCTSTR type, LPCTSTR value)
{
	if (key) {
		if (strlen(key)) {
			if (NULL == type)
				type = "NULL";
			if (! strlen(type))
				type = "NULL";
			CMapStringToString *map;
			if (! fh.Lookup(type, map)) {
				map = new CMapStringToString;
				fh.SetAt(type, map);
			}
			map->SetAt(key, value);
		}
	}
}

bool CSpectrum::FHGet(LPCTSTR key, CString &value)
{
	POSITION pos = fh.GetStartPosition();
	while (pos) {
		CString unused;
		CMapStringToString *map;
		fh.GetNextAssoc(pos, unused, map);
		if (map->Lookup(key, value))
			return(true);
	}
	return(false);
}

float CSpectrum::GetAt(int i)
{
	return data[i];
}

void CSpectrum::GetFixedHeaderItems(HD &hd)
{
	frequency = hd.dim_freq[0];
	if (! frequency) {
		frequency =hd.f1.freq;
		if (! frequency) {
			frequency = hd.cf_base;
			if (! frequency)
				frequency = 600.0;
		}
	}
	sweepwidth = hd.spec_width[0];
	offset = double(hd.of[0]);
	npoints = (UINT)hd.dim[0];
	ftflag = hd.ftflag[0] ? true : false;
	f1_lo_sideband = hd.f1.lo_sideband ? true : false;
	phasec = hd.phaseb[3];
}

bool CSpectrum::Server(CNMRServer &server, LPCTSTR str)
{
	HD hd;
	server.ReadSocket(&hd, sizeof(HD));
	server.ByteReverseHeader(&hd);
	GetFixedHeaderItems(hd);
	while (int len = atoi(server.GetSockString())) {
		CString buf;
		char *pName = buf.GetBuffer(len);
		server.ReadSocket(pName, len);
		char *pType = pName + strlen(pName) + 1;
		char *pValue = pType + strlen(pType) + 1;
		FHSet(pName, pType, pValue);
		buf.ReleaseBuffer();
	}
	raw.resize(npoints);
	server.ReadSocket(raw.data(), sizeof(complex<float>)*npoints);
	server.ByteReverseData(raw);
	data.resize(npoints);
	for (UINT i=0; i<npoints; i++)
		data[i] = raw[i].real();
	if (ftflag)
		raw.resize(0);
	if (hd.groupnum)
		pathname.Format("%s-%dof%d", str, hd.groupcur, hd.groupnum);
	else
		pathname = str;
	Initialize();
	return hd.groupcur!=hd.groupnum;
}

void CSpectrum::File(CFile &file)
{
	HD hd;
	file.Read(&hd, sizeof(HD));
	GetFixedHeaderItems(hd);
	raw.resize(npoints);
	file.Read(raw.data(), sizeof(complex<float>)*npoints);
	data.resize(npoints);
	for (UINT i=0; i<npoints; i++)
		data[i] = raw[i].real();
	if (ftflag)
		raw.resize(0);
	int len;
	file.Read(&len, sizeof(int));
	while (len) {
		CString buf;
		char *pKey = buf.GetBuffer(len);
		file.Read( pKey, len );
		char *pType = pKey + strlen(pKey) + 1;
		char *pValue = pType + strlen(pType) + 1;
		FHSet(pKey, pType, pValue);
		buf.ReleaseBuffer();
		file.Read(&len, sizeof(int));
	}
	Initialize();
}

void CSpectrum::FileX(CFile &file)
{
	CString unused;
	CArchive ar(&file, CArchive::load);
	ar.ReadString(unused);	// Net12NMR Version
	ar.ReadString(unused);	// FileVersion When there is a new version, this will no longer be unused.
	ar.ReadString(unused);	// fullpathname
	ar.ReadString(unused);	// f1nucleus
	ar.ReadString(unused);	// f2nucleus
	ar.ReadString(unused);	// f3nucleus
	ar.ReadString(unused);	// locknucleus
	SPECTRUMPARAMETERS sp;
	ar.Read(&sp, sizeof(SPECTRUMPARAMETERS));
	sweepwidth = sp.sw;
	frequency = sp.sf;
	offset = sp.of;
	ftflag = sp.ftflag;
	f1_lo_sideband = sp.ssb;
	phasea = sp.pha;
	phaseb = sp.phb;
	phasec = sp.phc;
	// flex header
	UINT i;
	ar >> i;
	while (i--) {
		CString type;
		ar.ReadString(type);
		UINT j;
		ar >> j;
		while (j--) {
			CString key, value;
			ar.ReadString(key);
			ar.ReadString(value);
			FHSet(key, type, value);
		}
	}
	// data
	ar >> npoints;
	raw.resize(npoints);
	data.resize(npoints);
	ar.Read(raw.data(), npoints * sizeof(complex<float>));
	ar.Close();
	for (i=0; i<npoints; i++)
		data[i] = raw[i].real();
	if (ftflag)
		raw.resize(0);
	Initialize();
}

float CSpectrum::AmoTry(int ihi, int & nfunk, float fac)
{
	float fac1 = (1.0f-fac)/2.0f;
	float fac2 = fac1 - fac;
	float suma = param[0][0] + param[1][0] + param[2][0];
	float sumb = param[0][1] + param[1][1] + param[2][1];
	theApp.gphA = suma*fac1 - param[ihi][0]*fac2;
	theApp.gphB = sumb*fac1 - param[ihi][1]*fac2;
	GlobalPhase();
	float ytry = EvalPhase();
	nfunk++;
	if (ytry < yvals[ihi]) {
		yvals[ihi] = ytry;
		param[ihi][0] = theApp.gphA;
		param[ihi][1] = theApp.gphB;
	}
	return ytry;
}

float CSpectrum::EvalPhase()
{
	theApp.spare.resize(npoints);
	FindBaseline(8, 15.0f, 0, npoints);
	double A=0.0, B=0.0, D=0.0, E=0.0, F=0.0, G=0.0, H=0.0, I=0.0;
	for (UINT i=0; i<npoints; i++) {
		if (! theApp.spare[i]) {
			double y = (double)data[i];
			double x = (double)i/(double)npoints;
			double x2 = x*x;
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
	if (0.0 == F) return 1.0e10f;
	double C = D;
	double J = 1.0/(C*(2.0*B*E-C*D)-A*E*E+F*(A*D-B*B));
	double a = J*(C*(E*H-D*I)+B*(E*I-F*H)+G*(D*F-E*E));
	double b = J*(B*(C*I-F*G)+H*(A*F-C*C)+E*(C*G-A*I));
	double c = J*(G*(B*E-C*D)+H*(B*C-A*E)+I*(A*D-B*B));
	UINT i;
	for (A=0.0, F=0.0, i=0; i<npoints; i++) {
		if (! theApp.spare[i]) {
			double x = (double)i/(double)npoints;
			A += pow(data[i] - (x*(a*x+b)+c), 2);
			F++;
		}
	}
	if (F==1.0) return 1.0e10f;
	return (float)(sqrt(A/(F-1.0f)));
}

void CSpectrum::AutoPhase()
{
	CWaitCursor wait;
	if (0 == raw.size())
		return;
	param[0][0] = theApp.gphA;
	param[0][1] = theApp.gphB;
	param[1][0] = theApp.gphA+10.0f;
	param[1][1] = theApp.gphB;
	param[2][0] = theApp.gphA;
	param[2][1] = theApp.gphB+10.0f;
	for (int i=0; i<3; i++) {
		theApp.gphA = param[i][0];
		theApp.gphB = param[i][1];
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
		float rtol = (float)sqrt(pow((double)abs(param[ihi][0]-param[ilo][0]), 2)+pow((double)abs(param[ihi][1]-param[ilo][1]), 2));
		if (rtol<0.5 || nfunk>1000) {
			theApp.gphA = param[ilo][0];
			theApp.gphB = param[ihi][1];
			GlobalPhase();
			float yfinal = EvalPhase();
			//TRACE3("FINAL    A=%.2f B=%.2f y=%g\n", theApp.gphA, theApp.gphB, yfinal);
			break;
		}
		float ytry = AmoTry(ihi, nfunk, -1.0f);
		//TRACE3("REFLECT  A=%.2f B=%.2f y=%g\n", theApp.gphA, theApp.gphB, ytry);
		if (ytry <= yvals[ilo]) {
			ytry = AmoTry(ihi, nfunk, 2.0f);
			//TRACE3("EXPAND   A=%.2f B=%.2f y=%g\n", theApp.gphA, theApp.gphB, ytry);
		} else if (ytry >= yvals[inhi]) {
			float ysave = yvals[ihi];
			ytry = AmoTry(ihi, nfunk, 0.5f);
			//TRACE3("CONTRACT A=%.2f B=%.2f y=%g\n", theApp.gphA, theApp.gphB, ytry);
			if (ytry >= ysave) {
				for (int i=0; i<3; i++) {
					if (i != ilo) {
						theApp.gphA = param[i][0] = 0.5f * (param[i][0] + param[ilo][0]);
						theApp.gphB = param[i][1] = 0.5f * (param[i][1] + param[ilo][1]);
						GlobalPhase();
						yvals[i] = EvalPhase();
						//TRACE3("SHRINK   A=%.2f B=%.2f y=%g\n", theApp.gphA, theApp.gphB, yvals[i]);
					}
				}
				nfunk += 2;
			}
		}
	}
	UINT imax;
	float max = 0.0;
	for (UINT i=0; i<npoints; i++) {
		float m = abs(raw[i]);
		if (m > max) {
			max = m;
			imax = i;
		}
	}
	if (0.0f > data[imax]) {
		theApp.gphA += 180.0f;
		GlobalPhase();
	}
}

void CSpectrum::GlobalPhase()
{
	CWaitCursor wait;
	if (0 == raw.size() || 0 == data.size())
		return;
	if (data.size() != raw.size())
		data.resize(raw.size());
	while ( theApp.gphA < 0.0f )
		theApp.gphA += 360.0f;
	while ( theApp.gphA >= 360.0f )
		theApp.gphA -= 360.0f;
	if (0.0f==theApp.gphB) {
		if (0.0f==theApp.gphA) {
			for (UINT i=0; i<npoints; i++)
				data[i] = raw[i].real();
		} else {
			float theta = 1.74532925199e-2f * theApp.gphA;
			for (UINT i=0; i<npoints; i++)
				data[i] = cos(theta) * raw[i].real() + sin(theta) * raw[i].imag();
		}
	} else {
		for (UINT i=0; i<npoints; i++) {
			float theta = 1.74532925199e-2f * (theApp.gphA + float(int(i)-int(npoints/2)) * theApp.gphB / float(npoints));
			data[i] = raw[i].real() * cos(theta) + raw[i].imag() * sin(theta);
		}
	}
	phasea = theApp.gphA;
	phaseb = theApp.gphB;
	CString s;
	s.Format("A:%.1f B:%.1f", theApp.gphA, theApp.gphB );
	FHSet("Phase", "NetNMR", s);
	FHRemove("PolyBase");
}

void CSpectrum::FindBaseline(int width, float sn, int start, int stop)
{
	CWaitCursor wait;
	theApp.spare.resize(npoints);
	for (UINT i=0; i<npoints; i++)
		theApp.spare[i] = false;
	// FIND SIGMA
	double sigma = 0.0;
	int span = npoints / 32;
	for (UINT i=0; i<npoints; i+=span) {
		double ave = 0.0;
		for (int j=0; j<span; j++)
			ave += data[i+j];
		ave /= span;
		double tmp = 0.0;
		for (int j=0; j<span; j++)
			tmp += pow(data[i+j] - ave, 2);
		tmp = sqrt(tmp/double(span-1));
		if (sigma > 0.0) {
			if (tmp>0.0 && tmp<sigma)
				sigma = tmp;
		} else
			sigma = tmp;
	}
	// IS POINT NOT IN BASELINE?
	if (start < width) {
		for (int i=start; i<width; i++)
			theApp.spare[i] = true;
		start = width;
	}
	if (stop > (int)npoints-width) {
		for (UINT i=npoints-(UINT)width; i<npoints; i++)
			theApp.spare[i] = true;
		stop = npoints - width;
	}
	for (int i=start; i<stop; i++) {
		float min = data[i];
		float max = min;
		for (int j=i-width; j<i+width; j++) {
			float d = data[j];
			if (d > max)
				max = d;
			else if (d < min)
				min = d;
		}
		if (max-min > sn * float(sigma))
			theApp.spare[i] = true;
	}
}

void CSpectrum::SHFT(float &a, float &b, float &c, float d)
{
	a = b;
	b = c;
	c = d;
}

void CSpectrum::Bracket(float &ax, float &bx, float &cx, float &fa, float &fb, float &fc)
{
	const float GOLD = 1.618034f;
	float dum = 0.0f;
	if (fb > fa) {
		SHFT(dum, ax, bx, dum);
		SHFT(dum, fb, fa, dum);
	}
	theApp.gphA = cx = bx + GOLD * (bx - ax);
	GlobalPhase();
	fc = EvalPhase();
	while (fb > fc) {
		float r = (bx - ax) * (fb - fc);
		float q = (bx - cx) * (fb - fa);
		float fu = 0.0f;
		float u = bx - ((bx-cx)*q - (bx-ax)*r) / (2.0f * (float)_copysign(__max(fabsf(q-r),1.0e-20f), q-r));
		float ulim = bx + 100.0f * (cx-bx);
		if ((bx-u)*(u-cx) > 0.0f) {
			theApp.gphA = u;
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
			theApp.gphA = u = cx + GOLD * (cx - bx);
			GlobalPhase();
			fu = EvalPhase();
		} else if ((cx-u)*(u-ulim) > 0.0f) {
			theApp.gphA = u;
			GlobalPhase();
			fu = EvalPhase();
			if (fu < fc) {
				theApp.gphA = dum = cx + GOLD * (cx-bx);
				SHFT(bx, cx, u, dum);
				GlobalPhase();
				dum = EvalPhase();
				SHFT(fb, fc, fu, dum);
			}
		} else if ((u-ulim)*(ulim-cx) >= 0.0f) {
			theApp.gphA = u = ulim;
			GlobalPhase();
			fu = EvalPhase();
		} else {
			theApp.gphA = u = cx + GOLD * (cx-bx);
			GlobalPhase();
			fu = EvalPhase();
		}
		SHFT(ax, bx, cx, u);
		SHFT(fa, fb, fc, fu);
	}
}

void CSpectrum::AutoPhaseAOnly()
{
	CWaitCursor wait;
	if (0 == raw.size())
		return;
	const float R = 0.61803399f;
	const float C = 1.0f - R;
	float f0, f1, f2, f3, x0, x1, x2, x3;
	theApp.gphB = 0.0f;
	float ax = theApp.gphA;
	GlobalPhase();
	float fa = EvalPhase();
	float bx = ax + 10.0f;
	theApp.gphA = bx;
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
	theApp.gphA = x1;
	GlobalPhase();
	f1 = EvalPhase();
	theApp.gphA = x2;
	GlobalPhase();
	f2 = EvalPhase();
	while (fabsf(x3-x0) > 0.00001f * (fabsf(x1)+fabsf(x2))) {
		if (f2 < f1) {
			float dum = R*x1 + C*x3;
			SHFT(x0, x1, x2, dum);
			theApp.gphA = x2;
			GlobalPhase();
			dum = EvalPhase();
			SHFT(f0, f1, f2, dum);
		} else {
			float dum = R*x2 + C*x0;
			SHFT(x3, x2, x1, dum);
			theApp.gphA = x1;
			GlobalPhase();
			dum = EvalPhase();
			SHFT(f3, f2, f1, dum);
		}
	}
	if (f1 < f2)
		theApp.gphA = x1;
	else
		theApp.gphA = x2;
	GlobalPhase();
	UINT imax;
	double max = 0.0;
	for (UINT i=0; i<npoints; i++) {
		double m = abs(raw[i]);
		if (m > max) {
			max = m;
			imax = i;
		}
	}
	if (0.0f > data[imax]) {
		theApp.gphA += 180.0f;
		GlobalPhase();
	}
}

void CSpectrum::ZeroFill(bool bigger)
{
	CWaitCursor wait;
	if (0 == raw.size())
		return;
	UINT size = 2;
	UINT n = npoints;
	if (bigger) {
		while (size <= npoints)
			size *= 2;
	} else {
		while (2*size < npoints)
			size *= 2;
	}
	raw.resize(size);
	data.resize(size);
	if (bigger) {
		for (UINT i=npoints; i<size; i++) {
			data[i] = 0.0f;
			raw[i] = complex<float>(0.0f, 0.0f);
		}
	}
	npoints = size;
}

void CSpectrum::Apodize(int type, double value, int units)
{
	CWaitCursor wait;
	if (0 == raw.size())
		return;
	CString v;
	v.Format("%g", value );
	if (units)
		value *= frequency;
	double a = value * (type ? 1.570796326795 : 1.836896377053) / sweepwidth;
	for (UINT i=0; i<npoints; i++) {
		double b = double(i) * a;
		if ( type )
			b *= b;
		b = exp(-b);
		raw[i] *= float(b);
		data[i] = raw[i].real();
	}
	FHSet("apo_type", "NetNMR", type?"Gaussian":"Lorentzian");
	FHSet("apo_value", "NetNMR", v);
	FHSet("apo_unit", "NetNMR", units?"PPM":"Hertz");
}

void CSpectrum::DCCorrect()
{
	CWaitCursor wait;
	if (0 == raw.size())
		return;
	UINT size = npoints;
	UINT n = size / 16;
	if (n < 8) {
		n = size / 4;
		if (n == 0)
			return;
	}
	complex<float> c(0.0f, 0.0f);
	for (UINT m=1; m<=n; m++) 
		c += raw[size-m];
	c /= float(n);
	while (size--) {
		raw[size] -= c;
		data[size] = raw[size].real();
	}
	FHSet("DC", "NetNMR", "yes");
}

void CSpectrum::FFT()
{
	CWaitCursor wait;
	if (0 == raw.size())
		return;
	UINT n = 2;
	while (n < raw.size())
		n *= 2;
	if (n != npoints)
		ZeroFill(true);
	n = raw.size();
	UINT i = 2;
	UINT m = 1;
	// inverse imaginary, if needed
	if (! f1_lo_sideband) {
		for (i=0; i<n; i++)
			raw[i] = conj(raw[i]);
	}
	/* compute power of 2 corresponding to n */
	i = 2;
	m = 1;
	while (i < n) {
		i = 2 * i;
		m = m + 1;
 	}
 	n = i;
	UINT n1 = n - 1;
	UINT n2 = 2 * n;
	for (UINT k = 1; k<m; k++) {
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
			while ( isxx < n ) {
				for(UINT i0=isxx-1; i0<n1; i0=i0+id) {
					UINT i1 = i0 + n4;
					UINT i2 = i1 + n4;
					UINT i3 = i2 + n4;
					float r1 = raw[i0].real() - raw[i2].real();
					float r2 = raw[i1].real() - raw[i3].real();
					float s1 = raw[i0].imag() - raw[i2].imag();
					float s2 = raw[i1].imag() - raw[i3].imag();
					raw[i0] += raw[i2];
					raw[i1] += raw[i3];
					float s3 = r1 - s2;
					r1 += s2;
					s2 = r2 - s1;
					r2 += s1;
					raw[i2] = complex<float>(r1*cc1-s2*ss1, -s2*cc1-r1*ss1);
					raw[i3] = complex<float>(s3*cc3+r2*ss3, r2*cc3-s3*ss3);
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
		for (UINT i1=isxx; i1<=n; i1=i1+id) {
			UINT i0 = i1 - 1;
			complex<float> tmp(raw[i0]);
			raw[i0] = tmp + raw[i1];
			raw[i1] = tmp - raw[i1];
		}
		isxx = 2 * id - 1;
		id = 4 * id;
	}
	/* bit reverse counter */
	UINT j = 1;
	for ( i=1; i<=n1; i++ ) {
		if ( i < j ) {
			UINT i0 = i - 1;
			UINT i1 = j - 1;
			complex<float> tmp(raw[i0]);
			raw[i0] = raw[i1];
			raw[i1] = tmp;
		}
		UINT k = n / 2;
		while (k >= 2 && k < j) {
			j = j - k;
			k = k / 2;
		}
		j = j + k;
	}
	UINT k = n / 2;
	float norm = sqrt(1.0f / float(n));
	for (UINT i0=0, i1=k-1, i2=k, i3=n-1; i0<k/2; i0++, i1--, i2++, i3-- ) {
		complex<float> tmp(raw[i0] * norm);
		raw[i0] = raw[i1] * norm;
		raw[i1] = tmp;
		tmp = raw[i2] * norm;
		raw[i2] = raw[i3] * norm;
		raw[i3] = tmp;
	}
	ftflag = true;
	if (phasec) {
		for (UINT i=0; i<npoints; i++) {
			float theta = 1.74532925199e-2f * float(int(i)-int(npoints/2)) * phasec / float(npoints);
			raw[i] = polar(abs(raw[i]), arg(raw[i]) - theta);
		}
		phasec = 0.0f;
	}
	GlobalPhase();
}

void CSpectrum::Magnitude()
{
	CWaitCursor wait;
	if (! ftflag)
		return;
	for (UINT i=0; i<npoints; i++)
		data[i] = abs(raw[i]);
}

void CSpectrum::FHGetNames(CStringList &list)
{
	list.RemoveAll();
	POSITION pos = fh.GetStartPosition();
	while (pos) {
		CString type;
		CMapStringToString *map;
		fh.GetNextAssoc(pos, type, map);
		if (type.Compare("HIDDEN")) {
			POSITION mpos = map->GetStartPosition();
			while (mpos) {
				CString name, unused;
				map->GetNextAssoc(mpos, name, unused);
				if (name.Left(2).Compare("##"))
					list.AddTail(name);
			}
		}
	}
}

void CSpectrum::FHRemove(LPCTSTR key)
{
	POSITION pos = fh.GetStartPosition();
	while (pos) {
		CString type;
		CMapStringToString *map;
		fh.GetNextAssoc(pos, type, map);
		if (map->RemoveKey(key))
			break;
	}
}

void CSpectrum::SetGlobalPhase()
{
	theApp.gphA = phasea;
	theApp.gphB	= phaseb;
}

void CSpectrum::CalcBaseline(int baseorder)
{
	double a[10][10];
	double vv[10];
	int indx[10];
	for ( int i=0; i<10; i++ ) {
		basecoef[i] = 0.0;
		for ( int j=0; j<10; j++ )
			a[i][j] = 0.0;
	}
	CMainFrame *pMain = STATIC_DOWNCAST( CMainFrame, AfxGetMainWnd() );
	CChildFrame *pChild = STATIC_DOWNCAST( CChildFrame, pMain->MDIGetActive() );
	CNetPlotDoc *pDoc = STATIC_DOWNCAST(CNetPlotDoc, pChild->GetActiveDocument());
	for (int i=from; i<=to; i++ ) {
		double y, d, x;
		if ( ! pDoc->bIntegral[i] ) {
			d = x = 1.0f;
			y = data[i];
			a[baseorder-1][baseorder-1] += 1.0f;
			basecoef[baseorder-1] += y;
			for ( int j=baseorder-2; j>=0; j-- ) {
				x *= double( i );
				a[j][j+1] += x;
				d *= double( i );
				basecoef[j] += d * y;
				x *= double( i );
				a[j][j] += x;
			}
		}
	}
	if ( (int)a[baseorder-1][baseorder-1] < baseorder )
		return;
	for (int i=0; i<baseorder-2; i++ ) {
		for ( int j=i+2; j<baseorder; j++ ) {
			int k = i + j;
			a[i][j] = a[k/2][k/2+k%2];
		}
	}
	for (int i=1; i<baseorder; i++ )
		for ( int j=0; j<i; j++ )
			a[i][j] = a[j][i];
	double temp;
	for (int i=0; i<baseorder; i++ ) {
		double big = 0.0;
		for ( int j=0; j<baseorder; j++)
			if ( ( temp = fabs( a[i][j] ) ) > big )
				big = temp;
			if ( big == 0.0 ) {
				return;
			}
		vv[i] = 1.0 / big;
	}
	for ( int j=0; j<baseorder; j++ ) {
		for (int i=0; i<j; i++ ) {
			double sum = a[i][j];
			for ( int k=0; k<i; k++ )
				sum -= a[i][k] * a[k][j];
			a[i][j] = sum;
		}
		double big = 0.0f;
		int imax;
		for (int i=j; i<baseorder; i++ ) {
			double sum = a[i][j];
			for( int k=0; k<j; k++ )
				sum -= a[i][k] * a[k][j];
			a[i][j] = sum;
			double dum = vv[i] * fabs( sum );
			if ( dum >= big ) {
				big = dum;
				imax = i;
			}
		}
		if ( j != imax ) {
			for( int k=0; k<baseorder; k++ ) {
				double dum = a[imax][k];
				a[imax][k] = a[j][k];
				a[j][k] = dum;
			}
			vv[imax] = vv[j];
		}
		indx[j] = imax;
		if ( a[j][j] == 0.0 )
			a[j][j] = 1.0e-20;
		if ( j != baseorder ) {
			double dum = 1.0f / a[j][j];
			for(int i=j+1; i<baseorder; i++ )
				a[i][j] *= dum;
		}
	}
	int	ii = -1, ip;
	for (int i=0; i<baseorder; i++ ) {
		ip = indx[i];
		double sum = basecoef[ip];
		basecoef[ip] = basecoef[i];
		if ( ii>-1 )
			for (int j=ii; j<=i-1; j++ )
				sum -= a[i][j] * basecoef[j];
		else if ( sum )
			ii = i;
		basecoef[i] = sum;
	}
	for (int i=baseorder-1; i>=0; i-- ) {
		double sum = basecoef[i];
		for (int j=i+1;j<baseorder; j++ )
			sum -= a[i][j] * basecoef[j];
		basecoef[i] = sum / a[i][i];
	}
	if (npoints > baseline.size())
		baseline.resize(npoints);
	for (int i=from; i<=to; i++) {
		double y = basecoef[0];
		for (int j=1; j<baseorder; j++) {
			y *= float( i );
			y += basecoef[j];
		}
		baseline[i] = float(y);
	}
}

float CSpectrum::GetBaseAt(int i)
{
	if ((UINT)i<baseline.size() && i >=0)
		return baseline[i];
	else
		return 0.0f;
}

void CSpectrum::SubtractBaseline(int order)
{
	for (int i=from; i<=to; i++)
		data[i] -= baseline[i];
	CString s;
	s.Format("%d", order);
	FHSet("PolyBase", "NetNMR", s);
}

void CSpectrum::SetOffset(double offset)
{
	this->offset = offset;
}

float CSpectrum::GetYExpansion()
{
	return yexpansion;
}

void CSpectrum::SetYExpansion(float y)
{
	if (y)
		yexpansion = abs(y);
}

void CSpectrum::SubtractData(vector<complex<float>> &spec, float scale)
{
	UINT size = data.size();
	if (size != spec.size())
		return;
	baseline.resize(size);
	for (UINT i=0; i<size; i++)
		baseline[i] = data[i] - (scale * spec[i].real());
	FindMinMax(false);
}

void CSpectrum::SubtractRaw(vector<complex<float>> &spec, float scale)
{
	UINT size = data.size();
	if (size != spec.size())
		return;
	if (size == raw.size()) {
		for (UINT i=0; i<size; i++) {
			raw[i] -= scale * spec[i];
			data[i] = raw[i].real();
		}
	} else {
		for (UINT i=0; i<size; i++)
			data[i] -= spec[i].real();
	}
	FindMinMax();
}

void CSpectrum::Copy(CSpectrum *pSource)
{
	frequency = pSource->frequency;
	sweepwidth = pSource->sweepwidth;
	offset = pSource->offset;
	ftflag = pSource->ftflag;
	f1_lo_sideband = pSource->f1_lo_sideband;
	phasea = pSource->phasea;
	phaseb = pSource->phaseb;
	phasec = pSource->phasec;
	UINT size = pSource->raw.size();
	if (size > 0) {
		raw.resize(size);
		for (UINT i=0; i<size; i++)
			raw[i] = pSource->raw[i];
	}
	npoints = pSource->data.size();
	if (npoints > 0) {
		data.resize(npoints);
		for (UINT i=0; i<npoints; i++)
			data[i] = pSource->data[i];
	}
	size = pSource->baseline.size();
	if (size > 0) {
		baseline.resize(size);
		for (UINT i=0; i<size; i++)
			baseline[i] = pSource->baseline[i];
	}
	POSITION pos = pSource->fh.GetStartPosition();
	while (pos) {
		CMapStringToString *map;
		CString type;
		pSource->fh.GetNextAssoc(pos, type, map);
		CMapStringToString *newmap = new CMapStringToString;
		POSITION tpos = map->GetStartPosition();
		while (tpos) {
			CString key, value;
			map->GetNextAssoc(tpos, key, value);
			newmap->SetAt(key, value);
		}
		fh.SetAt(type, newmap);
	}
	yexpansion = pSource->yexpansion;
	start = pSource->start;
	stop = pSource->stop;
	ymin = pSource->ymin;
	ymax = pSource->ymax;
	from = pSource->from;
	to = pSource->to;
	color = pSource->color;
	label = pSource->label;
	pathname = pSource->pathname;
}

int CSpectrum::GetClass()
{
	if (dataclass < 0) {
		CString strclass;
		if (! FHGet("dataclass", strclass))
			strclass = "x";
		int dataclass = theApp.DataClass.Index(strclass);
	}
	return dataclass;
}
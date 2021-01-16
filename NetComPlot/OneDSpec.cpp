
#include "StdAfx.h"
#include "onedspec.h"

COneDSpec::COneDSpec(void)
: dt(NULL)
, strInfo(_T(""))
, iMin(0)
, iMax(0)
, fSlope(0)
, fYint(0)
, nYmin(0)
, nYmax(0)
, fYSlope(0)
{
	param.ceiling = 100.0f;
	param.floor = 0.0f;
	param.color = 0;
}

COneDSpec::~COneDSpec(void)
{
	if (dt) {
		delete[] dt;
		dt = NULL;
	}
	strInfo.Empty();
}

BOOL COneDSpec::Initialize(void)
{
	dt = new int[SpecDim.Size()];
	return (dt != NULL);
}

void COneDSpec::GetRange(void)
{
	iMin = iMax = dt[0];
	for (int i=1; i<SpecDim.Size(); i++) {
		int v = dt[i];
		if (v > iMax)
			iMax = v;
		else if (v < iMin)
			iMin = v;
	}
}

void COneDSpec::InitTransform(float fSize, float fHeight, float fFrom, float fTo, int nRange)
{
	int limit = SpecDim.Size() - 1;
	if (0 == nRange) {	//FULL RANGE
		SpecDim.from = 0;
		SpecDim.to = limit;
		fSlope = 1000.0f * fSize / (float)(limit+1);
		fYint = 0.5f;
	} else {
		float fnFrom, fnTo, fstart, fstop;
		if (1 == nRange) { // ppmRange
			fnFrom = SpecDim.PPM2N(fFrom);
			fnTo = SpecDim.PPM2N(fTo);
		} else {
			fnFrom = SpecDim.Hertz2N(fFrom);
			fnTo = SpecDim.Hertz2N(fTo);
		}
		// fnFrom and fnTo are the unbounded indices at the user requested fFrom and fTo limits
		SpecDim.from = (int)ceilf(fnFrom);
		SpecDim.to = (int)floorf(fnTo);
		// SpecDim.from and SpecDim.to are cropped into the nearest data
		if (SpecDim.from < 0)
			SpecDim.from = 0;
		if (SpecDim.to > limit)
			SpecDim.to = limit;
		if (1 == nRange) {
			fstart = SpecDim.N2PPM((float)SpecDim.from);
			fstop = SpecDim.N2PPM((float)SpecDim.to);
		} else {
			fstart = SpecDim.N2Hertz((float)SpecDim.from);
			fstop = SpecDim.N2Hertz((float)SpecDim.to);
		}
		// fstart and fstop are the spectral limits of the actual data being plotted
		float d = 1000.0f * fSize / (fFrom-fTo);
		fYint = (fFrom-fstart) * d + 0.5f;
		fSlope = (fstart-fstop) * d/(float)(SpecDim.to-SpecDim.from+1);
	}
	double dDif = (double)(iMax-iMin);
	nYmax = (int)ceil((0.01*param.ceiling)*dDif+(double)iMin+0.5);
	nYmin = (int)ceil((0.01*param.floor)*dDif+(double)iMin+0.5);
	fYSlope = 1000.0f * fHeight / (float)(nYmax - nYmin);
	nYmax = (int)floorf(fHeight*1000.0f+0.5f);
}

CPoint COneDSpec::Tranform(int x, int y, BOOL bIsHoriz)
{
	int a = (int)floorf((float)(x-SpecDim.from+1) * fSlope + fYint);
	int b = (int)floorf(fYSlope * (float)(y - nYmin) + 0.5f);
	if (b < 0)
		b = 0;
	else if (b > nYmax)
		b = nYmax;
	if (bIsHoriz)
		return CPoint(a, b);
	else
		return CPoint(-b, a);
}

void COneDSpec::Archive(CArchive & ar)
{
	if (ar.IsStoring()) {
		CString info = strInfo.Trim() + "\r\n.\r\n";
		ar.WriteString(info);
		ar << iMin << iMax;
		SpecDim.Archive(ar);
		ar.Write(&param, sizeof(ONEDPARAM));
		ar.Write(dt, SpecDim.Size()*sizeof(int));
	} else {
		CString strLine;
		strInfo.Empty();
		while (1) {
			ar.ReadString(strLine);
			if (0 == strLine.Trim().Compare("."))
				break;
			strInfo += strLine.Trim() + "\r\n";
		}
		ar >> iMin >> iMax;
		SpecDim.Archive(ar);
		ar.Read(&param, sizeof(ONEDPARAM));
		dt = new int[SpecDim.Size()];
		ar.Read(dt, SpecDim.Size()*sizeof(int));
	}
}

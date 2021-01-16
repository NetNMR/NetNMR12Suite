#include "StdAfx.h"
#include "twodspec.h"
#include ".\twodspec.h"
#include "resource.h"

CTwoDSpec::CTwoDSpec(void)
: dt(NULL)
, strInfo(_T(""))
, strDataClass(_T(""))
, iMax(0)
, fHSlope(0)
, fVSlope(0)
, fHYint(0)
, fVYint(0)
{
	param.ceiling = 100.0f;
	param.contour = 5;
	param.floor = 0.5f;
	param.mode = posMode;
	param.negcolor = 0;
	param.poscolor = 0;
	param.spacing = logSpacing;
}

CTwoDSpec::~CTwoDSpec(void)
{
	if (dt) {
		for (int i=0; i<VSpecDim.Size(); i++) {
			if (dt[i]) {
				delete[] dt[i];
				dt[i] = NULL;
			}
		}
		delete[] dt;
		dt = NULL;
	}
	strInfo.Empty();
}

BOOL CTwoDSpec::Initialize(void)
{
	dt = new int*[VSpecDim.Size()];
	if (! dt)
		return FALSE;
	for (int i=0; i<VSpecDim.Size(); i++)
		dt[i] = NULL;
	for (int i=0; i<VSpecDim.Size(); i++) {
		dt[i] = new int[HSpecDim.Size()];
		if (! dt[i])
			return NULL;
	}
	return TRUE;
}

void CTwoDSpec::GetRange(void)
{
	int imin, imax;
	imax = imin = dt[0][0];
	for (int i=0; i<VSpecDim.Size(); i++) {
		for (int j=0; j<HSpecDim.Size(); j++) {
			int *f = &dt[i][j];
			if (*f > imax)
				imax = *f;
			else if (*f < imin)
				imin = *f;
		}
	}
	imax = abs(imax);
	imin = abs(imin);
	if (imin > imax) imax = imin;
	iMax = (double)imax;
}

void CTwoDSpec::InitTransform(float *fSize, float *fFrom, float *fTo, int hRange, int vRange)
{
	int limit = HSpecDim.Size() - 1;
	if (0 == hRange) {	// fullRange
		HSpecDim.from = 0;
		HSpecDim.to = limit;
		fHSlope = 1000.0f * fSize[0] / (float)(limit+1);
		fHYint = 0.5f;
	} else {
		float fnFrom, fnTo, fstart, fstop;
		if (1 == hRange) { // ppmRange
			fnFrom = HSpecDim.PPM2N(fFrom[0]);
			fnTo = HSpecDim.PPM2N(fTo[0]);
		} else {
			fnFrom = HSpecDim.Hertz2N(fFrom[0]);
			fnTo = HSpecDim.Hertz2N(fTo[0]);
		}
		// fnFrom and fnTo are the unbounded indices at the user requested fFrom and fTo limits
		HSpecDim.from = (int)ceilf(fnFrom);
		HSpecDim.to = (int)floorf(fnTo);
		// HSpecDim.from and HSpecDim.to are cropped into the nearest data 
		if (HSpecDim.from < 0)
			HSpecDim.from = 0;
		if (HSpecDim.to > limit)
			HSpecDim.to = limit;
		if (1 == hRange) {
			fstart = HSpecDim.N2PPM((float)HSpecDim.from);
			fstop = HSpecDim.N2PPM((float)HSpecDim.to);
		} else {
			fstart = HSpecDim.N2Hertz((float)HSpecDim.from);
			fstop = HSpecDim.N2Hertz((float)HSpecDim.to);
		}
		// fstart and fstop are the spectral limits of the actual data being plotted
		float d = 1000.0f * fSize[0] / (fFrom[0]-fTo[0]);
		fHYint = (fFrom[0]-fstart) * d + 0.5f;
		fHSlope = (fstart-fstop) * d/(float)(HSpecDim.to-HSpecDim.from+1);
	}
 	limit = VSpecDim.Size() - 1;
	if (0 == vRange) {	// fullRange
		VSpecDim.from = 0;
		VSpecDim.to = limit;
		fVSlope = 1000.0f * fSize[1] / (float)(limit+1);
		fVYint = 0.5f;
	} else {
		float fnFrom, fnTo, fstart, fstop;
		if (1 == vRange) { // ppmRange
			fnFrom = VSpecDim.PPM2N(fFrom[1]);
			fnTo = VSpecDim.PPM2N(fTo[1]);
		} else {
			fnFrom = VSpecDim.Hertz2N(fFrom[1]);
			fnTo = VSpecDim.Hertz2N(fTo[1]);
		}
		// fnFrom and fnTo are the unbounded indices at the user requested fFrom and fTo limits
		VSpecDim.from = (int)ceilf(fnFrom);
		VSpecDim.to = (int)floorf(fnTo);
		// VSpecDim.from and VSpecDim.to are cropped into the nearest data 
		if (VSpecDim.from < 0)
			VSpecDim.from = 0;
		if (VSpecDim.to > limit)
			VSpecDim.to = limit;
		if (1 == vRange) {
			fstart = VSpecDim.N2PPM((float)VSpecDim.from);
			fstop = VSpecDim.N2PPM((float)VSpecDim.to);
		} else {
			fstart = VSpecDim.N2Hertz((float)VSpecDim.from);
			fstop = VSpecDim.N2Hertz((float)VSpecDim.to);
		}
		// fstart and fstop are the spectral limits of the actual data being plotted
		float d = 1000.0f * fSize[1] / (fFrom[1]-fTo[1]);
		fVYint = (fFrom[1]-fstart) * d + 0.5f;
		fVSlope = (fstart-fstop) * d / (float)(VSpecDim.to-VSpecDim.from+1);
	}
	// Calculate Contours
	if (param.floor <= 0.0) param.floor = 1.0f;
	if (param.ceiling < param.floor) param.ceiling = 99.0f + param.floor;
	double floor = iMax * param.floor / 100.0;
	double ceiling = iMax * param.ceiling / 100.0;
	if (param.spacing == linSpacing) {
		double spacing = (ceiling-floor) / (param.contour-1);
		for (int i=0; i<param.contour; i++)
			posContour[i] = (int)ceil(floor + i * spacing);
	} else {
		floor = log(floor);
		ceiling = log(ceiling);
		double spacing = (ceiling-floor) / (param.contour-1);
		for (int i=0; i<param.contour; i++)
			posContour[i] = (int)ceil(exp(floor + i * spacing));
	}
	int j = param.contour;
	for (int i=0; i<param.contour; i++)
		negContour[i] = -posContour[--j];
}

CPoint CTwoDSpec::Translate(float x, float y)
{
	return CPoint((int)floorf(x * fHSlope + fHYint), (int)floorf(y * fVSlope + fVYint));
}

void CTwoDSpec::Archive(CArchive & ar)
{
	const CString szDataClass = "dataclass=";
	const CString szNewline = "\r\n";
	const CString szDot = ".";
	if (ar.IsStoring()) {
		ar.WriteString(strInfo + szNewline);
		if (! strDataClass.IsEmpty())
			ar.WriteString(szDataClass + strDataClass + szNewline);
		ar.WriteString(szDot + szNewline);
		ar << iMax;
		ar.Write(&param, sizeof(TWODPARAM));
		HSpecDim.Archive(ar);
		VSpecDim.Archive(ar);
		for (int i=0; i<VSpecDim.Size(); i++)
			ar.Write(dt[i], HSpecDim.Size()*sizeof(int));
	} else {
		CString strLine;
		strInfo.Empty();
		while (1) {
			ar.ReadString(strLine);
			if (0 == strLine.Trim().Compare(szDot))
				break;
			else if (0 == strLine.Trim().Compare(szDataClass))
				strDataClass = strLine.Trim().Mid(szDataClass.GetLength());
			else
				strInfo += strLine.Trim() + szNewline;
		}
		ar >> iMax;
		ar.Read(&param, sizeof(TWODPARAM));
		HSpecDim.Archive(ar);
		VSpecDim.Archive(ar);
		dt = new int*[VSpecDim.Size()];
		for (int i=0; i<VSpecDim.Size(); i++) {
			dt[i] = new int[HSpecDim.Size()];
			ar.Read(dt[i], HSpecDim.Size()*sizeof(int));
		}
	}
}

void CTwoDSpec::Symmetrize(UINT nID)
{
	int r = HSpecDim.Size();
	int c = VSpecDim.Size();
	if (r == c) {
		if (nID == ID_PROCESS_SYMAVE) {
			for (int i=0; i<c; i++) {
				for (int j=i+1; j<r; j++) {
					int k = dt[i][j]/2 + dt[j][i]/2;
					dt[i][j] = dt[j][i] = k;
				}
			}
		} else {
			for (int i=0; i<c; i++) {
				for (int j=i+1; j<r; j++) {
					if ((dt[i][j]>=0 && dt[j][i]<=0) || (dt[i][j]<=0 && dt[j][i]>=0))
						dt[i][j] = dt[j][i] = 0;
					else {
						if (abs(dt[i][j]) <= abs(dt[j][i]))
							dt[j][i] = dt[i][j];
						else
							dt[i][j] = dt[j][i];
					}
				}
			}
		}
	}
}

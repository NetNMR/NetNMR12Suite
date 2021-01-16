#include "StdAfx.h"
#include "NetComplot.h"
#include "SpecDim.h"
#include "OneDSpec.h"
#include "TwoDSpec.h"
#include "NetComPlotDoc.h"
#include "axis.h"

CAxis::CAxis(CNetComPlotDoc *pDoc)
{
	for (int i=0; i<2; i++) {
		dSize[i] = (double)floorf(1000.0f * pDoc->param.f2DSize[i] +0.5f);
		VIEWRANGE eRange = i ? pDoc->param.vRange : pDoc->param.hRange;
		dFrom[i] = (double)pDoc->param.fFrom[i];
		dTo[i] = (double)pDoc->param.fTo[i];
		double axisfactor = 0.0;
		dDelta[i] = log10((dFrom[i]-dTo[i])/(dSize[i]/3000.0));
		if (dDelta[i] < 0.0) {
			/* move delta to positive number and remember necessary factor */
			axisfactor = floor(dDelta[i]);
			dDelta[i] -= axisfactor;
		}
		double inc = pow(10.0, modf(dDelta[i], &dDelta[i]));
		dTicratio[i] = 5.0;
		if (inc < 2.0) {
			inc = 1.0;
		} else {
			if (inc < 5.0) {
				inc = 2.0;
				dTicratio[i] = 4.0;
			} else
				inc = 5.0;
		}
		dDelta[i] = inc * pow(10.0, dDelta[i]);
		/* and attach factor */
		if (axisfactor < 0.0)
			dDelta[i] *= pow(10.0, axisfactor);
	}
	font.CreateFont(600, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Courier New");
	color = theApp.arrColors[pDoc->param.nAxisColor].value;
}

CAxis::~CAxis(void)
{
}

void CAxis::Draw(CDC *pDC)
{
	CPen pen(PS_SOLID, 0, color);
	CPen *oldPen = pDC->SelectObject(&pen);
	CFont *oldFont = pDC->SelectObject(&font);
	CBrush *oldBrush = (CBrush *)pDC->SelectStockObject(NULL_BRUSH);
	int oldBkMode = pDC->SetBkMode(TRANSPARENT);
	COLORREF oldColor = pDC->SetTextColor(color);
	pDC->Rectangle(0, 0, (int)dSize[0], (int)dSize[1]);
	Tics(pDC, FALSE, 0);
	Tics(pDC, TRUE, 0);
	Tics(pDC, FALSE, 1);
	Tics(pDC, TRUE, 1);
	pDC->SetBkMode(oldBkMode);
	pDC->SelectObject(oldFont);
	pDC->SelectObject(oldPen);
	pDC->SelectObject(oldBrush);
	pDC->SetTextColor(oldColor);
}

void CAxis::Tics(CDC *pDC, BOOL bLabel, int i)
{
	int ticlength = bLabel ? 300 : 100;
	double inc = dDelta[i];
	int places = 0;
	if (inc < 1.0)
		places = int(-floor(log10(dDelta[i])));
	double sw = dFrom[i] - dTo[i];
	if (! bLabel) inc /= dTicratio[i];
	double val = inc * floor(dFrom[i] / inc);
	while (val > dTo[i]) {
		CString str;
		if (bLabel) {
			if (places) {
				CString fstr;
				fstr.Format("%%.%df", places);
				str.Format(fstr, val);
			} else
				str.Format("%g", val);
		}
		int pos = (int)floor(dSize[i] * (dFrom[i] - val) / sw + 0.5);
		if (i) { // VERTICAL
			pDC->MoveTo(0, pos);
			pDC->LineTo(-ticlength, pos);
			int w = (int)dSize[0];
			pDC->MoveTo(w, pos);
			pDC->LineTo(w+ticlength, pos);
			if (bLabel) pDC->TextOut(w+ticlength+100, pos+300, str);
		} else { // HORIZONTAL
			pDC->MoveTo(pos, 0);
			pDC->LineTo(pos, -ticlength);
			int h = (int)dSize[1];
			pDC->MoveTo(pos, h);
			pDC->LineTo(pos, h+ticlength);
			if (bLabel) pDC->TextOut(pos-str.GetLength()*140, -(ticlength+100), str);
		}
		val -= inc;
		if (val < 1.0e-15 && val > -1.0e-15)
			val = 0.0;
	}
}

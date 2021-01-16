// Line.cpp : implementation file
//

#include "stdafx.h"
#include "NetSpan.h"
#include "Line.h"
#include "NetSpanDoc.h"


// CLine

CLine::CLine(double l, double w, double s, int size, CNetSpanDoc *pDocument, LPCTSTR strname)
{
	id = theApp.lineindex++;
	bshape = bwidth = bshift = TRUE;
	pDoc = pDocument;
	line.SetSize(size);
	shape = l;
	width = w;
	shift = s;
	name = strname;
	ReCalculate();
}

CLine::~CLine()
{
	line.RemoveAll();
}

// CLine member functions

int CLine::ParCount()
{
	int count = 0;
	if (bshape)
		count++;
	if (bwidth)
		count++;
	if (bshift)
		count++;
	return count;
}

double CLine::GetShape()
{
	return shape;
}

double CLine::GetWidth()
{
	return width;
}

double CLine::GetShift()
{
	return shift;
}

double CLine::ChangeShape(double newvalue)
{
	shape = newvalue;
	if (shape < 0.0)
		shape = 0.0;
	else if (shape > 1.0)
		shape = 1.0;
	return shape;
}

double CLine::ChangeWidth(double newvalue)
{
	double minwidth = 2.0 / double(line.GetCount() - 1);
	double maxwidth = 0.5 * (pDoc->xnorm[pDoc->to] - pDoc->xnorm[pDoc->from]);
	width = newvalue;
	if (width > maxwidth)
		width = maxwidth;
	else if (width < minwidth)
		width = minwidth;
	return width;
}

double CLine::ChangeShift(double newvalue)
{
	shift = newvalue;
	double maxshift = pDoc->xnorm[pDoc->to];
	double minshift = pDoc->xnorm[pDoc->from];
	if (shift > maxshift)
		shift = maxshift;
	else if (shift < minshift)
		shift = minshift;
	return shift;
}

void CLine::ReCalculate(void)
{
	double fgauss = 1.0 - shape;
	for (int i=0; i<pDoc->rows; i++) {
		double x = 2.0 * (pDoc->xnorm[i] - shift) / width;
		x *= x;
		line[i] = shape/(1.0 + x) + fgauss*exp(-0.693147180559945309417*x);
	}
}

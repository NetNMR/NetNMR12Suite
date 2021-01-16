// SpecDim.cpp : implementation file
//

#include "stdafx.h"
#include "SpecDim.h"


// CSpecDim

CSpecDim::CSpecDim()
: freq(0)
, sw(0)
, of(0)
, np(0)
, from(0)
, to(0)
{
}

CSpecDim::~CSpecDim()
{
}


// CSpecDim member functions

void CSpecDim::Initialize(float f, float s, float o, int n)
{
	freq = (float)fabs((double)f);
	if (freq == 0.0f)
		freq = 100.0f;
	sw = (float)fabs((double)s);
	if (sw == 0.0f)
		sw = 1000.0f;
	of = o;
	np = abs(n);
	if (np < 4)
		np = 4;
	hzperpt = -sw / (float)np;
	yint = of * freq + 0.5f * hzperpt;
	return;
}

void CSpecDim::ChangeOffset(float newoffset, float atn)
{
	float hertz = N2Hertz(atn);
	yint += newoffset-hertz;
	of = yint / freq;
}

float CSpecDim::N2Hertz(float n)
{
	return hzperpt * n + yint;
}

float CSpecDim::Frequency(void)
{
	return freq;
}

float CSpecDim::SpecWidth(void)
{
	return sw;
}

float CSpecDim::Offset(void)
{
	return of;
}

int CSpecDim::Size(void)
{
	return np;
}

float CSpecDim::N2PPM(float n)
{
	return N2Hertz(n) / freq;
}

float CSpecDim::Hertz2N(float hertz)
{
	return (hertz - yint) / hzperpt;
}

float CSpecDim::PPM2N(float ppm)
{
	return Hertz2N(ppm * freq);
}

void CSpecDim::Archive(CArchive & ar)
{
	if (ar.IsStoring()) {
		ar << freq << sw << of << np;
	} else {
		float f, s, o;
		int n;
		ar >> f >> s >> o >> n;
		Initialize(f, s, o, n);
	}
}

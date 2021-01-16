// Spectrum.cpp : implementation file
//

#include "stdafx.h"
#include "NetSpan.h"
#include "Spectrum.h"


// CSpectrum

CSpectrum::CSpectrum()
{
	SetBaseline(0.0, 0.0, 0.0, 0.0);
	intensity.InitHashTable(47);
}

CSpectrum::~CSpectrum()
{
	label.Empty();
	calc.RemoveAll();
	diff.RemoveAll();
	obs.RemoveAll();
	singleline.RemoveAll();
	intensity.RemoveAll();
}

// CSpectrum member functions

double CSpectrum::GetMin()
{
	return ymin;
}

double CSpectrum::GetMax()
{
	return ymax;
}

void CSpectrum::GetBaseline(double &dc, double &slope, double &curve, double &cube)
{
	dc = baseline[0];
	slope = baseline[1];
	curve = baseline[2];
	cube = baseline[3];
}

void CSpectrum::SetBaseline(double dc, double slope, double curve, double cube)
{
	baseline[0] = dc;
	baseline[1] = slope;
	baseline[2] = curve;
	baseline[3] = cube;
}

void CSpectrum::CalcBaseline(int from, int to)
{
	double size = (double)calc.GetCount();
	for (int i=from; i<=to; i++) {
		double x = (double)i/size;
		calc[i] = baseline[0] + x * (baseline[1] + x * (baseline[2] + x * baseline[3]));
	}
}

void CSpectrum::FindMinMax(int from, int to)
{
	ymin = ymax = obs[to];
	for (int i=from; i<to; i++) {
		if (obs[i] > ymax)
			ymax = obs[i];
		else if (obs[i] < ymin)
			ymin = obs[i];
	}
}

double CSpectrum::SumSq(int from, int to)
{
	double sumsq = 0.0;
	for (int i=from; i<=to; i++)
		sumsq += diff[i] * diff[i];
	return sumsq;
}
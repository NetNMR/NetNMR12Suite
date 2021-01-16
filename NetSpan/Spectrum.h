#pragma once

// CSpectrum command target
class CLine;

class CSpectrum : public CObject
{
public:
	CString label;
	CRect scrRect;
	double eBaseline[4];
	CArray<double, double> calc;
	CArray<double, double> diff;
	CArray<double, double> obs;
	CArray<double, double> singleline;
	CMap<UINT, UINT, double, double &> intensity;
	CMap<UINT, UINT, double, double &> eintensity;
	CSpectrum();
	virtual ~CSpectrum();
	void FindMinMax(int from, int to);
	double GetMin(void);
	double GetMax(void);
	void GetBaseline(double &dc, double &slope, double &curve, double &cube);
	void SetBaseline(double dc, double slope, double curve, double cube);
	void CalcBaseline(int from, int to);
	double SumSq(int from, int to);
private:
	double ymin, ymax;
	double baseline[4];
};



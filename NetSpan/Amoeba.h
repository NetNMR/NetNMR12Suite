#pragma once

// CAmoeba command target

class CMainFrame;
class CNetSpanView;
class CNetSpanDoc;

class CAmoeba
{
public:
	CAmoeba();
	~CAmoeba();
	BOOL Create(void);
	bool Optimize(double tolerance, int itermax);
private:
	CMainFrame *pMain;
	CNetSpanView *pView;
	CNetSpanDoc *pDoc;
	HWND pwnd;
	double **p;
	double *y, *psum, *ptry;
	int ndim, nline, nspec;
	double Evaluate(double * param);
	void CalcPSum(void);
	double Try(double fac, const int hi, int &iter);
};



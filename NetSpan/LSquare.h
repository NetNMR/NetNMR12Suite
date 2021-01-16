#pragma once

#include "DblMatrix.h"

class CMainFrame;
class CNetSpanView;
class CNetSpanDoc;

class CLSquare
{
public:
	CLSquare(void);
	~CLSquare(void);
	BOOL Create(bool fullset);
	BOOL Optimize(double tolerance, int itermax, bool fullset);
private:
	CMainFrame *pMain;
	CNetSpanView *pView;
	CNetSpanDoc *pDoc;
	int nspec, nline, ndim;
	double *prtdrv, *b, *borig;
	double serr;
	int *indx;
	CDblMatrix a, aorig;
	void Build(bool fullset);
	void Evaluate(bool fullset);
	void CalculateErrors(bool fullset);
};

#pragma once

#include "Term.h"
#include "tnt_array1d.h"
#include "tnt_array2d.h"

// CDecay command target

class CILTDoc;

typedef struct mcterms_tag {
	union {
		float f;
		int i;
	} x;
	union {
		float f;
		int i;
	} y;
} SMCTerm;

class CDecay
{
private:
	CDecay();
public:
	// properties
	int iMethod, iSIRTCount;
	BOOL bSimplify, bOptimize;
	double dblAlpha, dblSIRTTolerance;
	CString strTitle;
	CTypedPtrList<CPtrList, CTerm *> Terms;
	CTypedPtrList<CPtrList, SMCTerm *> mcterms[10];

	// methods
	CDecay(CILTDoc *pDocument, int size, LPCTSTR title);
	virtual ~CDecay();
	void SetAt(int i, double val);
	double GetAt(int i);
	double GetCalc(int i);
	double GetDiff(int i);
	void Calculate(void);
	double CalcValue(double time);
	double SSQ(void);
	double Min(void);
	double Max(void);
	double Diff(void);
	double DiffMin(void);
	double DiffMax(void);
	double DiffDiff(void);
	bool IsCalculated(void);
	CTerm *NewTerm(void);
	void GetStats(double &sumsq, double &rsumsq);
	void DoProcess(int method, BOOL simplify, BOOL optimize, const double alpha, const int sirtcount, const double sirttolerance, const double from, const double to, const int n);
	void Serialize(CArchive& ar);

private:
	// properties
	CILTDoc *pDoc;
	bool isJoint;
	double ssq, rssq;
	// observed minimum and maximum
	int iymin, iymax;
	// the difference minimum and maximum
	int idmin, idmax;
	// the arrays
	Array1D<double> obs;
	Array1D<double> calc;
	Array1D<double> diff;

	// methods
	Array1D<double> nnls(Array2D<double> &A, Array1D<double> &b);
	Array1D<double> makedecay(const double relaxrate);
	Array2D<double> gradientintegral(void);
	double amofunc(const Array1D<double> &p);
	double amotry(Array2D<double> &p, Array1D<double> &y, Array1D<double> &psum, const int ihi, int &iterations, double fac);
	void nnlswrapper(void);
	double PEalpha(const double xvalue);
	double PEalpha(const Array1D<double> &Ealpha);
	double findnextr(void);
	void ILT(const double alpha, const double from, const double to, const int n);
	void WEGL(void);
	void SIRT(const int itercount, const double tolerance, const double from, const double to, const int n);
	int CombinePairs(void);
	bool DiscardTerm(double chisq, double from, double to);
	bool Amoeba(void);
	void ParamErrors(void);
	void RedoConstant(void);
};



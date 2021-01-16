// NetSpanDoc.h : interface of the CNetSpanDoc class
//


#pragma once

#include "Spectrum.h"
#include "Line.h"

class CNetSpanDoc : public CDocument
{
protected: // create from serialization only
	CNetSpanDoc();
	DECLARE_DYNCREATE(CNetSpanDoc)

// Attributes
public:
	int from, to;
	int cols, rows;
	int nbase;
	CTypedPtrList<CObList, CSpectrum *> Spectra;
	CTypedPtrList<CObList, CLine *> Lines;
	CArray<double, double> ppm;
	CArray<double, double> xnorm;
	double ymin, ymax, sumsq;
	double sf;
	double ynorm, xsw, xoffset;
	CString gammastr, tcubedstr, dactogstr, sfstr;

// Operations
public:
	void MakeILTTextFile();

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	void OpenTextSpanFile(LPCTSTR pathname);
	void OpenNetPlotFile(LPCTSTR pathname);

// Implementation
public:
	virtual ~CNetSpanDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void DeleteContents();
	void FindMinMax();
	void ReCalculate(void);
};



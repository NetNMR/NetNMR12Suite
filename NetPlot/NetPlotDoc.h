// NetPlotDoc.h : interface of the CNetPlotDoc class
//


#pragma once

#define FILECHANGE      0x001
#define TITLECHANGE     0x002
#define SIZECHANGE      0x004
#define LABELCHANGE     0x008
#define POSITIONCHANGE  0x010
#define RANGECHANGE     0x020
#define GLOBALCHANGE    0x040
#define EXPANSIONCHANGE 0x080
#define FONTCHANGE		0x100

class CSpectrum;

enum ViewRange { RangeFull=IDC_RANGEFULL, RangeHertz=IDC_RANGEHERTZ, RangePPM=IDC_RANGEPPM };

class CNetPlotDoc : public CDocument
{
protected: // create from serialization only
	CNetPlotDoc();
	DECLARE_DYNCREATE(CNetPlotDoc)

// Attributes
public:
	bool IsLandscape, AllTheSame, ShowClass;
	int WhatChanged;
	CString Title, Label;
	ViewRange Range;
	int nLabelSize, nTitleSize, nAxisSize;
	int nBaseOrder;
	vector<bool> bIntegral;
	float ymin, ymax, Width, Height, OffsetX, OffsetY, LabelX, LabelY, RangeFrom, RangeTo, YExpansion;
	float PPMFrom, PPMTo, HertzFrom, HertzTo;
	BOOL GlobalScaling;
	CTypedPtrList<CPtrList, CSpectrum *> Spectra;

// Operations
public:
	void AddSpectrum(LPCTSTR str);
	void SetIntegralSize(int newsize);
	void SetMinMax(void);
	void AddFile(void);
	void EditParameters(void);
	void FileSaveAs(void);
	void DCLevel(void);
	void Zerofill(bool bigger);
	void FT();
	void Apodize(int shape, double value, int unit);
	void Magnitude(CSpectrum *p);
	void AutoPhaseA(CSpectrum *p);
	void AutoPhaseAB(CSpectrum *p);
	void ClonePhase(CSpectrum *p);
	void CalculateBaseline(void);
	void SubtractBaseline(void);

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CNetPlotDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};



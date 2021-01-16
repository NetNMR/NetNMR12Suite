// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://msdn.microsoft.com/officeui.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// Net12NMRDoc.h : interface of the CNetNMRDoc class
//

#pragma once

#include "..\NMRDataTypes.h"


typedef struct bregiontag {
	UINT start, stop;
} BREGION;

typedef struct pregiontag {
	UINT start, stop;
	float threshold;
} PREGION;

typedef struct iregiontag {
	bool selected;
	UINT start, stop;
	vector<float> integral;
	float total;
} IREGION;

typedef struct peaktag {
	UINT n;
	bool selected;
	double nfit;
	float height, heightfit;
} PEAK;

class CMyRibbonBar;

class CNet12NMRDoc : public CDocument
{
protected: // create from serialization only
	CNet12NMRDoc();
	DECLARE_DYNCREATE(CNet12NMRDoc)

// Attributes
public:
	Leaf leaf;
	int setintegralindex;
	double setintegralvalue;
	CString fullpathname;
	CString f1nucleus, f2nucleus, f3nucleus, locknucleus;
	SPECTRUMPARAMETERS sp;
	CTypedPtrMap<CMapStringToOb, CString, CMapStringToString *> fh;
	CMapStringToString *fhmap;
	vector<complex<float>> dt;
	vector<float> fbaseline;
	vector<bool> bbaseline;
	CTypedPtrList<CPtrList, PEAK *> peaks;
	CTypedPtrList<CPtrList, BREGION *> baseregions;
	CTypedPtrList<CPtrList, PREGION *> peakregions;
	CTypedPtrList<CPtrList, IREGION *> intregions;

private:
	float param[3][2];
	float yvals[3];


// Operations
public:
	void Truncate(void);
	void DCLevelCorrect(void);
	void QuadratureCorrection(void);
	void LeftShift(void);
	void RightShift(void);
	void Apodize();
	void AutoPhase(void);
	void AutoPhaseAOnly(void);
	void FindBaseline(UINT width, float sn, UINT start, UINT stop);
	void ZeroFill(void);
	void FourierTransform(void);
	void Magnitude(void);
	double ntoppm(UINT n);
	double ntoppm(int n);
	double ntoppm(double n);
	int ppmton(double ppm);
	void GlobalPhase(void);
	void UpdateFlexHeader(LPCTSTR key, LPCTSTR type, LPCTSTR value);
	bool GetFlexHeaderItem(LPCTSTR key, CString &value);
	void FilterComp(void);
	void Level(int from, int to);
	void UpdateRibbon(CMyRibbonBar *pRibbon);
	void GetMinMax(float &min, float &max);
	void Integrate(IREGION *p);
	void RecalcIntegrals(void);
	void RecalcPeaks(void);
	void SaveState(void);
	void LoadState(void);
	void InitializeBaseline(void);
	void CalcBaseline(int from, int to);

private:
	float sq(float x);
	float EvalPhase(void);
	float AmoTry(int ihi, int & nfunk, float fac);
	void Bracket(float &ax, float &bx, float &cx, float &fa, float &fb, float &fc);
	void initsp(void);
	void SHFT(float &a, float &b, float &c, float &d);
	bool IsAPeak(UINT i, UINT size, UINT &from, UINT &to, float &sign);
	void DefinePeak(PEAK *peak, UINT from, UINT to, float sign);
	double basecoef[10];

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CNet12NMRDoc();
	void DeleteAllNewData();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	afx_msg void OnHeadernamecombo();
	afx_msg void OnUpdateHeaderdeletebutton(CCmdUI *pCmdUI);
	afx_msg void OnHeaderdeletebutton();
	afx_msg void OnUpdateHeaderupdatebutton(CCmdUI *pCmdUI);
	afx_msg void OnHeaderupdatebutton();
	afx_msg void OnUpdateHeadernewedit(CCmdUI *pCmdUI);
	afx_msg void OnUpdateHeadernewbutton(CCmdUI *pCmdUI);
	afx_msg void OnHeadernewbutton();
	virtual void DeleteContents();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();
};

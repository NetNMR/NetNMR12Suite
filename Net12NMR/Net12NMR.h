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

// Net12NMR.h : main header file for the NetNMR application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include "ServerList.h"
#include "..\NMRDataTypes.h"
#include "..\DataClass.h"
#include "Shelf.h"

enum ApoType {
	lorenApoType,
	gaussApoType
};

enum ApoUnits {
	hertzApoUnits,
	ppmApoUnits
};

enum DisplayMode {
	normalMode,
	baselineMode,
	peakpickMode,
	integralMode,
	phaseMode,
	shelfMode
};

enum PhaseIncrement {
	tenPhaseIncrement,
	onePhaseIncrement,
	tenthPhaseIncrement
};

enum PhaseTerm {
	constPhaseTerm,
	linearPhaseTerm
};

enum IntegerConstants {
	constColorSize = 18
};

typedef struct tmpspectag {
	Leaf leaf;
	CString path;
	HD hd;
	vector<complex<float>> dt;
	CTypedPtrMap<CMapStringToOb, CString, CMapStringToString *> fh;
	CMapStringToString *fhmap;
	CTypedPtrList<CPtrList, REGION *> bregion, pregion, iregion;
} TMPSPECTRUM;


// CNet12NMRApp:
// See Net12NMR.cpp for the implementation of this class
//
class CNet12NMRApp : public CWinAppEx
{
public:
	CNet12NMRApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
public:
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;
	DisplayMode disMode;
	CString settings;
	CString AppVersion;
	CServerList servers;
	CDataClass DataClassification;
	COLORREF Color[constColorSize], ColorScheme[2][constColorSize];
	bool useparabolicfit, labelisppm;
	bool viewparameters, viewfileinfo;
	int digitsPPM, digitsHz;
	ApoType apoType;
	ApoUnits apoUnits;
	double apoValue;
	PhaseIncrement phaseIncrement;
	PhaseTerm phaseTerm;
	int m_nABCWidth;
	float m_fABCSN;
	int m_nPolyorder;
	int m_nMAWidth;
	bool polybaseline;
	float GlobalPhaseA, GlobalPhaseB, GlobalIntegralConst;
	CShelf shelf;
	CMapStringToString m_paraMap, m_flexMap;
	CTypedPtrMap<CMapStringToPtr, CString, CStringArray *> m_procparMap;

private:
	CString m_strAppPath;
	CTypedPtrMap<CMapStringToPtr, CString, double *> VarianNucTable;
	double GetReferenceFrequency(LPCTSTR strNuc, double lockref);
	void InitializeColors(void);
	CString MakeDocumentTitle(CNet12NMRDoc *pDoc);

public:
	const CString GetAppPath(void);
	virtual void PreLoadState(void);
	virtual void LoadCustomState(void);
	virtual void SaveCustomState(void);
	BOOL ReadAsciiFile(CString &str, UINT param);
	void MapJCAMPFile(CString &file);
	void MapProcparFile(CString &file);
	void DeleteProcparMap(void);
	bool ImportNMRFile(CString path, Leaf leaf);
	bool GetTmpSpectra(CString path, Leaf leaf, CTypedPtrList<CPtrList, TMPSPECTRUM *> &spectra);
	bool GetLocalBrukerFile(Leaf leaf, LPCTSTR reqfilename, CTypedPtrList<CPtrList, TMPSPECTRUM *> &spectra);
	bool GetLocalVarianFile(Leaf leaf, LPCTSTR reqfilename, CTypedPtrList<CPtrList, TMPSPECTRUM *> &spectra);
	void MoveTmpToDoc(TMPSPECTRUM *s, CNet12NMRDoc *pDoc);
	void SequelArray(LPCTSTR flag, LPCTSTR name);
	float LookupPhase(int decim, int dspfvs);
	void MapPropFile(CString &file);

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnUpdateFileinfocheck(CCmdUI *pCmdUI);
	afx_msg void OnFileinfocheck();
	afx_msg void OnUpdateViewparameterscheck(CCmdUI *pCmdUI);
	afx_msg void OnViewparameterscheck();
	afx_msg void OnUpdateBaseshelfcountedit(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePeakcountshelfedit(CCmdUI *pCmdUI);
	afx_msg void OnUpdateIntegralshelfcountedit(CCmdUI *pCmdUI);
	afx_msg void OnLoadshelfbutton();
	afx_msg void OnUpdateSaveshelfbutton(CCmdUI *pCmdUI);
	afx_msg void OnSaveshelfbutton();
	afx_msg void OnAboutbutton();
	afx_msg void OnApotypecombo();
	afx_msg void OnApounitscombo();
	afx_msg void OnApovaluecombo();
	afx_msg void OnUpdateTencheck(CCmdUI *pCmdUI);
	afx_msg void OnTencheck();
	afx_msg void OnUpdateOnecheck(CCmdUI *pCmdUI);
	afx_msg void OnOnecheck();
	afx_msg void OnUpdateTenthcheck(CCmdUI *pCmdUI);
	afx_msg void OnTenthcheck();
	afx_msg void OnUpdateAcheck(CCmdUI *pCmdUI);
	afx_msg void OnAcheck();
	afx_msg void OnUpdateBcheck(CCmdUI *pCmdUI);
	afx_msg void OnBcheck();
	afx_msg void OnFileOpen();
	afx_msg void OnUpdateClearbaseshelfbutton(CCmdUI *pCmdUI);
	afx_msg void OnClearbaseshelfbutton();
	afx_msg void OnUpdateClearpeakshelfbutton(CCmdUI *pCmdUI);
	afx_msg void OnClearpeakshelfbutton();
	afx_msg void OnUpdateClearintegralshelfbutton(CCmdUI *pCmdUI);
	afx_msg void OnClearintegralshelfbutton();
};

extern CNet12NMRApp theApp;

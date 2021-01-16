// ILTDoc.h : interface of the CILTDoc class
//


#pragma once

#include "tnt_array1d.h"
#include "tnt_array1d_utils.h"
#include "tnt_i_refvec.h"

class CDecay;

typedef struct model_tag {
	double a, r;
} SMCModel;

class CILTDoc : public CDocument
{
protected: // create from serialization only
	CILTDoc();
//	DECLARE_DYNCREATE(CILTDoc)
	DECLARE_SERIAL(CILTDoc)

// Attributes
public:
	double dblFrom, dblTo;
	Array1D<double> Xvalue;
	CTypedPtrList<CPtrList, CDecay *> Decays;
	CTypedPtrList<CPtrList, SMCModel *> Model;
	CString strTime;
	CString strSolvent;
	int iConfidenceInterval;
	float mcymax;
	EKernelType kernelType;

private:
	Array1D<double> Xorig;
	BOOL bSimplify, bOptimize;
	int iMethod, iSIRTCount, iCount, iIterations, iSpacing, iPoints;
	double dblSIRTTolerance, dblAlpha;
	double dblDACtoG, dblGamma, dblTimeCubed;
	double dblViscosity, dblTemperature;
	double dblObsFrom, dblObsTo;
	double dblStdErr;

// Operations
private:
	double text2double(LPCTSTR);

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CILTDoc();
	double Gamma(void) { return dblGamma; }
	double TimeCubed(void) { return dblTimeCubed; }
	double DACtoG(void) { return dblDACtoG; }
	double Viscosity(void) { return dblViscosity; }
	double Temperature(void) { return dblTemperature; }
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	void OpenTextILTFile(LPCTSTR filename);
	virtual void DeleteContents();
	afx_msg void OnUpdateFileSave(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFileSaveAs(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFilePrint(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFilePrintPreview(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFileNew(CCmdUI *pCmdUI);
	afx_msg void OnProcessIlt();
	afx_msg void OnUpdateProcessIlt(CCmdUI *pCmdUI);
	afx_msg void OnProcessKerneltype();
	afx_msg void OnProcessMontecarlo();
	afx_msg void OnUpdateProcessMontecarlo(CCmdUI *pCmdUI);
};



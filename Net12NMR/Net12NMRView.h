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

// Net12NMRView.h : interface of the CNetNMRView class
//

#pragma once

typedef struct cursortag {
	int left, right;
} CURSOR;

enum AxisType {
	nullUnits,
	noUnits,
	pointsUnits,
	timeUnits,
	hertzUnits,
	ppmUnits
};

enum YScaleMode {
	fromTop = ID_VIEW_Y_TOP,
	fromMiddle = ID_VIEW_Y_CENTER,
	fromBottom = ID_VIEW_Y_BOTTOM
};

class CNet12NMRView : public CView
{
protected: // create from serialization only
	CNet12NMRView();
	DECLARE_DYNCREATE(CNet12NMRView)

// Attributes
public:
	CNet12NMRDoc* GetDocument() const;
	bool AutoYscale, ViewPeakLabels, ViewIntegral;
	CURSOR zoom, cursor;
	float dmin, dmax;
	AxisType axisUnits;
	YScaleMode yMode;
	bool AutoYScale;

private:
	CPoint WindowOrigin, ViewportOrigin, CursorPosition;
	CSize WindowExtent, ViewportExtent;
	CRect SpecRect;
	int CursorY;	// only used for peak thresholds
	double axisStart, axisStop;
	CString axisLabel;
	vector<complex<float>> vdt;	// used for interactive phasing
	bool phaseonleft;
	float phaseA, phaseB;
	float maxdtphasemode, offsetdtphasemode;
	CURSOR index;
	int logpixelsx;
	float integralmax, integralmin;

// Operations
public:
	void SetIntegral(IREGION *p);
	void SetPeakOffset(PEAK *p);
private:
	void SetOffsetCore(double oldoffset);
	void OnDrawCore(CNet12NMRDoc *pDoc, CDC *pDC, int width);
	void FindMinMax(void);
	void SetAxisRange(void);
	double PtoH(unsigned int n);
	void EnableIfFTflag(CCmdUI *pCmdUI, BOOL value);
	void OnViewY(float factor);
	void PhaseDelta(bool isIncrement);
	void DrawPhase(CDC *pDC);
	void OnBaseline(bool bval);
	void AddIntegral(UINT start, UINT stop, bool value);
	void OnIntegral(bool bval);
	void AddThreshold(UINT start, UINT stop, float threshold);
	void OnThreshold(float y);
	void UpdatePeakList(bool recalc=true);
	void UpdateIntegralList(bool recalc=true);
	LPCTSTR GetCursorDifference(void);
	void FindIntegralMax(void);
	void MoveZoomCursors(int x);
	void SplitZoomCursors(void);
// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CNet12NMRView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnAutoybutton();
	afx_msg void OnUpybutton();
	afx_msg void OnDownybutton();
	afx_msg void OnViewYTop();
	afx_msg void OnUpdateViewYTop(CCmdUI *pCmdUI);
	afx_msg void OnViewYCenter();
	afx_msg void OnUpdateViewYCenter(CCmdUI *pCmdUI);
	afx_msg void OnViewYBottom();
	afx_msg void OnUpdateViewYBottom(CCmdUI *pCmdUI);
	afx_msg void OnTruncatebutton();
	afx_msg void OnUpdateTruncatebutton(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDclevelbutton(CCmdUI *pCmdUI);
	afx_msg void OnDclevelbutton();
	afx_msg void OnUpdateQcbutton(CCmdUI *pCmdUI);
	afx_msg void OnQcbutton();
	afx_msg void OnUpdateLeftshiftbutton(CCmdUI *pCmdUI);
	afx_msg void OnLeftshiftbutton();
	afx_msg void OnUpdateRightshiftbutton(CCmdUI *pCmdUI);
	afx_msg void OnRightshiftbutton();
	afx_msg void OnApodizebutton();
	afx_msg void OnUpdateApodizebutton(CCmdUI *pCmdUI);
	afx_msg void OnUpdateZerofillbutton(CCmdUI *pCmdUI);
	afx_msg void OnZerofillbutton();
	afx_msg void OnUpdateFtbutton(CCmdUI *pCmdUI);
	afx_msg void OnFtbutton();
	afx_msg void OnUpdateMagnitudebutton(CCmdUI *pCmdUI);
	afx_msg void OnMagnitudebutton();
	afx_msg void OnUpdateZerophasebutton(CCmdUI *pCmdUI);
	afx_msg void OnZerophasebutton();
	afx_msg void OnUpdateAutophasebutton(CCmdUI *pCmdUI);
	afx_msg void OnAutophasebutton();
	afx_msg void OnUpdateAutophaseaonlybutton(CCmdUI *pCmdUI);
	afx_msg void OnAutophaseaonlybutton();
	afx_msg void OnUpdateFlipabutton(CCmdUI *pCmdUI);
	afx_msg void OnFlipabutton();
	afx_msg void OnUpdateIncrementbutton(CCmdUI *pCmdUI);
	afx_msg void OnIncrementbutton();
	afx_msg void OnUpdateDecrementbutton(CCmdUI *pCmdUI);
	afx_msg void OnDecrementbutton();
	afx_msg void OnUpdateFiltercompbutton(CCmdUI *pCmdUI);
	afx_msg void OnFiltercompbutton();
	afx_msg void OnUpdateEditZoom(CCmdUI *pCmdUI);
	afx_msg void OnEditZoom();
	afx_msg void OnUpdateEditFull(CCmdUI *pCmdUI);
	afx_msg void OnEditFull();
	afx_msg void OnEditSetoffset();
	afx_msg void OnUpdateEditSetoffset(CCmdUI *pCmdUI);
	afx_msg void OnUpdateAtcursorbutton(CCmdUI *pCmdUI);
	afx_msg void OnAtcursorbutton();
	afx_msg void OnUpdateAbslevelbutton(CCmdUI *pCmdUI);
	afx_msg void OnAbslevelbutton();
	afx_msg void OnUpdateAbsresetbutton(CCmdUI *pCmdUI);
	afx_msg void OnAbsresetbutton();
	afx_msg void OnUpdateAbsfindbutton(CCmdUI *pCmdUI);
	afx_msg void OnAbsfindbutton();
	afx_msg void OnAbcwidthedit();
	afx_msg void OnAbcsnedit();
	afx_msg void EnableIfZoomAndFT(CCmdUI *pCmdUI);
	afx_msg void OnAddbaselinebutton();
	afx_msg void OnRemovebaselinebutton();
	afx_msg void OnPolycheck();
	afx_msg void OnUpdatePolycheck(CCmdUI *pCmdUI);
	afx_msg void OnMovecheck();
	afx_msg void OnUpdateMovecheck(CCmdUI *pCmdUI);
	afx_msg void OnCalcbasebutton();
	afx_msg void OnUpdateCalcbasebutton(CCmdUI *pCmdUI);
	afx_msg void OnPolyordercombo();
	afx_msg void OnUpdatePolyordercombo(CCmdUI *pCmdUI);
	afx_msg void OnMvavedit();
	afx_msg void OnUpdateMvavedit(CCmdUI *pCmdUI);
	afx_msg void OnApplybasebutton();
	afx_msg void OnUpdateApplybasebutton(CCmdUI *pCmdUI);
	afx_msg void OnBasetoshelfbutton();
	afx_msg void OnUpdateBasetoshelfbutton(CCmdUI *pCmdUI);
	afx_msg void OnShelftobasebutton();
	afx_msg void OnUpdateShelftobasebutton(CCmdUI *pCmdUI);
	afx_msg void OnAddintegralbutton();
	afx_msg void OnRemoveintegralbutton();
	afx_msg void OnUpdateClearintegralbutton(CCmdUI *pCmdUI);
	afx_msg void OnClearintegralbutton();
	afx_msg void OnUpdateInttoshelfbutton(CCmdUI *pCmdUI);
	afx_msg void OnInttoshelfbutton();
	afx_msg void OnUpdateShelftointbutton(CCmdUI *pCmdUI);
	afx_msg void OnShelftointbutton();
	afx_msg void OnUpdateBreakintegralbutton(CCmdUI *pCmdUI);
	afx_msg void OnBreakintegralbutton();
	afx_msg void OnSetintegralbutton();
	afx_msg void OnUpdateClearpeakregionbutton(CCmdUI *pCmdUI);
	afx_msg void OnClearpeakregionbutton();
	afx_msg void OnUpdateSpreadsheetbutton(CCmdUI *pCmdUI);
	afx_msg void OnSpreadsheetbutton();
	afx_msg void OnUpdateParapeakcheck(CCmdUI *pCmdUI);
	afx_msg void OnParapeakcheck();
	afx_msg void OnUpdatePpmpeakcheck(CCmdUI *pCmdUI);
	afx_msg void OnPpmpeakcheck();
	afx_msg void OnUpdateHzpeakcheck(CCmdUI *pCmdUI);
	afx_msg void OnPpmpeakcombo();
	afx_msg void OnHzpeakcombo();
	afx_msg void OnPeaksetthreshold();
	afx_msg void OnPeakremovethreshold();
	afx_msg void OnUpdatePeaktoshelfbutton(CCmdUI *pCmdUI);
	afx_msg void OnPeaktoshelfbutton();
	afx_msg void OnUpdateShelftopeakbutton(CCmdUI *pCmdUI);
	afx_msg void OnShelftopeakbutton();
	afx_msg void OnUpdateHertzbutton(CCmdUI *pCmdUI);
	afx_msg void OnHertzbutton();
	afx_msg void OnPpmbutton();
	afx_msg void OnUpdateTimebutton(CCmdUI *pCmdUI);
	afx_msg void OnTimebutton();
	afx_msg void OnPointsbutton();
	afx_msg void OnNoaxisbutton();
	afx_msg void OnUpdateReloadbutton(CCmdUI *pCmdUI);
	afx_msg void OnReloadbutton();
	afx_msg void OnUpdateZerodatabutton(CCmdUI *pCmdUI);
	afx_msg void OnZerodatabutton();
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateParfitcheck(CCmdUI *pCmdUI);
	afx_msg void OnParfitcheck();
	afx_msg void OnUpdatePlaybutton(CCmdUI *pCmdUI);
	afx_msg void OnPlaybutton();
	afx_msg void OnEditZoomto();
	afx_msg void OnUpdateGlobalintegralcheck(CCmdUI *pCmdUI);
	afx_msg void OnGlobalintegralcheck();
	afx_msg void OnUpdateLocaltoglobalbutton(CCmdUI *pCmdUI);
	afx_msg void OnLocaltoglobalbutton();
	afx_msg void OnUpdateGlobaltolocalbutton(CCmdUI *pCmdUI);
	afx_msg void OnGlobaltolocalbutton();
	afx_msg void OnUpdateViewPeaklabel(CCmdUI *pCmdUI);
	afx_msg void OnViewPeaklabel();
	afx_msg void OnUpdateViewIntegrals(CCmdUI *pCmdUI);
	afx_msg void OnViewIntegrals();
	afx_msg void OnUpdateAutoybutton(CCmdUI *pCmdUI);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
};

#ifndef _DEBUG  // debug version in Net12NMRView.cpp
inline CNet12NMRDoc* CNet12NMRView::GetDocument() const
   { return reinterpret_cast<CNet12NMRDoc*>(m_pDocument); }
#endif


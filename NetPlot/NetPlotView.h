// NetPlotView.h : interface of the CNetPlotView class
//


#pragma once

enum DisplayMode {
	normalMode = 0,
	phaseMode = 1,
	baseMode = 2,
	yexpandMode = 3,
	subtractMode = 4
};

class CNetPlotView : public CView
{
protected: // create from serialization only
	CNetPlotView();
	DECLARE_DYNCREATE(CNetPlotView)

// Attributes
public:
	CNetPlotDoc* GetDocument() const;
	DisplayMode disMode;
	int iLeft;
	CSpectrum *oneSpec;
	float oneRange, oneMin;
private:
	CSize page;
	CPoint viewOrg, windOrg;
	CSize viewExt, windExt;
	struct zoomtag {
		int left, right;
	} zoomCursor;
	CPen zoomPen, cursorPen;
	CSize specSize;
	int mainCursor;
	float curcurpos, leftcurpos, rightcurpos;

// Operations
public:
	void CopytoClipboard();
	void AutoArrange(float &width, float &height, float &offsetx, float &offsety, float &labelx, float &labely);
	void GetZoomCursors(int &left, int &right);
	float GetCursorDifference(void);
private:
	void PaintCore(CDC *pDC);
	void PaintPhase(CDC *pDC);
	BOOL CanDoFull(void);
	void OnIntegral(bool value);
	BOOL OnUpdateIntegral();
	BOOL OnUpdateClearAll();
	BOOL OnUpdateFitIntegrals();
	void OnWheel(float delta);

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
	virtual ~CNetPlotView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnUpdateFilePrint(CCmdUI *pCmdUI);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnViewZoom();
	afx_msg void OnUpdateViewZoom(CCmdUI *pCmdUI);
	afx_msg void OnViewFull();
	afx_msg void OnUpdateViewFull(CCmdUI *pCmdUI);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnIntegralClearall();
	afx_msg void OnUpdateIntegralAddintegral(CCmdUI *pCmdUI);
	afx_msg void OnIntegralAddintegral();
	afx_msg void OnIntegralRemoveintegral();
	afx_msg void OnUpdateIntegralRemoveintegral(CCmdUI *pCmdUI);
	afx_msg void OnUpdateIntegralClearall(CCmdUI *pCmdUI);
	afx_msg void OnUpdateContextSinglepointintegral(CCmdUI *pCmdUI);
	afx_msg void OnUpdateIntegralFitintegrals(CCmdUI *pCmdUI);
	afx_msg void OnIntegralFitintegrals();
	afx_msg void OnIntegralILTintegrals();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnYDown();
	afx_msg void OnUpdateYDown(CCmdUI *pCmdUI);
	afx_msg void OnYUp();
	afx_msg void OnUpdateYUp(CCmdUI *pCmdUI);
	afx_msg void OnYX1();
	afx_msg void OnUpdateYX1(CCmdUI *pCmdUI);
	afx_msg void OnViewAutomatic();
	afx_msg void OnUpdateViewAutomatic(CCmdUI *pCmdUI);
	afx_msg void OnContextSetoffsethere();
	afx_msg void OnGlobalView();
	afx_msg void OnUpdateGlobalView(CCmdUI *pCmdUI);
	afx_msg void OnViewDataclass();
	afx_msg void OnUpdateViewDataclass(CCmdUI *pCmdUI);
};

#ifndef _DEBUG  // debug version in NetPlotView.cpp
inline CNetPlotDoc* CNetPlotView::GetDocument() const
   { return reinterpret_cast<CNetPlotDoc*>(m_pDocument); }
#endif


// NetComPlotView.h : interface of the CNetComPlotView class
//


#pragma once


class CNetComPlotView : public CView
{
protected: // create from serialization only
	CNetComPlotView();
	DECLARE_DYNCREATE(CNetComPlotView)

// Attributes
public:
	CNetComPlotDoc* GetDocument() const;

// Operations
public:

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
	virtual ~CNetComPlotView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
private:
	void fillzero();
	void OnDrawCore(CDC *pDC, CNetComPlotDoc *pDoc);
	void Draw2DContour(CDC *pDC, CTwoDSpec *twodspec);
	void Draw1DSpectrum(CDC *pDC, COneDSpec *p, BOOL bIsHoriz);
	int nbmapsize;
	CPoint cp2DOffset;
	CSize pagesize;
	u_long *bmap;
	void MarkBitmap(int offset);
	void DrawCursor();
	void Annotate(CDC *pDC, CNetComPlotDoc *pDoc);
	BOOL GetBit(int offset);
	CRect scrRect;
	CRect zoomRect;
	CPoint zoomStart;
	CPoint cpCursor[4];
	CPoint cpHInitOffset, cpHOffset, cpVInitOffset, cpVOffset;
	float fHContext, fVContext;
	char *strHUnits, *strVUnits;
	CSpecDim *pHSpecDim, *pVSpecDim;
public:
	afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSetoffsethere();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnViewFull();
	afx_msg void OnViewZoom();
	afx_msg void OnViewIntegral();
	afx_msg void OnViewPrevious();
	afx_msg void OnViewPort();
	afx_msg void OnViewLand();
	afx_msg void OnUpdateViewFull(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewZoom(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewIntegral(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewPrevious(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewLand(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewPort(CCmdUI *pCmdUI);
	afx_msg void OnViewInformation();
	afx_msg void OnUpdateViewInformation(CCmdUI *pCmdUI);
};

#ifndef _DEBUG  // debug version in NetComPlotView.cpp
inline CNetComPlotDoc* CNetComPlotView::GetDocument() const
   { return reinterpret_cast<CNetComPlotDoc*>(m_pDocument); }
#endif


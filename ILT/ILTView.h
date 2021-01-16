// ILTView.h : interface of the CILTView class
//


#pragma once

class CDecay;

class CILTView : public CView
{
protected: // create from serialization only
	CILTView();
	DECLARE_DYNCREATE(CILTView)

// Attributes
public:
	CILTDoc* GetDocument() const;
	bool viewDecay, viewSimpleCI;

private:
	double costheta[128], sintheta[128];

// Operations
public:
private:
	void DrawAxes(CDC *pDC, double xmin, double xmax, double ymin, double ymax, LPCTSTR label);
	double ChiSq(bool bIsSimple);

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	void OnDrawCore(CDC *pDC);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CILTView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnViewTransform();
	afx_msg void OnUpdateViewTransform(CCmdUI *pCmdUI);
	afx_msg void OnViewText();
	afx_msg void OnUpdateViewText(CCmdUI *pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
	afx_msg void OnViewExponential();
	afx_msg void OnUpdateViewExponential(CCmdUI *pCmdUI);
	afx_msg void OnViewNext();
	afx_msg void OnUpdateViewNext(CCmdUI *pCmdUI);
	afx_msg void OnViewPrevious();
	afx_msg void OnUpdateViewPrevious(CCmdUI *pCmdUI);
	afx_msg void OnConfidence(UINT uID);
	afx_msg void OnUpdateConfidenceInterval(CCmdUI *pCmdUI);
	afx_msg void OnCIType(UINT uID);
	afx_msg void OnUpdateCIType(CCmdUI *pCmdUI);
};

#ifndef _DEBUG  // debug version in ILTView.cpp
inline CILTDoc* CILTView::GetDocument() const
   { return reinterpret_cast<CILTDoc*>(m_pDocument); }
#endif


// NetSpanView.h : interface of the CNetSpanView class
//


#pragma once

enum NetSpanView {viewAll=IDC_VIEWALLRADIO, viewTop=IDC_VIEWTOPRADIO, viewBottom=IDC_VIEWBOTTOMRADIO, viewSpaced=IDC_VIEWSPACEDRADIO};

class CNetSpanView : public CView
{
protected: // create from serialization only
	CNetSpanView();
	DECLARE_DYNCREATE(CNetSpanView)

// Attributes
public:
	CNetSpanDoc* GetDocument() const;
	double serr;
	NetSpanView myView;
private:
	CPoint cpCursor[4];
	struct {
		int left, right;
	} zoomCursor;
	CRect *pRect;
	int index;
	CFont labelFont, prlabelFont;
	CPen zoomPen, cursorPen;
	CStringList textList;
	CSize viewExt, windExt;
	CPoint viewOrg, windOrg;

// Operations
private:
	void ExcelColumn(CString &str, int col);
	bool ShowSpectrum(int index);

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
	virtual ~CNetSpanView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint point);
	afx_msg void OnContextNewlinehere();
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
	afx_msg void OnFileAddspectra();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextZoom();
	afx_msg void OnContextFull();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	void ClearZoomCursor(void);
	void EraseCursor(void);
protected:
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
private:
	int FillText(void);
	void DrawCursor();
	UINT CanZoom(void);
	UINT CanFull(void);
};

#ifndef _DEBUG  // debug version in NetSpanView.cpp
inline CNetSpanDoc* CNetSpanView::GetDocument() const
   { return reinterpret_cast<CNetSpanDoc*>(m_pDocument); }
#endif


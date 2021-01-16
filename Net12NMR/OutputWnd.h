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

#pragma once

#include "Net12NMR.h"

class CNet12NMRView;

/////////////////////////////////////////////////////////////////////////////
// COutputList window
#include "Net12NMRDoc.h"

class COutputList : public CListBox
{
// Construction
public:
	COutputList();

protected:
	CNet12NMRView *GetActiveView();
	int GetSelectCount(void);

// Implementation
public:
	virtual ~COutputList();

protected:
	//afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//afx_msg void OnEditCopy();
	//afx_msg void OnEditClear();
	//afx_msg void OnViewOutput();
	DECLARE_MESSAGE_MAP()
};

class CPeakList : public COutputList
{
public:
	CPeakList();
	virtual ~CPeakList();

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnUpdatePeakoutputSetoffset(CCmdUI *pCmdUI);
	afx_msg void OnPeakoutputSetoffset();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint point);
	afx_msg void OnLbnSelchange();
	afx_msg void OnUpdatePeakoutputCopyselected(CCmdUI *pCmdUI);
	afx_msg void OnPeakoutputCopyselected();
	afx_msg void OnUpdatePeakoutputCopyall(CCmdUI *pCmdUI);
	afx_msg void OnPeakoutputCopyall();
	afx_msg void OnUpdatePeakoutputDeleteall(CCmdUI *pCmdUI);
	afx_msg void OnPeakoutputDeleteall();
};

class CIntegralList : public COutputList
{
public:
	CIntegralList();
	virtual ~CIntegralList();

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint point);
	afx_msg void OnLbnSelchange();
	afx_msg void OnUpdateIntegraloutputSetintegral(CCmdUI *pCmdUI);
	afx_msg void OnIntegraloutputSetintegral();
	afx_msg void OnUpdateIntegraloutputDeleteselected(CCmdUI *pCmdUI);
	afx_msg void OnIntegraloutputDeleteselected();
	afx_msg void OnUpdateIntegraloutputDeleteall(CCmdUI *pCmdUI);
	afx_msg void OnIntegraloutputDeleteall();
	afx_msg void OnUpdateIntegraloutputCopyselected(CCmdUI *pCmdUI);
	afx_msg void OnIntegraloutputCopyselected();
	afx_msg void OnUpdateIntegraloutputCopyall(CCmdUI *pCmdUI);
	afx_msg void OnIntegraloutputCopyall();
};

class CShelfList : public COutputList
{
public:
	CShelfList();
	virtual ~CShelfList();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint point);
	afx_msg void OnUpdateShelfoutputDeleteselected(CCmdUI *pCmdUI);
	afx_msg void OnShelfoutputDeleteselected();
	afx_msg void OnUpdateShelfoutputDeleteall(CCmdUI *pCmdUI);
	afx_msg void OnShelfoutputDeleteall();
	afx_msg void OnUpdateShelfoutputCopy(CCmdUI *pCmdUI);
	afx_msg void OnShelfoutputCopy();
};

class COutputWnd : public CDockablePane
{
// Construction
public:
	COutputWnd();

	void UpdateFonts();

// Attributes
protected:
	CMFCTabCtrl	m_wndTabs;

	CIntegralList m_wndOutputIntegral;
	CPeakList m_wndOutputPeaks;
	CShelfList m_wndOutputShelf;

protected:
	void AdjustHorzScroll(CListBox& wndListBox);

// Implementation
public:
	CFont font;
	virtual ~COutputWnd();
	void AddPeak(LPCTSTR str, PEAK *p);
	void AddIntegral(LPCTSTR str, IREGION *p);
	void AddShelfItem(LPCTSTR str, void *p);
	void ClearPeaks(void);
	void ClearIntegrals(void);
	void ClearShelfItems(void);
	void ShowOutputList(DisplayMode dm);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
};


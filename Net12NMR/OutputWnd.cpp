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

#include "stdafx.h"

#include "OutputWnd.h"
#include "Resource.h"
#include "MainFrame.h"
#include "ChildFrame.h"
#include "Net12NMRView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputBar

COutputWnd::COutputWnd()
{
	font.CreateFont(16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Courier New");
}

COutputWnd::~COutputWnd()
{
}

BEGIN_MESSAGE_MAP(COutputWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

int COutputWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create tabs window:
	if (!m_wndTabs.Create(CMFCTabCtrl::STYLE_FLAT, rectDummy, this, 1))
	{
		TRACE0("Failed to create output tab window\n");
		return -1;      // fail to create
	}

	// Create output panes:
	const DWORD dwStyle = LBS_EXTENDEDSEL | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | LBS_USETABSTOPS;

	if (!m_wndOutputPeaks.Create(dwStyle, rectDummy, &m_wndTabs, 2) ||
		!m_wndOutputIntegral.Create(dwStyle, rectDummy, &m_wndTabs, 3) ||
		!m_wndOutputShelf.Create(dwStyle, rectDummy, &m_wndTabs, 4))
	{
		TRACE0("Failed to create output windows\n");
		return -1;      // fail to create
	}

	int tabsize = 24;
	m_wndOutputIntegral.SetTabStops(tabsize);
	UpdateFonts();

	CString strTabName;

	m_wndTabs.AddTab(&m_wndOutputPeaks, "Peaks", (UINT)0);
	m_wndTabs.AddTab(&m_wndOutputIntegral, "Integrals", (UINT)1);
	m_wndTabs.AddTab(&m_wndOutputShelf, "Shelf", (UINT)2);

	return 0;
}

void COutputWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// Tab control should cover the whole client area:
	m_wndTabs.SetWindowPos(NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}

void COutputWnd::AdjustHorzScroll(CListBox& wndListBox)
{
	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(&afxGlobalData.fontRegular);

	int cxExtentMax = 0;

	for (int i = 0; i < wndListBox.GetCount(); i ++)
	{
		CString strItem;
		wndListBox.GetText(i, strItem);

		cxExtentMax = max(cxExtentMax, dc.GetTextExtent(strItem).cx);
	}

	wndListBox.SetHorizontalExtent(cxExtentMax);
	dc.SelectObject(pOldFont);
}

void COutputWnd::AddPeak(LPCTSTR str, PEAK *p)
{
	int index = m_wndOutputPeaks.AddString(str);
	m_wndOutputPeaks.SetItemDataPtr(index, p);
	if (p && p->selected)
		m_wndOutputPeaks.SetSel(index);
}

void COutputWnd::AddIntegral(LPCTSTR str, IREGION *p)
{
	int index = m_wndOutputIntegral.AddString(str);
	m_wndOutputIntegral.SetItemDataPtr(index, p);
	if (p && p->selected)
		m_wndOutputIntegral.SetSel(index);
}

void COutputWnd::AddShelfItem(LPCTSTR str, void *p)
{
	int index = m_wndOutputShelf.AddString(str);
	m_wndOutputShelf.SetItemDataPtr(index, p);
}

void COutputWnd::ClearPeaks()
{
	while (m_wndOutputPeaks.GetCount())
		m_wndOutputPeaks.DeleteString(0);
}

void COutputWnd::ClearIntegrals()
{
	while (m_wndOutputIntegral.GetCount())
		m_wndOutputIntegral.DeleteString(0);
}

void COutputWnd::ClearShelfItems()
{
	while (m_wndOutputShelf.GetCount())
		m_wndOutputShelf.DeleteString(0);
}

void COutputWnd::UpdateFonts()
{
	m_wndOutputPeaks.SetFont(&font);
	m_wndOutputIntegral.SetFont(&font);
	m_wndOutputShelf.SetFont(&font);
}

void COutputWnd::ShowOutputList(DisplayMode dm)
{
	switch (dm) {
	case peakpickMode:
		m_wndTabs.SetActiveTab(m_wndTabs.GetTabFromHwnd(m_wndOutputPeaks.m_hWnd));
		break;
	case integralMode:
		m_wndTabs.SetActiveTab(m_wndTabs.GetTabFromHwnd(m_wndOutputIntegral.m_hWnd));
		break;
	case shelfMode:
		m_wndTabs.SetActiveTab(m_wndTabs.GetTabFromHwnd(m_wndOutputShelf.m_hWnd));
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////
// COutputList

COutputList::COutputList()
{
}

COutputList::~COutputList()
{
}

CNet12NMRView *COutputList::GetActiveView()
{
	CNet12NMRView *pView = NULL;
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if (pMain) {
		CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->MDIGetActive());
		if (pChild)
			pView = STATIC_DOWNCAST(CNet12NMRView, pChild->GetActiveView());
	}
	return pView;
}

int COutputList::GetSelectCount()
{
	vector<int> selected;
	int count = GetSelCount();
	if (count) {
		selected.resize(count);
		GetSelItems(count, selected.data());
		if (0 == selected[0])
			count--;
	}
	return count;
}

BEGIN_MESSAGE_MAP(COutputList, CListBox)
	//ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// COutputList message handlers

//void COutputList::OnViewOutput()
//{
//	CDockablePane* pParentBar = DYNAMIC_DOWNCAST(CDockablePane, GetOwner());
//	CMDIFrameWndEx* pMainFrame = DYNAMIC_DOWNCAST(CMDIFrameWndEx, GetTopLevelFrame());
//
//	if (pMainFrame != NULL && pParentBar != NULL) {
//		pMainFrame->SetFocus();
//		pMainFrame->ShowPane(pParentBar, FALSE, FALSE, FALSE);
//		pMainFrame->RecalcLayout();
//
//	}
//}

//////////////////////////////////////////////////////////////////////////////////////////////
// CPeakList

CPeakList::CPeakList()
{
}

CPeakList::~CPeakList()
{
}

BEGIN_MESSAGE_MAP(CPeakList, COutputList)
	ON_UPDATE_COMMAND_UI(ID_PEAKOUTPUT_SETOFFSET, &CPeakList::OnUpdatePeakoutputSetoffset)
	ON_COMMAND(ID_PEAKOUTPUT_SETOFFSET, &CPeakList::OnPeakoutputSetoffset)
	ON_WM_CONTEXTMENU()
	ON_CONTROL_REFLECT(LBN_SELCHANGE, &CPeakList::OnLbnSelchange)
	ON_UPDATE_COMMAND_UI(ID_PEAKOUTPUT_COPYSELECTED, &CPeakList::OnUpdatePeakoutputCopyselected)
	ON_COMMAND(ID_PEAKOUTPUT_COPYSELECTED, &CPeakList::OnPeakoutputCopyselected)
	ON_UPDATE_COMMAND_UI(ID_PEAKOUTPUT_COPYALL, &CPeakList::OnUpdatePeakoutputCopyall)
	ON_COMMAND(ID_PEAKOUTPUT_COPYALL, &CPeakList::OnPeakoutputCopyall)
	ON_UPDATE_COMMAND_UI(ID_PEAKOUTPUT_DELETEALL, &CPeakList::OnUpdatePeakoutputDeleteall)
	ON_COMMAND(ID_PEAKOUTPUT_DELETEALL, &CPeakList::OnPeakoutputDeleteall)
END_MESSAGE_MAP()

void CPeakList::OnUpdatePeakoutputSetoffset(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(1==GetSelectCount() ? TRUE : FALSE);
}

void CPeakList::OnPeakoutputSetoffset()
{
	CNet12NMRView *pView = GetActiveView();
	if (pView) {
		int count = GetSelCount();
		vector<int> selected;
		selected.resize(count);
		GetSelItems(count, selected.data());
		PEAK *p = (PEAK *)GetItemDataPtr(selected[count-1]);
		pView->SetPeakOffset(p);
	}
}

void CPeakList::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	CMenu menu;
	menu.LoadMenu(IDR_PEAKOUTPUTMENU);

	CMenu* pSumMenu = menu.GetSubMenu(0);

	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			return;

		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		UpdateDialogControls(this, FALSE);
	}

	SetFocus();
}

void CPeakList::OnLbnSelchange()
{
	int count = GetCount();
	for (int i=0; i<count; i++) {
		PEAK *p = (PEAK *)GetItemDataPtr(i);
		if (p)
			p->selected = false;
	}
	vector<int> selected;
	count = GetSelCount();
	selected.resize(count);
	GetSelItems(count, selected.data());
	for (int i=0; i<count; i++) {
		PEAK *p = (PEAK *)GetItemDataPtr(selected[i]);
		if (p)
			p->selected = true;
	}
	GetActiveView()->Invalidate();
}

void CPeakList::OnUpdatePeakoutputCopyselected(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetSelectCount() ? TRUE : FALSE);
}

void CPeakList::OnPeakoutputCopyselected()
{
	int count = GetSelCount();
	vector<int> selected;
	selected.resize(count);
	GetSelItems(count, selected.data());
	CString clip;
	if (0 != selected[0]) {
		GetText(0, clip);
		clip += "\r\n";
	}
	for (int n=0; n<count; n++) {
		CString line;
		GetText(selected[n], line);
		clip += line + "\r\n";
	}
	if (OpenClipboard()) {
		if (EmptyClipboard()) {
			int size = clip.GetLength() + 1;
			GLOBALHANDLE hgText = GlobalAlloc(GHND, size);
			if (hgText) {
				char *pgmem = (char *)GlobalLock(hgText);
				if (pgmem)
					strcpy_s(pgmem, size, clip);
				GlobalUnlock(hgText);
				SetClipboardData(CF_TEXT, hgText);
			}
		}
		CloseClipboard();
	}
}


void CPeakList::OnUpdatePeakoutputCopyall(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetCount() ? TRUE : FALSE);
}


void CPeakList::OnPeakoutputCopyall()
{
	CString clip;
	int count = GetCount();
	for (int n=0; n<count; n++) {
		CString line;
		GetText(n, line);
		clip += line + "\r\n";
	}
	if (OpenClipboard()) {
		if (EmptyClipboard()) {
			int size = clip.GetLength() + 1;
			GLOBALHANDLE hgText = GlobalAlloc(GHND, size);
			if (hgText) {
				char *pgmem = (char *)GlobalLock(hgText);
				if (pgmem)
					strcpy_s(pgmem, size, clip);
				GlobalUnlock(hgText);
				SetClipboardData(CF_TEXT, hgText);
			}
		}
		CloseClipboard();
	}
}


void CPeakList::OnUpdatePeakoutputDeleteall(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetCount() ? TRUE : FALSE);
}


void CPeakList::OnPeakoutputDeleteall()
{
	CNet12NMRView *pView = GetActiveView();
	CNet12NMRDoc *pDoc = pView->GetDocument();
	while (pDoc->peakregions.GetCount())
		delete pDoc->peakregions.RemoveHead();
	while (pDoc->peaks.GetCount())
		delete pDoc->peaks.RemoveHead();
	while (GetCount())
		DeleteString(0);
	pDoc->UpdateAllViews(NULL);
	pDoc->SetModifiedFlag();
}

///////////////////////////////////////////////////////////////////////////////////////
// CIntegralList

CIntegralList::CIntegralList()
{
}

CIntegralList::~CIntegralList()
{
}

BEGIN_MESSAGE_MAP(CIntegralList, COutputList)
	ON_WM_CONTEXTMENU()
	ON_CONTROL_REFLECT(LBN_SELCHANGE, &CIntegralList::OnLbnSelchange)
	ON_UPDATE_COMMAND_UI(ID_INTEGRALOUTPUT_SETINTEGRAL, &CIntegralList::OnUpdateIntegraloutputSetintegral)
	ON_COMMAND(ID_INTEGRALOUTPUT_SETINTEGRAL, &CIntegralList::OnIntegraloutputSetintegral)
	ON_UPDATE_COMMAND_UI(ID_INTEGRALOUTPUT_DELETESELECTED, &CIntegralList::OnUpdateIntegraloutputDeleteselected)
	ON_COMMAND(ID_INTEGRALOUTPUT_DELETESELECTED, &CIntegralList::OnIntegraloutputDeleteselected)
	ON_UPDATE_COMMAND_UI(ID_INTEGRALOUTPUT_DELETEALL, &CIntegralList::OnUpdateIntegraloutputDeleteall)
	ON_COMMAND(ID_INTEGRALOUTPUT_DELETEALL, &CIntegralList::OnIntegraloutputDeleteall)
	ON_UPDATE_COMMAND_UI(ID_INTEGRALOUTPUT_COPYSELECTED, &CIntegralList::OnUpdateIntegraloutputCopyselected)
	ON_COMMAND(ID_INTEGRALOUTPUT_COPYSELECTED, &CIntegralList::OnIntegraloutputCopyselected)
	ON_UPDATE_COMMAND_UI(ID_INTEGRALOUTPUT_COPYALL, &CIntegralList::OnUpdateIntegraloutputCopyall)
	ON_COMMAND(ID_INTEGRALOUTPUT_COPYALL, &CIntegralList::OnIntegraloutputCopyall)
END_MESSAGE_MAP()

void CIntegralList::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	CMenu menu;
	menu.LoadMenu(IDR_INTEGRALOUTPUTMENU);

	CMenu* pSumMenu = menu.GetSubMenu(0);

	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			return;

		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		UpdateDialogControls(this, FALSE);
	}

	SetFocus();
}

void CIntegralList::OnLbnSelchange()
{
	int count = GetCount();
	for (int i=0; i<count; i++) {
		IREGION *p = (IREGION *)GetItemDataPtr(i);
		if (p)
			p->selected = false;
	}
	vector<int> selected;
	count = GetSelCount();
	selected.resize(count);
	GetSelItems(count, selected.data());
	for (int i=0; i<count; i++) {
		IREGION *p = (IREGION *)GetItemDataPtr(selected[i]);
		if (p)
			p->selected = true;
	}
	GetActiveView()->Invalidate();
}

void CIntegralList::OnUpdateIntegraloutputSetintegral(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetSelectCount() ? TRUE : FALSE);
}

void CIntegralList::OnIntegraloutputSetintegral()
{
	IREGION *p = NULL;
	if (1 == GetSelectCount()) {
		for (int i=1; i<GetCount(); i++) {	// skip the header (int i=1)
			IREGION *pInt = (IREGION *)GetItemDataPtr(i);
			if (pInt->selected) {
				p = pInt;
				break;
			}
		}
	}
	GetActiveView()->SetIntegral(p);
}

void CIntegralList::OnUpdateIntegraloutputCopyselected(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetSelectCount() ? TRUE : FALSE);
}

void CIntegralList::OnIntegraloutputCopyselected()
{
	int count = GetSelCount();
	vector<int> selected;
	selected.resize(count);
	GetSelItems(count, selected.data());
	CString clip;
	if (0 != selected[0]) {
		GetText(0, clip);
		clip += "\r\n";
	}
	for (int n=0; n<count; n++) {
		CString line;
		GetText(selected[n], line);
		clip += line + "\r\n";
	}
	if (OpenClipboard()) {
		if (EmptyClipboard()) {
			int size = clip.GetLength() + 1;
			GLOBALHANDLE hgText = GlobalAlloc(GHND, size);
			if (hgText) {
				char *pgmem = (char *)GlobalLock(hgText);
				if (pgmem)
					strcpy_s(pgmem, size, clip);
				GlobalUnlock(hgText);
				SetClipboardData(CF_TEXT, hgText);
			}
		}
		CloseClipboard();
	}
}

void CIntegralList::OnUpdateIntegraloutputCopyall(CCmdUI *pCmdUI)
{
	pCmdUI->Enable((GetCount()>1) ? TRUE : FALSE);
}

void CIntegralList::OnIntegraloutputCopyall()
{
	CString clip;
	int count = GetCount();
	for (int n=0; n<count; n++) {
		CString line;
		GetText(n, line);
		clip += line + "\r\n";
	}
	if (OpenClipboard()) {
		if (EmptyClipboard()) {
			int size = clip.GetLength() + 1;
			GLOBALHANDLE hgText = GlobalAlloc(GHND, size);
			if (hgText) {
				char *pgmem = (char *)GlobalLock(hgText);
				if (pgmem)
					strcpy_s(pgmem, size, clip);
				GlobalUnlock(hgText);
				SetClipboardData(CF_TEXT, hgText);
			}
		}
		CloseClipboard();
	}
}

void CIntegralList::OnUpdateIntegraloutputDeleteselected(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetSelectCount() ? TRUE : FALSE);
}

void CIntegralList::OnIntegraloutputDeleteselected()
{
	CNet12NMRView *pView = GetActiveView();
	CNet12NMRDoc *pDoc = pView->GetDocument();
	vector<int> selected;
	int n = GetSelCount();
	selected.resize(n);
	GetSelItems(n, selected.data());
	for (int i=0; i<n; i++) {
		IREGION *pDelete = (IREGION *)GetItemDataPtr(selected[i]);
		if (pDelete) {
			POSITION pos = pDoc->intregions.GetHeadPosition();
			while (pos) {
				IREGION *p = pDoc->intregions.GetAt(pos);
				if (pDelete == p) {
					delete pDelete;
					pDoc->intregions.RemoveAt(pos);
					break;
				} else
					pDoc->intregions.GetNext(pos);
			}
		}
	}
	pDoc->UpdateAllViews(NULL);
	for (int i=n-1; i>=0; i--)
		if (selected[i])
		DeleteString(selected[i]);
	pDoc->SetModifiedFlag();
}

void CIntegralList::OnUpdateIntegraloutputDeleteall(CCmdUI *pCmdUI)
{
	pCmdUI->Enable((GetCount()>1) ? TRUE : FALSE);
}

void CIntegralList::OnIntegraloutputDeleteall()
{
	CNet12NMRView *pView = GetActiveView();
	pView->OnClearintegralbutton();
}

//////////////////////////////////////////////////////////////////////////////////////
// CShelfList

CShelfList::CShelfList()
{
}

CShelfList::~CShelfList()
{
}

BEGIN_MESSAGE_MAP(CShelfList, COutputList)
	ON_WM_CONTEXTMENU()
	ON_UPDATE_COMMAND_UI(ID_SHELFOUTPUT_DELETESELECTED, &CShelfList::OnUpdateShelfoutputDeleteselected)
	ON_COMMAND(ID_SHELFOUTPUT_DELETESELECTED, &CShelfList::OnShelfoutputDeleteselected)
	ON_UPDATE_COMMAND_UI(ID_SHELFOUTPUT_DELETEALL, &CShelfList::OnUpdateShelfoutputDeleteall)
	ON_COMMAND(ID_SHELFOUTPUT_DELETEALL, &CShelfList::OnShelfoutputDeleteall)
	ON_UPDATE_COMMAND_UI(ID_SHELFOUTPUT_COPY, &CShelfList::OnUpdateShelfoutputCopy)
	ON_COMMAND(ID_SHELFOUTPUT_COPY, &CShelfList::OnShelfoutputCopy)
END_MESSAGE_MAP()

void CShelfList::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	CMenu menu;
	menu.LoadMenu(IDR_SHELFOUTPUTMENU);

	CMenu* pSumMenu = menu.GetSubMenu(0);

	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			return;

		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		UpdateDialogControls(this, FALSE);
	}

	SetFocus();
}

void CShelfList::OnUpdateShelfoutputDeleteselected(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetSelectCount() ? TRUE : FALSE);
}

void CShelfList::OnShelfoutputDeleteselected()
{
	vector<int> selected;
	int n = GetSelCount();
	selected.resize(n);
	GetSelItems(n, selected.data());
	for (int i=n-1; i>=0; i--) {
		if (selected[i]) {
			theApp.shelf.DeleteByPointer(GetItemDataPtr(selected[i]));
			DeleteString(selected[i]);
		}
	}
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	theApp.shelf.RibbonUpdate(pMain->m_wndRibbonBar);
}

void CShelfList::OnUpdateShelfoutputDeleteall(CCmdUI *pCmdUI)
{
	pCmdUI->Enable((GetCount()>1) ? TRUE : FALSE);
}

void CShelfList::OnShelfoutputDeleteall()
{
	theApp.shelf.RemoveAll();
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	theApp.shelf.RibbonUpdate(pMain->m_wndRibbonBar);
}

void CShelfList::OnUpdateShelfoutputCopy(CCmdUI *pCmdUI)
{
	pCmdUI->Enable((GetCount()>1) ? TRUE : FALSE);
}

void CShelfList::OnShelfoutputCopy()
{
	CString clip;
	int count = GetCount();
	for (int n=0; n<count; n++) {
		CString line;
		GetText(n, line);
		clip += line + "\r\n";
	}
	if (OpenClipboard()) {
		if (EmptyClipboard()) {
			int size = clip.GetLength() + 1;
			GLOBALHANDLE hgText = GlobalAlloc(GHND, size);
			if (hgText) {
				char *pgmem = (char *)GlobalLock(hgText);
				if (pgmem)
					strcpy_s(pgmem, size, clip);
				GlobalUnlock(hgText);
				SetClipboardData(CF_TEXT, hgText);
			}
		}
		CloseClipboard();
	}
}

// MainFrame.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "NetPlot.h"

#include "MainFrame.h"
#include "ProcessWnd.h"
#include "ProcSheet.h"
#include "ChildFrame.h"
#include "NetPlotDoc.h"
#include "..\NMRDataTypes.h"
#include "Spectrum.h"
#include "NetPlotView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	ON_WM_CREATE()
	// Global help commands
	ON_COMMAND(ID_HELP_FINDER, &CMDIFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, &CMDIFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, &CMDIFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, &CMDIFrameWnd::OnHelpFinder)
	ON_COMMAND_RANGE(ID_PROCESS_PROCESS, ID_PROCESS_YEXPANSION, &CMainFrame::OnProcess)
	ON_UPDATE_COMMAND_UI(ID_CURSOR_DIFFERENCE, &CMainFrame::OnUpdateCursorDifference)
	ON_UPDATE_COMMAND_UI_RANGE(ID_PROCESS_PROCESS, ID_PROCESS_YEXPANSION, &CMainFrame::OnUpdateProcess)
	ON_WM_SIZE()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_CURSOR_DIFFERENCE,
};


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
: midscreen(0)
, windowCount(0)
, dialogFrame(NULL)
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	UINT n = 0;
	CRect *r = NULL;
	if ( theApp.GetProfileBinary("Settings", "Rect", (LPBYTE*)&r, &n) ) {
			if (n == sizeof(CRect)) {
				if ( MonitorFromRect(r, MONITOR_DEFAULTTONULL) ) {
					cs.x = r->left;
					cs.y = r->top;
					cs.cx = r->Width();
					cs.cy = r->Height();
				} else
					n = 0;
			} else
				n = 0;
	} 
	if (0 == n) {
		CRect cr;
		SystemParametersInfo( SPI_GETWORKAREA, NULL, cr, NULL );
		cs.x = cs.y = 0;
		cs.cx = cr.Width() - 400;
		cs.cy = cr.Height();
	}
	if (r)
		delete [] r;
	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::MessagetoBar(LPCTSTR msg)
{
	m_wndStatusBar.SetPaneText(0, msg, TRUE);
}

void CMainFrame::HideProcessSheet(void)
{
	if (dialogFrame != NULL)
		dialogFrame->ShowWindow(SW_HIDE);
	CChildFrame *pChild = STATIC_DOWNCAST( CChildFrame, MDIGetActive() );
	if ( pChild ) {
		CNetPlotView *pView = STATIC_DOWNCAST( CNetPlotView, pChild->GetActiveView() );
		if ( pView ) {
			pView->disMode = normalMode;
			pView->Invalidate();
		}
	}
}

void CMainFrame::OnProcess(UINT nID)
{
	// If mini frame does not already exist, create a new one.
	// Otherwise, unhide it
	if (dialogFrame == NULL)
	{
		dialogFrame = new CProcessWnd;
		CRect rect(0, 0, 0, 0);
		if (!dialogFrame->Create(NULL, "NetPlot Process Control",
			WS_POPUP | WS_CAPTION | WS_SYSMENU, rect, this))
		{
			dialogFrame = NULL;
			return;
		}
	}
	dialogFrame->procSheet->SetActivePage(nID-ID_PROCESS_PROCESS);

	if (dialogFrame != NULL && !dialogFrame->IsWindowVisible())
		dialogFrame->ShowWindow(SW_SHOW);
}

void CMainFrame::OnUpdateProcess(CCmdUI *pCmdUI)
{
	BOOL rtrn = FALSE;
	if (windowCount) {
		CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, MDIGetActive());
		CNetPlotDoc *pDoc = STATIC_DOWNCAST(CNetPlotDoc, pChild->GetActiveDocument());
		rtrn = pDoc->Spectra.GetCount() ? TRUE : FALSE;
	}
	pCmdUI->Enable(rtrn);
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CMDIFrameWnd::OnSize(nType, cx, cy);
	midscreen.x = cx / 2;
	midscreen.y = cy / 2;
}

void CMainFrame::OnUpdateCursorDifference(CCmdUI *pCmdUI)
{
	CString str = "";
	CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, MDIGetActive());
	if (pChild) {
		CNetPlotView *pView = STATIC_DOWNCAST(CNetPlotView, pChild->GetActiveView());
		CNetPlotDoc *pDoc = pView->GetDocument();
		if (pDoc->Spectra.GetCount() && pDoc->AllTheSame) {
			float diff = fabs(pView->GetCursorDifference());
			if (diff > 0.0f)
				str.Format("diff = %.2f %s", diff, (RangeHertz==pDoc->Range)?"Hz":"PPM");
		}
	} 
	pCmdUI->SetText(str);
}

void CMainFrame::OnClose()
{
	CRect r;
	GetWindowRect(&r);
	theApp.WriteProfileBinary("Settings", "Rect", (LPBYTE)&r, sizeof(CRect));

	CMDIFrameWnd::OnClose();
}

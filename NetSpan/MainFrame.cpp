// MainFrame.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "NetSpan.h"

#include "MainFrame.h"
#include "NetSpanDoc.h"
#include "NetSpanView.h"
#include "SFDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	// Global help commands
	ON_COMMAND(ID_HELP_FINDER, &CFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, &CFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, &CFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, &CFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_EDIT_LINES, &CMainFrame::OnEditLines)
	ON_COMMAND(ID_EDIT_SPECTRA, &CMainFrame::OnEditSpectra)
	ON_UPDATE_COMMAND_UI(ID_EDIT_LINES, &CMainFrame::OnUpdateEditLines)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SPECTRA, &CMainFrame::OnUpdateEditSpectra)
	ON_COMMAND(ID_EDIT_FIT, &CMainFrame::OnEditFit)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FIT, &CMainFrame::OnUpdateEditFit)
	ON_COMMAND(ID_EDIT_SPECFREQUENCY, &CMainFrame::OnEditSpecfrequency)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_LABEL		// Spectrum indicator
};


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	tabDlg = NULL;
}

CMainFrame::~CMainFrame()
{
	if (tabDlg)
		delete tabDlg;
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
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

	m_wndStatusBar.SetPaneInfo(1, ID_INDICATOR_LABEL, SBPS_NORMAL, 70);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.x = cs.y = 0;
	CRect r;
	SystemParametersInfo(SPI_GETWORKAREA, NULL, r, NULL);
	cs.cx = r.Width() - 480;
	cs.cy = r.Height();

	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::SetText(int pane, LPCTSTR text)
{
	m_wndStatusBar.SetPaneText(pane, text, TRUE);
}

void CMainFrame::ShowTabbedDialog(int page)
{
	if (! tabDlg) {
		tabDlg = new CTabbedDlg("NetSpan Control");
		tabDlg->Create(this);
	}
	if (tabDlg) {
		tabDlg->SetActivePage(page);
		if (! tabDlg->IsWindowVisible())
			tabDlg->ShowWindow(SW_SHOW);
	}
}

void CMainFrame::OnEditSpectra()
{
	ShowTabbedDialog(0);
}

void CMainFrame::OnEditLines()
{
	ShowTabbedDialog(1);
}

void CMainFrame::OnEditFit()
{
	ShowTabbedDialog(2);
}

void CMainFrame::OnUpdateEditSpectra(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(((CNetSpanDoc *)GetActiveDocument())->Spectra.GetCount() > 0);
}

void CMainFrame::OnUpdateEditLines(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(((CNetSpanDoc *)GetActiveDocument())->Lines.GetCount() > 0);
}

void CMainFrame::OnUpdateEditFit(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(((CNetSpanDoc *)GetActiveDocument())->Lines.GetCount() > 0);
}

void CMainFrame::OnEditSpecfrequency()
{
	CSFDlg dlg;
	if (IDOK == dlg.DoModal()) {
		((CNetSpanDoc *)GetActiveDocument())->sf = dlg.m_sfDouble;
	}
}

void CMainFrame::RefillSpecList(void)
{
	if (tabDlg)
		tabDlg->RefillSpecList();
}

BOOL CMainFrame::OnEraseBkgnd(CDC* pDC)
{
	CNetSpanView *pView = STATIC_DOWNCAST(CNetSpanView, GetActiveView());
	if (pView) {
		pView->EraseCursor();
	}
	return CFrameWnd::OnEraseBkgnd(pDC);
}
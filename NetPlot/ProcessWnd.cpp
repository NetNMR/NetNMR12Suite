// ProcessWnd.cpp : implementation file
//

#include "stdafx.h"
#include "NetPlot.h"
#include "ProcessWnd.h"
#include "MainFrame.h"
#include "ProcSheet.h"


// CProcessWnd

IMPLEMENT_DYNCREATE(CProcessWnd, CMiniFrameWnd)

CProcessWnd::CProcessWnd()
{

}

CProcessWnd::~CProcessWnd()
{
}


BEGIN_MESSAGE_MAP(CProcessWnd, CMiniFrameWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_SETFOCUS()
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()


// CProcessWnd message handlers
int CProcessWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMiniFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	procSheet = new CProcSheet("");
	if (!procSheet->Create(this, WS_CHILD | WS_VISIBLE, 0))
	{
		AfxMessageBox( "Can't create process control!" );
		delete procSheet;
		procSheet = NULL;
		return -1;
	}
	// Resize the mini frame so that it fits around the child property
	// sheet.
	CRect rectClient, rectWindow;
	procSheet->GetWindowRect(rectClient);
	rectWindow = rectClient;

	// CMiniFrameWnd::CalcWindowRect adds the extra width and height
	// needed from the mini frame.
	CalcWindowRect(rectWindow);
	CRect r;

	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	pMain->GetWindowRect(&r);
	HMONITOR hM = MonitorFromRect(&r, MONITOR_DEFAULTTOPRIMARY);
	MONITORINFO mi;
	mi.cbSize = sizeof(MONITORINFO);
	if ( GetMonitorInfo(hM, &mi) )
		r = mi.rcWork;
	else
		SystemParametersInfo(SPI_GETWORKAREA, NULL, r, NULL );
	SetWindowPos(NULL, 
		r.right-rectWindow.Width(),
		r.bottom-rectWindow.Height(),
		rectWindow.Width(), rectWindow.Height(),
		SWP_NOZORDER | SWP_NOACTIVATE);
	procSheet->SetWindowPos(NULL, 0, 0,
		rectClient.Width(), rectClient.Height(),
		SWP_NOZORDER | SWP_NOACTIVATE);

	return 0;
}

void CProcessWnd::OnClose()
{
	// Hide it, don't close it!
	CMainFrame* pMainFrame = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	pMainFrame->HideProcessSheet();
}

void CProcessWnd::OnSetFocus(CWnd* pOldWnd)
{
	CMiniFrameWnd::OnSetFocus(pOldWnd);

	ASSERT_VALID(procSheet);
	procSheet->SetFocus();	
}

void CProcessWnd::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CMiniFrameWnd::OnActivate(nState, pWndOther, bMinimized);

	// Forward any WM_ACTIVATEs to the property sheet...
	// Like the dialog manager itself, it needs them to save/restore the focus.
	ASSERT_VALID(procSheet);
	// Use GetCurrentMessage to get unmodified message data.
	const MSG* pMsg = GetCurrentMessage();
	ASSERT(pMsg->message == WM_ACTIVATE);
	procSheet->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
}

void CProcessWnd::UpdateContent()
{
	procSheet->UpdateContent();
}
// ZoomToDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Net12NMR.h"
#include "MainFrame.h"
#include "Net12NMRDoc.h"
#include "Net12NMRView.h"
#include "ZoomToDlg.h"

// CZoomToDlg dialog

static CString Zoom2To = "";
static CString Zoom2From = "";
static int Zoom2Check = -1;

IMPLEMENT_DYNAMIC(CZoomToDlg, CDialog)
CZoomToDlg::CZoomToDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CZoomToDlg::IDD, pParent)
{
	ptsmsg = "Enter ordinal (starts with zero) indicies.";
	timemsg = "Enter times in seconds.";
	hertzmsg = "Enter range in Hertz.";
	ppmmsg = "Enter range in PPM.";
	m_ValueA = Zoom2From;
	m_ValueB = Zoom2To;
	m_bHertz = FALSE;
	m_bPPM = FALSE;
	m_bPTS = FALSE;
	m_bTime = FALSE;
}

CZoomToDlg::~CZoomToDlg()
{
}

void CZoomToDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ZOOMTO_MSG, m_MsgCtrl);
	DDX_Control(pDX, IDC_ZOOMTO_TIME, m_Time);
	DDX_Control(pDX, IDC_ZOOMTO_PTS, m_Pts);
	DDX_Control(pDX, IDC_ZOOMTO_PPM, m_PPM);
	DDX_Control(pDX, IDC_ZOOMTO_HERTZ, m_Hertz);
	DDX_Text(pDX, IDC_ZOOMTO_A, m_ValueA);
	DDV_MaxChars(pDX, m_ValueA, 8);
	DDX_Text(pDX, IDC_ZOOMTO_B, m_ValueB);
	DDV_MaxChars(pDX, m_ValueB, 8);
	DDX_Check(pDX, IDC_ZOOMTO_HERTZ, m_bHertz);
	DDX_Check(pDX, IDC_ZOOMTO_PPM, m_bPPM);
	DDX_Check(pDX, IDC_ZOOMTO_PTS, m_bPTS);
	DDX_Check(pDX, IDC_ZOOMTO_TIME, m_bTime);
}


BEGIN_MESSAGE_MAP(CZoomToDlg, CDialog)
	ON_BN_CLICKED(IDC_ZOOMTO_PTS, &CZoomToDlg::OnZoomtoPts)
	ON_BN_CLICKED(IDC_ZOOMTO_TIME, &CZoomToDlg::OnZoomtoTime)
	ON_BN_CLICKED(IDC_ZOOMTO_HERTZ, &CZoomToDlg::OnZoomtoHertz)
	ON_BN_CLICKED(IDC_ZOOMTO_PPM, &CZoomToDlg::OnZoomtoPpm)
END_MESSAGE_MAP()

// CZoomToDlg message handlers

BOOL CZoomToDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CMainFrame *pMainWnd = STATIC_DOWNCAST(CMainFrame, AfxGetApp()->m_pMainWnd);
	CNet12NMRView *pView = STATIC_DOWNCAST(CNet12NMRView, pMainWnd->MDIGetActive()->GetActiveView());
	CNet12NMRDoc *pDoc = pView->GetDocument();
	if (pDoc->sp.ftflag) {
		m_Time.ShowWindow(SW_HIDE);
		m_PPM.SetCheck(1);
		m_MsgCtrl.SetWindowText(ppmmsg);
		switch (Zoom2Check) {
		case 0:
			OnZoomtoPts();
			break;
		case 2:
			OnZoomtoHertz();
			break;
		}
	} else {
		m_Hertz.ShowWindow(SW_HIDE);
		m_PPM.ShowWindow(SW_HIDE);
		m_Time.SetCheck(1);
		m_MsgCtrl.SetWindowText(ptsmsg);
		if (Zoom2Check==0)
			OnZoomtoPts();
		
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CZoomToDlg::OnZoomtoPts() 
{
	m_Time.SetCheck(0);
	m_Hertz.SetCheck(0);
	m_PPM.SetCheck(0);
	m_Pts.SetCheck(1);
	m_MsgCtrl.SetWindowText(ptsmsg);
}

void CZoomToDlg::OnZoomtoTime() 
{
	m_Pts.SetCheck(0);
	m_Time.SetCheck(1);
	m_MsgCtrl.SetWindowText(timemsg);
}

void CZoomToDlg::OnZoomtoHertz() 
{
	m_Pts.SetCheck(0);
	m_Hertz.SetCheck(1);
	m_PPM.SetCheck(0);
	m_MsgCtrl.SetWindowText(hertzmsg);
}

void CZoomToDlg::OnZoomtoPpm() 
{
	m_Pts.SetCheck(0);
	m_Hertz.SetCheck(0);
	m_PPM.SetCheck(1);
	m_MsgCtrl.SetWindowText(ppmmsg);
}

void CZoomToDlg::OnOK() 
{
	CDialog::OnOK();
	Zoom2From = m_ValueA;
	Zoom2To = m_ValueB;
	if (m_Pts.GetCheck())
		Zoom2Check = 0;
	else if (m_Time.GetCheck())
		Zoom2Check = 1;
	else if (m_Hertz.GetCheck())
		Zoom2Check = 2;
	else
		Zoom2Check = 3;
}

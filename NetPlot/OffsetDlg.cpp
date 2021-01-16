// OffsetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NetPlot.h"
#include "OffsetDlg.h"
#include "MainFrame.h"
#include "ChildFrame.h"
#include "NetPlotDoc.h"
#include "NetPlotView.h"


// COffsetDlg dialog

IMPLEMENT_DYNAMIC(COffsetDlg, CDialog)

COffsetDlg::COffsetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COffsetDlg::IDD, pParent)
	, m_strStatic(_T(""))
	, m_floatOffset(0)
{

}

COffsetDlg::~COffsetDlg()
{
}

void COffsetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_OFFSETSTATIC, m_strStatic);
	DDX_Text(pDX, IDC_OFFSETEDIT, m_floatOffset);
	DDX_Control(pDX, IDC_OFFSETEDIT, m_offsetEdit);
}


BEGIN_MESSAGE_MAP(COffsetDlg, CDialog)
END_MESSAGE_MAP()


// COffsetDlg message handlers

BOOL COffsetDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->MDIGetActive());
	pDoc = STATIC_DOWNCAST(CNetPlotDoc, pChild->GetActiveDocument());
	pView = STATIC_DOWNCAST(CNetPlotView, pChild->GetActiveView());
	if (IDC_RANGEPPM==pDoc->Range)
		m_strStatic = "Offset in PPM:";
	else if (IDC_RANGEHERTZ==pDoc->Range)
		m_strStatic = "Offset in Hertz:";
	else
		m_strStatic = "UNKNOWN OFFSET UNITS!";

	m_offsetEdit.SetSel(0,-1);
	m_offsetEdit.SetFocus();
	return FALSE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

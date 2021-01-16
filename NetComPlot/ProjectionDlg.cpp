// ProjectionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NetComPlot.h"
#include "ProjectionDlg.h"
#include ".\projectiondlg.h"

// CProjectionDlg dialog

IMPLEMENT_DYNAMIC(CProjectionDlg, CDialog)
CProjectionDlg::CProjectionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProjectionDlg::IDD, pParent)
	, m_bcolmaxCheck(FALSE)
	, m_bcolminCheck(FALSE)
	, m_browmaxCheck(FALSE)
	, m_browminCheck(FALSE)
{
}

CProjectionDlg::~CProjectionDlg()
{
}

void CProjectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_COLMAXCHECK, m_bcolmaxCheck);
	DDX_Check(pDX, IDC_COLMINCHECK, m_bcolminCheck);
	DDX_Check(pDX, IDC_ROWMAXCHECK, m_browmaxCheck);
	DDX_Check(pDX, IDC_ROWMINCHECK, m_browminCheck);
}


BEGIN_MESSAGE_MAP(CProjectionDlg, CDialog)
END_MESSAGE_MAP()


// CProjectionDlg message handlers

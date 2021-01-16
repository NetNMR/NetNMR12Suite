// EditInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NetComPlot.h"
#include "EditInfoDlg.h"


// CEditInfoDlg dialog

IMPLEMENT_DYNAMIC(CEditInfoDlg, CDialog)
CEditInfoDlg::CEditInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditInfoDlg::IDD, pParent)
	, m_strInfo(_T(""))
{
}

CEditInfoDlg::~CEditInfoDlg()
{
}

void CEditInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_INFOEDIT, m_strInfo);
}


BEGIN_MESSAGE_MAP(CEditInfoDlg, CDialog)
END_MESSAGE_MAP()


// CEditInfoDlg message handlers

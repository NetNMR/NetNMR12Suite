// EditorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ConnectMgr.h"
#include "EditorDlg.h"


// CEditorDlg dialog

IMPLEMENT_DYNAMIC(CEditorDlg, CDialog)
CEditorDlg::CEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditorDlg::IDD, pParent)
	, m_userStr(_T(""))
	, m_homeStr(_T(""))
	, m_pw1Str(_T(""))
	, m_pw2Str(_T(""))
	, m_staticServerName(_T(""))
{
}

CEditorDlg::~CEditorDlg()
{
}

void CEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_USEREDIT, m_userStr);
	DDX_Text(pDX, IDC_HOMEEDIT, m_homeStr);
	DDX_Text(pDX, IDC_PW1EDIT, m_pw1Str);
	DDX_Text(pDX, IDC_PW2EDIT, m_pw2Str);
	DDX_Text(pDX, IDC_IPNAMESTATIC, m_staticServerName);
}


BEGIN_MESSAGE_MAP(CEditorDlg, CDialog)
END_MESSAGE_MAP()


// CEditorDlg message handlers

void CEditorDlg::OnOK()
{
	UpdateData(TRUE);
	m_userStr = m_userStr.Trim();
	m_homeStr = m_homeStr.Trim();
	m_pw1Str = m_pw1Str.Trim();
	m_pw2Str = m_pw2Str.Trim();
	UpdateData(FALSE);
	if (m_userStr.IsEmpty()) {
		AfxMessageBox("User name is empty!", MB_ICONERROR);
		return;
	}
	if (m_homeStr.GetLength() < 3) {
		AfxMessageBox("Home directory path too short!", MB_ICONERROR);
		return;
	}
	if (1 == m_homeStr.Find(":\\")) {
		if (m_homeStr.Right(1).Compare("\\")) {
			AfxMessageBox("Windows path must end with '\\'!", MB_ICONERROR);
		}
	}else {
		if (m_homeStr.Left(1).Compare("/") || m_homeStr.Right(1).Compare("/")) {
			AfxMessageBox("UNIX home directory must begin and end with '/'!", MB_ICONERROR);
			return;
		}
		if (-1 < m_homeStr.FindOneOf(" \t")) {
			AfxMessageBox("UNIX home directory cannot contain spaces!", MB_ICONERROR);
			return;
		}
	}
	if (m_pw1Str.Compare(m_pw2Str)) {
		AfxMessageBox("Passwords do not match!", MB_ICONERROR);
		return;
	}
	CDialog::OnOK();
}

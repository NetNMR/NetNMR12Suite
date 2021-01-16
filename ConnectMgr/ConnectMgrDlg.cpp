// ConnectMgrDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ConnectMgr.h"
#include "ConnectMgrDlg.h"
#include "EditorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	DWORD dword;
	CString name = AfxGetAppName();
	CString exename = name + ".exe";
	LPTSTR pname = exename.GetBuffer();
	DWORD size = GetFileVersionInfoSize(pname, &dword);
	PBYTE lpdata = new BYTE[size];
	if (lpdata) {
		if (GetFileVersionInfo(pname, 0, size, lpdata)) {
			VS_FIXEDFILEINFO *pVer;
			UINT len;
			if (VerQueryValue(lpdata, "\\", (LPVOID *)&pVer, &len)) {
				CString x;
				x.Format("%s Version %d.%d.%d", name,
					HIWORD(pVer->dwFileVersionMS), 
					LOWORD(pVer->dwFileVersionMS),
					HIWORD(pVer->dwFileVersionLS));
				GetDlgItem(IDC_VERSIONSTATIC)->SetWindowText(x);
			}
		}
		delete[] lpdata;
	}
	exename.ReleaseBuffer();

	return TRUE;
}

// CConnectMgrDlg dialog




CConnectMgrDlg::CConnectMgrDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConnectMgrDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	Server.Initialize();
}

void CConnectMgrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SERVERCOMBO, m_serverCBox);
}

BEGIN_MESSAGE_MAP(CConnectMgrDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_EDITBUTTON, OnBnClickedEditbutton)
END_MESSAGE_MAP()


// CConnectMgrDlg message handlers

BOOL CConnectMgrDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	for (int i=0; strcmp(Server.Name(i), NMRSERVERNOTFOUND); i++) {
		m_serverCBox.SetItemData(m_serverCBox.AddString(Server.Name(i)), i);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CConnectMgrDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CConnectMgrDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CConnectMgrDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CConnectMgrDlg::OnBnClickedEditbutton()
{
	CEditorDlg dlg;
	int i = m_serverCBox.GetCurSel();
	int nserv = (int)m_serverCBox.GetItemData(i);
	m_serverCBox.GetLBText(i, dlg.m_staticServerName);
	dlg.m_userStr = Server.User(nserv);
	dlg.m_homeStr = Server.Home(nserv);
	if (strlen(Server.Pass(nserv))) {
		dlg.m_pw2Str = dlg.m_pw1Str = Server.Pass(nserv);
	} else {
		dlg.m_pw1Str.Empty();
		dlg.m_pw2Str.Empty();
	}
	if (IDOK == dlg.DoModal()) {
		HKEY serverkey;
		CString server;
		server.Format("%s\\%s", NMRSERVERREGKEY, Server.Name(nserv));
		if (ERROR_SUCCESS == RegOpenKey(HKEY_CURRENT_USER, server, &serverkey)) {
			if (dlg.m_userStr.Compare(Server.User(nserv))) {
				LPSTR buf = dlg.m_userStr.GetBuffer();
				if (ERROR_SUCCESS == RegSetValueEx(serverkey, "user", NULL, REG_SZ, (LPBYTE)buf, (DWORD)(1+strlen(buf))))
					Server.SetUser(nserv, dlg.m_userStr);
				else
					AfxMessageBox("Could not set user!", MB_ICONERROR);
				dlg.m_userStr.ReleaseBuffer();
			}
			if (dlg.m_homeStr.Compare(Server.Home(nserv))) {
				LPSTR buf = dlg.m_homeStr.GetBuffer();
				if (ERROR_SUCCESS == RegSetValueEx(serverkey, "home", NULL, REG_SZ, (LPBYTE)buf, (DWORD)(1+strlen(buf))))
					Server.SetHome(nserv, dlg.m_homeStr);
				else
					AfxMessageBox("Could not set home!", MB_ICONERROR);
				dlg.m_homeStr.ReleaseBuffer();
			}
			if (dlg.m_pw1Str.IsEmpty()) {
				if (strlen(Server.Pass(nserv))) {
					if (ERROR_SUCCESS == RegDeleteValue(serverkey, "password"))
						Server.SetPass(nserv, "");
					else
						AfxMessageBox("Could not delete the password!", MB_ICONERROR);
				}
			} else {
				if (dlg.m_pw1Str.Compare(Server.Pass(nserv))) {
					CString pw = dlg.m_pw1Str;
					Server.EnDecrypt(pw);
					LPSTR buf = pw.GetBuffer();
					if (ERROR_SUCCESS == RegSetValueEx(serverkey, "password", NULL, REG_SZ, (LPBYTE)buf, (DWORD)(1+strlen(buf))))
						Server.SetPass(nserv, dlg.m_pw1Str);
					else
						AfxMessageBox("Could not set the password!", MB_ICONERROR);
					pw.ReleaseBuffer();
				}
			}
			RegCloseKey(serverkey);
		} else {
			AfxMessageBox("Could not open server key!", MB_ICONERROR);
		}
	}
}

BOOL CConnectMgrDlg::DestroyWindow()
{
	// TODO: Add your specialized code here and/or call the base class
	return CDialog::DestroyWindow();
}

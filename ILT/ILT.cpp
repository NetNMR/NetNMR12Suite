// ILT.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ILT.h"
#include "MainFrm.h"

#include "ILTDoc.h"
#include "ILTView.h"
#include "ColorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CILTApp

BEGIN_MESSAGE_MAP(CILTApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CILTApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	//ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)
	ON_COMMAND(ID_FILE_OPEN, &CILTApp::OnFileOpen)
	ON_COMMAND(ID_EDIT_COLORS, &CILTApp::OnEditColors)
END_MESSAGE_MAP()


// CILTApp construction

CILTApp::CILTApp()
: pDecay(NULL)
{
	EnableHtmlHelp();
}


// The one and only CILTApp object

CILTApp theApp;


// CILTApp initialization

BOOL CILTApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// REQUIRED FOR RICH EDIT
	AfxInitRichEdit();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("NMR Lab"));

	ReadRegistryColors();
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CILTDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CILTView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);


	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();
	return TRUE;
}



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

// App command to run the dialog
void CILTApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CILTApp message handlers

void CILTApp::SetPaneText(int pane, LPCTSTR text)
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, m_pMainWnd);
	pMain->SetText(pane, text);
}

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

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

int CILTApp::ExitInstance()
{

	return CWinApp::ExitInstance();
}

CDocument* CILTApp::OpenDocumentFile(LPCTSTR lpszFileName)
{
	CString filename = lpszFileName;
	if (0 == filename.Right(4).CompareNoCase(".ilt"))
		return CWinApp::OpenDocumentFile(filename);
	else {
		CWinApp::OnFileNew();
		CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, m_pMainWnd);
		CILTDoc *pDoc = STATIC_DOWNCAST(CILTDoc, pMain->GetActiveDocument());
		//pDoc->OnNewDocument();
		pDoc->OpenTextILTFile(filename);
		return pDoc;
	}
}

void CILTApp::OnFileOpen()
{
	CFileDialog dlg(TRUE, "ilt", NULL, OFN_FILEMUSTEXIST|OFN_HIDEREADONLY, "ILT Files (*.ilt)|*.ilt|Text Files (*.txt)|*.txt||");
	if (IDOK == dlg.DoModal()) {
		CILTApp::OpenDocumentFile(dlg.GetPathName());
	}
}

void CILTApp::SetDefaultColors(COLORREF *colors)
{
	colors[0]   = RGB(255, 255, 255);	//   0 Data
	colors[1]   = RGB(  0, 255,   0);	//   1 Calc
	colors[2]   = RGB(255,   0,   0);	//   2 Diff
	colors[3]   = RGB(255,   0, 255);	//   3 Spec
	colors[4]   = RGB(127, 127, 127);	//   4 Error
	colors[5]   = RGB(  0, 255, 255);	//   5 Text
	colors[6]   = RGB(192, 192, 192);	//   6 Axis
	colors[7]   = RGB(  0,   0,   0);	//   7 Bkgrnd
	colors[8]   = RGB(255,   0,   0);	//   8 MC1
	colors[9]   = RGB(  0, 255,   0);	//   9 MC2
	colors[10]  = RGB(255, 255, 255);	//  10 MC3
	colors[11]  = RGB(  0,   0, 255);	//  11 MC4
	colors[12]  = RGB(255, 255,   0);	//  12 MC5
	colors[13]  = RGB(255,   0, 255);	//  13 MC6
	colors[14]  = RGB(  0, 255, 255);	//  14 MC7
	colors[15]  = RGB(127,   0,   0);	//  15 MC8
	colors[16]  = RGB(  0, 127,   0);	//  16 MC9
	colors[17]  = RGB(  0,   0, 127);	//  17 MC10+
}

void CILTApp::ReadRegistryColors(void)
{
	COLORREF *pColor = NULL;
	UINT size;
	if (GetProfileBinary("Settings", "Colors", (LPBYTE *)&pColor, &size)) {
		if (NCOLORS*sizeof(COLORREF)==size) {
			memcpy(Colors, pColor, size);
		} else {
			delete[] pColor;
			pColor = NULL;
		}
	}
	if (NULL == pColor) {
		SetDefaultColors(Colors);
	} else
		delete[] pColor;
}

void CILTApp::OnEditColors()
{
	CColorDlg dlg;
	if (IDOK == dlg.DoModal()) {
		WriteProfileBinary("Settings", "Colors", (LPBYTE)Colors, NCOLORS*sizeof(COLORREF));
	}
}

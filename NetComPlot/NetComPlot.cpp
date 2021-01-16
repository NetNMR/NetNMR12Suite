// NetComPlot.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "NetComPlot.h"
#include "MainFrame.h"
#include "SpecDim.h"
#include "OneDSpec.h"
#include "TwoDSpec.h"
#include "NetComPlotDoc.h"
#include "NetComPlotView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CNetComPlotApp

BEGIN_MESSAGE_MAP(CNetComPlotApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CNetComPlotApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


// CNetComPlotApp construction

CNetComPlotApp::CNetComPlotApp()
: nserv(-1)
{
	EnableHtmlHelp();

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CNetComPlotApp object

CNetComPlotApp theApp;


// CNetComPlotApp initialization

BOOL CNetComPlotApp::InitInstance()
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

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

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
	SetRegistryKey("NMR Lab");
	Server.Initialize();
	DataClass.Initialize();
	InitColors();
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CNetComPlotDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CNetComPlotView));
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
void CNetComPlotApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CNetComPlotApp message handlers

void CNetComPlotApp::GetPageSize(short dmPageSize)
{
	switch (dmPageSize) {
		default:
			AfxMessageBox("The paper format you have selected is not supported.", MB_OK|MB_ICONEXCLAMATION);
		case DMPAPER_NOTE:
		case DMPAPER_LETTERSMALL:
		case DMPAPER_LETTER:
			csPageSize = CSize(21590, 27940);
			break;
		case DMPAPER_LEGAL:
			csPageSize = CSize(21590, 35560);
			break;
		case DMPAPER_10X14:
			csPageSize = CSize(25400, 35560);
			break;
		case DMPAPER_11X17:
			csPageSize = CSize(27940, 43180);
			break;
		case DMPAPER_A3:
			csPageSize = CSize(29700, 42000);
			break;
		case DMPAPER_A4:
			csPageSize = CSize(21000, 29700);
			break;
		case DMPAPER_A5:
			csPageSize = CSize(14800, 21000);
			break;
		case DMPAPER_B5:
			csPageSize = CSize(18200, 25700);
			break;
		case DMPAPER_EXECUTIVE:
			csPageSize = CSize(18415, 26416);
			break;
	}
}

int CNetComPlotApp::ExitInstance()
{
	delete[] arrColors;
	return CWinApp::ExitInstance();
}

void CNetComPlotApp::InitColors(void)
{
	nColors = 9;
	arrColors = new COLOR[nColors];
	arrColors[0].name = "Black"; arrColors[0].value = RGB(0, 0, 0);
	arrColors[1].name = "Red"; arrColors[1].value = RGB(255, 0, 0);
	arrColors[2].name = "Green"; arrColors[2].value = RGB(0, 255, 0);
	arrColors[3].name = "Blue"; arrColors[3].value = RGB(0, 0, 255);
	arrColors[4].name = "Yellow"; arrColors[4].value = RGB(255, 255, 0);
	arrColors[5].name = "Purple"; arrColors[5].value = RGB(255, 0, 255);
	arrColors[6].name = "Cyan"; arrColors[6].value = RGB(0, 255, 255);
	arrColors[7].name = "Gray"; arrColors[7].value = RGB(92, 92, 92);
	arrColors[8].name = "White"; arrColors[8].value = RGB(255, 255, 255);
}

void CNetComPlotApp::OnFilePrintSetup()
{
	CWinApp::OnFilePrintSetup();
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, m_pMainWnd);
	CNetComPlotView *pView = STATIC_DOWNCAST(CNetComPlotView, pMain->GetActiveView());
	pView->Invalidate();
}

void CNetComPlotApp::Message(LPCTSTR str)
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, m_pMainWnd);
	pMain->Message(str);
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
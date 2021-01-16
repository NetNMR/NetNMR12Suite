// NetSpan.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "NetSpan.h"
#include "MainFrame.h"

#include "NetSpanDoc.h"
#include "NetSpanView.h"
#include "ColorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CNetSpanApp

BEGIN_MESSAGE_MAP(CNetSpanApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CNetSpanApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CNetSpanApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CNetSpanApp::OnFilePrintSetup)
	ON_COMMAND(ID_EDIT_COLORS, &CNetSpanApp::OnEditColors)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT, &CNetSpanApp::OnUpdateFilePrint)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, &CNetSpanApp::OnUpdateFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, &CNetSpanApp::OnUpdateFilePrint)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, &CNetSpanApp::OnUpdateFilePrint)
END_MESSAGE_MAP()


// CNetSpanApp construction

CNetSpanApp::CNetSpanApp()
: nColors(0)
, arrColors(NULL)
, lineindex(0)
, nserv(-1)
, pCurrentLine(NULL)
{
	EnableHtmlHelp();

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CNetSpanApp object

CNetSpanApp theApp;


// CNetSpanApp initialization

BOOL CNetSpanApp::InitInstance()
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

	// REQUIRED FOR RICH EDIT IN CFitTextDlg
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
	SetRegistryKey("NMR Lab");
	InitColors();
	Server.Initialize();
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CNetSpanDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CNetSpanView));
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
void CNetSpanApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CNetSpanApp message handlers

void CNetSpanApp::InitColors(void)
{
	nColors = 9;
	arrColors = new COLOR[nColors];
	arrColors[0].name = "Black";  arrColors[0].value = RGB(  0,   0,   0);
	arrColors[1].name = "Red";    arrColors[1].value = RGB(255,   0,   0);
	arrColors[2].name = "Green";  arrColors[2].value = RGB(  0, 255,   0);
	arrColors[3].name = "Blue";   arrColors[3].value = RGB(  0,   0, 255);
	arrColors[4].name = "Yellow"; arrColors[4].value = RGB(255, 255,   0);
	arrColors[5].name = "Purple"; arrColors[5].value = RGB(255,   0, 255);
	arrColors[6].name = "Cyan";   arrColors[6].value = RGB(  0, 255, 255);
	arrColors[7].name = "Gray";   arrColors[7].value = RGB( 92,  92,  92);
	arrColors[8].name = "White";  arrColors[8].value = RGB(255, 255, 255);
}

int CNetSpanApp::RegistryInt(RegistryAction action, RegistryVariable name, int value)
{
	CString var;
	switch (name) {
		case BkgrColor:
			var = "BackgroundColor";
			break;
		case SpecColor:
			var = "SpectraColor";
			break;
		case CalcColor:
			var = "CalculatedColor";
			break;
		case SingColor:
			var = "SingleLineColor";
			break;
		case DiffColor:
			var = "DifferenceColor";
			break;
		case LablColor:
			var = "LabelColor";
			break;
		case CrsrColor:
			var = "CursorColor";
			break;
		case ZoomColor:
			var = "ZoomCursorColor";
			break;
	}
	if (! var.IsEmpty()) {
		CString section = "Settings";
		if (action == ReadInt)
			return GetProfileInt(section, var, value);
		else
			WriteProfileInt(section, var, value);
	}
	return value;
}

int CNetSpanApp::ExitInstance()
{
	delete [] arrColors;
	return CWinApp::ExitInstance();
}

void CNetSpanApp::OnFilePrintSetup()
{
	CWinApp::OnFilePrintSetup();
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, m_pMainWnd);
	CNetSpanView *pView = STATIC_DOWNCAST(CNetSpanView, pMain->GetActiveView());
	pView->Invalidate();
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

void CNetSpanApp::OnFileOpen()
{
	CFileDialog dlg(TRUE, "nms", NULL, OFN_FILEMUSTEXIST|OFN_HIDEREADONLY, "Span Files (*.nms)|*.nms|NetPlot Files (*.npl)|*.npl|Text Files (*.txt)|*.txt||");
	if (IDOK == dlg.DoModal()) {
		CString ext = dlg.GetFileName().Right(4);
		if (0 == ext.CompareNoCase(".nms"))
			CWinApp::OpenDocumentFile(dlg.GetPathName());
		else {
			CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, m_pMainWnd);
			CNetSpanDoc *pDoc = STATIC_DOWNCAST(CNetSpanDoc, pMain->GetActiveDocument());
			pDoc->OnNewDocument();
			if (0 == ext.CompareNoCase(".npl"))
				pDoc->OpenNetPlotFile(dlg.GetPathName());
			else
				pDoc->OpenTextSpanFile(dlg.GetPathName());
			CString name = dlg.GetPathName();
			name = name.Left(name.GetLength() - 4);
			pDoc->SetPathName(name, FALSE);
		}
	}
}

void CNetSpanApp::SetPaneText(int pane, LPCTSTR text)
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, m_pMainWnd);
	pMain->SetText(pane, text);
}

void CNetSpanApp::OnEditColors()
{
	CColorDlg dlg;
	if (dlg.DoModal())
		m_pMainWnd->Invalidate();
}
void CNetSpanApp::OnUpdateFilePrint(CCmdUI *pCmdUI)
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, m_pMainWnd);
	CNetSpanDoc *pDoc = STATIC_DOWNCAST(CNetSpanDoc, pMain->GetActiveDocument());
	pCmdUI->Enable(pDoc->Spectra.GetCount() > 0);
}

void CNetSpanApp::OnUpdateFileSave(CCmdUI *pCmdUI)
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, m_pMainWnd);
	CNetSpanDoc *pDoc = STATIC_DOWNCAST(CNetSpanDoc, pMain->GetActiveDocument());
	pCmdUI->Enable(pDoc->Spectra.GetCount()>0 && pDoc->IsModified());
}

// NetPlot.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "..\NMRDataTypes.h"
#include "Spectrum.h"
#include "NetPlot.h"
#include "NMRServer.h"
#include "MainFrame.h"
#include "ChildFrame.h"
#include "NetPlotDoc.h"
#include "NetPlotView.h"
#include "ColorsDlg.h"
#include "LabelsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CNetPlotApp

BEGIN_MESSAGE_MAP(CNetPlotApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CNetPlotApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)
	ON_COMMAND(ID_EDIT_PARAMETERS, &CNetPlotApp::OnEditParameters)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PARAMETERS, &CNetPlotApp::OnUpdateEditParameters)
	ON_COMMAND(ID_EDIT_COLORS, &CNetPlotApp::OnEditColors)
	ON_COMMAND(ID_EDIT_LABELS, &CNetPlotApp::OnEditLabels)
	ON_UPDATE_COMMAND_UI(ID_EDIT_LABELS, &CNetPlotApp::OnUpdateEditLabels)
	ON_COMMAND(ID_FILE_ADD, &CNetPlotApp::OnFileAdd)
	ON_UPDATE_COMMAND_UI(ID_FILE_ADD, &CNetPlotApp::OnUpdateFileAdd)
	ON_COMMAND(ID_EDIT_COPY, &CNetPlotApp::OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &CNetPlotApp::OnUpdateEditCopy)
	ON_COMMAND(ID_VIEW_LANDSCAPE, &CNetPlotApp::OnViewLandscape)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LANDSCAPE, &CNetPlotApp::OnUpdateViewLandscape)
	ON_COMMAND(ID_VIEW_PORT, &CNetPlotApp::OnViewPort)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PORT, &CNetPlotApp::OnUpdateViewPort)
	ON_COMMAND(ID_FILE_MYSAVEAS, &CNetPlotApp::OnFileMysaveas)
	ON_UPDATE_COMMAND_UI(ID_FILE_MYSAVEAS, &CNetPlotApp::OnUpdateFileMysaveas)
END_MESSAGE_MAP()


// CNetPlotApp construction

CNetPlotApp::CNetPlotApp()
: nColors(0)
, arrColors(NULL)
, gphA(0.0f)
, gphB(0.0f)
{
	EnableHtmlHelp();

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CNetPlotApp object

CNetPlotApp theApp;

// CNetPlotApp initialization

BOOL CNetPlotApp::InitInstance()
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
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_NetPlotTYPE,
		RUNTIME_CLASS(CNetPlotDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CNetPlotView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;
	// call DragAcceptFiles only if there's a suffix
	//  In an MDI app, this should occur immediately after setting m_pMainWnd
	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

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
	// The main window has been initialized, so show and update it
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

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
void CNetPlotApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CNetPlotApp message handlers

void CNetPlotApp::InitColors(void)
{
	nColors = 14;
	arrColors = new COLOR[nColors];
	arrColors[ 0].name = "Black";  arrColors[ 0].value = RGB(  0,   0,   0);
	arrColors[ 1].name = "Red";    arrColors[ 1].value = RGB(255,   0,   0);
	arrColors[ 2].name = "Green";  arrColors[ 2].value = RGB(  0, 255,   0);
	arrColors[ 3].name = "Blue";   arrColors[ 3].value = RGB(  0,   0, 255);
	arrColors[ 4].name = "Yellow"; arrColors[ 4].value = RGB(255, 255,   0);
	arrColors[ 5].name = "Purple"; arrColors[ 5].value = RGB(255,   0, 255);
	arrColors[ 6].name = "Cyan";   arrColors[ 6].value = RGB(  0, 255, 255);
	arrColors[ 7].name = "Gray";   arrColors[ 7].value = RGB( 92,  92,  92);
	arrColors[ 8].name = "White";  arrColors[ 8].value = RGB(255, 255, 255);
	arrColors[ 9].name = "DrkRed"; arrColors[ 9].value = RGB(127,   0,   0);
	arrColors[10].name = "DrkGrn"; arrColors[10].value = RGB(  0, 127,   0);
	arrColors[11].name = "DrkBlu"; arrColors[11].value = RGB(  0,   0, 127);
	arrColors[12].name = "LtGray"; arrColors[12].value = RGB(127, 127, 127);
	arrColors[13].name = "DrkGry"; arrColors[13].value = RGB( 45,  45,  45);
}

int CNetPlotApp::ExitInstance()
{
	if (arrColors)
		delete[] arrColors;
	if (spare.size())
		spare.resize(0);
	return CWinApp::ExitInstance();
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

int CNetPlotApp::RegistryInt(RegistryAction action, RegistryVariable name, int value)
{
	CString var;
	switch (name) {
		case BaselineColor:
			var = "BaselineColor";
			break;
		case BackgroundColor:
			var = "BackgroundColor";
			break;
		case AxisColor:
			var = "AxisColor";
			break;
		case IntegralColor:
			var = "IntegralColor";
			break;
		case SpectraColor:
			var = "SpectraColor";
			break;
		case TitleColor:
			var = "TitleColor";
			break;
		case LabelsColor:
			var = "LabelsColor";
			break;
		case MainCursorColor:
			var = "MainCursorColor";
			break;
		case ZoomCursorColor:
			var = "ZoomCursorColor";
			break;
		case TitleFontSize:
			var = "TitleFontSize";
			break;
		case LabelFontSize:
			var = "LabelFontSize";
			break;
		case AxisFontSize:
			var = "AxisFontSize";
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

void CNetPlotApp::OnFileAdd()
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->MDIGetActive());
	CNetPlotDoc *pDoc = STATIC_DOWNCAST(CNetPlotDoc, pChild->GetActiveDocument());
	pDoc->AddFile();
}

void CNetPlotApp::OnUpdateFileAdd(CCmdUI *pCmdUI)
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if (pMain) {
		CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->MDIGetActive());
		if (pChild) {
			CNetPlotView *pView = STATIC_DOWNCAST(CNetPlotView, pChild->GetActiveView());
			pCmdUI->Enable(normalMode == pView->disMode);
		} else
			pCmdUI->Enable(FALSE);
		return;
	}
	pCmdUI->Enable(FALSE);
}

void CNetPlotApp::OnEditParameters()
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->MDIGetActive());
	CNetPlotDoc *pDoc = STATIC_DOWNCAST(CNetPlotDoc, pChild->GetActiveDocument());
	pDoc->EditParameters();
}

void CNetPlotApp::OnUpdateEditParameters(CCmdUI *pCmdUI)
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if (pMain) {
		CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->MDIGetActive());
		if (pChild) {
			CNetPlotDoc *pDoc = STATIC_DOWNCAST(CNetPlotDoc, pChild->GetActiveDocument());
			pCmdUI->Enable(pDoc->Spectra.GetCount() > 0);
			return;
		}
	}
	pCmdUI->Enable(FALSE);
}

void CNetPlotApp::OnEditColors()
{
	CColorsDlg dlg;
	if (IDOK == dlg.DoModal()) {
		CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
		CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->MDIGetActive());
		if (pChild) {
			CNetPlotDoc *pDoc = STATIC_DOWNCAST(CNetPlotDoc, pChild->GetActiveDocument());
			pDoc->UpdateAllViews(NULL);
		}
	}
}

void CNetPlotApp::OnEditLabels()
{
	CLabelsDlg dlg;
	if (IDOK == dlg.DoModal()) {
		CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
		CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->MDIGetActive());
		CNetPlotDoc *pDoc = STATIC_DOWNCAST(CNetPlotDoc, pChild->GetActiveDocument());
		pDoc->UpdateAllViews(NULL);
		theApp.RegistryInt(WriteInt, LabelFontSize, pDoc->nLabelSize);
		pDoc->SetModifiedFlag();
	}
}

void CNetPlotApp::OnUpdateEditLabels(CCmdUI *pCmdUI)
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if (pMain) {
		CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->MDIGetActive());
		if (pChild) {
			CNetPlotDoc *pDoc = STATIC_DOWNCAST(CNetPlotDoc, pChild->GetActiveDocument());
			pCmdUI->Enable(pDoc->Label.GetLength() > 0);
			return;
		}
	}
	pCmdUI->Enable(FALSE);
}

void CNetPlotApp::OnEditCopy()
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->MDIGetActive());
	CNetPlotView *pView = STATIC_DOWNCAST(CNetPlotView, pChild->GetActiveView());
	pView->CopytoClipboard();
}

void CNetPlotApp::OnUpdateEditCopy(CCmdUI *pCmdUI)
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if (pMain) {
		CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->MDIGetActive());
		if (pChild) {
			CNetPlotDoc *pDoc = STATIC_DOWNCAST(CNetPlotDoc, pChild->GetActiveDocument());
			pCmdUI->Enable(pDoc->Spectra.GetCount() > 0);
			return;
		}
	}
	pCmdUI->Enable(FALSE);
}

void CNetPlotApp::OnViewLandscape()
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->MDIGetActive());
	CNetPlotDoc *pDoc = STATIC_DOWNCAST(CNetPlotDoc, pChild->GetActiveDocument());
	pDoc->IsLandscape = true;
	pDoc->UpdateAllViews(NULL);
	pDoc->SetModifiedFlag();
}

void CNetPlotApp::OnUpdateViewLandscape(CCmdUI *pCmdUI)
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if (pMain) {
		CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->MDIGetActive());
		if (pChild) {
			CNetPlotDoc *pDoc = STATIC_DOWNCAST(CNetPlotDoc, pChild->GetActiveDocument());
			pCmdUI->SetCheck(pDoc->IsLandscape ? 1 : 0);
			return;
		}
	}
	pCmdUI->SetCheck(0);
}

void CNetPlotApp::OnViewPort()
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->MDIGetActive());
	CNetPlotDoc *pDoc = STATIC_DOWNCAST(CNetPlotDoc, pChild->GetActiveDocument());
	pDoc->IsLandscape = false;
	pDoc->UpdateAllViews(NULL);
	pDoc->SetModifiedFlag();
}

void CNetPlotApp::OnUpdateViewPort(CCmdUI *pCmdUI)
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if (pMain) {
		CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->MDIGetActive());
		if (pChild) {
			CNetPlotDoc *pDoc = STATIC_DOWNCAST(CNetPlotDoc, pChild->GetActiveDocument());
			pCmdUI->SetCheck(pDoc->IsLandscape ? 0 : 1);
			return;
		}
	}
	pCmdUI->SetCheck(0);
}

void CNetPlotApp::OnFileMysaveas()
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, m_pMainWnd);
	CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->MDIGetActive());
	CNetPlotDoc *pDoc = STATIC_DOWNCAST(CNetPlotDoc, pChild->GetActiveDocument());
	pDoc->FileSaveAs();
}

void CNetPlotApp::OnUpdateFileMysaveas(CCmdUI *pCmdUI)
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if (pMain) {
		CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->MDIGetActive());
		if (pChild) {
			CNetPlotDoc *pDoc = STATIC_DOWNCAST(CNetPlotDoc, pChild->GetActiveDocument());
			if (pDoc->Spectra.GetCount() > 0) {
				pCmdUI->Enable(TRUE);
				return;
			}
		}
	}
	pCmdUI->Enable(FALSE);
}



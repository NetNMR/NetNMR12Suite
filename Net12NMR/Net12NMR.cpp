// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://msdn.microsoft.com/officeui.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// Net12NMR.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "Net12NMR.h"
#include "MainFrame.h"

#include "ChildFrame.h"
#include "Net12NMRDoc.h"
#include "Net12NMRView.h"
#include "ServerSocket.h"
#include "data.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CNet12NMRApp

BEGIN_MESSAGE_MAP(CNet12NMRApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CNet12NMRApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
	ON_UPDATE_COMMAND_UI(ID_FILEINFOCHECK, &CNet12NMRApp::OnUpdateFileinfocheck)
	ON_COMMAND(ID_FILEINFOCHECK, &CNet12NMRApp::OnFileinfocheck)
	ON_UPDATE_COMMAND_UI(ID_VIEWPARAMETERSCHECK, &CNet12NMRApp::OnUpdateViewparameterscheck)
	ON_COMMAND(ID_VIEWPARAMETERSCHECK, &CNet12NMRApp::OnViewparameterscheck)
	ON_UPDATE_COMMAND_UI(ID_BASESHELFCOUNTEDIT, &CNet12NMRApp::OnUpdateBaseshelfcountedit)
	ON_UPDATE_COMMAND_UI(ID_PEAKCOUNTSHELFEDIT, &CNet12NMRApp::OnUpdatePeakcountshelfedit)
	ON_UPDATE_COMMAND_UI(ID_INTEGRALSHELFCOUNTEDIT, &CNet12NMRApp::OnUpdateIntegralshelfcountedit)
	ON_COMMAND(ID_LOADSHELFBUTTON, &CNet12NMRApp::OnLoadshelfbutton)
	ON_UPDATE_COMMAND_UI(ID_SAVESHELFBUTTON, &CNet12NMRApp::OnUpdateSaveshelfbutton)
	ON_COMMAND(ID_SAVESHELFBUTTON, &CNet12NMRApp::OnSaveshelfbutton)
	ON_COMMAND(ID_ABOUTBUTTON, &CNet12NMRApp::OnAboutbutton)
	ON_COMMAND(ID_APOTYPECOMBO, &CNet12NMRApp::OnApotypecombo)
	ON_COMMAND(ID_APOUNITSCOMBO, &CNet12NMRApp::OnApounitscombo)
	ON_COMMAND(ID_APOVALUECOMBO, &CNet12NMRApp::OnApovaluecombo)
	ON_UPDATE_COMMAND_UI(ID_TENCHECK, &CNet12NMRApp::OnUpdateTencheck)
	ON_COMMAND(ID_TENCHECK, &CNet12NMRApp::OnTencheck)
	ON_UPDATE_COMMAND_UI(ID_ONECHECK, &CNet12NMRApp::OnUpdateOnecheck)
	ON_COMMAND(ID_ONECHECK, &CNet12NMRApp::OnOnecheck)
	ON_UPDATE_COMMAND_UI(ID_TENTHCHECK, &CNet12NMRApp::OnUpdateTenthcheck)
	ON_COMMAND(ID_TENTHCHECK, &CNet12NMRApp::OnTenthcheck)
	ON_UPDATE_COMMAND_UI(ID_ACHECK, &CNet12NMRApp::OnUpdateAcheck)
	ON_COMMAND(ID_ACHECK, &CNet12NMRApp::OnAcheck)
	ON_UPDATE_COMMAND_UI(ID_BCHECK, &CNet12NMRApp::OnUpdateBcheck)
	ON_COMMAND(ID_BCHECK, &CNet12NMRApp::OnBcheck)
	ON_COMMAND(ID_FILE_OPEN, &CNet12NMRApp::OnFileOpen)
	ON_UPDATE_COMMAND_UI(ID_CLEARBASESHELFBUTTON, &CNet12NMRApp::OnUpdateClearbaseshelfbutton)
	ON_COMMAND(ID_CLEARBASESHELFBUTTON, &CNet12NMRApp::OnClearbaseshelfbutton)
	ON_UPDATE_COMMAND_UI(ID_CLEARPEAKSHELFBUTTON, &CNet12NMRApp::OnUpdateClearpeakshelfbutton)
	ON_COMMAND(ID_CLEARPEAKSHELFBUTTON, &CNet12NMRApp::OnClearpeakshelfbutton)
	ON_UPDATE_COMMAND_UI(ID_CLEARINTEGRALSHELFBUTTON, &CNet12NMRApp::OnUpdateClearintegralshelfbutton)
	ON_COMMAND(ID_CLEARINTEGRALSHELFBUTTON, &CNet12NMRApp::OnClearintegralshelfbutton)
END_MESSAGE_MAP()


// CNet12NMRApp construction

CNet12NMRApp::CNet12NMRApp()
{
	EnableHtmlHelp();

	m_bHiColorIcons = TRUE;
	settings = "Settings";

	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// If the application is built using Common Language Runtime support (/clr):
	//     1) This additional setting is needed for Restart Manager support to work properly.
	//     2) In your project, you must add a reference to System.Windows.Forms in order to build.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("GEResearch.Net12NMRSuite.Net12NMR.Version1"));

	// TODO: add construction code here,
	disMode = normalMode;
	apoType = lorenApoType;
	apoUnits = hertzApoUnits;
	apoValue = 0.25;
	phaseIncrement = onePhaseIncrement;
	phaseTerm = constPhaseTerm;
	polybaseline = true;
	GlobalPhaseA = GlobalPhaseB = 0.0f;
	GlobalIntegralConst = 1.0f;
	// Place all significant initialization in InitInstance
}

// The one and only CNet12NMRApp object

CNet12NMRApp theApp;


// CNet12NMRApp initialization

BOOL CNet12NMRApp::InitInstance()
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

	CWinAppEx::InitInstance();

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

	EnableTaskbarInteraction();

	// AfxInitRichEdit2() is required to use RichEdit control	
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("NMR Lab"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)
	CString entry;
	entry.LoadString(IDC_PARFITCHECK);
	useparabolicfit = GetProfileInt(settings, entry, 0) ? true : false;
	entry.LoadString(IDC_PEAKPPM);
	labelisppm = GetProfileInt(settings, entry, 0) ? true : false;
	entry.LoadString(IDC_DIGITSPPM);
	digitsPPM = GetProfileInt(settings, entry, 2);
	entry.LoadString(IDC_DIGITSHZ);
	digitsHz = GetProfileInt(settings, entry, 1);
	entry.LoadString(IDC_VIEWPARAMETERS);
	viewparameters = GetProfileInt(settings, entry, 1) ? true : false;
	entry.LoadString(IDC_VIEWFILEINFO);
	viewfileinfo = GetProfileInt(settings, entry, 1) ? true : false;
	servers.Initialize();
	DataClassification.Initialize();

	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	InitializeColors();
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_Net12NMRTYPE,
		RUNTIME_CLASS(CNet12NMRDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CNet12NMRView));
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

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);


	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (cmdInfo.m_nShellCommand != CCommandLineInfo::FileNew) {
		if (!ProcessShellCommand(cmdInfo))
			return FALSE;
	}
	// The main window has been initialized, so show and update it
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	// Set printer default to landscape
	PRINTDLG pd;
	pd.lStructSize = (DWORD)sizeof(PRINTDLG);
	if (GetPrinterDeviceDefaults(&pd)) {
		DEVMODE FAR *pDevMode = (DEVMODE FAR *)::GlobalLock(m_hDevMode);
		if (pDevMode) {
			pDevMode->dmOrientation = DMORIENT_LANDSCAPE;
			::GlobalUnlock(m_hDevMode);
		}
	}

	// find and set the version #
	CString name = this->m_pszExeName;
	CString exename = name + ".exe";
	DWORD dword;
	DWORD size = GetFileVersionInfoSize(exename, &dword);
	PBYTE lpdata = new BYTE[size];
	if (lpdata) {
		if (GetFileVersionInfo(exename, 0, size, lpdata)) {
			VS_FIXEDFILEINFO *pVer;
			UINT len;
			if (VerQueryValue(lpdata, "\\", (LPVOID *)&pVer, &len)) {
				AppVersion.Format("%s Version %d.%d.%d", name,
					HIWORD(pVer->dwFileVersionMS), 
					LOWORD(pVer->dwFileVersionMS),
					HIWORD(pVer->dwFileVersionLS));
			}
		}
		delete[] lpdata;
	}
	m_strAppPath = m_pszHelpFilePath;
	int i = m_strAppPath.ReverseFind('\\') + 1;
	m_strAppPath = m_strAppPath.Left(i);
	return TRUE;
}

int CNet12NMRApp::ExitInstance()
{
	shelf.RemoveAll();
	DeleteProcparMap();

	POSITION pos = VarianNucTable.GetStartPosition();
	while (pos) {
		CString key;
		double * p;
		VarianNucTable.GetNextAssoc(pos, key, p);
		delete p;
		VarianNucTable.RemoveKey(key);
	}

	AfxOleTerm(FALSE);

	return CWinAppEx::ExitInstance();
}

// CNet12NMRApp message handlers


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
//	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

// App command to run the dialog
void CNet12NMRApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CNet12NMRApp customization load/save methods

void CNet12NMRApp::PreLoadState()
{
	GetContextMenuManager()->AddMenu("Edit", IDR_POPUP_EDIT);
	GetContextMenuManager()->AddMenu("Baseline", IDR_BASELINEMENU);
	GetContextMenuManager()->AddMenu("Integral", IDR_INTEGRALMENU);
	GetContextMenuManager()->AddMenu("Peak", IDR_PEAKMENU);
	GetContextMenuManager()->AddMenu("Explorer", IDR_POPUP_EXPLORER);
}

void CNet12NMRApp::LoadCustomState()
{
}

void CNet12NMRApp::SaveCustomState()
{
}

// CNet12NMRApp message handlers

const CString CNet12NMRApp::GetAppPath()
{
	return m_strAppPath;
}


BOOL CNet12NMRApp::ReadAsciiFile(CString &str, UINT param)
{
	CFile file;
	if (file.Open(str, param)) {
		ULONGLONG len = file.GetLength();
		int read, l;
		if (len <= INT_MAX) {
			l = (int)len;
			TCHAR *buf = str.GetBuffer(l+1);
			read = file.Read(buf, l*sizeof(TCHAR));
			buf[read] = NULL;
			str.ReleaseBuffer();
		}
		file.Close();
		if (read == l)
			return TRUE;
	}
	return FALSE;
}

void CNet12NMRApp::MapJCAMPFile(CString &file)
{
	int prev = 0;
	int next = 0;
	m_paraMap.RemoveAll();
	while (next >= 0) {
		next = file.Find(_T("\n##"), prev);
		if (next >= 0) {
			CString val = file.Mid(prev, next-prev+1);
			prev = next+3;
			if (val.GetLength()) {
				if (val.GetAt(0)=='$') {
					val = val.Mid(1);
					int e = val.Find('=');
					if (e > 0) {
						CString key = val.Left(e++).Trim();
						val = val.Mid(e).Trim();
						if (0 == val.Find(_T("(0.."))) {
							e = val.Find(_T(")"));
							if (e > 4)
								val = val.Mid(++e).Trim();
							e = 0;
							CString ele = val.Tokenize(_T(" \t\n\r"), e);
							for (int i=0; ele.GetLength(); i++) {
								CString akey;
								akey.Format(_T("%s%d"), key, i);
								m_paraMap.SetAt(akey, ele.Trim(_T(" \t\r\n<>")));
								ele = val.Tokenize(_T(" \t\n\r"), e);
							}
						} else
							m_paraMap.SetAt(key, val.Trim(_T(" \t\r\n<>")));
					}
				}
			}
		}
	}
}

void CNet12NMRApp::MapProcparFile(CString &file)
{
	DeleteProcparMap();
	const CString sp = " \t\r\n";
	const CString nl = "\r\n";
	int count = 0;
	CString aline = file.Tokenize(nl, count).Trim();
	while (count > 0) {
		int subtype, basictype, ggroup, dgroup, protection, active, intptr;
		double maxval, minval, stepsize;
		intptr = 0;
		CString key = aline.Tokenize(sp, intptr);
		aline = aline.Mid(intptr).Trim();
		int i = sscanf_s(aline, "%i %i %lg %lg %lg %i %i %i %i %i", &subtype, &basictype, &maxval, &minval, &stepsize, &ggroup, &dgroup, &protection, &active, &intptr);
		if (i==10 && intptr==64) {
			CStringArray *pValues = new CStringArray;
			if (pValues) {
				m_procparMap.SetAt(key, pValues);
				int vcount = atoi(file.Tokenize(sp, count).Trim());
				pValues->SetSize(vcount);
				switch (basictype) {
				case 1:
					for (i=0; i<vcount; i++)
						pValues->SetAt(i, file.Tokenize(sp, count).Trim());
					break;
				case 2:
					for (i=0; i<vcount; i++) {
						CString pstr = file.Tokenize(nl, count);
						while (pstr.Left(1).Compare("\"")) {
							CString anotherline = file.Tokenize(sp, count).Trim();
							pstr += "\n" + anotherline;
						}
						pstr = pstr.Trim("\"");
						pValues->SetAt(i, pstr.Trim());
					}
					break;
				}
			}
		}
		aline = file.Tokenize(nl, count).Trim();
	}
}

void CNet12NMRApp::DeleteProcparMap()
{
	POSITION pos = m_procparMap.GetStartPosition();
	while (pos) {
		CString key;
		CStringArray *pValues;
		m_procparMap.GetNextAssoc(pos, key, pValues);
		pValues->RemoveAll();
		delete pValues;
		m_procparMap.RemoveKey(key);
	}
}

bool CNet12NMRApp::GetTmpSpectra(CString path, Leaf leaf, CTypedPtrList<CPtrList, TMPSPECTRUM *> &spectra)
{
	bool success = true;
	if (shared == leaf.s.location) {
		if (netnmrfile == leaf.s.leaftype) {
			TMPSPECTRUM *s = new TMPSPECTRUM;
			spectra.AddTail(s);
			s->leaf.l = leaf.l;
			s->path = path;
			CFile file;
			if (! file.Open(path, CFile::modeRead|CFile::shareDenyWrite)) {
				delete s;
				AfxMessageBox("Can't open this file!");
				return false;
			}
			CArchive ar(&file, CArchive::load, 8192);
			if (sizeof(HD) == ar.Read(&s->hd, sizeof(HD))) {
				s->hd.groupcur = s->hd.groupnum = 0;
				s->dt.resize(s->hd.dim[0]);
				if (s->hd.dim[0]*sizeof(complex<float>) == ar.Read(s->dt.data(), s->hd.dim[0]*sizeof(complex<float>))) {
					int count;
					if (sizeof(int) == ar.Read(&count, sizeof(int))) {
						while (success && count) {
							CString key, type;
							char *p = key.GetBuffer(count);
							if (count == ar.Read(p, count)) {
								p += strlen(p) + 1;
								if (*p) {
									type = p;
									p += strlen(p) + 1;
								} else {
									type = "NULL";
									p++;
								}
								CString value = p;
								key.ReleaseBuffer();
								if (! s->fh.Lookup(type, s->fhmap)) {
									s->fhmap = new CMapStringToString;
									s->fh.SetAt(type, s->fhmap);
								}
								s->fhmap->SetAt(key, value);
								if (sizeof(int) != ar.Read(&count, sizeof(int)))
									success = false;
							} else
								success = false;
						}
					} else
						success = false;
					if (success && sizeof(int) == ar.Read(&count, sizeof(int))) {
						while (success && count--) {
							REGION *pr = new REGION;
							if (sizeof(REGION) == ar.Read(pr, sizeof(REGION)))
								s->pregion.AddTail(pr);
							else
								success = false;
						}
					} else
						success = false;
					if (success && sizeof(int) == ar.Read(&count, sizeof(int))) {
						while (success && count--) {
							REGION *ir = new REGION;
							ir->value = 0.0f;
							UINT iread = ar.Read(&ir->start, sizeof(int));
							iread += ar.Read(&ir->stop, sizeof(int));
							if ( 2*sizeof(int) == iread ) {
								for(int i=ir->start; i<ir->stop; i++)
									ir->value += s->dt[i].real();
								s->iregion.AddTail(ir);
							} else
								success = false;
						}
					} else
						success = false;
				} else
					success = false;
			} else 
				success = false;
			ar.Close();
			file.Close();
		} else if ( fid==leaf.s.leaftype || spectrum==leaf.s.leaftype) {
			switch (leaf.s.vendor) {
			case bruker:
				success = GetLocalBrukerFile(leaf, path, spectra);
				break;
			case agilent:
				success = GetLocalVarianFile(leaf, path, spectra);
				break;
			default:
				AfxMessageBox("Can't open this file type!");
				success = false;
				break;
			}
		}
	} else {
		// served file
		CServerSocket sock;
		if (sock.Open(leaf.s.drivenumber)) {
			CString cmd;
			cmd.Format("f%s\n", path);
			sock.Send(cmd, cmd.GetLength());
			TMPSPECTRUM *s;
			do {
				int i;
				s = new TMPSPECTRUM;
				s->leaf = leaf;
				s->path = path;
				if (sizeof(HD) == sock.ReadSocket(&s->hd, sizeof(HD))) {
					// Swap bytes
					// First: Swap all 32-bit values (int and float)
					char *ch = (char *)&s->hd;
					for (i=0; i<sizeof(HD); i+=4) {
						// Don't reverse character strings
						switch (i) {
						case 508:	// f1.nucleus
						case 512:
						case 600:	// f2 4 control chars.
						case 604:	// f2.nucleus
						case 608:
						case 696:	// f3 4 control chars
						case 700:	// f3.nucleus
						case 704:
						case 848:	// lock.nucleus
						case 852:
							break;		// do nothing
						default:
							char c = ch[i];
							ch[i] = ch[i+3];
							ch[i+3] = c;
							c = ch[i+1];
							ch[i+1] = ch[i+2];
							ch[i+2] = c;
						}
					}
					// Then: take care of all doubles.
					int *in = (int *)&s->hd;
					for (i=0; i<144; i+=8) {	// at the begining
						int j = in[i/4];
						in[i/4] = in[i/4+1];
						in[i/4+1] = j;
					}
					for (i=464; i<488; i+=8) {	// f1 struct
						int j = in[i/4];
						in[i/4] = in[i/4+1];
						in[i/4+1] = j;
					}
					for (i=528; i<552; i+=8) {	// f2 struct
						int j = in[i/4];
						in[i/4] = in[i/4+1];
						in[i/4+1] = j;
					}
					for (i=624; i<648; i+=8) {	// f3 struct
						int j = in[i/4];
						in[i/4] = in[i/4+1];
						in[i/4+1] = j;
					}
					for (i=776; i<800; i+=8) {	// lock struct
						int j = in[i/4];
						in[i/4] = in[i/4+1];
						in[i/4+1] = j;
					}
					// Get the flex header
					i = atoi(sock.GetSockString());
					while (i) {
						CString key, type, value;
						char *p = key.GetBuffer(i);
						sock.ReadSocket(p, i);
						p += strlen(p) + 1;
						if (*p) {
							type = p;
							p += strlen(p) + 1;
						} else {
							type = "NULL";
							p++;
						}
						value = p;
						key.ReleaseBuffer();
						if (! s->fh.Lookup(type, s->fhmap)) {
							s->fhmap = new CMapStringToString;
							s->fh.SetAt(type, s->fhmap);
						}
						s->fhmap->SetAt(key, value);
						i = atoi(sock.GetSockString());
					}
					s->dt.resize(s->hd.dim[0]);
					sock.ReadSocket(s->dt.data(), s->hd.dim[0]*sizeof(complex<float>));
					ch = (char *)s->dt.data();
					for ( i=0; i<2*s->hd.dim[0]; i++ ) {
						char c = ch[i*4];
						ch[i*4] = ch[i*4+3];
						ch[i*4+3] = c;
						c = ch[i*4+1];
						ch[i*4+1] = ch[i*4+2];
						ch[i*4+2] = c;
					}
				} else {
					success = false;
					break;
				}
				spectra.AddTail(s);
			} while (s->hd.groupcur != s->hd.groupnum);
		} else 
			success = false;
	}
	return success;
}

CString CNet12NMRApp::MakeDocumentTitle(CNet12NMRDoc *pDoc)
{
	if (0 == pDoc->fullpathname.GetLength())
		return CString("UnknownName");
	CString title;
	CStringList list;
	CString path = pDoc->fullpathname;
	int i = path.Find(':');
	if (i >= 0)
		path = path.Mid(i+1);
	LPCSTR sep = "\\";
	if (path.Find("/") >= 0)
		sep = "/";
	i = 0;
	// break up the full path into folders and a filename
	while (i >= 0) {
		CString newname = path.Tokenize(sep, i);
		if (newname.GetLength())
			list.AddTail(newname);
	}

	if (list.GetCount() > 3) {
		// test for name/expno/pdata/procno
		int expno = 0;
		int procno = 0;
		POSITION pos = list.GetTailPosition();
		CString pstr = list.GetPrev(pos);
		CString pdata = list.GetPrev(pos);
		CString estr = list.GetPrev(pos);
		if (0 == pdata.CompareNoCase("pdata")) {
			expno = atoi(estr);
			procno = atoi(pstr);
			CString name = list.GetPrev(pos);
			if (procno == 1) {
				if (expno == 1)
					title  = name;
				else
					title = name + "." + estr;
			} else
				title = name + "." + estr + "." + pstr;
		}
	}
	if (title.IsEmpty() && list.GetCount() > 1) {
		CString last = list.RemoveTail();
		CString name = list.RemoveTail();
		bool isnumber = true;
		for (i=0; i<last.GetLength(); i++) {
			if (! isdigit(last[i])) {
				isnumber = false;
				break;
			}
		}
		if (isnumber) {	// test name/expno
			i = atoi(last);
			if (i > 1)
				title = name + "." + last;
			else
				title = name;
		} else {	// look for Agilent or ?
			i = last.GetLength();
			if (i > 4) {
				CString ext = last.Right(4);
				if (0==ext.CompareNoCase(".raw") || 0==ext.CompareNoCase(".fid") || 0==ext.CompareNoCase(".pro")) {
					 i -= 4;
					last = last.Left(i);
				}
			}
			if (i==1 && (0==last.CompareNoCase("h") || 0==last.CompareNoCase("c") || 0==last.CompareNoCase("n") || 0==last.CompareNoCase("p")))
				title = name + "." + last;
			else if (9==i && (0==last.Left(7).CompareNoCase("proton_") || 0==last.Left(7).CompareNoCase("carbon_"))) {
				title = name + "." + ((0==last.Left(1).CompareNoCase("p")) ? "h" : "c");
				int n = atoi(last.Right(2));
				if (n > 1) {
					CString nsuffix;
					nsuffix.Format(".%d", n);
					title += nsuffix;
				}
			} else
				title = last;
		}
	} 
	if (title.IsEmpty()) { // only one name!
		CString name = list.RemoveTail();
		i = name.GetLength();
		if (i > 4) {
			CString ext = name.Right(4);
			if (0==ext.CompareNoCase(".raw") || 0==ext.CompareNoCase(".fid") || 0==ext.CompareNoCase(".pro")) {
				i -= 4;
				name = name.Left(i);
			}
		}
		title = name;
	}
	list.RemoveAll();
	do {
		i = title.FindOneOf("~!@#$%^&*()`={}[]|'/?\"\\");
		if (i >= 0)
			title.SetAt(i, '-');
	} while (i >= 0);
	if (pDoc->sp.groupnum) {
		CString add;
		add.Format(".%dof%d", pDoc->sp.groupcur, pDoc->sp.groupnum);
		title += add;
	}
	return title;
}

bool CNet12NMRApp::ImportNMRFile(CString path, Leaf leaf)
{
	// if it's a native Net12NMR doucment, use built-in to load it!
	if (shared == leaf.s.location && net12nmrfile == leaf.s.leaftype) {
		CDocument *aDoc = OpenDocumentFile(path);
		if (aDoc) {
			CNet12NMRDoc *pDoc = STATIC_DOWNCAST(CNet12NMRDoc, aDoc);
			pDoc->leaf.l = leaf.l;
			pDoc->fullpathname = path;
			return  true;
		} else
			return false;
	}
	CTypedPtrList<CPtrList, TMPSPECTRUM *> spectra;
	bool success = GetTmpSpectra(path, leaf, spectra); // may return multiple spectra
	if (success) {
		POSITION pos = GetFirstDocTemplatePosition();
		if (pos) {
			CMultiDocTemplate *pDT = STATIC_DOWNCAST(CMultiDocTemplate, GetNextDocTemplate(pos));
			if (pDT) {
				while (spectra.GetCount()) {	// loop over all the spectra
					TMPSPECTRUM *s = spectra.RemoveHead();
					// create a new document
					CNet12NMRDoc *pDoc = STATIC_DOWNCAST(CNet12NMRDoc, pDT->OpenDocumentFile(NULL));
					if (served == s->leaf.s.location)
						pDoc->fullpathname.Format("%s:%s", servers.Name(s->leaf.s.drivenumber), s->path);
					else
						pDoc->fullpathname = s->path;
					pDoc->leaf.l = s->leaf.l;	// needed for the reload command
					MoveTmpToDoc(s, pDoc);		// move the temporary spectrum into the new document
					delete s;	// delete the temporary spectrum, MoveTmpDoc() does the cleanup as it progresses
					// now come up with a document name based on imported filename
					pDoc->SetTitle(MakeDocumentTitle(pDoc));
				}
			}
		}
	} else {
		// there were some problems so we will dump everything!
		while (spectra.GetCount()) {
			TMPSPECTRUM *s = spectra.RemoveHead();
			s->dt.resize(0);
			POSITION pos = s->fh.GetStartPosition();
			while (pos) {
				CString type;
				s->fh.GetNextAssoc(pos, type, s->fhmap);
				s->fh.RemoveKey(type);
				s->fhmap->RemoveAll();
			}
			while (s->pregion.GetCount())
				delete s->pregion.RemoveHead();
			while (s->iregion.GetCount())
				delete s->iregion.RemoveHead();
			while (s->bregion.GetCount())
				delete s->bregion.RemoveHead();
			delete s;
		}
	}
	return success;
}

void CNet12NMRApp::MoveTmpToDoc(TMPSPECTRUM *s, CNet12NMRDoc *pDoc)
{
	// stuff from the header
	// doubles
	pDoc->sp.at = s->hd.at;
	pDoc->sp.de = s->hd.de;
	pDoc->sp.df = s->hd.dim_freq[1];
	pDoc->sp.dof = s->hd.f2.nu_offset;
	pDoc->sp.dw = s->hd.dwell_time[0];
	pDoc->sp.f1freq = s->hd.f1.freq;
	pDoc->sp.f2.freq = s->hd.f2.freq;
	pDoc->sp.f2.offset = s->hd.f2.nu_offset;
	pDoc->sp.f3.freq = s->hd.f3.freq;
	pDoc->sp.f3.offset = s->hd.f3.nu_offset;
	pDoc->sp.of = double(s->hd.of[0]);
	pDoc->sp.pha = s->hd.phasea[0];
	pDoc->sp.phb = s->hd.phaseb[0];
	pDoc->sp.phc = s->hd.phaseb[3];
	pDoc->sp.rg = s->hd.rg;
	pDoc->sp.sf = s->hd.dim_freq[0];
	pDoc->sp.sw = s->hd.spec_width[0];
	pDoc->sp.temp = s->hd.vt.set_point;
	// floats
	pDoc->sp.f2.rate = s->hd.f2.rate;
	pDoc->sp.f3.rate = s->hd.f3.rate;
	pDoc->sp.filterfreq = float(s->hd.filter.freq);
	pDoc->sp.gain = s->hd.gain.gain_val;
	pDoc->sp.set_temp = s->hd.vt.set_point;
	// integers
	pDoc->sp.acqmode = s->hd.acqmode;
	pDoc->sp.blocksize = s->hd.block_size;
	pDoc->sp.dg = s->hd.dg;
	pDoc->sp.f1power = s->hd.f1.pwr_val;
	pDoc->sp.f2.power = s->hd.f2.pwr_val;
	pDoc->sp.f2.program = s->hd.f2.rom_program;
	pDoc->sp.f3.power = s->hd.f3.pwr_val;
	pDoc->sp.f3.program = s->hd.f3.rom_program;
	pDoc->sp.filtermode = s->hd.filter.mode;
	pDoc->sp.lock_gain = s->hd.lock.gain;
	pDoc->sp.lock_power = s->hd.lock.power;
	pDoc->sp.ns = s->hd.ns;
	pDoc->sp.groupnum = s->hd.groupnum;
	pDoc->sp.groupcur = s->hd.groupcur;
	// booleans
	pDoc->sp.f1highpower = s->hd.f1.hi_pwr_flag ? true : false;
	pDoc->sp.f2.cw = s->hd.f2.cw_flag ? true : false;
	pDoc->sp.f3.cw = s->hd.f3.cw_flag ? true : false;
	pDoc->sp.filterlocktrap = s->hd.filter.locktrap ? true : false;
	pDoc->sp.filterpreamp = s->hd.filter.preamp ? true : false;
	pDoc->sp.ftflag = s->hd.ftflag[0] ? true : false;
	pDoc->sp.ssb = s->hd.f1.lo_sideband ? true : false;
	pDoc->sp.tempset = s->hd.vt.unused1 ? true : false;
	// strings
	pDoc->f1nucleus = s->hd.f1.nucleus;
	pDoc->f2nucleus = s->hd.f2.nucleus;
	pDoc->f3nucleus = s->hd.f3.nucleus;
	pDoc->locknucleus = s->hd.lock.nucleus;

	// Clear everything allocated in s as we go.
	// the complex data.
	pDoc->dt = s->dt;
	s->dt.resize(0);

	// baseline regions
	while (s->bregion.GetCount()) {
		REGION *p = s->bregion.RemoveHead();
		if (pDoc->sp.ftflag) {
			BREGION *pNew = new BREGION;
			pNew->start = (UINT)p->start;
			pNew->stop = (UINT)p->stop;
			pDoc->baseregions.AddTail(pNew);
		}
		delete p;
	}

	// integral regions
	while (s->iregion.GetCount()) {
		REGION *p = s->iregion.RemoveHead();
		if (pDoc->sp.ftflag) {
			IREGION *pNew = new IREGION;
			pNew->selected = false;
			pNew->start = (UINT)p->start;
			pNew->stop = (UINT)p->stop;
			UINT n = pNew->stop - pNew->start;
			pNew->integral.resize(n);
			pNew->total = 0.0;
			for (UINT i=0; i<n; i++) {
				pNew->total += pDoc->dt[i+pNew->start].real();
				pNew->integral[i] = pNew->total;
			}
			pDoc->intregions.AddTail(pNew);
		}
		delete p;
	}

	// peak regions
	while (s->pregion.GetCount()) {
		REGION *p = s->pregion.RemoveHead();
		if (pDoc->sp.ftflag) {
			PREGION *pNew = new PREGION;
			pNew->start = (UINT)p->start;
			pNew->stop = (UINT)p->stop;
			pNew->threshold = p->value;
			pDoc->peakregions.AddTail(pNew);
		}
		delete p;
	}

	// flex header
	POSITION tpos = s->fh.GetStartPosition();
	while (tpos) {
		CString type, value;
		s->fh.GetNextAssoc(tpos, type, s->fhmap);
		s->fh.RemoveKey(type);
		// scan for flex header items we want to move into the new SPECTRUMPARAMTERS
		if (s->fhmap->Lookup("IntegralConst", value)) {
			pDoc->sp.integralconst = (float)atof(value);
			if (pDoc->sp.integralconst <= 0.0f)
				pDoc->sp.integralconst = 1.0f;
			pDoc->sp.bintegralconst = true;
			s->fhmap->RemoveKey("IntegralConst");
		}
		if (s->fhmap->Lookup("DC", value)) {
			pDoc->sp.dclevel = true;
			s->fhmap->RemoveKey("DC");
		}
		if (s->fhmap->Lookup("QC", value)) {
			sscanf_s(value, "a:%f g:%f", &pDoc->sp.qca, &pDoc->sp.qcg);
			s->fhmap->RemoveKey("QC");
		}
		if (s->fhmap->Lookup("LeftShift", value)) {
			pDoc->sp.lrshift = atoi(value);
			s->fhmap->RemoveKey("LeftShift");
		}
		if (s->fhmap->Lookup("apo_type", value)) {
			pDoc->sp.isLoren = (0 == value.CompareNoCase("Lorentzian"));
			s->fhmap->RemoveKey("apo_type");
		}
		if (s->fhmap->Lookup("apo_value", value)) {
			pDoc->sp.apoValue = (float)atof(value);
			s->fhmap->RemoveKey("apo_value");
		}
		if (s->fhmap->Lookup("apo_unit", value)) {
			pDoc->sp.isPPM = (0 == value.CompareNoCase("PPM"));
			s->fhmap->RemoveKey("apo_unit");
		}
		if (s->fhmap->Lookup("Magnitude", value)) {
			pDoc->sp.magnitude = true;
			s->fhmap->RemoveKey("Magnitude");
		}
		if (s->fhmap->Lookup("Phase", value))
			s->fhmap->RemoveKey("Phase");
		if (s->fhmap->Lookup("Level", value)) {
			pDoc->sp.level = true;
			s->fhmap->RemoveKey("Level");
		}
		if (s->fhmap->Lookup("PolyBase", value)) {
			pDoc->sp.baseline = pDoc->sp.polybaseline = true;
			s->fhmap->RemoveKey("PolyBase");
		}
		if (s->fhmap->Lookup("FilterComp", value)) {
			pDoc->sp.filtercomp = true;
			s->fhmap->RemoveKey("FilterComp");
		}
		if (s->fhmap->IsEmpty())
			delete s->fhmap;
		else
			pDoc->fh.SetAt(type, s->fhmap);
	}
}

void CNet12NMRApp::InitializeColors()
{
	//initialize colors
	ColorScheme[0][imfidColor] = RGB(255, 255,   0);	//  0 im FID
	ColorScheme[0][refidColor] = RGB(255, 255, 255);	//  1 re FID
	ColorScheme[0][spectColor] = RGB(  0, 255,   0);	//  2 spectrum
	ColorScheme[0][integColor] = RGB(255,   0,   0);	//  3 integral
	ColorScheme[0][intehColor] = RGB(255, 255,   0);	//  4 integral selected
	ColorScheme[0][axis_Color] = RGB(255, 128,  64);	//  5 axis
	ColorScheme[0][titleColor] = RGB(128, 128, 255);	//  6 title
	ColorScheme[0][parboColor] = RGB(255, 255,   0);	//  7 paramater box
	ColorScheme[0][parheColor] = RGB(  0,   0,   0);	//  8 paramater header
	ColorScheme[0][paramColor] = RGB(  0, 255,   0);	//  9 parameters
	ColorScheme[0][peaklColor] = RGB(  0, 255, 255);	// 10 peak labels
	ColorScheme[0][peakhColor] = RGB(255, 255,   0);	// 11 peak selected
	ColorScheme[0][peaktColor] = RGB(  0, 255, 255);	// 12 peak thershold
	ColorScheme[0][basenColor] = RGB(255,   0,   0);	// 13 baseline on
	ColorScheme[0][basefColor] = RGB(192, 192, 192);	// 14 baseline off
	ColorScheme[0][crossColor] = RGB(255,   0,   0);	// 15 crosshairs
	ColorScheme[0][zoomcColor] = RGB(  0,   0, 255);	// 16 zoom cursor
	ColorScheme[0][backgColor] = RGB(  0,   0,   0);	// 17 background

	ColorScheme[1][imfidColor] = RGB(  0,   0,   0);	//  0 im FID
	ColorScheme[1][refidColor] = RGB(  0,   0,   0);	//  1 re FID
	ColorScheme[1][spectColor] = RGB(  0,   0,   0);	//  2 spectrum
	ColorScheme[1][integColor] = RGB(192, 192, 192);	//  3 integral
	ColorScheme[1][intehColor] = RGB(  0,   0, 255);	//  4 selected integral
	ColorScheme[1][axis_Color] = RGB(  0,   0,   0);	//  5 axis
	ColorScheme[1][titleColor] = RGB(  0,   0,   0);	//  6 title
	ColorScheme[1][parboColor] = RGB(  0,   0,   0);	//  7 paramater box
	ColorScheme[1][parheColor] = RGB(255, 255, 255);	//  8 paramater header
	ColorScheme[1][paramColor] = RGB(  0,   0,   0);	//  9 parameters
	ColorScheme[1][peaklColor] = RGB(  0,   0,   0);	// 10 peak labels
	ColorScheme[1][peakhColor] = RGB(  0,   0, 255);	// 11 selected peak labels
	ColorScheme[1][peaktColor] = RGB(128, 128, 128);	// 12 peak thershold
	ColorScheme[1][basenColor] = RGB(255,   0,   0);	// 13 baseline on
	ColorScheme[1][basefColor] = RGB(128, 128, 128);	// 14 baseline off
	ColorScheme[1][crossColor] = RGB(255,   0,   0);	// 15 crosshairs
	ColorScheme[1][zoomcColor] = RGB(  0,   0, 255);	// 16 zoom cursor
	ColorScheme[1][backgColor] = RGB(255, 255, 255);	// 17 background

	memcpy(Color, ColorScheme[0], constColorSize*sizeof(COLORREF)); // initialize in case there is nothing in the registry
	COLORREF *pColor = NULL;
	UINT size;
	if (GetProfileBinary(settings, "Colors", (LPBYTE *)&pColor, &size)) {
		if (constColorSize*sizeof(COLORREF)==size)
			memcpy(Color, pColor, size);
	}
	if (size)
		delete[] pColor;
}

double CNet12NMRApp::GetReferenceFrequency(LPCTSTR strNuc, double lockref)
{
	if (0 == VarianNucTable.GetCount()) {
		CString nucfile = GetAppPath() + "nuctabref.txt";
		if (ReadAsciiFile(nucfile, CFile::modeRead)) {
			int i = 0;
			CString line = nucfile.Tokenize("\r\n", i).Trim();
			while (i > 0) {
				if (line.GetLength()>3 && line.Left(1).Compare("#")) {
					int j = 0;
					CString key = line.Tokenize(" \t", j).Trim();
					if (j > 0) {
						CString value = line.Tokenize(" \t", j).Trim();
						if (j > 0) {
							double *pValue = new double;
							*pValue = atof(value);
							VarianNucTable.SetAt(key, pValue);
						}
					}
				}
				line = nucfile.Tokenize("\r\n", i).Trim();
			}
		}
	}
	double *value;
	if (VarianNucTable.Lookup(strNuc, value))
		return *value * lockref;
	else
		return 100.0;
}

bool CNet12NMRApp::GetLocalVarianFile(Leaf leaf, LPCTSTR reqfilename, CTypedPtrList<CPtrList, TMPSPECTRUM *> &spectra)
{
	HD hd;
	memset(&hd, 0, sizeof(HD));
	m_flexMap.RemoveAll();
	hd.dimension = 1;
	hd.f2.hetero_flag = '2';
	strcpy_s(hd.f2.nucleus, 8, "off");
	strcpy_s(hd.f3.nucleus, 8, "off");
	strcpy_s(hd.lock.nucleus, 8, "off");
	hd.ftflag[0] = 0;
	CString fbase = reqfilename;
	if (0 == fbase.Right(4).CompareNoCase(".pro")) {
		fbase = fbase.Left(fbase.GetLength()-3) + "fid";
		hd.ftflag[0] = 1;
	}
	CString file = fbase + "\\procpar";
	if (ReadAsciiFile(file, CFile::modeRead))
		MapProcparFile(file);
	else
		return false;
	CStringArray *pValues;
	CStringArray *pArrayValues = NULL;
	CString arrayname;
	if (m_procparMap.Lookup("array", pValues)) {
		arrayname = pValues->GetAt(0);
		if (! arrayname.IsEmpty()) {
			if (! m_procparMap.Lookup(arrayname, pArrayValues))
				arrayname.Empty();
		}
	}
	if (m_procparMap.Lookup("at", pValues))
		hd.at = atof(pValues->GetAt(0));
	if (m_procparMap.Lookup("ct", pValues))
		hd.ns = atoi(pValues->GetAt(0));
	if (m_procparMap.Lookup("d1", pValues)) {
		double x = atof(pValues->GetAt(0));
		CString s;
		s.Format("sequel\n%gs", x);
		m_flexMap.SetAt("D1", s);
	}
	if (m_procparMap.Lookup("d2", pValues)) {
		double x = atof(pValues->GetAt(0));
		CString s;
		s.Format("sequel\n%gs", x);
		m_flexMap.SetAt("D2", s);
	}
	const CString DOSY = "DOSY\n";
	const CString nl = "\n";
	if (m_procparMap.Lookup("DAC_to_G", pValues))
		m_flexMap.SetAt("DAC_to_G", DOSY + pValues->GetAt(0));
	if (m_procparMap.Lookup("date", pValues))
		m_flexMap.SetAt("date", nl + pValues->GetAt(0));
	if (m_procparMap.Lookup("dfrq", pValues))
		hd.f2.freq = atof(pValues->GetAt(0));
	CString dm = "nnn";
	if (m_procparMap.Lookup("dm", pValues)) {
		dm = pValues->GetAt(0);
		m_flexMap.SetAt("DecouplerMode", nl + dm);
	}
	if (m_procparMap.Lookup("dmf", pValues))
		hd.f2.rate = 0.25f * (float)atof(pValues->GetAt(0));
	if (m_procparMap.Lookup("dmm", pValues))
		switch (pValues->GetAt(0).GetAt(0)) {
		case 'c':
			hd.f2.cw_flag = 1;
			break;
		case 'f':
			hd.f2.rom_program = 10;
			break;
		case 'n':
			hd.f2.rom_program = 11;
			break;
		case 'r':
			hd.f2.rom_program = 0;
			break;
		case 'g':
			hd.f2.rom_program = 9;
			break;
		case 'm':
			hd.f2.rom_program = 1;
			break;
		case 'w':
			hd.f2.rom_program = 3;
			break;
		default:
			hd.f2.rom_program = 999;
			break;
	}
	if (m_procparMap.Lookup("dn", pValues) && dm.Find('y')>=0)
		strcpy_s(hd.f2.nucleus, 8, pValues->GetAt(0));
	if (m_procparMap.Lookup("dosygamma", pValues))
		m_flexMap.SetAt("dosygamma", DOSY + pValues->GetAt(0));
	if (m_procparMap.Lookup("dosytimecubed", pValues))
		m_flexMap.SetAt("timecubed", DOSY + pValues->GetAt(0));
	if (m_procparMap.Lookup("dpwr", pValues))
		hd.f2.pwr_val = atoi(pValues->GetAt(0));
	if (m_procparMap.Lookup("gain", pValues))
		hd.gain.gain_val = (float)atof(pValues->GetAt(0));
	bool sampleinfo = false;
	if (m_procparMap.Lookup("gesample", pValues)) {
		sampleinfo = true;
		m_flexMap.SetAt("sample", nl + pValues->GetAt(0));
		if (m_procparMap.Lookup("gecomment", pValues))
			m_flexMap.SetAt("comment", nl + pValues->GetAt(0));
		if (m_procparMap.Lookup("gecustomer", pValues))
			m_flexMap.SetAt("customer", nl + pValues->GetAt(0));
		if (m_procparMap.Lookup("geanalysis", pValues))
			m_flexMap.SetAt("workorder", nl + pValues->GetAt(0));
		if (m_procparMap.Lookup("geproject", pValues))
			m_flexMap.SetAt("project", nl + pValues->GetAt(0));
		if (m_procparMap.Lookup("gesubmitted", pValues))
			m_flexMap.SetAt("submitted", nl + pValues->GetAt(0));
		if (m_procparMap.Lookup("gedataclass", pValues))
			m_flexMap.SetAt("dataclass", nl + pValues->GetAt(0));
	}
	if (m_procparMap.Lookup("homo", pValues)) {
		if ('y' == pValues->GetAt(0).GetAt(0))
			hd.f2.hetero_flag = '1';
	}
	if (m_procparMap.Lookup("lockfreq_", pValues)) {
		hd.lock.freq = atof(pValues->GetAt(0));
	}
	if (m_procparMap.Lookup("lockgain_", pValues))
		hd.lock.gain = atoi(pValues->GetAt(0));
	if (m_procparMap.Lookup("lockpower_", pValues))
		hd.lock.power = atoi(pValues->GetAt(0));
	if (m_procparMap.Lookup("np", pValues))
		hd.block_size = atoi(pValues->GetAt(0))/2;
	if (m_procparMap.Lookup("p1", pValues)) {
		double val = atof(pValues->GetAt(0));
		CString s; s.Format("sequel\n%gs", val);
		m_flexMap.SetAt("P1", s);
	}
	const CString sequel = "sequel\n";
	if (m_procparMap.Lookup("probe_", pValues))
		m_flexMap.SetAt("Probe", sequel + pValues->GetAt(0));
	if (m_procparMap.Lookup("pw", pValues)) {
		double val = atof(pValues->GetAt(0));
		CString s; s.Format("sequel\n%gs", val);
		m_flexMap.SetAt("PW", s);
	}
	if (m_procparMap.Lookup("pw90", pValues)) {
		double val = atof(pValues->GetAt(0));
		CString s; s.Format("sequel\n%gs", val);
		m_flexMap.SetAt("PW90", s);
	}
	//if (m_procparMap.Lookup("reffrq", pValues))
	//	hd.dim_freq[0] = atof(pValues->GetAt(0));
	if (m_procparMap.Lookup("seqfil", pValues)) {
		CString s = "seq_source";
		m_flexMap.SetAt(s, s + "\n" + pValues->GetAt(0));
	}
	if (m_procparMap.Lookup("sfrq", pValues))
		hd.f1.freq = atof(pValues->GetAt(0));
	if (m_procparMap.Lookup("solvent", pValues)) {
		CString sfile = GetAppPath() + "solvents.txt";
		if (ReadAsciiFile(sfile, CFile::modeRead)) {
			int count = 0;
			CString line = sfile.Tokenize("\r\n", count).Trim();
			while (count > 0) {
				if (line.GetLength()>3 && line.Left(1).Compare("#")) {
					int lc = 0;
					CString name = line.Tokenize(" \t", lc).Trim();
					if (lc > 0 && 0 == name.CompareNoCase(pValues->GetAt(0))) {
						hd.lock.ref_freq = hd.lock.freq /(1.0 + 1.0e-6 * atof(line.Tokenize(" \t", lc).Trim()));
						break;
					}
				}
				line = sfile.Tokenize("\r\n", count).Trim();
			}
		}
		m_flexMap.SetAt("LockSolvent", nl + pValues->GetAt(0));
	}
	if (m_procparMap.Lookup("spin", pValues))
		hd.air.spin_rate = atoi(pValues->GetAt(0));
	if (m_procparMap.Lookup("ss", pValues))
		hd.dg = atoi(pValues->GetAt(0));
	if (m_procparMap.Lookup("sw", pValues)) {
		double sw = atof(pValues->GetAt(0));
		hd.spec_width[0] = sw;
		hd.dwell_time[0] = 1.0 / sw;
	}
	if (m_procparMap.Lookup("temp", pValues))
		hd.vt.set_point = (float)atof(pValues->GetAt(0));
	if (m_procparMap.Lookup("time_run", pValues))
		m_flexMap.SetAt("time_start", nl + pValues->GetAt(0));
	if (m_procparMap.Lookup("time_complete", pValues))
		m_flexMap.SetAt("time_finish", nl + pValues->GetAt(0));
	if (m_procparMap.Lookup("tn", pValues)) {
		strcpy_s(hd.f1.nucleus, 8, pValues->GetAt(0));
		hd.dim_freq[0] = hd.f1.ref_freq = GetReferenceFrequency(hd.f1.nucleus, hd.lock.ref_freq);
	}
	if (m_procparMap.Lookup("tpwr", pValues)) {
		hd.f1.hi_pwr_val = atoi(pValues->GetAt(0));
		hd.f1.hi_pwr_flag = 1;
	}

	float lpval = 0.0f;
	float rpval = 0.0f;
	if (hd.ftflag[0]) {
		if (m_procparMap.Lookup("rp", pValues)) {
			rpval = (float)atof(pValues->GetAt(0));
			hd.phasea[0] = rpval;
			rpval *= 0.0174532925199433f;
		}
		if (m_procparMap.Lookup("lp", pValues)) {
			lpval = (float)atof(pValues->GetAt(0));
			hd.phaseb[0] = lpval;
			lpval *= 0.0174532925199433f;
		}
		if (m_procparMap.Lookup("lb", pValues)) {
			if (atof(pValues->GetAt(0))) {
				CString proc = "NetNMR\n";
				m_flexMap.SetAt("apo_value", proc + pValues->GetAt(0));
				m_flexMap.SetAt("apo_type", proc + "Lorentzian");
				m_flexMap.SetAt("apo_units", proc+ "Hertz");
			}
		}
	}

	if (hd.lock.ref_freq) {
		strcpy_s(hd.lock.nucleus, 8, "H2");
		hd.dim_freq[1] = GetReferenceFrequency(hd.f1.nucleus, hd.lock.ref_freq);
	}
	hd.of[0] = (float)(1.0e6 * (hd.f1.freq - hd.dim_freq[0]));
	if (hd.f2.freq) {
		double f2ref = GetReferenceFrequency(hd.f2.nucleus, hd.lock.ref_freq);
		hd.dim_freq[1] = hd.f2.ref_freq = f2ref;
		hd.f2.nu_offset = 1.0e6 * (hd.f2.freq - f2ref) / f2ref;
		hd.of[1] = (float)(1.0e6 * (hd.f2.freq - f2ref));
	}
	if (! sampleinfo) {
		CString text = fbase + "\\text";
		if (ReadAsciiFile(text, CFile::modeRead)) {
			int count = 0;
			for (int i=0; i<6; i++) {
				CString line = text.Tokenize("\r\n", count).Trim();
				if (count > 0) {
					switch (i) {
					case 0:
						m_flexMap.SetAt("sample", nl + line);
						break;
					case 1:
						m_flexMap.SetAt("comment", nl + line);
						break;
					case 2:
						m_flexMap.SetAt("user", nl + line);
						break;
					case 3:
						m_flexMap.SetAt("workorder", nl + line);
						break;
					case 4:
						m_flexMap.SetAt("project", nl + line);
						break;
					case 5:
						m_flexMap.SetAt("submitted", nl + line);
						break;
					}
				} else
					break;
			}
		}
	}
	CFile fp;
	if (! fp.Open(fbase + (hd.ftflag[0] ? "\\datdir\\data" : "\\fid"), CFile::modeRead))
		return false;
	dfilehead dfh;
	if (sizeof(dfilehead) != fp.Read(&dfh, sizeof(dfilehead))) {
		fp.Close();
		return false;
	}
	DATAFILEHEADER_CONVERT_NTOH(&dfh);
	hd.dim[0] = dfh.np / 2;
	if (dfh.ebytes != 2)
		dfh.ebytes = 4;
	int limit = 1;
	if (pArrayValues)
		limit = pArrayValues->GetCount();
	if (limit > 1)
		hd.groupnum = limit;
	// need the array type name, if it was assigned in the above code
	CString arraytype;
	if (m_flexMap.Lookup(arrayname, arraytype)) {
		int x = 0;
		arraytype = arraytype.Tokenize("\n", x);
	} else
		arraytype = "";
	for (int i=0; i<limit; i++) {
		if (pArrayValues) {
			m_flexMap.SetAt(arrayname, arraytype + '\n' + pArrayValues->GetAt(i));
			hd.groupcur = i + 1;
		}
		TMPSPECTRUM *pSpec = new TMPSPECTRUM;
		pSpec->leaf.l = leaf.l;
		pSpec->path = fbase;
		memcpy(&pSpec->hd, &hd, sizeof(HD));
		POSITION pos = m_flexMap.GetStartPosition();
		while (pos) {
			CString key, val;
			m_flexMap.GetNextAssoc(pos, key, val);
			CString type = "NULL";
			if ('\n' != val.GetAt(0)) {
				int istart = 0;
				type = val.Tokenize("\n", istart);
				val = val.Tokenize("\n", istart);
			} else
				val = val.Trim();
			if (! pSpec->fh.Lookup(type, pSpec->fhmap)) {
				pSpec->fhmap = new CMapStringToString;
				pSpec->fh.SetAt(type, pSpec->fhmap);
			}
			pSpec->fhmap->SetAt(key, val);
		}
		// finally, read the data!
		pSpec->dt.resize(hd.dim[0]);
		// skip past the block header, we don't need it!
		fp.Seek(dfh.nbheaders*sizeof(dblockhead), SEEK_CUR);
		for (int n=0; n<hd.dim[0]; n++) {
			if (2 == dfh.ebytes) {
				// data is short ints
				short int y[2];
				fp.Read(y, 2*sizeof(short int));
				pSpec->dt[n] = complex<float>(float(ntohs(y[0])), float(ntohs(y[1])));
			} else {
				union {
					UINT u[2];
					float f[2];
					int i[2];
				} y;
				fp.Read(y.u, 2*sizeof(UINT));
				y.u[0] = ntohl(y.u[0]);
				y.u[1] = ntohl(y.u[1]);
				if (dfh.status & S_FLOAT)
					pSpec->dt[n] = complex<float>(y.f[0], y.f[1]);
				else
					pSpec->dt[n] = complex<float>(float(y.i[0]), float(y.i[1]));
			}
		}
		if (hd.ftflag[0]) {
			// do phase correction;
			for (int i=0; i<hd.dim[0]; i++) {
				float theta = rpval + lpval * (float)(hd.dim[0]-i)/(float)hd.dim[0];
				complex<float> c = polar((float)abs(pSpec->dt[i]), (float)arg(pSpec->dt[i])-theta);
				pSpec->dt[i] = c;
			}
		}
		spectra.AddTail(pSpec);
	}
	fp.Close();
	m_flexMap.RemoveAll();
	POSITION pos = m_procparMap.GetStartPosition();
	while (pos) {
		CString key;
		m_procparMap.GetNextAssoc(pos, key, pValues);
		pValues->RemoveAll();
		delete pValues;
		m_procparMap.RemoveKey(key);
	}
	return true;
}

bool CNet12NMRApp::GetLocalBrukerFile(Leaf leaf, LPCTSTR reqfilename, CTypedPtrList<CPtrList, TMPSPECTRUM *> &spectra)
{
	CString arraystr, arrayname;
	HD hd;
	memset(&hd, 0, sizeof(HD));
	CString fbase = reqfilename;
	int i = fbase.Find("\\pdata\\");
	int npoints;
	if (i > 0) {
		hd.ftflag[0] = -1;
		fbase = fbase.Left(i);
	} else
		hd.ftflag[0] = 0;
	// GET PARAMETERS FROM THE PULSE PROGRAM
	CString cnst('-', 64), cpd('-', 8), cpdamx("--"), d('-', 64), dl('-', 64);
	CString l('-', 64), p('-', 64), pcpd('-', 64), pl('-', 64), tl('-', 64);
	CString file = fbase + "\\format.temp";
	if (ReadAsciiFile(file, CFile::modeRead)) {
		int current = 0;
		CString token = "dummy";
		while ( 0 != token.GetLength() ) {
			token = file.Tokenize(" \t\n\r", current);
			if (! token.Compare("T_NAME")) {
				token = file.Tokenize(" \t\n\r", current);
				switch (token.GetAt(0)) {
					case 'C':
						if (0==token.Find("CNST") && isdigit(token.GetAt(4))) {
							i = atoi(token.Mid(4,2));
							if (i < cnst.GetLength())
								cnst.SetAt(i, '+');
						} else if (0==token.Find("CPD")) {
							if (isdigit(token.GetAt(3))) {
								i = atoi(token.Mid(3,2));
								if (i < cpd.GetLength())
									cpd.SetAt(i, '+');
							} else if ('T' == token.GetAt(3)) {
								cpdamx.SetAt(0, '+');
							} else {
								cpdamx.SetAt(1, '+');
							}
						}
						break;
					case 'D':
						if ('L'==token.GetAt(1) && isdigit(token.GetAt(2))) {
							i = token.GetAt(2) - '0';
							if (i < dl.GetLength())
								dl.SetAt(i, '+');
						} else if (isdigit(token.GetAt(1))) {
							i = atoi(token.Mid(1, 2));
							if (i < d.GetLength())
								d.SetAt(i, '+');
						}
						break;
					case 'L':
						if (isdigit(token.GetAt(1))) {
							i = atoi(token.Mid(1, 2));
							if (i < l.GetLength())
								l.SetAt(i, '+');
						}
						break;
					case 'P':
						if (isdigit(token.GetAt(1))) {
							i = atoi(token.Mid(1, 2));
							if (i < p.GetLength())
								p.SetAt(i, '+');
						} else if ('L'==token.GetAt(1) && isdigit(token.GetAt(2))) {
							i = atoi(token.Mid(2, 2));
							if (i < pl.GetLength())
								pl.SetAt(i, '+');
						} else if (0==token.Find("PCPD") && isdigit(token.GetAt(4))) {
							i = atoi(token.Mid(4, 2));
							if (i < pcpd.GetLength())
								pcpd.SetAt(i, '+');
						}
						break;
					case 'T':
						if ('L'==token.GetAt(1) && isdigit(token.GetAt(2))) {
							i = token.GetAt(2) - '0';
							if (i < tl.GetLength())
								tl.SetAt(i, '+');
						}
						break;
				}
			}
		}
	}
	// GET TD OUT OF THE acqu2s FILE
	file = fbase + "\\acqu2s";
	if (ReadAsciiFile(file, CFile::modeRead)) {
		MapJCAMPFile(file);
		CString s;
		hd.dimension = 2;
		if (m_paraMap.Lookup("TD", s))
			hd.dim[1] = atoi(s);
	} else
		hd.dimension = hd.dim[1] = 1;
	// GET STUFF OUT OF THE ACQUS FILE
	int bytord = 1;
	int decim = 0;
	int dspfvs = 0;
	double grpdly = 0.0;
	file = fbase + "\\acqus";
	if (ReadAsciiFile(file, CFile::modeRead)) {
		MapJCAMPFile(file);
		hd.f2.hetero_flag = 'b';
		CString s;
		if (m_paraMap.Lookup("AQ_mod", s))
			hd.acqmode = atoi(s) + 10;
		if (m_paraMap.Lookup("BF1", s))
			hd.dim_freq[0] = atof(s);
		if (m_paraMap.Lookup("BF2", s))
			hd.dim_freq[1] = atof(s);
		if ('+' == cpdamx.GetAt(0)) {
			if (m_paraMap.Lookup("CPDPRGT", s)) {
				if (! s.IsEmpty()) {
					CString val;
					val.Format("sequel\n%s", s);
					m_flexMap.SetAt("CPDPRGT", val);
				}
			}
		}
		if ('+' == cpdamx.GetAt(1)) {
			if (m_paraMap.Lookup("CPDPRG", s)) {
				if (! s.IsEmpty()) {
					CString val;
					val.Format("sequel\n%s", s);
					m_flexMap.SetAt("CPDPRG", val);
				}
			}
		}
		for (i=0; i<8; i++) {
			if ('+' == cpd.GetAt(i)) {
				CString pname;
				pname.Format("CPDPRG%d", i);
				if (m_paraMap.Lookup(pname, s)) {
					if (! s.IsEmpty()) {
						CString newfh;
						newfh.Format("sequel\n%s", s);
						m_flexMap.SetAt(pname, newfh);
						if (i == 2) {
							if (0 <= s.Find("mlev"))
								hd.f2.rom_program = 1;
							else if (0 <= s.Find("waltz"))
								hd.f2.rom_program = 3;
							else if (0 <= s.Find("garp"))
								hd.f2.rom_program = 9;
							else if (0 <= s.Find("bb"))
								hd.f2.rom_program = 8;
							else if (0 <= s.Find("tppm"))
								hd.f2.rom_program = 12;
							else
								hd.f2.rom_program = 999;
						}
					}
				}
			}
			if (m_paraMap.Lookup("PCPD", s) && '+'==pcpd.GetAt(2)) {
				i = 0;
				CString val = s.Tokenize(" \t", i);
				val = s.Tokenize(" \t", i);
				val = s.Tokenize(" \t", i);
				double x = atof(val);
				if (x > 0.0)
					hd.f2.rate = (float)(2.5e5/x);
			}
		}
		if (m_paraMap.Lookup("BYTORDA", s))
			bytord = atoi(s);
		SequelArray(cnst, "CNST");
		SequelArray(d, "D");
		if (m_paraMap.Lookup("DATE", s)) {
			time_t t = (time_t)atoi(s);
			CTime filetime(t);
			CString flxhd;
			flxhd.Format("\n%s", filetime.Format("%c"));
			m_flexMap.SetAt("date", flxhd);
		}
		SequelArray(dl, "DL");
		if (m_paraMap.Lookup("DECIM", s))
			decim = atoi(s);
		if (m_paraMap.Lookup("DS", s))
			hd.dg = atoi(s);
		if (m_paraMap.Lookup("DSPFVS", s))
			dspfvs = atoi(s);
		if (m_paraMap.Lookup("GRPDLY", s))
			grpdly = atof(s);
		SequelArray(l, "L");
		if (m_paraMap.Lookup("LGAIN", s))
			hd.lock.gain = atoi(s);
		if (m_paraMap.Lookup("LOCKPOW", s))
			hd.lock.power = atoi(s);
		if (m_paraMap.Lookup("LOCNUC", s))
			strcpy_s(hd.lock.nucleus, 8, s.Trim(" \t\n"));
		if (m_paraMap.Lookup("NS", s))
			hd.ns = atoi(s);
		if (m_paraMap.Lookup("NUCLEUS", s)) {
			if (0 <= s.Find("off"))
				m_paraMap.Lookup("NUC1", s);
			strcpy_s(hd.f1.nucleus, 8, s.Trim(" \n"));
		}
		if (m_paraMap.Lookup("DECNUC", s)) {
			if (0 <= s.Find("off"))
				m_paraMap.Lookup("NUC2", s);
			strcpy_s(hd.f2.nucleus, 8, s.Trim(" \n"));
		}
		if (m_paraMap.Lookup("O1", s))
			hd.of[0] = (float)atof(s);
		if (m_paraMap.Lookup("O2", s)) {
			hd.of[1] = (float)atof(s);
			if (0.0 != hd.dim_freq[1])
				hd.f2.nu_offset = hd.of[1] / hd.dim_freq[1];
		}
		SequelArray(p, "P");
		SequelArray(pl, "PL");
		if (m_paraMap.Lookup("PROBHD", s)) {
			CString flexhd;
			flexhd.Format("sequel\n%s", s.Trim(" \n\r"));
			m_flexMap.SetAt("Probe", flexhd);
		}
		if (m_paraMap.Lookup("PULPROG", s)) {
			CString flexhd;
			flexhd.Format("seq_source\n%s", s.Trim(" \n\r"));
			m_flexMap.SetAt("seq_source", flexhd);
		}
		if (m_paraMap.Lookup("RG", s))
			hd.gain.gain_val = (float)atof(s);
		if (m_paraMap.Lookup("SFO1", s))
			hd.f1.freq = atof(s);
		if (m_paraMap.Lookup("SFO2", s))
			hd.f2.freq = atof(s);
		if (m_paraMap.Lookup("SOLVENT", s)) {
			CString flxhd;
			flxhd.Format("\n%s", s.Trim(" \t\n"));
			m_flexMap.SetAt("LockSolvent", flxhd);
		}
		if (m_paraMap.Lookup("SW_h", s)) {
			hd.spec_width[0] = atof(s);
			if (0.0 != hd.spec_width[0])
				hd.dwell_time[0] = 1.0 / hd.spec_width[0];
		}
		if (m_paraMap.Lookup("TE", s)) {
			hd.vt.set_point = (float)(atof(s)-273.15);
			hd.vt.unused1 = 1;
		}
		if (m_paraMap.Lookup("TD", s)) {
			npoints = hd.block_size = hd.dim[0] = atoi(s) / 2;
		}
		SequelArray(tl, "TL");
		if (2 == hd.dimension) {
			m_paraMap.Lookup("VDLIST", s);
			if (! s.IsEmpty()) {
				arraystr = fbase + "\\" + s;
				if (ReadAsciiFile(arraystr, CFile::modeRead))
					arrayname = "VD";
				else {
					arraystr = fbase + "\\vdlist";
					if (ReadAsciiFile(arraystr, CFile::modeRead))
						arrayname = "VD";
				}
			}
			if (arrayname.IsEmpty()) {
				m_paraMap.Lookup("VPLIST", s);
				if (! s.IsEmpty()) {
					arraystr = fbase + "\\" + s;
					if (ReadAsciiFile(arraystr, CFile::modeRead))
						arrayname = "VP";
					else {
						arraystr = fbase + "\\vplist";
						if (ReadAsciiFile(arraystr, CFile::modeRead))
							arrayname = "VP";
					}
				}
			}
			if (arrayname.IsEmpty()) {
				m_paraMap.Lookup("VCLIST", s);
				if (! s.IsEmpty()) {
					arraystr = fbase + "\\" + s;
					if (ReadAsciiFile(arraystr, CFile::modeRead))
						arrayname = "VC";
					else {
						arraystr = fbase + "\\vclist";
						if (ReadAsciiFile(arraystr, CFile::modeRead))
							arrayname = "VC";
					}
				}
			}
			if (arrayname.IsEmpty()) {
				m_paraMap.Lookup("VTLIST", s);
				if (! s.IsEmpty()) {
					arraystr = fbase + "\\" + s;
					if (ReadAsciiFile(arraystr, CFile::modeRead))
						arrayname = "VT";
					else {
						arraystr = fbase + "\\vtlist";
						if (ReadAsciiFile(arraystr, CFile::modeRead))
							arrayname = "VT";
					}
				}
			}
		}
	}
	int norcon = 1;
	if (hd.ftflag[0]) {
		file = reqfilename;
		file += "\\procs";
		if (ReadAsciiFile(file, CFile::modeRead)) {
			CString s;
			MapJCAMPFile(file);
			if (m_paraMap.Lookup("BYTORDP", s))
				bytord = atoi(s);
			if (m_paraMap.Lookup("FTSIZE", s)) {
				npoints = hd.dim[0] = atoi(s);
			}
			if(m_paraMap.Lookup("NC_proc", s))
				norcon = atoi(s);
			if(m_paraMap.Lookup("WDW", s)) {
				i = atoi(s);
				if (i == 1) {
					CString v;
					if (m_paraMap.Lookup("LB", v)) {
						m_flexMap.SetAt("apo_type", "\nLB");
						s.Format("\n%s", v);
						m_flexMap.SetAt("apo_value", s);
						m_flexMap.SetAt("apo_unit", "\nHertz");
					}
				} else if (i == 2) {
					CString v;
					if (m_paraMap.Lookup("GM", v)) {
						m_flexMap.SetAt("apo_type", "\nGM");
						s.Format("\n%s"), v;
						m_flexMap.SetAt("apo_value", s);
						m_flexMap.SetAt("apo_unit", "\nHertz");
					}
				}
			}
		}
	} else {
		if (grpdly > 0.0)
			hd.phaseb[3] = float(360.0 * grpdly);
		else
			hd.phaseb[3] = LookupPhase(decim, dspfvs);
	}
	hd.at = hd.dwell_time[0] * hd.block_size;
	file = fbase + "\\info";
	if (ReadAsciiFile(file, CFile::modeRead)) {
		int current = 0;
		CString key = file.Tokenize("\n", current).Trim();
		while (key.GetLength()) {
			CString flxhd;
			CString val = file.Tokenize("\n", current).Trim();
			if (0==key.Compare("CUSTOMER")) {
				flxhd.Format("\n%s", val);
				m_flexMap.SetAt("user", flxhd);
			} else if (0==key.Compare("SAMPLE")) {
				flxhd.Format("\n%s", val);
				m_flexMap.SetAt("sample", flxhd);
			} else if (0==key.Compare("COMMENT")) {
				flxhd.Format("\n%s", val);
				m_flexMap.SetAt("comment", flxhd);
			} else if (0==key.Compare("WORK_ORDER")) {
				flxhd.Format("\n%s", val);
				m_flexMap.SetAt("workorder", flxhd);
			} else if (0==key.Compare("PROJECT")) {
				flxhd.Format("\n%s", val);
				m_flexMap.SetAt("project", flxhd);
			} else if (0==key.Compare("SUBMISSION_DATE")) {
				flxhd.Format("\n%s", val);
				m_flexMap.SetAt("submitted", flxhd);
			}
			key = file.Tokenize("\n", current).Trim();
		}
	} else {
		file = fbase + "\\sample_info.prop";
		if (ReadAsciiFile(file, CFile::modeRead)) {
			MapPropFile(file);
			CString nl = "\n";
			CString s;
			if (m_paraMap.Lookup("sample", s))
				m_flexMap.SetAt("sample", nl+s);
			if (m_paraMap.Lookup("comment", s))
				m_flexMap.SetAt("comment", nl+s);
			if (m_paraMap.Lookup("user", s))
				m_flexMap.SetAt("user", nl+s);
			if (m_paraMap.Lookup("workorder", s))
				m_flexMap.SetAt("workorder", nl+s);
			if (m_paraMap.Lookup("project", s))
				m_flexMap.SetAt("project", nl+s);
			if (m_paraMap.Lookup("submitted", s))
				m_flexMap.SetAt("submitted", nl+s);
			if (m_paraMap.Lookup("dataclass", s))
				m_flexMap.SetAt("dataclass", nl+s);
		} else {
			BOOL inforead = FALSE;
			if (hd.ftflag[0]) {
				file.Format("%s\\title", reqfilename);
				inforead = ReadAsciiFile(file, CFile::modeRead);
			} else {
				file = fbase + "\\title";
				inforead = ReadAsciiFile(file, CFile::modeRead);
				if (! inforead) {
					file = fbase + "\\pdata\\1\\title";
					inforead = ReadAsciiFile(file, CFile::modeRead);
				}
			}
			if (inforead) {
				if (! file.Trim().IsEmpty()) {
					CString token, sep="\r\n", nl="\n";
					int i = 0;
					token = file.Tokenize(sep, i).Trim();
					if (! token.IsEmpty()) {
						m_flexMap.SetAt("sample", nl + token);
						token = file.Tokenize(sep, i).Trim();
						if (! token.IsEmpty()) {
							m_flexMap.SetAt("comment", nl + token);
							token = file.Tokenize(sep, i).Trim();
							if (! token.IsEmpty()) {
								m_flexMap.SetAt("user", nl + token);
								token = file.Tokenize(sep, i).Trim();
								if (! token.IsEmpty()) {
									m_flexMap.SetAt("workorder", nl + token);
									token = file.Tokenize(sep, i).Trim();
									if (! token.IsEmpty()) {
										m_flexMap.SetAt("project", nl + token);
										token = file.Tokenize(sep, i).Trim();
										if (! token.IsEmpty()) {
											m_flexMap.SetAt("submitted", nl + token);
											token = file.Tokenize(sep, i).Trim();
											if (! token.IsEmpty())
												m_flexMap.SetAt("dataclass", nl + token);
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	if (hd.dim[1] > 1) {
		CString value = "\n";
		value += arrayname;
		m_flexMap.SetAt("array", value);
		hd.groupnum = hd.dim[1];
	}
	CFile rfile, ifile, sfile;
	if (1 == hd.dimension) {
		if (hd.ftflag[0]) {
			file = reqfilename;
			if (! rfile.Open(file+"\\1r", CFile::modeRead|CFile::typeBinary|CFile::shareDenyNone)) {
				return false;
			}
			if(! ifile.Open(file+"\\1i", CFile::modeRead|CFile::typeBinary|CFile::shareDenyNone)) {
				rfile.Close();
				return false;
			}
		} else {
			if (! sfile.Open(fbase+"\\fid", CFile::modeRead|CFile::typeBinary|CFile::shareDenyNone)) {
				return false;
			}
		}
	} else {
		if (hd.ftflag[0]) {
			file = reqfilename;
			if (! rfile.Open(file+"\\2rr", CFile::modeRead|CFile::typeBinary|CFile::shareDenyNone)) {
				return false;
			}
			if (! ifile.Open(file+"\\2ii", CFile::modeRead|CFile::typeBinary|CFile::shareDenyNone)) {
				rfile.Close();
				return false;
			}
		} else {
			if(! sfile.Open(fbase+"\\ser", CFile::modeRead|CFile::typeBinary|CFile::shareDenyNone)) {
				return false;
			}
		}
	}
	int curpos = 0;
	for (int n=0; n<hd.dim[1]; n++) {
		if (hd.dim[1] > 1) {
			hd.groupcur = n + 1;
			CString arrayvalue = arraystr.Tokenize(" \t\r\n", curpos);
			if (arrayvalue.IsEmpty())
				arrayvalue.Format("\nindex=%i", n);
			else {
				CString tmp = "sequel\n";
				tmp += arrayvalue;
				arrayvalue = tmp;
			}
			m_flexMap.SetAt(arrayname, arrayvalue);
		}
		TMPSPECTRUM *pSpec = new TMPSPECTRUM;
		POSITION pos = m_flexMap.GetStartPosition();
		pSpec->leaf.l = leaf.l;
		pSpec->path = reqfilename;
		memcpy(&pSpec->hd, &hd, sizeof(HD));
		while (pos) {
			CString key, val;
			m_flexMap.GetNextAssoc(pos, key, val);
			CString type = "NULL";
			if ('\n' != val.GetAt(0)) {
				int istart = 0;
				type = val.Tokenize("\n", istart);
				val = val.Tokenize("\n", istart);
			}
			if (! pSpec->fh.Lookup(type, pSpec->fhmap)) {
				pSpec->fhmap = new CMapStringToString;
				pSpec->fh.SetAt(type, pSpec->fhmap);
			}
			pSpec->fhmap->SetAt(key, val.Trim());
		}
		pSpec->dt.resize(pSpec->hd.dim[0]);
		union UNION {
			u_long u;
			int i;
		} re, im;
		if (pSpec->hd.ftflag[0]) {
			float fnorm = powf(2.0, (float)norcon);
			for (i=0; i<pSpec->hd.dim[0]; i++) {
				rfile.Read(&re, sizeof(int));
				ifile.Read(&im, sizeof(int));
				if (bytord==1) {
					re.u = ntohl(re.u);
					im.u = ntohl(im.u);
				}
				pSpec->dt[i] = complex<float>(float(re.i), float(im.i)) * fnorm;
			}
		} else {
			for (i=0; i<pSpec->hd.dim[0]; i++) {
				sfile.Read(&re, sizeof(int));
				sfile.Read(&im, sizeof(int));
				if (bytord==1) {
					re.u = ntohl(re.u);
					im.u = ntohl(im.u);
				}
				pSpec->dt[i] = complex<float>(float(re.i), float(-im.i));
			}
		}
		spectra.AddTail(pSpec);
	}
	m_flexMap.RemoveAll();
	m_paraMap.RemoveAll();
	if (rfile.m_hFile != CFile::hFileNull)
		rfile.Close();
	if (ifile.m_hFile != CFile::hFileNull)
		ifile.Close();
	if (sfile.m_hFile != CFile::hFileNull)
		sfile.Close();
	return true;
}

void CNet12NMRApp::SequelArray(LPCTSTR flag, LPCTSTR name)
{
	for (int i=0; i<(int)strlen(flag); i++) {
		if ('+' == flag[i]) {
			CString parname, parvalue;
			parname.Format("%s%d", name, i);
			if (m_paraMap.Lookup(parname, parvalue)) {
				double x = atof(parvalue.Trim());
				if (0==strcmp(name,"D") || 0==strcmp(name,"P")) {
					if (name[0] == 'D') {
						if (x < 1.0e-3)
							parvalue.Format("sequel\n%gus", x*1.0e6);
						else if (x < 1.0)
							parvalue.Format("sequel\n%gms", x*1.0e3);
						else
							parvalue.Format("sequel\n%gs", x);
					} else {
						if (x > 1.0e6)
							parvalue.Format("sequel\n%gs", x*1.0e-6);
						else if (x > 1.0e3)
							parvalue.Format("sequel\n%gms", x*1.0e-3);
						else
							parvalue.Format("sequel\n%gus", x);
					}
				} else
					parvalue.Format("sequel\n%g", x);
				m_flexMap.SetAt(parname, parvalue);
			}
		}
	}
}

float CNet12NMRApp::LookupPhase(int decim, int dspfvs)
{
	switch(dspfvs) {
		case 10:
			switch(decim) {
				case 2:
					return (float)(179.0 * 180.0 / decim);
				case 3:
					return (float)(201.0 * 180.0 / decim);
				case 4:
					return (float)(533.0 * 180.0 / decim);
				case 6:
					return (float)(709.0 * 180.0 / decim);
				case 8:
					return (float)(1097.0 * 180.0 / decim);
				case 12:
					return (float)(1449.0 * 180.0 / decim);
				case 16:
					return (float)(2225.0 * 180.0 / decim);
				case 24:
					return (float)(2929.0 * 180.0 / decim);
				case 32:
					return (float)(4481.0 * 180.0 / decim);
				case 48:
					return (float)(5889.0 * 180.0 / decim);
				case 64:
					return (float)(8993.0 * 180.0 / decim);
				case 96:
					return (float)(11809.0 * 180.0 / decim);
				case 128:
					return (float)(18017.0 * 180.0 / decim);
				case 192:
					return (float)(23649.0 * 180.0 / decim);
				case 256:
					return (float)(36065.0 * 180.0 / decim);
				case 384:
					return (float)(47329.0 * 180.0 / decim);
				case 512:
					return (float)(72161.0 * 180.0 / decim);
				case 768:
					return (float)(94689.0 * 180.0 / decim);
				case 1024:
					return (float)(144353.0 * 180.0 / decim);
				case 1536:
					return (float)(189409.0 * 180.0 / decim);
				case 2048:
					return (float)(288737.0 * 180.0 / decim);
			}
			break;
		case 11:
		case 12:
			switch(decim) {
				case 2:
					return (float)(184.0 * 180.0 / decim);
				case 3:
					return (float)(219.0 * 180.0 / decim);
				case 4:
					return (float)(384.0 * 180.0 / decim);
				case 6:
					return (float)(602.0 * 180.0 / decim);
				case 8:
					return (float)(852.0 * 180.0 / decim);
				case 12:
					return (float)(1668.0 * 180.0 / decim);
				case 16:
					return (float)(2292.0 * 180.0 / decim);
				case 24:
					return (float)(3368.0 * 180.0 / decim);
				case 32:
					return (float)(4616.0 * 180.0 / decim);
				case 48:
					return (float)(6768.0 * 180.0 / decim);
				case 64:
					return (float)(9264.0 * 180.0 / decim);
				case 96:
					return (float)(13568.0 * 180.0 / decim);
				case 128:
					return (float)(18560.0 * 180.0 / decim);
				case 192:
					return (float)(27392.0 * 180.0 / decim);
				case 256:
					return (float)(36992.0 * 180.0 / decim);
				case 384:
					return (float)(55040.0 * 180.0 / decim);
				case 512:
					return (float)(73856.0 * 180.0 / decim);
				case 768:
					return (float)(110336.0 * 180.0 / decim);
				case 1024:
					return (float)(147584.0 * 180.0 / decim);
				case 1536:
					return (float)(220928.0 * 180.0 / decim);
				case 2048:
					return (float)(295040.0 * 180.0 / decim);
			}
			break;
		case 13:
			switch(decim) {
				case 2:
					return (float)(11.0 * 180.0 / decim);
				case 3:
					return (float)(17.0 * 180.0 / decim);
				case 4:
					return (float)(23.0 * 180.0 / decim);
				case 6:
					return (float)(35.0 * 180.0 / decim);
				case 8:
					return (float)(47.0 * 180.0 / decim);
				case 12:
					return (float)(71.0 * 180.0 / decim);
				case 16:
					return (float)(95.0 * 180.0 / decim);
				case 24:
					return (float)(143.0 * 180.0 / decim);
				case 32:
					return (float)(191.0 * 180.0 / decim);
				case 48:
					return (float)(287.0 * 180.0 / decim);
				case 64:
					return (float)(383.0 * 180.0 / decim);
				case 96:
					return (float)(575.0 * 180.0 / decim);
			}
			break;
	}
	return (float)0.0;
}

void CNet12NMRApp::MapPropFile(CString &file)
{
	m_paraMap.RemoveAll();
	int current = 0;
	CString line = file.Tokenize("\r\n", current);
	while (line.GetLength()) {
		int i = line.Find("=NM=");
		if (i) {
			line = line.Mid(i+4);
			i = line.Find(", NM2=");
			if (i) {
				CString key = line.Left(i);
				line = line.Mid(i+6);
				i = line.Find(", HEIGHT=");
				if (i) {
					CString value = line.Left(i);
					m_paraMap.SetAt(key.MakeLower(), value);
				}
			}
		}
		line = file.Tokenize("\r\n", current);
	}
}

void CNet12NMRApp::OnUpdateFileinfocheck(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(viewfileinfo ? 1 : 0);
}

void CNet12NMRApp::OnFileinfocheck()
{
	viewfileinfo = !viewfileinfo;

	CString entry;
	entry.LoadString(IDC_VIEWFILEINFO);
	WriteProfileInt(settings, entry, viewfileinfo?1:0);

	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, m_pMainWnd);
	if (pMain != pMain->GetActiveFrame()) {
		CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->GetActiveFrame());
		CNet12NMRDoc *pDoc = STATIC_DOWNCAST(CNet12NMRDoc, pChild->GetActiveDocument());
		pDoc->UpdateAllViews(NULL);
	}
}

void CNet12NMRApp::OnUpdateViewparameterscheck(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(viewparameters ? 1 : 0);
}

void CNet12NMRApp::OnViewparameterscheck()
{
	viewparameters = !viewparameters;

	CString entry;
	entry.LoadString(IDC_VIEWPARAMETERS);
	WriteProfileInt(settings, entry, viewparameters?1:0);

	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, m_pMainWnd);
	if (pMain != pMain->GetActiveFrame()) {
		CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->GetActiveFrame());
		CNet12NMRDoc *pDoc = STATIC_DOWNCAST(CNet12NMRDoc, pChild->GetActiveDocument());
		pDoc->UpdateAllViews(NULL);
	}
}

void CNet12NMRApp::OnUpdateBaseshelfcountedit(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(FALSE);
}

void CNet12NMRApp::OnUpdatePeakcountshelfedit(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(FALSE);
}

void CNet12NMRApp::OnUpdateIntegralshelfcountedit(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(FALSE);
}

void CNet12NMRApp::OnLoadshelfbutton()
{
	CFileDialog dlg(TRUE, "nmp", NULL, OFN_HIDEREADONLY, "Net12NMRParameters (*.nmp)|*nmp||");
	if (IDOK == dlg.DoModal()) {
		// remove existing shelf
		shelf.RemoveAll();
		shelf.Read(dlg.GetPathName());
	}
}

void CNet12NMRApp::OnUpdateSaveshelfbutton(CCmdUI *pCmdUI)
{
	pCmdUI->Enable((shelf.HasBaseline()||shelf.HasPeaks()||shelf.HasIntegrals()) ? TRUE : FALSE);
}

void CNet12NMRApp::OnSaveshelfbutton()
{
	CFileDialog fileDlg(FALSE, "nmp", "ShelfParameters", OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, "NetNMR Parameters (*.nmp)|*.nmp||");
	if (IDOK == fileDlg.DoModal())
		shelf.Write(fileDlg.GetPathName());
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	GetDlgItem(IDC_NET12NMR_VERSION)->SetWindowText(theApp.AppVersion);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CNet12NMRApp::OnAboutbutton()
{
	CAboutDlg dlg;
	dlg.DoModal();
}

void CNet12NMRApp::OnApotypecombo()
{
	CMFCRibbonBar *pRibbon = ((CMDIFrameWndEx *)AfxGetMainWnd())->GetRibbonBar();
	ASSERT_VALID(pRibbon);
	CMFCRibbonComboBox *pApoType = DYNAMIC_DOWNCAST(CMFCRibbonComboBox, pRibbon->FindByID(ID_APOTYPECOMBO));
	apoType = (0 == pApoType->GetCurSel()) ? lorenApoType : gaussApoType;
}

void CNet12NMRApp::OnApounitscombo()
{
	CMFCRibbonBar *pRibbon = ((CMDIFrameWndEx *)AfxGetMainWnd())->GetRibbonBar();
	ASSERT_VALID(pRibbon);
	CMFCRibbonComboBox *pApoUnits = DYNAMIC_DOWNCAST(CMFCRibbonComboBox, pRibbon->FindByID(ID_APOUNITSCOMBO));
	apoUnits = (0 == pApoUnits->GetCurSel()) ? hertzApoUnits : ppmApoUnits;
}

void CNet12NMRApp::OnApovaluecombo()
{
	CMFCRibbonBar *pRibbon = ((CMDIFrameWndEx *)AfxGetMainWnd())->GetRibbonBar();
	ASSERT_VALID(pRibbon);
	CMFCRibbonComboBox *pApoValue = DYNAMIC_DOWNCAST(CMFCRibbonComboBox, pRibbon->FindByID(ID_APOVALUECOMBO));
	//apoValue = atof(pApoValue->GetItem(pApoValue->GetCurSel()));
	apoValue = atof(pApoValue->GetEditText());
}

void CNet12NMRApp::OnUpdateTencheck(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck((tenPhaseIncrement==phaseIncrement) ? 1 : 0);
}

void CNet12NMRApp::OnTencheck()
{
	phaseIncrement = tenPhaseIncrement;
}

void CNet12NMRApp::OnUpdateOnecheck(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck((onePhaseIncrement==phaseIncrement) ? 1 : 0);
}

void CNet12NMRApp::OnOnecheck()
{
	phaseIncrement = onePhaseIncrement;
}

void CNet12NMRApp::OnUpdateTenthcheck(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck((tenthPhaseIncrement==phaseIncrement) ? 1 : 0);
}

void CNet12NMRApp::OnTenthcheck()
{
	phaseIncrement = tenthPhaseIncrement;
}

void CNet12NMRApp::OnUpdateAcheck(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck((constPhaseTerm==phaseTerm) ? 1 : 0);
}

void CNet12NMRApp::OnAcheck()
{
	phaseTerm = constPhaseTerm;
}

void CNet12NMRApp::OnUpdateBcheck(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck((linearPhaseTerm==phaseTerm) ? 1 : 0);
}

void CNet12NMRApp::OnBcheck()
{
	phaseTerm = linearPhaseTerm;
}

void CNet12NMRApp::OnFileOpen()
{
	// I don't understand why a currently active document is not getting a deactive message,
	// but it's not, so I have to tell it to save it's state info!
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, m_pMainWnd);
	CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->MDIGetActive());
	if (pChild) {
		CNet12NMRDoc *pDoc = STATIC_DOWNCAST(CNet12NMRDoc, pChild->GetActiveDocument());
		pDoc->SaveState();
	}
	CString filename;
	CFileDialog nmrfile(TRUE, "nmrx", NULL, OFN_FILEMUSTEXIST|OFN_ALLOWMULTISELECT|OFN_HIDEREADONLY, 
		"All NMR Files (*.nmrx;*.nmr)|*.nmr;*.nmrx|Net12NMR Files (*.nmrx)|*.nmrx|NetNMR Files (*.nmr)|*.nmr||");
	if (IDOK == nmrfile.DoModal()) {
		POSITION pos = nmrfile.GetStartPosition();
		while (pos) {
			CString path = nmrfile.GetNextPathName(pos);
			Leaf leaf;
			leaf.l = NULL;
			leaf.s.dimension = simple;
			leaf.s.drivenumber = path[0] - 'A' + 1;
			leaf.s.leaftype = (0 == path.Right(5).CompareNoCase(".nmrx")) ? net12nmrfile : netnmrfile;
			leaf.s.location = shared;
			ImportNMRFile(path, leaf);
		}
	}
}

void CNet12NMRApp::OnUpdateClearbaseshelfbutton(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(shelf.HasBaseline() ? TRUE : FALSE);
}

void CNet12NMRApp::OnClearbaseshelfbutton()
{
	shelf.RemoveBaseline();
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, this->m_pMainWnd);
	shelf.RibbonUpdate(pMain->m_wndRibbonBar);
}

void CNet12NMRApp::OnUpdateClearpeakshelfbutton(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(shelf.HasPeaks() ? TRUE : FALSE);
}

void CNet12NMRApp::OnClearpeakshelfbutton()
{
	shelf.RemovePeaks();
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, this->m_pMainWnd);
	shelf.RibbonUpdate(pMain->m_wndRibbonBar);
}

void CNet12NMRApp::OnUpdateClearintegralshelfbutton(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(shelf.HasIntegrals() ? TRUE : FALSE);
}

void CNet12NMRApp::OnClearintegralshelfbutton()
{
	shelf.RemoveIntegrals();
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, this->m_pMainWnd);
	shelf.RibbonUpdate(pMain->m_wndRibbonBar);
}

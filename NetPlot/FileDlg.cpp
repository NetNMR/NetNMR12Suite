// FileDlg.cpp : implementation file

#include "stdafx.h"
#include "..\NMRDataTypes.h"
#include "Spectrum.h"
#include "NetPlot.h"
#include "MainFrame.h"
#include "ChildFrame.h"
#include "NetPlotDoc.h"
#include "NMRServer.h"
#include "FileDlg.h"
#include "ProcessWnd.h"

#include <direct.h>
#include ".\filedlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileDlg dialog

extern CNetPlotApp theApp;

CFileDlg::CFileDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFileDlg::IDD, pParent)
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->MDIGetActive());
	pDoc = STATIC_DOWNCAST(CNetPlotDoc, pChild->GetActiveDocument() );
}


void CFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DRIVECOMBO, m_DriveList);
	DDX_Control(pDX, IDC_REMOVEALL, m_RemoveAll);
	DDX_Control(pDX, IDC_REMOVE, m_Remove);
	DDX_Control(pDX, IDC_MOVEUP, m_MoveUp);
	DDX_Control(pDX, IDC_MOVEDOWN, m_MoveDown);
	DDX_Control(pDX, IDC_FILELIST, m_FileList);
	DDX_Control(pDX, IDC_CURRENTPATH, m_CurrentPath);
	DDX_Control(pDX, IDC_DIRLIST, m_DirList);
	DDX_Control(pDX, IDC_SERVERLIST, m_ServerList);
	DDX_Control(pDX, IDC_ADDBUTTON, m_addButton);
	DDX_Control(pDX, IDC_REVERSE, m_Reverse);
}


BEGIN_MESSAGE_MAP(CFileDlg, CDialog)
	ON_LBN_SELCHANGE(IDC_DIRLIST, OnLbnSelchangeDirlist)
	ON_LBN_SELCHANGE(IDC_FILELIST, OnSelchangeFilelist)
	ON_LBN_SELCHANGE(IDC_SERVERLIST, OnSelchangeServerlist)
	ON_CBN_SELCHANGE(IDC_DRIVECOMBO, OnSelchangeDriveCombo)
	ON_LBN_DBLCLK(IDC_DIRLIST, OnBnClickedAddbutton)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_REMOVEALL, OnRemoveall)
	ON_BN_CLICKED(IDC_MOVEDOWN, OnMovedown)
	ON_BN_CLICKED(IDC_MOVEUP, OnMoveup)
	ON_BN_CLICKED(IDC_ADDBUTTON, OnBnClickedAddbutton)
	ON_BN_CLICKED(IDC_REVERSE, OnBnClickedReverse)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileDlg message handlers

BOOL CFileDlg::OnInitDialog() 
{
	CWaitCursor wait;
	CDialog::OnInitDialog();

	// Fill the drivelist
	for (int i=0; i<26; i++) {
		CString str;
		str.Format("%c:\\", i + 'A');
		if ( 1 < GetDriveType(str) ) {
			str.Format("[-%c-]", i + 'A');
			m_DriveList.SetItemData( m_DriveList.AddString(str), i+1 );
		}
	}

	// Fill the server list
	for (int i=0; strcmp(theApp.Server.Name(i), NMRSERVERNOTFOUND); i++) {
		if (0 == strcmp(theApp.Server.Name(i), "My Personal Server")) {
			if (CWnd::FindWindow(NULL, "ConsoleServer"))
				m_ServerList.AddString(theApp.Server.Name(i));
		} else
			m_ServerList.AddString(theApp.Server.Name(i));
	}
	currentname = "Local .nmr Files";
	m_ServerList.SetCurSel(m_ServerList.AddString( currentname));
	FillDirList();

	// FILL THE FILE LIST
	POSITION pos = pDoc->Spectra.GetHeadPosition();
	if ( pos )
		m_RemoveAll.EnableWindow();
	while ( pos ) {
		CSpectrum *pSpec = pDoc->Spectra.GetNext( pos );
		m_FileList.SetItemDataPtr( m_FileList.AddString( pSpec->pathname ), pSpec);
	}
	if ( 1 < m_FileList.GetCount() )
		m_Reverse.EnableWindow();
	SetHExtFileList();
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CFileDlg::FillDirList()
{
	CWaitCursor wait;
	m_DirList.ResetContent();
	m_addButton.EnableWindow(FALSE);
	int nserv = theApp.Server.Find(currentname);
	if ( nserv >= 0 ) {
		m_DriveList.SetCurSel(-1);
		m_DriveList.EnableWindow( false );
		if ( ! NMRPort.IsConnected() )
			return;
		CString buf( "l\n" );
		NMRPort.SendString( buf );
		char *p = NMRPort.GetSockString();
		while ( strcmp( p, "." ) ) {
			char c = p[strlen(p)-1];
			m_DirList.SetItemData( m_DirList.AddString( p ),
		 		( '/'==c || '\\'==c ) ? _A_SUBDIR : NULL );
			p = NMRPort.GetSockString();
		}
	} else {
		char path[_MAX_PATH];
		_getcwd( path, _MAX_PATH );
		m_CurrentPath.SetWindowText( path );
		m_DriveList.EnableWindow( true );
		CString drive;
		drive.Format("[-%c-]", path[0]);
		m_DriveList.SetCurSel( m_DriveList.FindString( -1, drive ) );
		CFileFind entry;
		BOOL isentry = entry.FindFile();
		while (isentry) {
			isentry = entry.FindNextFile();
			if (entry.MatchesMask(FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_TEMPORARY) || 0==entry.GetFileName().Compare("."))
				continue;
			if (entry.IsDirectory()) {
				m_DirList.SetItemData(m_DirList.AddString(entry.GetFileName() + '\\'), (DWORD)_A_SUBDIR);
			} else {
				if (0==entry.GetFileName().Right(4).CompareNoCase(".nmr") || 0==entry.GetFileName().Right(5).CompareNoCase(".nmrx"))
					m_DirList.SetItemData(m_DirList.AddString(entry.GetFileName()), (DWORD)0);
			}
		}
		entry.Close();
	}
}

void CFileDlg::OnSelchangeFilelist() 
{
	arraySelection.RemoveAll();
	int count = m_FileList.GetCount();
	int selec = m_FileList.GetSelCount();
	arraySelection.SetSize(selec);
	m_FileList.GetSelItems(selec, arraySelection.GetData());
	m_Reverse.EnableWindow( (count>1)?TRUE:FALSE );
	m_Remove.EnableWindow( (selec>0)?TRUE:FALSE );
	m_RemoveAll.EnableWindow( (count>0)?TRUE:FALSE );
	if (selec > 0) {
		if (arraySelection[selec-1]-arraySelection[0]+1 == selec) {
			// THE SELECTION IS CONTIGUOUS
			m_MoveUp.EnableWindow( (arraySelection[0] > 0)?TRUE:FALSE );
			m_MoveDown.EnableWindow( (count-1 > arraySelection[selec-1])?TRUE:FALSE );
		} else {
			m_MoveUp.EnableWindow(FALSE);
			m_MoveDown.EnableWindow(FALSE);
		}
	} else {
		m_MoveUp.EnableWindow(FALSE);
		m_MoveDown.EnableWindow(FALSE);
	}
}

void CFileDlg::OnRemove()
{
	for (INT_PTR i=arraySelection.GetCount()-1; i>=0; i--)
		m_FileList.DeleteString( arraySelection[i] );
	SetHExtFileList();
	OnSelchangeFilelist();
}

void CFileDlg::OnRemoveall() 
{
	m_FileList.ResetContent();
	SetHExtFileList();
	OnSelchangeFilelist();
}

void CFileDlg::OnMoveup() 
{
	int i = arraySelection[0] - 1;
	CString str;
	m_FileList.GetText( i, str );
	CSpectrum *pSpec = (CSpectrum *)m_FileList.GetItemDataPtr( i );
	m_FileList.DeleteString( i );
	i = arraySelection[arraySelection.GetCount() - 1];
	if ( i >= m_FileList.GetCount() )
		i = -1;
	m_FileList.SetItemDataPtr(m_FileList.InsertString(i, str), pSpec);
	OnSelchangeFilelist();
}

void CFileDlg::OnMovedown() 
{
	int i = arraySelection[arraySelection.GetCount() - 1] + 1;
	CString str;
	m_FileList.GetText(i, str);
	CSpectrum *pSpec = (CSpectrum *)m_FileList.GetItemDataPtr( i );
	m_FileList.DeleteString( i );
	i = arraySelection[0];
	m_FileList.SetItemDataPtr(m_FileList.InsertString(i, str), pSpec);
	OnSelchangeFilelist();
}

//void CFileDlg::MoveString(int direction)
//{
//	int i = m_FileList.GetCurSel();
//	CString str;
//	m_FileList.GetText( i, str );
//	CSpectrum *pSpec = (CSpectrum *)m_FileList.GetItemDataPtr( i );
//	m_FileList.DeleteString( i );
//	i += direction;
//	if ( i >= m_FileList.GetCount() )
//		i = -1;
//	int n = m_FileList.InsertString( i, str );
//	m_FileList.SetItemDataPtr( n, pSpec );
//	m_FileList.SetCurSel( n );
//	OnSelchangeFilelist();
//}

void CFileDlg::OnBnClickedReverse()
{
	int count = m_FileList.GetCount();
	if ( count > 1 ) {
		for ( int i=1; i<count; i++ ) {
			CString str;
			m_FileList.GetText( i, str );
			CSpectrum *pSpec = (CSpectrum *)m_FileList.GetItemDataPtr( i );
			m_FileList.DeleteString( i );
			m_FileList.InsertString( 0, str );
			m_FileList.SetItemDataPtr( 0, pSpec );
		}
	}
}

void CFileDlg::OnOK() 
{
	// FIRST, SEE IF THE FILELIST HAS CHANGED
	bool IsIdentical = TRUE;
	int count = m_FileList.GetCount();
	int i;
	if (pDoc->Spectra.GetCount() == count) {
		POSITION pos = pDoc->Spectra.GetHeadPosition();
		for ( int i=0; i<count; i++ ) {
			CString s;
			m_FileList.GetText( i, s );
			// CHECK FOR NULL POINTER, OR A DIFFERENT ORDER
			if ( NULL==m_FileList.GetItemDataPtr(i) || s.Compare( pDoc->Spectra.GetNext( pos )->pathname ) ) {
				IsIdentical = FALSE;
				break;
			}
		}
	} else
		IsIdentical = FALSE;
	
	if ( ! IsIdentical ) {
		pDoc->WhatChanged |= FILECHANGE | RANGECHANGE;
		// DELETE THE SPECTRA WE NO LONGER NEED
		POSITION pos = pDoc->Spectra.GetHeadPosition();
		while ( pos ) {
			CSpectrum *pSpec = pDoc->Spectra.GetNext( pos );
			for ( i=0; i<count; i++ ) {
				if ( m_FileList.GetItemDataPtr( i ) == pSpec )
					break;
			}
			if ( i >= count )
				delete pSpec;
		}
		// THEN REMOVE THE OLD LIST OF SPECTRA
		pDoc->Spectra.RemoveAll();

		// NEXT, BUILD THE NEW SPECTRUM LIST
		// ADD ANY NEW SPECTRA
		UINT cb=0;
		double sw=0.0, sf=0.0, of=0.0;
		for ( int i=0; i<count; i++ ) {
			CSpectrum *pSpec = (CSpectrum *)m_FileList.GetItemDataPtr(i);
			if (pSpec) {
				pDoc->Spectra.AddTail(pSpec);
			} else {
				CString str;
				m_FileList.GetText(i, str);
				pDoc->AddSpectrum(str);
			}
		}

		// UPDATE THE FROM - TO PPM AND HERTZ SPAN
		// ALSO CHECK FOR IDENTICAL WINDOW
		int count = 0;
		pDoc->AllTheSame = true;
		pos = pDoc->Spectra.GetHeadPosition();
		while (pos) {
			CSpectrum *pSpec = pDoc->Spectra.GetNext(pos);
			if (count++) {
				float ppmfrom, ppmto;
				pSpec->GetFromToRange(ppmfrom, ppmto);
				float hzfrom = ppmfrom * (float)pSpec->Frequency();
				float hzto = ppmto * (float)pSpec->Frequency();
				if (ppmfrom > pDoc->PPMFrom)
					pDoc->PPMFrom = ppmfrom;
				if (ppmto < pDoc->PPMTo)
					pDoc->PPMTo = ppmto;
				if (hzfrom > pDoc->HertzFrom)
					pDoc->HertzFrom = hzfrom;
				if (hzto < pDoc->HertzTo)
					pDoc->HertzTo = hzto;
				if (cb!=pSpec->Size() || sw!=pSpec->SweepWidth() || sf!=pSpec->Frequency() || of!=pSpec->Offset())
					pDoc->AllTheSame = false;
			} else {
				pSpec->GetFromToRange(pDoc->PPMFrom, pDoc->PPMTo);
				pDoc->HertzFrom = pDoc->PPMFrom * (float)pSpec->Frequency();
				pDoc->HertzTo = pDoc->PPMTo * (float)pSpec->Frequency();
				cb = pSpec->Size();
				sw = pSpec->SweepWidth();
				sf = pSpec->Frequency();
				of = pSpec->Offset();
			}
		}
		if (pDoc->AllTheSame) {
			if (cb && cb!=pDoc->bIntegral.size()) {
				pDoc->bIntegral.resize(cb);
				for (UINT i=0; i<cb; i++)
					pDoc->bIntegral[i] = false;
			}
		} else
			pDoc->bIntegral.resize(0);
		// Spectrum list has changed, so
		// reinitialization the filed based data.
		CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
		if (pMain->dialogFrame && pMain->dialogFrame->IsWindowVisible())
			pMain->dialogFrame->UpdateContent();
	}

	// CLOSE ANY OPEN CONNECTIONS
	if ( currentname.Compare("Local .nmr Files") && NMRPort.IsConnected() )
		NMRPort.Close();

	CDialog::OnOK();
}


void CFileDlg::OnCancel() 
{
	if ( currentname.Compare("Local .nmr Files") && NMRPort.IsConnected() ) {
		NMRPort.Close();
	}
	CDialog::OnCancel();
}

void CFileDlg::OnSelchangeServerlist() 
{
	// Close any open connection
	if ( currentname.Compare("Local .nmr Files") && NMRPort.IsConnected() )
		NMRPort.Close();

	// Set the new server name
	m_ServerList.GetText( m_ServerList.GetCurSel(), currentname );

	// And open the new connection
	int i = theApp.Server.Find(currentname);
	if (i>=0) {
		if ( NMRPort.MakeConnection(currentname) ) {
			CString buf;
			buf.Format( "/%s\n", theApp.Server.Path(i));
			NMRPort.SendString( buf );
			char *pstr = NMRPort.GetSockString();
			m_CurrentPath.SetWindowText( pstr );
			theApp.Server.SetPath(i, pstr);
		}
	}
	FillDirList();
}

void CFileDlg::OnSelchangeDriveCombo() 
{
	_chdrive((int)m_DriveList.GetItemData(m_DriveList.GetCurSel()));
	FillDirList();	
}

void CFileDlg::SetHExtFileList(void)
{
	// Find the longest string in the list box.
	CString str;
	CSize sz;
	int dx = 0;
	TEXTMETRIC tm;
	CDC *pDC = m_FileList.GetDC();
	CFont *pFont = m_FileList.GetFont();

	// Select the listbox font, save the old font
	CFont* pOldFont = pDC->SelectObject(pFont);
	// Get the text metrics for avg char width
	pDC->GetTextMetrics(&tm); 

	for (int i = 0; i < m_FileList.GetCount(); i++) {
		m_FileList.GetText(i, str);
		sz = pDC->GetTextExtent(str);
		// Add the avg width to prevent clipping
		sz.cx += tm.tmAveCharWidth;
		if (sz.cx > dx) dx = sz.cx;
	}
	// Select the old font back into the DC
	pDC->SelectObject(pOldFont);
	m_FileList.ReleaseDC(pDC);

	// Set the horizontal extent so every character of all strings 
	// can be scrolled to.
	m_FileList.SetHorizontalExtent(dx);
}

void CFileDlg::OnBnClickedAddbutton()
{
	if (m_addButton.IsWindowEnabled()) {
		int count = m_DirList.GetSelCount();
		int *list = new int[count];
		if (list) {
			m_DirList.GetSelItems(count, list);
			int nserv = theApp.Server.Find(currentname);
			if (nserv >= 0){
				CString scmd, sbuf;
				if (_A_SUBDIR == m_DirList.GetItemData(list[0])) {
					m_DirList.GetText(list[0], sbuf);
					scmd.Format("/%s\n", sbuf);
					NMRPort.SendString(scmd);
					sbuf = NMRPort.GetSockString();
					m_CurrentPath.SetWindowText(sbuf);
					theApp.Server.SetPath(nserv, sbuf);
					FillDirList();
					m_addButton.EnableWindow(FALSE);
				} else {
					for(int i=0; i<count; i++){
						m_DirList.GetText(list[i], sbuf);
						scmd.Format("%s:%s%s", currentname, theApp.Server.Path(nserv), sbuf);
						m_FileList.SetItemDataPtr(m_FileList.AddString(scmd), NULL);
					}
					SetHExtFileList();
				}
			}else {
				CString listname;
				if (_A_SUBDIR == m_DirList.GetItemData(list[0])) {
					m_DirList.GetText(list[0], listname);
					_chdir(listname);
					FillDirList();
				} else {
					CString path;
					_getcwd(path.GetBuffer(_MAX_PATH), _MAX_PATH);
					path.ReleaseBuffer();
					if (path.Right(1).Compare("\\"))
						path += '\\';
					for (int i=0; i<count; i++) {
						m_DirList.GetText(list[i], listname);
						CString fullname = path + listname;
						m_FileList.SetItemDataPtr(m_FileList.AddString(fullname), NULL);
					}
					SetHExtFileList();
				}
			}
			delete [] list;
		}
	}
	OnSelchangeFilelist();
}

void CFileDlg::OnLbnSelchangeDirlist()
{
	int count = m_DirList.GetSelCount();
	if (count) {
		int *list = new int[count];
		if (list) {
			int i;
			m_DirList.GetSelItems(count, list);
			for (i=0; i<count; i++) {
					if (_A_SUBDIR == m_DirList.GetItemData(list[i]))
						break;
			}
			if (1 == count) {
				if (i)
					m_addButton.SetWindowText("Add");
				else
					m_addButton.SetWindowText("Goto");
				m_addButton.EnableWindow(TRUE);
			} else {
				if (i==count) {
					m_addButton.SetWindowText("Add");
					m_addButton.EnableWindow(TRUE);
				} else
					m_addButton.EnableWindow(FALSE);
			}
			delete [] list;
		} else
			m_addButton.EnableWindow(FALSE);
	} else
		m_addButton.EnableWindow(FALSE);
}


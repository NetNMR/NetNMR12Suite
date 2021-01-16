// AddSpectraDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NetSpan.h"
#include "AddSpectraDlg.h"
#include "..\NMRDataTypes.h"
#include "MainFrame.h"
#include "NetSpanDoc.h"


// CAddSpectraDlg dialog

IMPLEMENT_DYNAMIC(CAddSpectraDlg, CDialog)
CAddSpectraDlg::CAddSpectraDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddSpectraDlg::IDD, pParent)
{
}

CAddSpectraDlg::~CAddSpectraDlg()
{
}

void CAddSpectraDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PATHSTATIC, m_pathStatic);
	DDX_Control(pDX, IDC_DRIVESTATIC, m_driveStatic);
	DDX_Control(pDX, IDC_SERVERLIST, m_serverList);
	DDX_Control(pDX, IDC_DRIVELIST, m_driveList);
	DDX_Control(pDX, IDC_DIRECTORYLIST, m_directoryList);
	DDX_Control(pDX, IDC_FILELIST, m_fileList);
	DDX_Control(pDX, IDC_GOTOBUTTON, m_gotoButton);
	DDX_Control(pDX, IDOK, m_okButton);
}


BEGIN_MESSAGE_MAP(CAddSpectraDlg, CDialog)
	ON_LBN_SELCHANGE(IDC_SERVERLIST, OnLbnSelchangeServerlist)
	ON_LBN_SELCHANGE(IDC_DRIVELIST, OnLbnSelchangeDrivelist)
	ON_LBN_SELCHANGE(IDC_DIRECTORYLIST, OnLbnSelchangeDirectorylist)
	ON_LBN_SELCHANGE(IDC_FILELIST, OnLbnSelchangeFilelist)
	ON_LBN_DBLCLK(IDC_DIRECTORYLIST, OnLbnDblclkDirectorylist)
	ON_LBN_DBLCLK(IDC_FILELIST, OnLbnDblclkFilelist)
	ON_BN_CLICKED(IDC_GOTOBUTTON, OnBnClickedGotobutton)
END_MESSAGE_MAP()


// CAddSpectraDlg message handlers

BOOL CAddSpectraDlg::OnInitDialog()
{
	CWaitCursor wait;
	CDialog::OnInitDialog();

	// Fill the drivelist
	for (int i=0; i<26; i++) {
		CString str;
		str.Format("%c:\\", i + 'A');
		if ( 1 < GetDriveType(str) ) {
			str.Format("[-%c-]", i + 'A');
			m_driveList.SetItemData( m_driveList.AddString(str), i+1 );
		}
	}

	// Fill the server list
	for(int i=0; strcmp(theApp.Server.Name(i), NMRSERVERNOTFOUND); i++) {
		if (0 == strcmp(theApp.Server.Name(i), "My Personal Server")) {
			if (CWnd::FindWindow(NULL, "ConsoleServer"))
				m_serverList.AddString(theApp.Server.Name(i));
		} else
			m_serverList.AddString(theApp.Server.Name(i));
	}
	m_serverList.SetCurSel(m_serverList.AddString("Local .nmr Files"));
	FillDirList();

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CAddSpectraDlg::FillDirList()
{
	CWaitCursor wait;
	m_directoryList.ResetContent();
	m_fileList.ResetContent();
	m_gotoButton.EnableWindow(FALSE);
	m_okButton.EnableWindow(FALSE);
	if (theApp.nserv >= 0) {
		m_driveList.SetCurSel(-1);
		m_driveList.ShowWindow(SW_HIDE);
		m_driveStatic.ShowWindow(SW_HIDE);
		if (! nmrClient.bConnected)
			return;
		CString buf("L\n");
		nmrClient.SendString(buf);
		nmrClient.GetSockString(buf);
		while (buf.Compare(".")) {
			CString lstr = buf.Right(1);
			if (lstr=="\\" || lstr=="/")
				m_directoryList.AddString(buf);
			else
				m_fileList.AddString(buf);
			nmrClient.GetSockString(buf);
		}
	} else {
		CString path;
		_getcwd(path.GetBuffer(_MAX_PATH), _MAX_PATH);
		path.ReleaseBuffer();
		m_pathStatic.SetWindowText(path);
		m_driveList.ShowWindow(SW_SHOW);
		m_driveStatic.ShowWindow(SW_SHOW);
		CString drive;
		drive.Format("[-%c-]", path[0]);
		m_driveList.SetCurSel(m_driveList.FindString(-1, drive));
		struct _finddata_t finfo;
		intptr_t fhandle = _findfirst("*.*", &finfo);
		do {
			CString name(finfo.name);
			if (name.Compare(".")) {
				if (name.Compare("..")) {
					if (finfo.attrib & _A_SUBDIR) {
						name += '\\';
						m_directoryList.AddString(name);
					} else {
						if (0 == name.Right(4).CompareNoCase(".nmr")) {
							CFile file;
							if (file.Open(name, CFile::modeRead | CFile::shareDenyWrite)) {
								HD hd;
								file.Read(&hd, sizeof(hd));
								if (hd.ftflag[0])
									m_fileList.AddString(name);
								file.Close();
							}
						} else if (0 == name.Right(5).CompareNoCase(".nmrx")) {
							CFile file;
							if (file.Open(name, CFile::modeRead | CFile::shareDenyWrite)) {
								CArchive ar(&file, CArchive::load);
								CString version;
								ar.ReadString(version);
								ar.ReadString(version);
								if (0 == version.Right(1).Compare("A")) {
									for (int i=0; i<5; i++)
										ar.ReadString(version);
									SPECTRUMPARAMETERS sp;
									ar.Read(&sp, sizeof(SPECTRUMPARAMETERS));
									if (sp.ftflag)
										m_fileList.AddString(name);
								}
								ar.Close();
								file.Close();
							}
						} else if (0 == name.Right(4).CompareNoCase(".npl"))
							m_fileList.AddString(name);
					}
				} else {
					if (path.Right(1).Compare("\\"))
						m_directoryList.AddString( "..\\" );
				}
			}
		} while (0 == _findnext(fhandle, &finfo));
		_findclose(fhandle);
	}
}

void CAddSpectraDlg::OnLbnSelchangeServerlist()
{
	// Close any open connection
	if (theApp.nserv>=0 && nmrClient.bConnected)
		nmrClient.BreakConnection();

	// Set the selected server
	CString name;
	m_serverList.GetText(m_serverList.GetCurSel(), name);
	theApp.nserv = theApp.Server.Find(name);

	// And open the new connection
	if (theApp.nserv >= 0) {
		if (nmrClient.MakeConnection()) {
			CString buf;
			buf.Format("/%s\n", theApp.Server.Path(theApp.nserv));
			nmrClient.SendString(buf);
			nmrClient.GetSockString(buf);
			m_pathStatic.SetWindowText(buf);
			theApp.Server.SetPath(theApp.nserv, buf);
		}
	}
	FillDirList();
}

void CAddSpectraDlg::OnLbnSelchangeDrivelist()
{
	_chdrive((int)m_driveList.GetItemData(m_driveList.GetCurSel()));
	FillDirList();
}

void CAddSpectraDlg::OnLbnDblclkFilelist()
{
	OnOK();
}

void CAddSpectraDlg::OnLbnDblclkDirectorylist()
{
	OnBnClickedGotobutton();
}

void CAddSpectraDlg::OnLbnSelchangeDirectorylist()
{
	m_fileList.SetSel(-1, FALSE);
	m_okButton.EnableWindow(FALSE);
	m_gotoButton.EnableWindow(TRUE);
}

void CAddSpectraDlg::OnLbnSelchangeFilelist()
{
	m_directoryList.SetCurSel(-1);
	m_gotoButton.EnableWindow(FALSE);
	m_okButton.EnableWindow(m_fileList.GetSelCount()>0);
}

void CAddSpectraDlg::OnBnClickedGotobutton()
{
	CString dstr;
	m_directoryList.GetText(m_directoryList.GetCurSel(), dstr);
	if (theApp.nserv >= 0) {
		CString cmd;
		cmd.Format("/%s\n", dstr);
		nmrClient.SendString(cmd);
		nmrClient.GetSockString(dstr);
		m_pathStatic.SetWindowText(dstr);
	} else
		_chdir(dstr);
	FillDirList();
}

void CAddSpectraDlg::OnOK()
{
	CWaitCursor wait;
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, theApp.m_pMainWnd);
	CNetSpanDoc *pDoc = STATIC_DOWNCAST(CNetSpanDoc, pMain->GetActiveDocument());
	if (pDoc->cols) {
		np = pDoc->rows;
		sw = pDoc->xsw;
		sf = pDoc->sf;
		offset = pDoc->xoffset;
	} else {
		np = 0;
		sf = sw = offset = 0.0;
	}
	// This should already be empty!
	while (Spectra.GetCount())
		delete Spectra.RemoveHead();

	int n = m_fileList.GetSelCount();
	int *items = new int[n];
	m_fileList.GetSelItems(n, items);
	if (items) {
		if (theApp.nserv >= 0) {
			// Data is from NMRServer
			for (int i=0; i<n; i++) {
				CString file;
				m_fileList.GetText(items[i], file);
				CString buf;
				buf.Format("f%s\n", file);
				nmrClient.SendString(buf);
				HD hd;
				CMapStringToString fh;
				do {
					nmrClient.ReadSocket(&hd, sizeof(HD));
					nmrClient.ByteReverseHeader(&hd);
					nmrClient.GetSockString(buf);
					int len = atoi(buf);
					while (len) {
						char *pName = buf.GetBuffer(len);
						nmrClient.ReadSocket(pName, len);
						char *pValue = pName + strlen(pName) + 1;
						pValue += strlen(pValue) + 1;
						fh.SetAt(pName, pValue);
						buf.ReleaseBuffer();
						nmrClient.GetSockString(buf);
						len = atoi(buf);
					}
					vector<complex<float>> dt;
					dt.resize(hd.dim[0]);
					nmrClient.ReadSocket(dt.data(), sizeof(float)*2*hd.dim[0]);
					nmrClient.ByteReverseData(dt, hd.dim[0]);
					if (0 == np) {
						np = hd.dim[0];
						sf = hd.dim_freq[0];
						sw = hd.spec_width[0] / sf;
						offset = (0.5 * hd.spec_width[0] + hd.of[0]) / sf;
						pDoc->sfstr.Format("sf=%g", hd.dim_freq[0]);
						if (fh.Lookup("dosygamma", buf))
							pDoc->gammastr.Format("gamma=%s", buf);
						if (fh.Lookup("timecubed", buf))
							pDoc->tcubedstr.Format("tcubed=%s", buf);
						if (fh.Lookup("DAC_to_G", buf))
							pDoc->dactogstr.Format("dactog=%s", buf);
					}
					if (np==hd.dim[0] && sf==hd.dim_freq[0] && sw==hd.spec_width[0]/hd.dim_freq[0] && offset==(0.5*hd.spec_width[0]+hd.of[0])/hd.dim_freq[0]) {
						CSpectrum *pSpec = new CSpectrum;
						if (pSpec) {
							pSpec->obs.SetSize(np);
							pSpec->calc.SetSize(np);
							pSpec->diff.SetSize(np);
							pSpec->singleline.SetSize(np);
							pSpec->SetBaseline(0.0, 0.0, 0.0, 0.0);
							for (int j=0; j<np; j++)
								pSpec->obs[j] = (double)dt[j].real();
							if (hd.groupnum)
								pSpec->label.Format("%s[%dof%d]", file, hd.groupcur, hd.groupnum);
							else
								pSpec->label = file;
							if (fh.Lookup("array", buf))
								fh.Lookup(buf, pSpec->label);
							Spectra.AddTail(pSpec);
						}
					}
					dt.resize(0);
					fh.RemoveAll();
				} while (hd.groupcur != hd.groupnum);
			}
			nmrClient.BreakConnection();
		} else {
			// Local data!
			for (int i=0; i<n; i++) {
				CString file;
				m_fileList.GetText(items[i], file);
				CFile specFile;
				if (specFile.Open(file, CFile::modeRead | CFile::shareDenyWrite)) {
					if (0 == file.Right(4).CompareNoCase(".nmr")) {
						HD hd;
						specFile.Read(&hd, sizeof(HD));
						if (np) {
							if (np!=hd.dim[0] || sw!=hd.spec_width[0]/hd.dim_freq[0] || offset!=(0.5*hd.spec_width[0]+hd.of[0])/hd.dim_freq[0]) {
								CString msg;
								msg.Format("%s is not the same spectrum Window. It will be ignored!", file);
								AfxMessageBox(msg, MB_ICONEXCLAMATION);
								continue;
							}
						} else {
							np = hd.dim[0];
							sf = hd.dim_freq[0];
							sw = hd.spec_width[0] / sf;
							offset = (0.5 * hd.spec_width[0] + hd.of[0]) / sf;
						}
						CSpectrum *pSpec = new CSpectrum;
						if (pSpec) {
							pSpec->obs.SetSize(np);
							pSpec->calc.SetSize(np);
							pSpec->diff.SetSize(np);
							pSpec->singleline.SetSize(np);
							pSpec->SetBaseline(0.0, 0.0, 0.0, 0.0);
							for (int i=0; i<np; i++) {
								float f[2];
								specFile.Read(f, 2*sizeof(float));
								pSpec->obs[i] = double(f[0]);
							}
							CMapStringToString fh;
							int len;
							specFile.Read(&len, sizeof(int));
							while (len) {
								CString buf;
								char *pKey = buf.GetBuffer(len);
								specFile.Read( pKey, len );
								char *pValue = pKey + strlen(pKey) + 1;
								pValue += strlen( pValue ) + 1;
								fh.SetAt(pKey, pValue);
								buf.ReleaseBuffer();
								specFile.Read(&len, sizeof(int));
							}
							pSpec->label = file;
							CString astr;
							if (fh.Lookup("array", astr))
								fh.Lookup(astr, pSpec->label);
							fh.RemoveAll();
							Spectra.AddTail(pSpec);
						}
					} else if (0 == file.Right(5).CompareNoCase(".nmrx")) {
						CString version;
						CArchive ar(&specFile, CArchive::load);
						ar.ReadString(version);
						ar.ReadString(version);
						if (version.Right(1).Compare("A"))
							continue;	// not the correct file version!
						int size = 5;
						while (size--)
							ar.ReadString(version);
						SPECTRUMPARAMETERS sp;
						ar.Read(&sp, sizeof(SPECTRUMPARAMETERS));
						ar >> size;	// number of fh types
						CMapStringToString fh;
						while (size--) {
							CString type;
							ar.ReadString(type);
							int j;
							ar >> j;	// number of key-values
							while (j--) {
								CString key, value;
								ar.ReadString(key);
								ar.ReadString(value);
								fh.SetAt(key, value);
							}
						}
						ar >> size;
						if (np) {
							if (np!=size || sw!=sp.sw/sp.sf || offset!=(0.5*sp.sw+sp.of)/sp.sf) {
								CString msg;
								msg.Format("%s is not the same spectrum Window. It will be ignored!", file);
								AfxMessageBox(msg, MB_ICONEXCLAMATION);
								fh.RemoveAll();
								continue;
							}
						} else {
							np = size;
							sf = sp.sf;
							sw = sp.sw / sf;
							offset = (0.5 * sp.sw + sp.of) / sf;
						}
						CSpectrum *pSpec = new CSpectrum;
						if (pSpec) {
							pSpec->obs.SetSize(np);
							pSpec->calc.SetSize(np);
							pSpec->diff.SetSize(np);
							pSpec->singleline.SetSize(np);
							pSpec->SetBaseline(0.0, 0.0, 0.0, 0.0);
							for (int i=0; i<np; i++) {
								float re, im;
								ar >> re >> im;
								pSpec->obs[i] = double(re);
							}
							ar.Close();
							pSpec->label = file;
							CString astr;
							if (fh.Lookup("array", astr))
								fh.Lookup(astr, pSpec->label);
							fh.RemoveAll();
							Spectra.AddTail(pSpec);
						}
					} else if (0 == file.Right(4).CompareNoCase(".npl")) {
						CArchive ar(&specFile, CArchive::load);
						CString sbuf;
						bool bUNUSED;
						float fUNUSED;
						int iUNUSED;
						int nspec, badspec=0;
						BOOL BUNUSED;
						ar.ReadString(sbuf);	//title
						ar.ReadString(sbuf);	//label
						ar >> bUNUSED;			//IsLandscape
						ar >> iUNUSED;			//ViewRange
						ar >> iUNUSED >> iUNUSED >> iUNUSED;	//font sizes
						for (int j=0; j<11; j++)
							ar >> fUNUSED;	//umin,ymax,width,height,offsetx,offsety,labelx,labely,rangefrom,rangeto,yexpansion
						ar >> BUNUSED;	//GlobalScaling
						ar >> nspec;
						for (int j=0; j<nspec; j++) {
							CSpectrum *pSpec = new CSpectrum;
							CMapStringToString fh;
							ar.ReadString(sbuf); // this is pathname
							ar.ReadString(pSpec->label);
							if (pSpec->label.IsEmpty())
								pSpec->label = sbuf;
							int nfh;
							ar >> nfh;	//the number of flex header items
							while (nfh--) {
								CString key, value;
								ar.ReadString(key);
								ar.ReadString(value);
								if (key.Left(2).Compare("##"))
									fh.SetAt(key,value);
							}
							int size, from, to, start, stop;
							double swlocal, sflocal, of;
							float ymax, ymin;
							ar >> from >> to;
							ar >> swlocal >> sflocal >> of;
							ar >> size;
							ar >> start >> stop;
							ar >> ymin >> ymax;
							pSpec->obs.SetSize(size);
							pSpec->calc.SetSize(size);
							pSpec->diff.SetSize(size);
							pSpec->singleline.SetSize(size);
							pSpec->SetBaseline(0.0, 0.0, 0.0, 0.0);
							for (int k=0; k<size; k++) {
								float yval;
								ar >> yval;
								pSpec->obs[k] = double(yval);
							}

							if (0 == np) {
								np = size;
								sf = sflocal;
								sw = swlocal / sf;
								offset = (0.5 * swlocal + of) / sf;
								pDoc->sfstr.Format("sf=%g", sf);
								if (fh.Lookup("dosygamma", sbuf))
									pDoc->gammastr.Format("gamma=%s", sbuf);
								if (fh.Lookup("timecubed", sbuf))
									pDoc->tcubedstr.Format("tcubed=%s", sbuf);
								if (fh.Lookup("DAC_to_G", sbuf))
									pDoc->dactogstr.Format("dactog=%s", sbuf);
							}
							if (np==size && sf==sflocal && sw==swlocal/sflocal && offset==(0.5*swlocal+of)/sflocal)
								Spectra.AddTail(pSpec);
							else {
								delete pSpec;
								badspec++;
							}
						}
						ar.Close();
						if (badspec) {
							if (1==badspec)
								sbuf.Format("One spectrum from %s didn't have the correct window!", file);
							else
								sbuf.Format("%d spectra from %s didn't have the correct window", badspec, file);
							AfxMessageBox(sbuf, MB_ICONEXCLAMATION);
						}
					}
					specFile.Close();
				}
			}
		}
		delete [] items;
	}
	CDialog::OnOK();
}

void CAddSpectraDlg::OnCancel()
{
	if (theApp.nserv >= 0 && nmrClient.bConnected)
		nmrClient.BreakConnection();
	CDialog::OnCancel();
}

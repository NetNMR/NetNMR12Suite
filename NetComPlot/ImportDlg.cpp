// ImportDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NetComPlot.h"
#include "SpecDim.h"
#include "OneDSpec.h"
#include "TwoDSpec.h"
#include "NetComPlotDoc.h"
#include "NetNMRClient.h"
#include "ImportDlg.h"
#include "EditInfoDlg.h"
#include "ProjectionDlg.h"

// CImportDlg dialog

IMPLEMENT_DYNAMIC(CImportDlg, CDialog)
CImportDlg::CImportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CImportDlg::IDD, pParent)
	, pDoc(NULL)
	, bIsModified(FALSE)
	, pstrInfo(NULL)
{
}

CImportDlg::~CImportDlg()
{
	Remove1DList.RemoveAll();
	Remove2DList.RemoveAll();
}

void CImportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SERVERLIST, m_listServer);
	DDX_Control(pDX, IDC_FILELIST, m_listFiles);
	DDX_Control(pDX, IDC_TWODLIST, m_listTwoD);
	DDX_Control(pDX, IDC_HONEDLIST, m_listHOneD);
	DDX_Control(pDX, IDC_VONEDLIST, m_listVOneD);
	DDX_Control(pDX, IDC_VERTTOHORIZ, m_butVerttoHoriz);
	DDX_Control(pDX, IDC_HORIZTOVERT, m_butHoriztoVert);
	DDX_Control(pDX, IDC_MOVEUP, m_butMoveUp);
	DDX_Control(pDX, IDC_MOVEDOWN, m_butMoveDown);
	DDX_Control(pDX, IDC_REMOVE, m_butRemove);
	DDX_Control(pDX, IDC_REMOVEALL, m_butRemoveAll);
	DDX_Control(pDX, IDC_EDITINFO, m_butEditInfo);
	DDX_Control(pDX, IDC_PROJECTIONBUTTON, m_butProjection);
	DDX_Control(pDX, IDC_CURRENTDIRECTORY, m_staticCurrentDirectory);
}


BEGIN_MESSAGE_MAP(CImportDlg, CDialog)
	ON_LBN_DBLCLK(IDC_FILELIST, OnLbnDblclkFilelist)
	ON_LBN_SELCHANGE(IDC_SERVERLIST, OnLbnSelchangeServerlist)
	ON_LBN_SELCHANGE(IDC_TWODLIST, OnLbnSelchangeTwodlist)
	ON_LBN_SELCHANGE(IDC_HONEDLIST, OnLbnSelchangeHonedlist)
	ON_LBN_SELCHANGE(IDC_VONEDLIST, OnLbnSelchangeVonedlist)
	ON_BN_CLICKED(IDC_REMOVEALL, OnBnClickedRemoveall)
	ON_BN_CLICKED(IDC_REMOVE, OnBnClickedRemove)
	ON_BN_CLICKED(IDC_HORIZTOVERT, OnBnClickedHoriztovert)
	ON_BN_CLICKED(IDC_VERTTOHORIZ, OnBnClickedVerttohoriz)
	ON_BN_CLICKED(IDC_MOVEDOWN, OnBnClickedMovedown)
	ON_BN_CLICKED(IDC_MOVEUP, OnBnClickedMoveup)
	ON_BN_CLICKED(IDC_EDITINFO, OnBnClickedEditinfo)
	ON_BN_CLICKED(IDC_PROJECTIONBUTTON, OnBnClickedProjection)
END_MESSAGE_MAP()


// CImportDlg message handlers

BOOL CImportDlg::OnInitDialog()
{
	CWaitCursor wait;
	CDialog::OnInitDialog();
	// ATTACH THE ICONS TO TWO BUTTONS
	m_butVerttoHoriz.SetIcon(theApp.LoadIcon(IDI_UPARROW));
	m_butHoriztoVert.SetIcon(theApp.LoadIcon(IDI_DOWNARROW));
	// Fill the server list
	for (int i=0; strcmp(theApp.Server.Name(i), NMRSERVERNOTFOUND); i++)
		if (0 == strcmp(theApp.Server.Name(i), "My Personal Server")) {
			if (CWnd::FindWindow(NULL, "ConsoleServer"))
				m_listServer.SetItemData(m_listServer.AddString(theApp.Server.Name(i)), i);
		} else
			m_listServer.SetItemData(m_listServer.AddString(theApp.Server.Name(i)), i);
	// FILL THE DIRECTORY AND FILE LIST
	if (theApp.nserv >= 0) {
		for (int i=0; i<m_listServer.GetCount(); i++) {
			if (theApp.nserv == (int)m_listServer.GetItemData(i)) {
				m_listServer.SetCurSel(i);
				NetNMRClient.MakeConnection();
				FillDirList();
				break;
			}
		}
	}
	// FILL THE 2D FILE LIST
	BOOL bRemoveAll = FALSE;
	POSITION pos = pDoc->TwoDSpec.GetHeadPosition();
	while (pos) {
		bRemoveAll = TRUE;
		CTwoDSpec *pSpec = pDoc->TwoDSpec.GetNext(pos);
		int dummy = 0;
		CString strName = pSpec->strInfo.Tokenize("\r\n", dummy);
		m_listTwoD.SetItemDataPtr(m_listTwoD.AddString(strName), pSpec);
	}
	pos = pDoc->HOneDSpec.GetHeadPosition();
	while (pos) {
		bRemoveAll = TRUE;
		COneDSpec *pSpec = pDoc->HOneDSpec.GetNext(pos);
		int dummy = 0;
		CString strName = pSpec->strInfo.Tokenize("\r\n", dummy);
		m_listHOneD.SetItemDataPtr(m_listHOneD.AddString(strName), pSpec);
	}
	pos = pDoc->VOneDSpec.GetHeadPosition();
	while (pos) {
		bRemoveAll = TRUE;
		COneDSpec *pSpec = pDoc->VOneDSpec.GetNext(pos);
		int dummy = 0;
		CString strName = pSpec->strInfo.Tokenize("\r\n", dummy);
		m_listVOneD.SetItemDataPtr(m_listVOneD.AddString(strName), pSpec);
	}
	m_butRemoveAll.EnableWindow(bRemoveAll);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CImportDlg::FillDirList(void)
{
	CWaitCursor wait;
	m_listFiles.ResetContent();
	if (! NetNMRClient.bConnected)
		return;
	//Get 1D Spectra and Directories
	CString buf("L\n");
	NetNMRClient.SendString(buf);
	NetNMRClient.GetSockString(buf);
	while (buf.Compare(".")) {
		m_listFiles.SetItemData( m_listFiles.AddString(buf),
		 	(buf.Right(1).FindOneOf("\\/") >= 0) ? IsDirectory : IsOneDNMR);
		NetNMRClient.GetSockString(buf);
	}
	// Get 2D Spectra and Ignore the Directories
	buf = "M\n";
	NetNMRClient.SendString(buf);
	NetNMRClient.GetSockString(buf);
	while (buf.Compare(".")) {
		if (-1 == buf.Right(1).FindOneOf("\\/")) {
			int i = m_listFiles.FindString(-1, buf);
			if (i >= 0)
				// Remove arrayed data from list!
				m_listFiles.DeleteString(i);
			else
				m_listFiles.SetItemData( m_listFiles.AddString(buf), IsTwoDNMR);
		}
		NetNMRClient.GetSockString(buf);
	}
}

void CImportDlg::OnLbnDblclkFilelist()
{
	CWaitCursor wait;
	int i = m_listFiles.GetCurSel();
	if (i == LB_ERR || theApp.nserv<0) return;
	CString name, s;
	m_listFiles.GetText(i, name);
	s.Format("%s:%s%s", theApp.Server.Name(theApp.nserv), theApp.Server.Path(theApp.nserv), name);
	switch (m_listFiles.GetItemData(i)) {
		case IsDirectory:
			s.Format("/%s\n", name);
			NetNMRClient.SendString(s);
			NetNMRClient.GetSockString(s);
			m_staticCurrentDirectory.SetWindowText(s);
			theApp.Server.SetPath(theApp.nserv, s);
			FillDirList();
			return;
		case IsOneDNMR:
			switch (AfxMessageBox("Yes: Add to Horizontal list\nNo: Add to Vertical list", MB_YESNOCANCEL|MB_ICONQUESTION)) {
				case IDYES:
					m_listHOneD.SetItemDataPtr(m_listHOneD.AddString(s), NULL);
					break;
				case IDNO:
					m_listVOneD.SetItemDataPtr(m_listVOneD.AddString(s), NULL);
					break;
			}
			break;
		case IsTwoDNMR:
			m_listTwoD.SetItemDataPtr(m_listTwoD.AddString(s), NULL);
			break;
	}
	m_butRemoveAll.EnableWindow();
}

void CImportDlg::OnOK()
{
	CWaitCursor wait;
	if (Remove1DList.GetCount() || Remove2DList.GetCount())
		bIsModified = TRUE;
	while (Remove1DList.GetCount())
		delete Remove1DList.RemoveHead();
	while (Remove2DList.GetCount())
		delete Remove2DList.RemoveHead();
	pDoc->TwoDSpec.RemoveAll();
	for (int i=0; i<m_listTwoD.GetCount(); i++) {
		if (m_listTwoD.GetItemDataPtr(i))
			pDoc->TwoDSpec.AddTail((CTwoDSpec *)m_listTwoD.GetItemDataPtr(i));
		else {
			CString name;
			m_listTwoD.GetText(i, name);
			CTwoDSpec *pSpec = Get2DSpectrum(name);
			if (pSpec) {
				pDoc->TwoDSpec.AddTail(pSpec);
				bIsModified = TRUE;
			}
		}
	}
	pDoc->HOneDSpec.RemoveAll();
	for (int i=0; i<m_listHOneD.GetCount(); i++) {
		if (m_listHOneD.GetItemDataPtr(i))
			pDoc->HOneDSpec.AddTail((COneDSpec *)m_listHOneD.GetItemDataPtr(i));
		else {
			CString name;
			m_listHOneD.GetText(i, name);
			COneDSpec *pSpec = Get1DSpectrum(name);
			if (pSpec) {
				pDoc->HOneDSpec.AddTail(pSpec);
				bIsModified = TRUE;
			}
		}
	}
	pDoc->VOneDSpec.RemoveAll();
	for (int i=0; i<m_listVOneD.GetCount(); i++) {
		if (m_listVOneD.GetItemDataPtr(i))
			pDoc->VOneDSpec.AddTail((COneDSpec *)m_listVOneD.GetItemDataPtr(i));
		else {
			CString name;
			m_listVOneD.GetText(i, name);
			COneDSpec *pSpec = Get1DSpectrum(name);
			if (pSpec) {
				pDoc->VOneDSpec.AddTail(pSpec);
				bIsModified = TRUE;
			}
		}
	}
	if (bIsModified)
		pDoc->SetModifiedFlag();
	NetNMRClient.BreakConnection();
	CDialog::OnOK();
}

void CImportDlg::OnCancel()
{
	Remove1DList.RemoveAll();
	Remove2DList.RemoveAll();
	NetNMRClient.BreakConnection();
	CDialog::OnCancel();
}

void CImportDlg::OnLbnSelchangeServerlist()
{
	// Close the current Connection
	NetNMRClient.BreakConnection();

	// Get the selected server name
	theApp.nserv = (int)m_listServer.GetItemData(m_listServer.GetCurSel());

	// And open the new connection
	if (NetNMRClient.MakeConnection()) {
		CString buf;
		buf.Format("/%s\n", theApp.Server.Path(theApp.nserv));
		NetNMRClient.SendString(buf);
		NetNMRClient.GetSockString(buf);
		m_staticCurrentDirectory.SetWindowText(buf);
		theApp.Server.SetPath(theApp.nserv, buf);
		FillDirList();
	} else {
		theApp.nserv = -1;
		m_listServer.SetSel(-1, FALSE);
		m_listFiles.ResetContent();
	}
}

void CImportDlg::OnLbnSelchange(CListBox &pList)
{
	BOOL IsTwoD = pList.m_hWnd == m_listTwoD.m_hWnd;
	BOOL IsHOneD = pList.m_hWnd == m_listHOneD.m_hWnd;
	BOOL IsVOneD = pList.m_hWnd == m_listVOneD.m_hWnd;
	if (! IsTwoD)
		m_listTwoD.SetCurSel(-1);
	if (! IsHOneD) 
		m_listHOneD.SetCurSel(-1);
	if (! IsVOneD) 
		m_listVOneD.SetCurSel(-1);
	m_butHoriztoVert.EnableWindow(IsHOneD);
	m_butVerttoHoriz.EnableWindow(IsVOneD);
	int count = pList.GetCount();
	int cursel = pList.GetCurSel();
	void *pData = pList.GetItemDataPtr(cursel);
	if (pData) {
		if (IsTwoD) {
			CTwoDSpec *p = (CTwoDSpec *)pData;
			pstrInfo = &p->strInfo;
		} else if (IsHOneD || IsVOneD) {
			COneDSpec *p = (COneDSpec *)pData;
			pstrInfo = &p->strInfo;
		} else
			pstrInfo = NULL;
	}
	m_butRemove.EnableWindow(cursel>=0);
	m_butMoveUp.EnableWindow(count>1 && cursel>0);
	m_butMoveDown.EnableWindow(count>1 && cursel<count-1);
	m_butEditInfo.EnableWindow(cursel>=0);
	m_butProjection.EnableWindow(IsTwoD && cursel>=0 && pData);
}

void CImportDlg::OnLbnSelchangeTwodlist()
{
	OnLbnSelchange(m_listTwoD);
}

void CImportDlg::OnLbnSelchangeHonedlist()
{
	OnLbnSelchange(m_listHOneD);
}

void CImportDlg::OnLbnSelchangeVonedlist()
{
	OnLbnSelchange(m_listVOneD);
}

void CImportDlg::OnBnClickedRemoveall()
{
	for (int i=0; i<3; i++) {
		CListBox *pList;
		switch (i) {
			case 0:
				pList = &m_listTwoD;
				break;
			case 1:
				pList = &m_listHOneD;
				break;
			default:
				pList = &m_listVOneD;
		}
		while (pList->GetCount()) {
			if (pList->GetItemDataPtr(0)) {
				if (i)
					Remove1DList.AddTail((COneDSpec *)pList->GetItemDataPtr(0));
				else
					Remove2DList.AddTail((CTwoDSpec *)pList->GetItemDataPtr(0));
			}
			pList->DeleteString(0);
		}
	}
	// Disable all buttons, execept ok and cancel
	OnLbnSelchange(m_listTwoD);
	m_butEditInfo.EnableWindow(FALSE);
	m_butRemoveAll.EnableWindow(FALSE);
}

void CImportDlg::OnBnClickedRemove()
{
	int i = m_listTwoD.GetCurSel();
	if (-1 < i) {
		if (m_listTwoD.GetItemDataPtr(i))
			Remove2DList.AddTail((CTwoDSpec *)m_listTwoD.GetItemDataPtr(i));
		m_listTwoD.DeleteString(i);
	} else {
		CListBox *pList;
		if (-1 < m_listHOneD.GetCurSel())
			pList = &m_listHOneD;
		else
			pList = &m_listVOneD;
		i = pList->GetCurSel();
		if (pList->GetItemDataPtr(i))
			Remove1DList.AddTail((COneDSpec *)pList->GetItemDataPtr(i));
		pList->DeleteString(i);
	}
	m_butHoriztoVert.EnableWindow(FALSE);
	m_butVerttoHoriz.EnableWindow(FALSE);
	m_butMoveUp.EnableWindow(FALSE);
	m_butMoveDown.EnableWindow(FALSE);
	m_butRemove.EnableWindow(FALSE);
	m_butEditInfo.EnableWindow(FALSE);
	m_butRemoveAll.EnableWindow(0 < m_listTwoD.GetCount()+m_listHOneD.GetCount()+m_listVOneD.GetCount());
}

void CImportDlg::MoveBetweenHandV(CListBox & listFrom, CListBox & listTo)
{
	int i = listFrom.GetCurSel();
	if (i < 0) return;
	CString name;
	listFrom.GetText(i, name);
	COneDSpec *pSpec = (COneDSpec *)listFrom.GetItemDataPtr(i);
	if (pSpec) bIsModified = TRUE;
	listFrom.DeleteString(i);
	i = listTo.AddString(name);
	listTo.SetItemDataPtr(i, pSpec);
	listTo.SetCurSel(i);
	OnLbnSelchange(listTo);
}

void CImportDlg::OnBnClickedHoriztovert()
{
	MoveBetweenHandV(m_listHOneD, m_listVOneD);
}

void CImportDlg::OnBnClickedVerttohoriz()
{
	MoveBetweenHandV(m_listVOneD, m_listHOneD);
}

void CImportDlg::MoveString(int direction)
{
	CListBox *pList;
	if (m_listTwoD.GetCurSel() >= 0)
		pList = &m_listTwoD;
	else if (m_listHOneD.GetCurSel() >= 0)
		pList = &m_listHOneD;
	else
		pList = &m_listVOneD;
	int i = pList->GetCurSel();
	CString str;
	pList->GetText(i, str);
	void *pSpec = pList->GetItemDataPtr(i);
	pList->DeleteString(i);
	// IF ONLY ONE ITEM IS LOADED WITH DATA THEN THIS IS NOT NECESSSARILY
	// A CHANGE, BECAUSE THE USER MAY REMOVE THE NEW DATA SET BEFORE
	// CLICKING "OK"
	if (pSpec) {
		int j = (-1 == direction) ? i - 1 : 1;
		if (pList->GetItemDataPtr(j))
			bIsModified = TRUE;
	}
	i += direction;
	if (i >= pList->GetCount())
		i = -1;
	i = pList->InsertString(i, str);
	pList->SetItemDataPtr(i, pSpec);
	pList->SetCurSel(i);
	OnLbnSelchange(*pList);
}

void CImportDlg::OnBnClickedMovedown()
{
	MoveString(1);
}

void CImportDlg::OnBnClickedMoveup()
{
	MoveString(-1);
}

CTwoDSpec *CImportDlg::Get2DSpectrum(LPCTSTR str)
{
	union Spec {
		float f;
		u_long u;
		int i;
	};
	Spec specbuf[9];
	CString filename = str;
	int i, l = filename.Find(':');
	if (l<1)
		return NULL;
	CString name = filename.Left(l);
	i = theApp.Server.Find(name);
	if (i < 0)
		return NULL;
	if (i != theApp.nserv) {
		NetNMRClient.BreakConnection();
		theApp.nserv = i;
		if (! NetNMRClient.MakeConnection())
			return NULL;
	}
	name = filename.Mid(l+1);
	CString buf;
	buf.Format("2%s\n", name);
	NetNMRClient.SendString(buf);
	CString strInfo = str;	// the first line of strInfo will be the file path.
	CString dataclass;
	while (1) {
		NetNMRClient.GetSockString(buf);
		if (0 == buf.Compare("."))
			break;
		if (0 == buf.Find("dataclass=") && theApp.DataClass.Count())
			dataclass = buf.Mid(10);
		else
			strInfo += "\r\n" + buf;
	}
	NetNMRClient.ReadSocket(specbuf, 9*sizeof(float));
	NetNMRClient.ByteReverseData(&specbuf[0].i, 9);
	if (specbuf[0].i != 2)
		return NULL;
	CTwoDSpec *pSpec = new CTwoDSpec;
	if (! pSpec)
		return NULL;
	pSpec->strInfo = strInfo;
	pSpec->strDataClass = dataclass;
	pSpec->HSpecDim.Initialize(specbuf[2].f, specbuf[3].f, specbuf[4].f, specbuf[1].i);
	pSpec->VSpecDim.Initialize(specbuf[6].f, specbuf[7].f, specbuf[8].f, specbuf[5].i);
	if (! pSpec->Initialize()) {
		delete pSpec;
		return NULL;
	}
	for (int i=0; i<pSpec->VSpecDim.Size(); i++) {
		NetNMRClient.ReadSocket(pSpec->dt[i], pSpec->HSpecDim.Size()*sizeof(int));
		NetNMRClient.ByteReverseData(pSpec->dt[i], pSpec->HSpecDim.Size());
	}
	pSpec->HSpecDim.from = pSpec->VSpecDim.from = 0;
	pSpec->HSpecDim.to = pSpec->HSpecDim.Size() - 1;
	pSpec->VSpecDim.to = pSpec->VSpecDim.Size() - 1;
	pSpec->GetRange();
	return pSpec;
}

COneDSpec * CImportDlg::Get1DSpectrum(LPCTSTR str)
{
	union Spec {
		float f;
		u_long u;
		int i;
	};
	Spec specbuf[5];
	CString filename = str;
	int i, l = filename.Find(':');
	if (l<1)
		return NULL;
	CString name = filename.Left(l);
	i = theApp.Server.Find(name);
	if (i < 0)
		return NULL;
	if (i != theApp.nserv) {
		NetNMRClient.BreakConnection();
		theApp.nserv = i;
		if (! NetNMRClient.MakeConnection())
			return NULL;
	}
	name = filename.Mid(l+1);
	CString buf;
	buf.Format("1%s\n", name);
	NetNMRClient.SendString(buf);
	CString strInfo = str;
	while (1) {
		NetNMRClient.GetSockString(buf);
		if (0 == buf.Compare("."))
			break;
		strInfo += "\r\n";
		strInfo += buf;
	}
	NetNMRClient.ReadSocket(specbuf, 5*sizeof(float));
	NetNMRClient.ByteReverseData(&specbuf[0].i, 5);
	if (specbuf[0].i != 1)
		return NULL;
	COneDSpec *pSpec = new COneDSpec;
	if (! pSpec)
		return NULL;
	pSpec->strInfo = strInfo;
	pSpec->SpecDim.Initialize(specbuf[2].f, specbuf[3].f, specbuf[4].f, specbuf[1].i);
	if (! pSpec->Initialize()) {
		delete pSpec;
		return NULL;
	}
	NetNMRClient.ReadSocket(pSpec->dt, pSpec->SpecDim.Size()*sizeof(int));
	NetNMRClient.ByteReverseData(pSpec->dt, pSpec->SpecDim.Size());
	pSpec->SpecDim.from = 0;
	pSpec->SpecDim.to = pSpec->SpecDim.Size() - 1;
	pSpec->GetRange();
	return pSpec;
}

void CImportDlg::OnBnClickedEditinfo()
{
	if (NULL == pstrInfo)
		return;
	static char *nl = "\r\n";
	CEditInfoDlg dlg;
	dlg.m_strInfo = *pstrInfo;
	if (IDOK == dlg.DoModal()) {
		int i = 0;
		CString newInfo = "";
		CString strLine = dlg.m_strInfo.Tokenize(nl, i);
		while (! strLine.IsEmpty()) {
			if (strLine.Trim().GetLength())
				newInfo += strLine.Trim() + nl;
			strLine = dlg.m_strInfo.Tokenize(nl, i);
		}
		if (newInfo.IsEmpty())
			newInfo = "UNKNOWN";
		if (newInfo.Compare(*pstrInfo)) {
			pDoc->SetModifiedFlag();
			*pstrInfo = newInfo;
		}
	}
}

void CImportDlg::OnBnClickedProjection()
{
	CProjectionDlg dlg;
	CTwoDSpec *p2DSpec = (CTwoDSpec *)m_listTwoD.GetItemDataPtr(m_listTwoD.GetCurSel());

	// SHOW DIALOG
	if (IDOK == dlg.DoModal()) {
		// ADD NEW PROJECTIONS
		int cols = p2DSpec->HSpecDim.Size();
		int rows = p2DSpec->VSpecDim.Size();
		for (int nproj=0; nproj<4; nproj++) {
			bool bIsRow, bIsMax;
			COneDSpec *pNew = NULL;
			switch (nproj) {
				case 0:
					if (dlg.m_browmaxCheck) {
						bIsRow = true;
						bIsMax = true;
						pNew = new COneDSpec;
					}
					break;
				case 1:
					if (dlg.m_browminCheck) {
						bIsRow = true;
						bIsMax = false;
						pNew = new COneDSpec;
					}
					break;
				case 2:
					if (dlg.m_bcolmaxCheck) {
						bIsRow = false;
						bIsMax = true;
						pNew = new COneDSpec;
					}
					break;
				case 3:
					if (dlg.m_bcolminCheck) {
						bIsRow = false;
						bIsMax = false;
						pNew = new COneDSpec;
					}
					break;
			}
			if (pNew) {
				CSpecDim *pDS;
				if (bIsRow)
					pDS = &p2DSpec->VSpecDim;
				else
					pDS = &p2DSpec->HSpecDim;
				pNew->SpecDim.Initialize(pDS->Frequency(), pDS->SpecWidth(), pDS->Offset(), pDS->Size());
				if (pNew->Initialize()) {
					pNew->strInfo = "Projection - ";
					pNew->strInfo += p2DSpec->strInfo;
					if (bIsRow) {
						for (int i=0; i<rows; i++)
							pNew->dt[i] = p2DSpec->dt[i][0];
						for (int i=0; i<rows; i++) {
							int *pdt = pNew->dt + i;
							for (int j=1; j<cols; j++) {
								int dt = p2DSpec->dt[i][j];
								if ((bIsMax&&(dt>*pdt)) || ((!bIsMax)&&(dt<*pdt)))
									*pdt = dt;
							}
						}
						int index = 0;
						m_listVOneD.SetItemDataPtr(m_listVOneD.AddString(pNew->strInfo.Tokenize("\r\n", index).Trim()), pNew);
					} else {
						for (int i=0; i<cols; i++)
							pNew->dt[i] = p2DSpec->dt[0][i];
						for (int i=0; i<cols; i++) {
							int *pdt = pNew->dt + i;
							for (int j=1; j<rows; j++) {
								int dt = p2DSpec->dt[j][i];
								if ((bIsMax&&(dt>*pdt)) || ((!bIsMax)&&(dt<*pdt)))
									*pdt = dt;
							}
						}
						int index = 0;
						m_listHOneD.SetItemDataPtr(m_listHOneD.AddString(pNew->strInfo.Tokenize("\r\n", index).Trim()), pNew);
					}
					pNew->SpecDim.from = 0;
					pNew->SpecDim.to = pNew->SpecDim.Size() - 1;
					pNew->GetRange();
				} else {
					delete pNew;
					AfxMessageBox("No Memory for projection data!", MB_ICONSTOP);
				}
			}
		}
	}
}
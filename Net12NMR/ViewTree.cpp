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

#include "stdafx.h"
#include "..\NMRDataTypes.h"
#include "ViewTree.h"

#include "Net12NMR.h"
#include "SortedIntList.h"
#include "ServerSocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewTree

CViewTree::CViewTree()
{
}

CViewTree::~CViewTree()
{
}

BEGIN_MESSAGE_MAP(CViewTree, CTreeCtrl)
//	ON_NOTIFY_REFLECT(TVN_GETDISPINFO, &CViewTree::OnTvnGetdispinfo)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, &CViewTree::OnTvnItemexpanding)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, &CViewTree::OnTvnItemexpanded)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewTree message handlers

BOOL CViewTree::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	BOOL bRes = CTreeCtrl::OnNotify(wParam, lParam, pResult);

	NMHDR* pNMHDR = (NMHDR*)lParam;
	ASSERT(pNMHDR != NULL);

	if (pNMHDR && pNMHDR->code == TTN_SHOW && GetToolTips() != NULL)
	{
		GetToolTips()->SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	}

	return bRes;
}


void CViewTree::OnTvnItemexpanding(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	HTREEITEM h = pNMTreeView->itemNew.hItem;
	if (TVE_EXPAND == pNMTreeView->action)
		GetChildren(h);
	*pResult = 0;
}


void CViewTree::OnTvnItemexpanded(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	HTREEITEM h = pNMTreeView->itemNew.hItem;
	if (TVE_COLLAPSE==pNMTreeView->action)
		DeleteChildren(h);
	*pResult = 0;
}
void CViewTree::GetChildren(HTREEITEM hParent)
{
	CWaitCursor waitcursor;
	if (hParent) {
		Leaf pleaf;
		pleaf.l = GetItemData(hParent);

		if (base == pleaf.s.leaftype) { // we're opening a base type to get access to shares or servers
			if (shared == pleaf.s.location) { // shared drives
				unsigned long drives = _getdrives();
				for (int i=0; i<26; i++) {
					if (drives & 1<<i) {
						CString label;
						label.Format(_T("%c:\\"), 'a'+i);
						Leaf d;
						d.s.leaftype = root;
						d.s.location = shared;
						d.s.drivenumber = i + 1;
						AddChild(hParent, label, d, TVI_LAST);
					}
				}
			} else { // nmrservers
				for (int i=0; i<theApp.servers.GetCount(); i++) {
					bool addit = true;
					if (0 == strcmp(theApp.servers.Name(i), "My Personal Server"))
						if (! CWnd::FindWindow(NULL, "ConsoleServer"))
							addit = false;
					if (addit) {
						Leaf s;
						s.s.leaftype = root;
						s.s.location = served;
						s.s.drivenumber = i;
						AddChild(hParent, theApp.servers.Name(i), s, TVI_LAST);
					}
				}
			}

		} else if (shared == pleaf.s.location) { // we're dealing with a PC drive

			if (folder==pleaf.s.leaftype || root==pleaf.s.leaftype) { // we're expanding a simple folder

				int cdrive = _getdrive();
				CString path;
				GetFullPath(hParent, path);
				if (_chdir(path)) // cd to the path
					return;
				//TRACE1("Changed directory to %s\n", path);
				// iterate over all the file in the directory
				CFileFind entry;
				BOOL isentry = entry.FindFile();
				while (isentry) {
					isentry = entry.FindNextFile();
					if (entry.MatchesMask(FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_TEMPORARY) || entry.IsDots())
						continue; // skip over useless stuff

					if (entry.IsDirectory()) {
						//TRACE1("Found directory %s\n", entry.GetFileName());
						// here is a directory, so let's initialize an item data structure
						Leaf leaf;
						leaf.s.leaftype = folder;
						leaf.s.drivenumber = pleaf.s.drivenumber;
						leaf.s.location = pleaf.s.location;

						// does this look like a bruker dataset?
						// first look for numeric expno folders
						CFileFind efile;
						CSortedIntList elist;
						BOOL efound = efile.FindFile(entry.GetFileName() + "\\*.*");
						while (efound) {
							efound = efile.FindNextFile();
							//TRACE1("Found subitem %s\n", efile.GetFileName());
							(void)elist.Add(efile.GetFileName());
						}
										
						while (! elist.IsEmpty()) {
							CString testdir;
							CFileStatus fs;
							int expno = elist.RemoveHead();
							testdir.Format(_T("%s\\%d"), entry.GetFileName(), expno);
							if (CFile::GetStatus(testdir+"\\acqus", fs)) {
								// acqus found, now see if its a 1D datafile
								if (CFile::GetStatus(testdir+"\\fid", fs)) {
									// a bruker 1D fid was found!
									leaf.s.leaftype = fid;
									leaf.s.dimension = simple;
									leaf.s.vendor = bruker;
								} else {
									if (CFile::GetStatus(testdir+"\\acqu2s", fs) && CFile::GetStatus(testdir+"\\ser", fs)) {
										leaf.s.leaftype = fid;
										leaf.s.dimension = twod;
										leaf.s.vendor = bruker;
										// it's a 2D file. Is it actually an array psuedo-2D?
										CString afile = entry.GetFileName() + "\\acqus";
										if(theApp.ReadAsciiFile(afile, CFile::modeRead)) {
											CString s;
											theApp.MapJCAMPFile(afile);
											if (theApp.m_paraMap.Lookup(_T("VDLIST"), s)) {
												if (CFile::GetStatus(testdir+"\\"+s, fs) || CFile::GetStatus(testdir+"\\vdlist", fs))
													leaf.s.dimension = arrayed;
											}
											if (theApp.m_paraMap.Lookup(_T("VPLIST"), s)) {
												if (CFile::GetStatus(testdir+"\\"+s, fs) || CFile::GetStatus(testdir+"\\vplist", fs))
													leaf.s.dimension = arrayed;
											}
											if (theApp.m_paraMap.Lookup(_T("VCLIST"), s)) {
												if (CFile::GetStatus(testdir+"\\"+s, fs) || CFile::GetStatus(testdir+"\\vclist", fs))
													leaf.s.dimension = arrayed;
											}
											if (theApp.m_paraMap.Lookup(_T("VTLIST"), s)) {
												if (CFile::GetStatus(testdir+"\\"+s, fs) || CFile::GetStatus(testdir+"\\vtlist", fs))
													leaf.s.dimension = arrayed;
											}
										}	// acqus sucessfully read
									} // 1D or 2D data?
								} // acqus found
								if (fid==leaf.s.leaftype && twod!=leaf.s.dimension) {
									// we found a fid and it is not a pure 2D datafile
									AddChild(hParent, testdir, leaf, TVI_SORT);
									// now, lets look for processed data
									int pcount = 0;
									CFileFind pfile;
									CSortedIntList plist;
									BOOL pfound = pfile.FindFile(testdir + "\\pdata\\*.*");
									while (pfound) {
										pfound = pfile.FindNextFileA();
										if (plist.Add(pfile.GetFileName()))
											pcount++;
									}
									while (! plist.IsEmpty()) {
										int procno = plist.RemoveHead();
										testdir.Format("%s\\%d\\pdata\\%d", entry.GetFileName(), expno, procno);
										if (CFile::GetStatus(testdir+"\\procs", fs)) {
											if (twod == leaf.s.dimension) {
												if (CFile::GetStatus(testdir+"\\2rr", fs) && CFile::GetStatus(testdir+"\\2ii", fs))
													leaf.s.leaftype = spectrum;
											} else {
												if (CFile::GetStatus(testdir+"\\1r", fs) && CFile::GetStatus(testdir+"\\1i", fs))
													leaf.s.leaftype = spectrum;
											}
											if (spectrum == leaf.s.leaftype)
												AddChild(hParent, testdir, leaf, TVI_SORT);
										}
									}
								}
							} // there was an acqus file in a subdirectory with an integer name
						} // loop for all the expno subdirectories
						// how about a Varian/Agilent file?
						if (0 == entry.GetFileName().Right(4).CompareNoCase(".fid")) {
							CFileStatus fs;
							CString base = entry.GetFileName();
							CString procpar = base + "\\procpar";
							if (CFile::GetStatus(procpar, fs) && CFile::GetStatus(base + "\\fid", fs)) {
								leaf.s.leaftype = fid;
								leaf.s.dimension = simple;
								leaf.s.vendor = agilent;
								if (theApp.ReadAsciiFile(procpar, CFile::modeRead)) {
									theApp.MapProcparFile(procpar);
									CStringArray *pValues;
									int ni = 0;
									if (theApp.m_procparMap.Lookup("ni", pValues))
										ni = atoi(pValues->GetAt(0));
									theApp.DeleteProcparMap();
									if (0 == ni) {
										AddChild(hParent, base, leaf, TVI_SORT);
										if (CFile::GetStatus(base+"\\datdir\\phasefile", fs) && CFile::GetStatus(base+"\\datdir\\data", fs)) {
											CString pro = base.Left(base.GetLength()-3) + "pro";
											leaf.s.leaftype = spectrum;
											AddChild(hParent, pro, leaf, TVI_SORT);
										}
									}
								}
							}
						}

						// if the leaftype is still a folder, then we need to add it to the tree
						if (folder == leaf.s.leaftype) {
							AddChild(hParent, entry.GetFileName()+"\\", leaf, TVI_SORT);
						}
					} else {
						// it's not a directory
						CString name = entry.GetFileName();
						int index = name.ReverseFind('.') + 1;
						if (index > 0) {
							CString extension = name.Right(name.GetLength()-index);
							if (0==extension.CompareNoCase("nmr") || 0==extension.CompareNoCase("nmrx")) {
								Leaf nmrleaf;
								nmrleaf.l = pleaf.l;
								nmrleaf.s.leaftype = (3==extension.GetLength()) ? netnmrfile : net12nmrfile;
								AddChild(hParent, entry.GetFileName(), nmrleaf, TVI_SORT);
							}
						}
					}
				} // loop over every entry
				_chdrive(cdrive);
			}	// expand a folder
		} else {
			// we have a server leaf
			CServerSocket nmrserver;
			if (nmrserver.Open(pleaf.s.drivenumber)) {
				CString path;
				GetFullPath(hParent, path);
				nmrserver.SendCommand('/', path);
				CString newpath = nmrserver.GetSockString();
				//TRACE1("New path is %s\n", newpath);
				int newpathlength = newpath.GetLength() - path.GetLength();
				if (4 == newpathlength) {
					newpath = newpath.Right(newpathlength);
					if (0 == newpath.Left(3).CompareNoCase("nmr")) {
						Leaf leaf;
						leaf.s.location = served;
						leaf.s.drivenumber = pleaf.s.drivenumber;
						leaf.s.leaftype = folder;
						HTREEITEM hChild = AddChild(hParent, newpath, leaf, TVI_SORT);
						if (hChild)
							return;
					}
				}
				if (root==pleaf.s.leaftype || folder==pleaf.s.leaftype) {
					// we need to expand this folder
					nmrserver.SendCommand('l');
					CString item = nmrserver.GetSockString();
					while (item.Compare(".")) {
						if (0 != item.Find("..")) {
							// we don't need ".." for the tree!
							Leaf leaf;
							leaf.s.location = served;
							leaf.s.drivenumber = pleaf.s.drivenumber;
							if (item.Right(1).Compare("/") && item.Right(1).Compare("\\")) {
								// nmrserver is giving us a file
								leaf.s.dimension = simple;
								CString type = item.Right(4);
								if (type.Compare(".fid") && type.Compare(".pro")) {
									leaf.s.vendor = bruker;
									leaf.s.leaftype = (item.Find("pdata") > 1) ? spectrum : fid;
								} else {
									leaf.s.vendor = agilent;
									leaf.s.leaftype = type.Compare(".fid") ? spectrum : fid;
								}
							} else {
								// nmrserver is giving us a folder
								leaf.s.leaftype = folder;
							}
							AddChild(hParent, item, leaf, TVI_SORT);
						}
						item = nmrserver.GetSockString();
					}
				}
			} else {
				CString err;
				err.Format("Can't open %s server!", theApp.servers.Name(pleaf.s.drivenumber));
				AfxMessageBox(err);
			}
		}
	} else {	// hParent==NULL, so initialize the two base types.
		Leaf b;
		b.s.leaftype = base;
		b.s.location = shared;
		AddChild(NULL, "Drives", b, TVI_LAST);
		b.s.location = served;
		AddChild(NULL, "Servers", b, TVI_LAST);
	}
}

void CViewTree::DeleteChildren(HTREEITEM hItem)
{
	HTREEITEM hChild = GetChildItem(hItem);

	while (hChild) {
		HTREEITEM hNext = GetNextItem(hChild, TVGN_NEXT);
		if (0 == DeleteItem(hChild))
			AfxMessageBox("Tree deletion unsucessful!", MB_ICONEXCLAMATION);
		hChild = hNext;
	}
	TV_ITEM item;
	item.hItem = hItem;
	item.mask = TVIF_HANDLE | TVIF_CHILDREN;
	item.cChildren = I_CHILDRENCALLBACK;
	SetItem(&item);
}

HTREEITEM CViewTree::AddChild(HTREEITEM hParent, LPCTSTR name, Leaf leaf, HTREEITEM hAfter)
{
	// get the icon offset for this item
	int offset;
	switch (leaf.s.leaftype) {
	case base:
	case root:
		offset = (shared==leaf.s.location) ? 0 : 1;
		break;
	case folder:
		offset = 2;
		break;
	case fid:
		offset = 3 + leaf.s.dimension;
		break;
	case spectrum:
		offset = 5 + leaf.s.dimension;
		break;
	default:	// a NetNMR or Net12NMR file
		offset = 7;
		break;
	}
	TV_ITEM item;
	item.hItem = InsertItem(name, offset, offset, hParent, hAfter);
	if (item.hItem) {
		item.mask = TVIF_CHILDREN;
		if (GetItem(&item)) {
			item.cChildren = (base==leaf.s.leaftype || root==leaf.s.leaftype || folder==leaf.s.leaftype) ? 1 : 0;
			SetItem(&item);
		}
		SetItemData(item.hItem, leaf.l);
	}
	return item.hItem;
}

void CViewTree::GetFullPath(HTREEITEM hItem, CString &path)
{
	path.Empty();
	Leaf leaf;
	do {
			leaf.l = GetItemData(hItem);
			if (root==leaf.s.leaftype && served==leaf.s.location) {
				path = theApp.servers.Home(leaf.s.drivenumber) + path;
			} else
				path = GetItemText(hItem) + path;
			hItem = GetParentItem(hItem);
	} while (NULL!=hItem && root!=leaf.s.leaftype);
	//TRACE1("GetFullPath()=%s\n", path);
}



void CViewTree::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	HTREEITEM hItem = GetSelectedItem();
	if (NULL == hItem)
		return;
	Leaf leaf;
	leaf.l = GetItemData(hItem);
	if (fid==leaf.s.leaftype || spectrum==leaf.s.leaftype || netnmrfile==leaf.s.leaftype || net12nmrfile==leaf.s.leaftype) {
		CString pathname;
		GetFullPath(hItem, pathname);
		theApp.ImportNMRFile(pathname, leaf);
	}
	CTreeCtrl::OnLButtonDblClk(nFlags, point);
}

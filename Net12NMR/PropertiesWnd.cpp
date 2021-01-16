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

#include "PropertiesWnd.h"
#include "Resource.h"
#include "MainFrame.h"
#include "ChildFrame.h"
#include "Net12NMR.h"
#include "Net12NMRDoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar

CPropertiesWnd::CPropertiesWnd()
{
}

CPropertiesWnd::~CPropertiesWnd()
{
}

BEGIN_MESSAGE_MAP(CPropertiesWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_EXPAND_ALL, OnExpandAllProperties)
	ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAllProperties)
	ON_COMMAND(ID_SORTPROPERTIES, OnSortProperties)
	ON_UPDATE_COMMAND_UI(ID_SORTPROPERTIES, OnUpdateSortProperties)
	ON_COMMAND(ID_PROPERTIES1, OnProperties1)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES1, OnUpdateProperties1)
	ON_COMMAND(ID_PROPERTIES2, OnProperties2)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES2, OnUpdateProperties2)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar message handlers

void CPropertiesWnd::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient; //,rectCombo;
	GetClientRect(rectClient);

	//m_wndObjectCombo.GetWindowRect(&rectCombo);

	//int cyCmb = rectCombo.Size().cy;
	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	//m_wndObjectCombo.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), 200, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top/* + cyCmb*/, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top/* + cyCmb*/ + cyTlb, rectClient.Width(), rectClient.Height() -(/*cyCmb+*/cyTlb), SWP_NOACTIVATE | SWP_NOZORDER);
}

int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create combo:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | CBS_SORT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	//if (!m_wndObjectCombo.Create(dwViewStyle, rectDummy, this, 1))
	//{
	//	TRACE0("Failed to create Properties Combo \n");
	//	return -1;      // fail to create
	//}

	////m_wndObjectCombo.AddString(_T("Application"));
	//m_wndObjectCombo.AddString(_T("Spectrum"));
	//m_wndObjectCombo.SetCurSel(0);

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("Failed to create Properties Grid \n");
		return -1;      // fail to create
	}

	InitPropList();

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* Is locked */);
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* Locked */);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.SetOwner(this);

	// All commands will be routed via this control , not via the parent frame:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	AdjustLayout();
	return 0;
}

void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CPropertiesWnd::OnExpandAllProperties()
{
	m_wndPropList.ExpandAll();
}

void CPropertiesWnd::OnUpdateExpandAllProperties(CCmdUI* /* pCmdUI */)
{
}

void CPropertiesWnd::OnSortProperties()
{
	m_wndPropList.SetAlphabeticMode(!m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnUpdateSortProperties(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnProperties1()
{
	// TODO: Add your command handler code here
}

void CPropertiesWnd::OnUpdateProperties1(CCmdUI* /*pCmdUI*/)
{
	// TODO: Add your command update UI handler code here
}

void CPropertiesWnd::OnProperties2()
{
	// TODO: Add your command handler code here
}

void CPropertiesWnd::OnUpdateProperties2(CCmdUI* /*pCmdUI*/)
{
	// TODO: Add your command update UI handler code here
}

void CPropertiesWnd::InitPropList()
{
	SetPropListFont();
	LPCTSTR strdefault = _T("Default");
	LPCTSTR strbonw    = _T("Black on white");
	LPCTSTR strcustom  =  _T("Custom");
	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	//m_wndPropList.MarkModifiedProperties();

	CMFCPropertyGridProperty *pcolorGroup = new CMFCPropertyGridProperty(_T("Display Colors"));
	m_colorschemeselect = new CMFCPropertyGridProperty(_T("Color Scheme"), _T("Default"), _T("Select a color scheme"));
	m_colorschemeselect->AddOption(strcustom);
	m_colorschemeselect->AddOption(strbonw);
	m_colorschemeselect->AddOption(strdefault);
	m_colorschemeselect->AllowEdit(FALSE);
	LPCTSTR str;
	if (0 == memcmp(theApp.Color, theApp.ColorScheme[0], sizeof(theApp.Color)))
		str = strdefault;
	else if (0 == memcmp(theApp.Color, theApp.ColorScheme[1], sizeof(theApp.Color)))
		str = strbonw;
	else
		str = strcustom;
	m_colorschemeselect->SetValue(str);
	pcolorGroup->AddSubItem(m_colorschemeselect);

	CMFCPropertyGridProperty *pColorItems = new CMFCPropertyGridProperty("Item Colors");
	m_colors[refidColor] = new CMFCPropertyGridColorProperty(_T("Real FID"), theApp.Color[refidColor], NULL, _T("The color of the real FID Data (along X)"));
	m_colors[imfidColor] = new CMFCPropertyGridColorProperty(_T("Imaginary FID"), theApp.Color[imfidColor], NULL, _T("The color of the imaginary FID Data (along Y)"));
	m_colors[spectColor] = new CMFCPropertyGridColorProperty(_T("Spectrum"), theApp.Color[spectColor], NULL, _T("The color of the spectrum"));
	m_colors[integColor] = new CMFCPropertyGridColorProperty(_T("Integral"), theApp.Color[integColor], NULL, _T("The color of the integral"));
	m_colors[intehColor] = new CMFCPropertyGridColorProperty(_T("Selected Integral"), theApp.Color[backgColor], NULL, _T("The color of a selected integral"));
	m_colors[axis_Color] = new CMFCPropertyGridColorProperty(_T("Axis"), theApp.Color[axis_Color], NULL, _T("The color of the axis"));
	m_colors[titleColor] = new CMFCPropertyGridColorProperty(_T("Title"), theApp.Color[titleColor], NULL, _T("The color of the title"));
	m_colors[parboColor] = new CMFCPropertyGridColorProperty(_T("Parameter box"), theApp.Color[parboColor], NULL, _T("The color of the parameter box"));
	m_colors[parheColor] = new CMFCPropertyGridColorProperty(_T("Parameter header"), theApp.Color[parheColor], NULL, _T("The color of the parameter header"));
	m_colors[paramColor] = new CMFCPropertyGridColorProperty(_T("Parameter"), theApp.Color[paramColor], NULL, _T("The color of the parameter"));
	m_colors[peaklColor] = new CMFCPropertyGridColorProperty(_T("Peak labels"), theApp.Color[peaklColor], NULL, _T("The color of the peak labels"));
	m_colors[peakhColor] = new CMFCPropertyGridColorProperty(_T("Selected peak"), theApp.Color[backgColor], NULL, _T("The color of a selected peak"));
	m_colors[peaktColor] = new CMFCPropertyGridColorProperty(_T("Peak threshold"), theApp.Color[peaktColor], NULL, _T("The color of the peak threshold"));
	m_colors[basenColor] = new CMFCPropertyGridColorProperty(_T("Selected baseline"), theApp.Color[basenColor], NULL, _T("The color of the selected baseline"));
	m_colors[basefColor] = new CMFCPropertyGridColorProperty(_T("Ignored baseline"), theApp.Color[basefColor], NULL, _T("The color of the ignored baseline"));
	m_colors[crossColor] = new CMFCPropertyGridColorProperty(_T("Crosshairs cursor"), theApp.Color[crossColor], NULL, _T("The color of the crosshairs cursor"));
	m_colors[zoomcColor] = new CMFCPropertyGridColorProperty(_T("Zoom cursors"), theApp.Color[zoomcColor], NULL, _T("The color of the zoom cursors"));
	m_colors[backgColor] = new CMFCPropertyGridColorProperty(_T("Background"), theApp.Color[backgColor], NULL, _T("The color of the background"));
	for (int i=0; i<constColorSize; i++) {
		m_colors[i]->EnableOtherButton(_T("Other..."));
		pColorItems->AddSubItem(m_colors[i]);
	}
	pColorItems->Expand(FALSE);
	pcolorGroup->AddSubItem(pColorItems);
	m_wndPropList.AddProperty(pcolorGroup);

	//pColorProp->EnableAutomaticButton(_T("Default"), ::GetSysColor(COLOR_3DFACE));
	//pGroup3->AddSubItem(pColorProp);
	//CMFCPropertyGridProperty* pSize = new CMFCPropertyGridProperty(_T("Window Size"), 0, TRUE);
	//CMFCPropertyGridProperty *pProp = new CMFCPropertyGridProperty(_T("Height"), (_variant_t) 250l, _T("Specifies the window's height"));
	//pProp->EnableSpinControl(TRUE, 50, 300);
	//pSize->AddSubItem(pProp);
	//pProp = new CMFCPropertyGridProperty( _T("Width"), (_variant_t) 160l, _T("Specifies the window's width"));
	//pProp->EnableSpinControl(TRUE, 50, 200);
	//pSize->AddSubItem(pProp);
	//m_wndPropList.AddProperty(pSize);

	//CMFCPropertyGridProperty* pGroup2 = new CMFCPropertyGridProperty(_T("Font"));
	//LOGFONT lf;
	//CFont* font = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	//font->GetLogFont(&lf);
	//lstrcpy(lf.lfFaceName, _T("Arial"));
	//pGroup2->AddSubItem(new CMFCPropertyGridFontProperty(_T("Font"), lf, CF_EFFECTS | CF_SCREENFONTS, _T("Specifies the default font for the window")));
	//pGroup2->AddSubItem(new CMFCPropertyGridProperty(_T("Use System Font"), (_variant_t) true, _T("Specifies that the window uses MS Shell Dlg font")));
	//m_wndPropList.AddProperty(pGroup2);

	//CMFCPropertyGridProperty* pGroup3 = new CMFCPropertyGridProperty(_T("Misc"));
	//pProp = new CMFCPropertyGridProperty(_T("(Name)"), _T("Application"));
	//pProp->Enable(FALSE);
	//pGroup3->AddSubItem(pProp);
	//CMFCPropertyGridColorProperty* pColorProp = new CMFCPropertyGridColorProperty(_T("Window Color"), RGB(210, 192, 254), NULL, _T("Specifies the default window color"));
	//pColorProp->EnableOtherButton(_T("Other..."));
	//pColorProp->EnableAutomaticButton(_T("Default"), ::GetSysColor(COLOR_3DFACE));
	//pGroup3->AddSubItem(pColorProp);
	//static const TCHAR szFilter[] = _T("Icon Files(*.ico)|*.ico|All Files(*.*)|*.*||");
	//pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("Icon"), TRUE, _T(""), _T("ico"), 0, szFilter, _T("Specifies the window icon")));
	//pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("Folder"), _T("c:\\")));
	//m_wndPropList.AddProperty(pGroup3);

	//CMFCPropertyGridProperty* pGroup4 = new CMFCPropertyGridProperty(_T("Hierarchy"));
	//CMFCPropertyGridProperty* pGroup41 = new CMFCPropertyGridProperty(_T("First sub-level"));
	//pGroup4->AddSubItem(pGroup41);
	//CMFCPropertyGridProperty* pGroup411 = new CMFCPropertyGridProperty(_T("Second sub-level"));
	//pGroup41->AddSubItem(pGroup411);
	//pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("Item 1"), (_variant_t) _T("Value 1"), _T("This is a description")));
	//pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("Item 2"), (_variant_t) _T("Value 2"), _T("This is a description")));
	//pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("Item 3"), (_variant_t) _T("Value 3"), _T("This is a description")));
	//pGroup4->Expand(FALSE);
	//m_wndPropList.AddProperty(pGroup4);

}

void CPropertiesWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}

void CPropertiesWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
}

void CPropertiesWnd::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	m_wndPropList.SetFont(&m_fntPropList);
}

// CAppProperties

IMPLEMENT_DYNAMIC(CAppProperties, CMFCPropertyGridCtrl)

CAppProperties::CAppProperties()
{

}

CAppProperties::~CAppProperties()
{
}


BEGIN_MESSAGE_MAP(CAppProperties, CMFCPropertyGridCtrl)
END_MESSAGE_MAP()



// CAppProperties message handlers




void CAppProperties::OnPropertyChanged(CMFCPropertyGridProperty* pProp) const
{
	CPropertiesWnd *pWnd = STATIC_DOWNCAST(CPropertiesWnd, GetParent());

	bool repaint = false;
	if (pProp == pWnd->m_colorschemeselect) {
		CString scheme(pProp->GetValue().bstrVal);
		switch (scheme[0]) {
		case 'B':
			memcpy(theApp.Color, theApp.ColorScheme[1], constColorSize*sizeof(COLORREF));
			repaint = true;
			break;
		case 'D':
			memcpy(theApp.Color, theApp.ColorScheme[0], constColorSize*sizeof(COLORREF));
			repaint = true;
			break;
		}
		if (repaint) {
			for (int i=0; i<constColorSize; i++)
				pWnd->m_colors[i]->SetColor(theApp.Color[i]);
		}
	} else {
		CMFCPropertyGridColorProperty *pItem = NULL;
		for (int i=0; i<constColorSize; i++) {
			if (pProp == pWnd->m_colors[i]) {
				pItem = (CMFCPropertyGridColorProperty *)pProp;
				theApp.Color[i] = pItem->GetColor();
				repaint = true;
				break;
			}
		}
		if (pItem) {
			// adjust the dropdown
			if (0 == memcmp(theApp.Color, theApp.ColorScheme[1], constColorSize*sizeof(COLORREF)))
				pWnd->m_colorschemeselect->SetValue(_T("Black on white"));
			else if (0 == memcmp(theApp.Color, theApp.ColorScheme[0], constColorSize*sizeof(COLORREF)))
				pWnd->m_colorschemeselect->SetValue(_T("Default"));
			else
				pWnd->m_colorschemeselect->SetValue(_T("Custom"));
		}
	}
	if (repaint) {
		CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
		if (pMain != pMain->GetActiveFrame()) {
			CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->GetActiveFrame());
			if (pChild) {
				CNet12NMRDoc *pDoc = STATIC_DOWNCAST(CNet12NMRDoc, pChild->GetActiveDocument());
				if (pDoc)
					pDoc->UpdateAllViews(NULL);
			}
		}
		theApp.WriteProfileBinary(theApp.settings, "Colors", (LPBYTE)theApp.Color, sizeof(theApp.Color));
	}
	return CMFCPropertyGridCtrl::OnPropertyChanged(pProp);
}

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

// Net12NMRView.cpp : implementation of the CNetNMRView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Net12NMR.h"
#endif

#include "Net12NMRDoc.h"
#include "MainFrame.h"
#include "Net12NMRView.h"
#include "Axis.h"
#include "SetOffsetDlg.h"
#include "ZoomToDlg.h"
#include "Param.h"
#include "MyRibbonBar.h"
#include "SetIntegralDlg.h"
#include "mmsystem.h"
#pragma comment(lib, "winmm.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CNet12NMRView

IMPLEMENT_DYNCREATE(CNet12NMRView, CView)

BEGIN_MESSAGE_MAP(CNet12NMRView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CNet12NMRView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_SETCURSOR()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_AUTOYBUTTON, &CNet12NMRView::OnAutoybutton)
	ON_COMMAND(ID_UPYBUTTON, &CNet12NMRView::OnUpybutton)
	ON_COMMAND(ID_DOWNYBUTTON, &CNet12NMRView::OnDownybutton)
	ON_COMMAND(ID_VIEW_Y_TOP, &CNet12NMRView::OnViewYTop)
	ON_UPDATE_COMMAND_UI(ID_VIEW_Y_TOP, &CNet12NMRView::OnUpdateViewYTop)
	ON_COMMAND(ID_VIEW_Y_CENTER, &CNet12NMRView::OnViewYCenter)
	ON_UPDATE_COMMAND_UI(ID_VIEW_Y_CENTER, &CNet12NMRView::OnUpdateViewYCenter)
	ON_COMMAND(ID_VIEW_Y_BOTTOM, &CNet12NMRView::OnViewYBottom)
	ON_UPDATE_COMMAND_UI(ID_VIEW_Y_BOTTOM, &CNet12NMRView::OnUpdateViewYBottom)
	ON_UPDATE_COMMAND_UI(ID_TRUNCATEBUTTON, &CNet12NMRView::OnUpdateTruncatebutton)
	ON_COMMAND(ID_TRUNCATEBUTTON, &CNet12NMRView::OnTruncatebutton)
	ON_UPDATE_COMMAND_UI(ID_DCLEVELBUTTON, &CNet12NMRView::OnUpdateDclevelbutton)
	ON_COMMAND(ID_DCLEVELBUTTON, &CNet12NMRView::OnDclevelbutton)
	ON_UPDATE_COMMAND_UI(ID_QCBUTTON, &CNet12NMRView::OnUpdateQcbutton)
	ON_COMMAND(ID_QCBUTTON, &CNet12NMRView::OnQcbutton)
	ON_UPDATE_COMMAND_UI(ID_LEFTSHIFTBUTTON, &CNet12NMRView::OnUpdateLeftshiftbutton)
	ON_COMMAND(ID_LEFTSHIFTBUTTON, &CNet12NMRView::OnLeftshiftbutton)
	ON_UPDATE_COMMAND_UI(ID_RIGHTSHIFTBUTTON, &CNet12NMRView::OnUpdateRightshiftbutton)
	ON_COMMAND(ID_RIGHTSHIFTBUTTON, &CNet12NMRView::OnRightshiftbutton)
	ON_UPDATE_COMMAND_UI(ID_APODIZEBUTTON, &CNet12NMRView::OnUpdateApodizebutton)
	ON_COMMAND(ID_APODIZEBUTTON, &CNet12NMRView::OnApodizebutton)
	ON_UPDATE_COMMAND_UI(ID_ZEROFILLBUTTON, &CNet12NMRView::OnUpdateZerofillbutton)
	ON_COMMAND(ID_ZEROFILLBUTTON, &CNet12NMRView::OnZerofillbutton)
	ON_UPDATE_COMMAND_UI(ID_FTBUTTON, &CNet12NMRView::OnUpdateFtbutton)
	ON_COMMAND(ID_FTBUTTON, &CNet12NMRView::OnFtbutton)
	ON_UPDATE_COMMAND_UI(IDC_MAGNITUDEBUTTON, &CNet12NMRView::OnUpdateMagnitudebutton)
	ON_COMMAND(IDC_MAGNITUDEBUTTON, &CNet12NMRView::OnMagnitudebutton)
	ON_UPDATE_COMMAND_UI(ID_ZEROPHASEBUTTON, &CNet12NMRView::OnUpdateZerophasebutton)
	ON_COMMAND(ID_ZEROPHASEBUTTON, &CNet12NMRView::OnZerophasebutton)
	ON_UPDATE_COMMAND_UI(ID_AUTOPHASEBUTTON, &CNet12NMRView::OnUpdateAutophasebutton)
	ON_COMMAND(ID_AUTOPHASEBUTTON, &CNet12NMRView::OnAutophasebutton)
	ON_UPDATE_COMMAND_UI(ID_AUTOPHASEAONLYBUTTON, &CNet12NMRView::OnUpdateAutophaseaonlybutton)
	ON_COMMAND(ID_AUTOPHASEAONLYBUTTON, &CNet12NMRView::OnAutophaseaonlybutton)
	ON_UPDATE_COMMAND_UI(ID_FLIPABUTTON, &CNet12NMRView::OnUpdateFlipabutton)
	ON_COMMAND(ID_FLIPABUTTON, &CNet12NMRView::OnFlipabutton)
	ON_UPDATE_COMMAND_UI(ID_INCREMENTBUTTON, &CNet12NMRView::OnUpdateIncrementbutton)
	ON_COMMAND(ID_INCREMENTBUTTON, &CNet12NMRView::OnIncrementbutton)
	ON_UPDATE_COMMAND_UI(ID_DECREMENTBUTTON, &CNet12NMRView::OnUpdateDecrementbutton)
	ON_COMMAND(ID_DECREMENTBUTTON, &CNet12NMRView::OnDecrementbutton)
	ON_UPDATE_COMMAND_UI(ID_FILTERCOMPBUTTON, &CNet12NMRView::OnUpdateFiltercompbutton)
	ON_COMMAND(ID_FILTERCOMPBUTTON, &CNet12NMRView::OnFiltercompbutton)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ZOOM, &CNet12NMRView::OnUpdateEditZoom)
	ON_COMMAND(ID_EDIT_ZOOM, &CNet12NMRView::OnEditZoom)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FULL, &CNet12NMRView::OnUpdateEditFull)
	ON_COMMAND(ID_EDIT_FULL, &CNet12NMRView::OnEditFull)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SETOFFSET, &CNet12NMRView::OnUpdateEditSetoffset)
	ON_COMMAND(ID_EDIT_SETOFFSET, &CNet12NMRView::OnEditSetoffset)
	ON_UPDATE_COMMAND_UI(IDC_ATCURSORBUTTON, &CNet12NMRView::OnUpdateAtcursorbutton)
	ON_COMMAND(IDC_ATCURSORBUTTON, &CNet12NMRView::OnAtcursorbutton)
	ON_UPDATE_COMMAND_UI(ID_ABSLEVELBUTTON, &CNet12NMRView::OnUpdateAbslevelbutton)
	ON_COMMAND(ID_ABSLEVELBUTTON, &CNet12NMRView::OnAbslevelbutton)
	ON_UPDATE_COMMAND_UI(ID_ABSRESETBUTTON, &CNet12NMRView::OnUpdateAbsresetbutton)
	ON_COMMAND(ID_ABSRESETBUTTON, &CNet12NMRView::OnAbsresetbutton)
	ON_UPDATE_COMMAND_UI(ID_ABSFINDBUTTON, &CNet12NMRView::OnUpdateAbsfindbutton)
	ON_COMMAND(ID_ABSFINDBUTTON, &CNet12NMRView::OnAbsfindbutton)
	ON_COMMAND(ID_ABCWIDTHEDIT, &CNet12NMRView::OnAbcwidthedit)
	ON_COMMAND(ID_ABCSNEDIT, &CNet12NMRView::OnAbcsnedit)
	ON_UPDATE_COMMAND_UI(ID_ADDBASELINEBUTTON,    &CNet12NMRView::EnableIfZoomAndFT)
	ON_UPDATE_COMMAND_UI(ID_REMOVEBASELINEBUTTON, &CNet12NMRView::EnableIfZoomAndFT)
	ON_UPDATE_COMMAND_UI(ID_ADDINTEGRALBUTTON,    &CNet12NMRView::EnableIfZoomAndFT)
	ON_UPDATE_COMMAND_UI(ID_REMOVEINTEGRALBUTTON, &CNet12NMRView::EnableIfZoomAndFT)
	ON_UPDATE_COMMAND_UI(ID_PEAKSETTHRESHOLD,     &CNet12NMRView::EnableIfZoomAndFT)
	ON_UPDATE_COMMAND_UI(ID_PEAKREMOVETHRESHOLD,  &CNet12NMRView::EnableIfZoomAndFT)
	ON_COMMAND(ID_ADDBASELINEBUTTON, &CNet12NMRView::OnAddbaselinebutton)
	ON_COMMAND(ID_REMOVEBASELINEBUTTON, &CNet12NMRView::OnRemovebaselinebutton)
	ON_COMMAND(ID_POLYCHECK, &CNet12NMRView::OnPolycheck)
	ON_UPDATE_COMMAND_UI(ID_POLYCHECK, &CNet12NMRView::OnUpdatePolycheck)
	ON_COMMAND(ID_MOVECHECK, &CNet12NMRView::OnMovecheck)
	ON_UPDATE_COMMAND_UI(ID_MOVECHECK, &CNet12NMRView::OnUpdateMovecheck)
	ON_COMMAND(ID_CALCBASEBUTTON, &CNet12NMRView::OnCalcbasebutton)
	ON_UPDATE_COMMAND_UI(ID_CALCBASEBUTTON, &CNet12NMRView::OnUpdateCalcbasebutton)
	ON_COMMAND(ID_POLYORDERCOMBO, &CNet12NMRView::OnPolyordercombo)
	ON_UPDATE_COMMAND_UI(ID_POLYORDERCOMBO, &CNet12NMRView::OnUpdatePolyordercombo)
	ON_COMMAND(ID_MVAVEDIT, &CNet12NMRView::OnMvavedit)
	ON_UPDATE_COMMAND_UI(ID_MVAVEDIT, &CNet12NMRView::OnUpdateMvavedit)
	ON_COMMAND(ID_APPLYBASEBUTTON, &CNet12NMRView::OnApplybasebutton)
	ON_UPDATE_COMMAND_UI(ID_APPLYBASEBUTTON, &CNet12NMRView::OnUpdateApplybasebutton)
	ON_COMMAND(ID_BASETOCLOUDBUTTON, &CNet12NMRView::OnBasetoshelfbutton)
	ON_UPDATE_COMMAND_UI(ID_BASETOCLOUDBUTTON, &CNet12NMRView::OnUpdateBasetoshelfbutton)
	ON_COMMAND(ID_CLOUDTOBASEBUTTON, &CNet12NMRView::OnShelftobasebutton)
	ON_UPDATE_COMMAND_UI(ID_CLOUDTOBASEBUTTON, &CNet12NMRView::OnUpdateShelftobasebutton)
	ON_COMMAND(ID_ADDINTEGRALBUTTON, &CNet12NMRView::OnAddintegralbutton)
	ON_COMMAND(ID_REMOVEINTEGRALBUTTON, &CNet12NMRView::OnRemoveintegralbutton)
	ON_UPDATE_COMMAND_UI(ID_CLEARINTEGRALBUTTON, &CNet12NMRView::OnUpdateClearintegralbutton)
	ON_COMMAND(ID_CLEARINTEGRALBUTTON, &CNet12NMRView::OnClearintegralbutton)
	ON_UPDATE_COMMAND_UI(ID_INTTOCLOUDBUTTON, &CNet12NMRView::OnUpdateInttoshelfbutton)
	ON_COMMAND(ID_INTTOCLOUDBUTTON, &CNet12NMRView::OnInttoshelfbutton)
	ON_UPDATE_COMMAND_UI(ID_CLOUDTOINTBUTTON, &CNet12NMRView::OnUpdateShelftointbutton)
	ON_COMMAND(ID_CLOUDTOINTBUTTON, &CNet12NMRView::OnShelftointbutton)
	ON_UPDATE_COMMAND_UI(ID_BREAKINTEGRALBUTTON, &CNet12NMRView::OnUpdateBreakintegralbutton)
	ON_UPDATE_COMMAND_UI(ID_SETINTEGRALBUTTON,   &CNet12NMRView::OnUpdateBreakintegralbutton)
	ON_COMMAND(ID_BREAKINTEGRALBUTTON, &CNet12NMRView::OnBreakintegralbutton)
	ON_COMMAND(ID_SETINTEGRALBUTTON, &CNet12NMRView::OnSetintegralbutton)
	ON_UPDATE_COMMAND_UI(ID_CLEARPEAKREGIONBUTTON, &CNet12NMRView::OnUpdateClearpeakregionbutton)
	ON_COMMAND(ID_CLEARPEAKREGIONBUTTON, &CNet12NMRView::OnClearpeakregionbutton)
	ON_UPDATE_COMMAND_UI(ID_SPREADSHEETBUTTON, &CNet12NMRView::OnUpdateSpreadsheetbutton)
	ON_COMMAND(ID_SPREADSHEETBUTTON, &CNet12NMRView::OnSpreadsheetbutton)
	ON_UPDATE_COMMAND_UI(ID_PARAPEAKCHECK, &CNet12NMRView::OnUpdateParapeakcheck)
	ON_COMMAND(ID_PARAPEAKCHECK, &CNet12NMRView::OnParapeakcheck)
	ON_UPDATE_COMMAND_UI(ID_PPMPEAKCHECK, &CNet12NMRView::OnUpdatePpmpeakcheck)
	ON_COMMAND(ID_PPMPEAKCHECK, &CNet12NMRView::OnPpmpeakcheck)
	ON_UPDATE_COMMAND_UI(ID_HZPEAKCHECK, &CNet12NMRView::OnUpdateHzpeakcheck)
	ON_COMMAND(ID_HZPEAKCHECK, &CNet12NMRView::OnPpmpeakcheck)
	ON_COMMAND(ID_PPMPEAKCOMBO, &CNet12NMRView::OnPpmpeakcombo)
	ON_COMMAND(ID_HZPEAKCOMBO, &CNet12NMRView::OnHzpeakcombo)
	ON_COMMAND(ID_PEAKSETTHRESHOLD, &CNet12NMRView::OnPeaksetthreshold)
	ON_COMMAND(ID_PEAKREMOVETHRESHOLD, &CNet12NMRView::OnPeakremovethreshold)
	ON_UPDATE_COMMAND_UI(ID_PEAKTOSHELFBUTTON, &CNet12NMRView::OnUpdatePeaktoshelfbutton)
	ON_COMMAND(ID_PEAKTOSHELFBUTTON, &CNet12NMRView::OnPeaktoshelfbutton)
	ON_UPDATE_COMMAND_UI(ID_PEAKFROMSHELFBUTTON, &CNet12NMRView::OnUpdateShelftopeakbutton)
	ON_COMMAND(ID_PEAKFROMSHELFBUTTON, &CNet12NMRView::OnShelftopeakbutton)
	ON_UPDATE_COMMAND_UI(IDC_HERTZBUTTON, &CNet12NMRView::OnUpdateHertzbutton)
	ON_UPDATE_COMMAND_UI(IDC_PPMBUTTON, &CNet12NMRView::OnUpdateHertzbutton)
	ON_COMMAND(IDC_HERTZBUTTON, &CNet12NMRView::OnHertzbutton)
	ON_COMMAND(IDC_PPMBUTTON, &CNet12NMRView::OnPpmbutton)
	ON_UPDATE_COMMAND_UI(IDC_TIMEBUTTON, &CNet12NMRView::OnUpdateTimebutton)
	ON_COMMAND(IDC_TIMEBUTTON, &CNet12NMRView::OnTimebutton)
	ON_COMMAND(IDC_POINTSBUTTON, &CNet12NMRView::OnPointsbutton)
	ON_COMMAND(IDC_NOAXISBUTTON, &CNet12NMRView::OnNoaxisbutton)
	ON_UPDATE_COMMAND_UI(ID_RELOADBUTTON, &CNet12NMRView::OnUpdateReloadbutton)
	ON_COMMAND(ID_RELOADBUTTON, &CNet12NMRView::OnReloadbutton)
	ON_UPDATE_COMMAND_UI(ID_ZERODATABUTTON, &CNet12NMRView::OnUpdateZerodatabutton)
	ON_COMMAND(ID_ZERODATABUTTON, &CNet12NMRView::OnZerodatabutton)
	ON_COMMAND(ID_EDIT_COPY, &CNet12NMRView::OnEditCopy)
	ON_UPDATE_COMMAND_UI(IDC_PARFITCHECK, &CNet12NMRView::OnUpdateParfitcheck)
	ON_COMMAND(IDC_PARFITCHECK, &CNet12NMRView::OnParfitcheck)
	ON_UPDATE_COMMAND_UI(ID_PLAYBUTTON, &CNet12NMRView::OnUpdatePlaybutton)
	ON_COMMAND(ID_PLAYBUTTON, &CNet12NMRView::OnPlaybutton)
	ON_COMMAND(ID_EDIT_ZOOMTO, &CNet12NMRView::OnEditZoomto)
	ON_UPDATE_COMMAND_UI(ID_GLOBALINTEGRALCHECK, &CNet12NMRView::OnUpdateGlobalintegralcheck)
	ON_COMMAND(ID_GLOBALINTEGRALCHECK, &CNet12NMRView::OnGlobalintegralcheck)
	ON_UPDATE_COMMAND_UI(ID_LOCALTOGLOBALBUTTON, &CNet12NMRView::OnUpdateLocaltoglobalbutton)
	ON_COMMAND(ID_LOCALTOGLOBALBUTTON, &CNet12NMRView::OnLocaltoglobalbutton)
	ON_UPDATE_COMMAND_UI(ID_GLOBALTOLOCALBUTTON, &CNet12NMRView::OnUpdateGlobaltolocalbutton)
	ON_COMMAND(ID_GLOBALTOLOCALBUTTON, &CNet12NMRView::OnGlobaltolocalbutton)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PEAKLABEL, &CNet12NMRView::OnUpdateViewPeaklabel)
	ON_COMMAND(ID_VIEW_PEAKLABEL, &CNet12NMRView::OnViewPeaklabel)
	ON_UPDATE_COMMAND_UI(ID_VIEW_INTEGRALS, &CNet12NMRView::OnUpdateViewIntegrals)
	ON_COMMAND(ID_VIEW_INTEGRALS, &CNet12NMRView::OnViewIntegrals)
	ON_UPDATE_COMMAND_UI(ID_AUTOYBUTTON, &CNet12NMRView::OnUpdateAutoybutton)
	END_MESSAGE_MAP()

// CNet12NMRView construction/destruction

CNet12NMRView::CNet12NMRView()
{
	zoom.right = zoom.left = -1;
	cursor.right = cursor.left = -1;
	CursorPosition.x = CursorPosition.y = -1;
	axisUnits = nullUnits;
	AutoYScale = true;
	ViewIntegral = true;
	ViewPeakLabels = true;
	yMode = fromBottom;
	axisLabel = "";
	dmin = dmax = 0.0f;
}

CNet12NMRView::~CNet12NMRView()
{
}

BOOL CNet12NMRView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CNet12NMRView drawing

void CNet12NMRView::DrawPhase(CDC *pDC)
{
	pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetWindowExt(22000, 18000);
	pDC->SetWindowOrg(-1000, -1000);
	GetClientRect(&SpecRect);
	int width = SpecRect.Width();
	int height = SpecRect.Height();
	pDC->SetViewportExt(width, -height);
	pDC->SetViewportOrg(0, height);
	const float ydif = 8000.0f/maxdtphasemode;
	CPen *oldPen, specPen;
	specPen.CreatePen(PS_SOLID, 0, theApp.Color[spectColor]);
	oldPen = pDC->SelectObject(&specPen);
	int i = zoom.left;
	while (1) {
		POINT pts[1025];
		int npts = 0;
		while (npts<1025 && i<zoom.right) {
			pts[npts].x = MulDiv( i - zoom.left, 20000, zoom.right - zoom.left );
			int y = int((vdt[i++].real() + maxdtphasemode + offsetdtphasemode) * ydif);
			if (y > 16000)
				y = 16000;
			else if (y < 0)
				y = 0;
			pts[npts++].y = y;
		}
		if (npts > 1)
			pDC->Polyline(pts, npts);
		if (i >= zoom.right)
			break;
		i--;
	}
	CPen zoomPen;
	zoomPen.CreatePen(PS_SOLID, 0, theApp.Color[zoomcColor]/*^theApp.Color[backgColor]*/);
	pDC->SelectObject(&zoomPen);
	if (phaseonleft) {
		pDC->MoveTo(cursor.left, 0);
		pDC->LineTo(cursor.left, 16000);
	} else {
		pDC->MoveTo(cursor.right, 0);
		pDC->LineTo(cursor.right, 16000);
	}
	pDC->SelectObject(oldPen);
}

void CNet12NMRView::OnDraw(CDC* pDC)
{
	if (phaseMode == theApp.disMode) {
		DrawPhase(pDC);
		return;
	}
	CNet12NMRDoc* pDoc = GetDocument();
	if (!pDoc)
		return;
	if (pDoc->dt.size() < 2)
		return;
	if (zoom.left < 0 || zoom.right>int(pDoc->dt.size())) {
		UINT size = pDoc->dt.size();
		if (zoom.left < 0 || zoom.left > int(size)-3)
			zoom.left = 0;
		if (zoom.right>int(size) || zoom.right<zoom.left+3)
			zoom.right = size;
		if (nullUnits == axisUnits) {
			axisUnits = pDoc->sp.ftflag ? ppmUnits : timeUnits;
		}
		if (! pDoc->sp.ftflag)
			axisUnits = timeUnits;
		SetAxisRange();
		FindMinMax();
		pDoc->LoadState();
		UpdatePeakList();
		UpdateIntegralList();
	}
	pDC->SetMapMode(MM_ANISOTROPIC);
	WindowExtent = CSize(22000, 18000);
	WindowOrigin = CPoint(-1000, -1000);
	if (theApp.viewparameters && (theApp.disMode==normalMode || theApp.disMode==shelfMode))
		WindowExtent.cx += 3000;
	if (theApp.viewfileinfo && (theApp.disMode==normalMode || theApp.disMode==shelfMode)) {
		WindowExtent.cy += 1000;
	}
	if (pDoc->peaks.GetCount() && (theApp.disMode==peakpickMode || ((theApp.disMode==normalMode || theApp.disMode==shelfMode) && ViewPeakLabels))) {
		WindowExtent.cy += 2000;
	}
	if (axisUnits != noUnits) {
		WindowExtent.cy += 1000;
		WindowOrigin.y -= 1000;
	}
	int width, height;
	pDC->SetWindowExt(WindowExtent);
	pDC->SetWindowOrg(WindowOrigin);
	if (pDC->IsPrinting()) {
		width = pDC->GetDeviceCaps(HORZRES);
		height = pDC->GetDeviceCaps(VERTRES);
		pDC->SetViewportExt(width, -height);
		pDC->SetViewportOrg(0, height);
	} else {
		GetClientRect(LPRECT(SpecRect));
		width = SpecRect.Width();
		height = SpecRect.Height();
		ViewportExtent = CSize(width, -height);
		ViewportOrigin = CPoint(0, height);
		pDC->SetViewportExt( ViewportExtent );
		pDC->SetViewportOrg( ViewportOrigin );
	}
	logpixelsx = pDC->GetDeviceCaps(LOGPIXELSX);
	OnDrawCore(pDoc, pDC, width);

	// Repaint the zoom cursors.
	if (! pDC->IsPrinting()) {
		CPoint pt = CursorPosition;
		CursorPosition.x = CursorPosition.y = -1;
		if ( cursor.left >= 0 ) {
			CPen zoomPen;
			zoomPen.CreatePen(PS_SOLID, 0, theApp.Color[zoomcColor]^theApp.Color[backgColor]);
			CPen *oldPen = pDC->SelectObject(&zoomPen);
			int oldMode = pDC->SetROP2(R2_XORPEN);
			pDC->MoveTo(cursor.left, 0);
			pDC->LineTo(cursor.left, 16000);
			if (cursor.right >= 0) {
				pDC->MoveTo(cursor.right, 0);
				pDC->LineTo(cursor.right, 16000);
			}
			pDC->SetROP2(oldMode);
			pDC->SelectObject(oldPen);
		}
	}
}

void CNet12NMRView::OnDrawCore(CNet12NMRDoc *pDoc, CDC *pDC, int width)
{
	CPen *oldPen;
	int oldbkmode = pDC->SetBkMode(TRANSPARENT);
	const float ydif = 16000.0f/(dmax - dmin);
	if (! pDoc->sp.ftflag) {
		CPen ifidPen;
		ifidPen.CreatePen(PS_SOLID, 0, theApp.Color[imfidColor]);
		oldPen = pDC->SelectObject(&ifidPen);
		int i = zoom.left;
		while (1) {
			POINT pts[1025];
			int npts = 0;
			while (npts<1025 && i<zoom.right) {
				pts[npts].x = MulDiv(i - zoom.left, 20000, zoom.right - zoom.left);
				int y = int((pDoc->dt[i++].imag() - dmin) * ydif);
				if (y > 16000)
					y = 16000;
				else if (y < 0)
					y = 0;
				pts[npts++].y = y;
			}
			if (npts > 1)
				pDC->Polyline(pts, npts);
			if (i >= zoom.right)
				break;
			i--;
		}
		pDC->SelectObject(oldPen);
	}
	{
		CPen specPen;
		specPen.CreatePen(PS_SOLID, 0, pDoc->sp.ftflag ? theApp.Color[spectColor] : theApp.Color[refidColor]);
		oldPen = pDC->SelectObject(&specPen);
		int i = zoom.left;
		while (1) {
			POINT pts[1025];
			int npts = 0;
			while (npts<1025 && i<zoom.right) {
				pts[npts].x = MulDiv( i - zoom.left, 20000, zoom.right - zoom.left );
				int y = int((pDoc->dt[i++].real() - dmin) * ydif);
				if (y > 16000)
					y = 16000;
				else if (y < 0)
					y = 0;
				pts[npts++].y = y;
			}
			if (npts > 1)
				pDC->Polyline(pts, npts);
			if (i >= zoom.right)
				break;
			i--;
		}
		pDC->SelectObject(oldPen);
	}
	if (theApp.disMode==baselineMode && pDoc->sp.ftflag) {
		CPen onPen, offPen;
		onPen.CreatePen(PS_SOLID, 0, theApp.Color[basenColor]);
		offPen.CreatePen(PS_SOLID, 0, theApp.Color[basefColor]);
		oldPen = pDC->SelectObject( &onPen );
		pDoc->InitializeBaseline();
		bool lastbaseline = false;
		for (int i=zoom.left; i<zoom.right; i++) {
			int x = MulDiv(i - zoom.left, 20000, zoom.right - zoom.left);
			int y = int((pDoc->fbaseline[i] - dmin) * ydif );
			if ( y > 16000 )
				y = 16000;
			else if ( y < 0 )
				y = 0;
			bool bi = pDoc->bbaseline[i];
			if (lastbaseline != bi) {
				pDC->SelectObject(bi ? &offPen : &onPen);
				lastbaseline = bi;
			}
			if (i == zoom.left)
				pDC->MoveTo(x, y);
			else
				pDC->LineTo(x, y);
		}
		pDC->SelectObject(oldPen);
	}
	if (pDoc->peaks.GetCount() && (theApp.disMode==peakpickMode || ((theApp.disMode==normalMode || theApp.disMode==shelfMode) && ViewPeakLabels))) {
		COLORREF oldColor;
		CPen peakPen, highPen, *oldPen;
		peakPen.CreatePen(PS_SOLID, 0, theApp.Color[peaklColor]);
		highPen.CreatePen(PS_SOLID, 0, theApp.Color[peakhColor]);
		oldPen = pDC->SelectObject( &peakPen );
		oldColor = pDC->SetTextColor(theApp.Color[peaklColor]);
		CFont peakFont;
		peakFont.CreateFont(360, 0, 2700, 2700, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Arial");
		CFont *oldFont = pDC->SelectObject( &peakFont );
		const int slotsize = 108;
		PEAK *slot[slotsize];
		for (int i=0; i<slotsize; i++)
			slot[i] = NULL;
		POSITION pos = pDoc->peaks.GetHeadPosition();
		int zdif = zoom.right - zoom.left;
		while (pos) {
			PEAK *peak = pDoc->peaks.GetNext(pos);
			if (zoom.left<=int(peak->n) && int(peak->n)<zoom.right) {
				int x = MulDiv(peak->n-zoom.left, slotsize-1, zdif);
				if (NULL == slot[x])
					// if you see your chance, take it!
					slot[x] = peak;
				else {
					// the slot is already filled, figure out where the pack is
					// average position of the pack
					int ap = peak->n;
					// low is the first available slot on the lowfield side
					int low = x - 1;
					while (low>=0 && slot[low]) {
						ap += slot[low]->n;
						low--;
					}
					// high is the first available slot on the highfield side
					int high = x + 1;
					while (high<slotsize && slot[high]) {
						ap += slot[high]->n;
						high++;
					}
					ap /= (high - low) - 1;
					ap = MulDiv(ap-zoom.left, slotsize-1, zdif);
					if (low>=0 && (ap-low<high-ap || high>=slotsize)) {
						// if there is a empty low and either the pack is skewed to high or there is no more roon on high side then
						// move pac to low by one slot
						for (int i=low+1; i<=high-1; i++)
							slot[i-1] = slot[i];
						slot[high-1] = peak;
					} else if (high < slotsize)
						slot[high] = peak;
					else
						break;
				}
			}
		}
		for (int i=0; i<slotsize; i++) {
			if (slot[i]) {
				pDC->SelectObject(slot[i]->selected ? &highPen : &peakPen);
				pDC->SetTextColor(slot[i]->selected ? theApp.Color[peakhColor] : theApp.Color[peaklColor]);
				double val = theApp.useparabolicfit ? pDoc->ntoppm(slot[i]->nfit) : pDoc->ntoppm(slot[i]->n);
				double from = pDoc->ntoppm(zoom.left);
				double dif = from - pDoc->ntoppm(zoom.right);
				int x = int((20000.0 * (from - val))/ dif + 0.5);
				pDC->MoveTo(x, 16100);
				pDC->LineTo(x, 16200);
				x = MulDiv(i, 20000, slotsize-1);
				pDC->LineTo(x, 16700);
				pDC->LineTo(x, 16800);
				if (! theApp.labelisppm) 
					val *= pDoc->sp.sf;
				CString str;
				str.Format("%.*f", theApp.labelisppm?theApp.digitsPPM:theApp.digitsHz, val);
				pDC->TextOut(x-140, 16900, str);
			}
		}
		pDC->SelectObject( oldFont );
		pDC->SelectObject( oldPen );
		pDC->SetTextColor( oldColor );
	}
	if (theApp.disMode==peakpickMode) {
		CPen thrsPen;
		thrsPen.CreatePen(PS_SOLID, 0, theApp.Color[peaktColor]);
		oldPen = pDC->SelectObject( &thrsPen );
		int zdif = zoom.right - zoom.left;
		POSITION pos = pDoc->peakregions.GetHeadPosition();
		while (pos) {
			PREGION *p = pDoc->peakregions.GetNext(pos);
			if (zoom.left < int(p->stop) && int(p->start) < zoom.right) {
				int x1 = MulDiv(int(p->start)-zoom.left, 20000, zdif);
				if (x1 < 0)
					x1 = 0;
				int x2 = MulDiv(int(p->stop)-zoom.left, 20000, zdif);
				if (x2 > 20000)
					x2 = 20000;
				int y = int((p->threshold-dmin) * ydif);
				if (y < 0)
					y = 0;
				else if (y > 16000)
					y = 16000;
				pDC->MoveTo(x1, y);
				pDC->LineTo(x2, y);
			}
		}
		pDC->SelectObject( oldPen );
	}
	if (pDoc->intregions.GetCount() && (theApp.disMode==integralMode || ((theApp.disMode==normalMode || theApp.disMode==shelfMode) && ViewIntegral))) {
		float IntegralConst = pDoc->sp.bintegralconst ? pDoc->sp.integralconst : theApp.GlobalIntegralConst;
		if (IntegralConst <= 0.0f)
			IntegralConst = 1.0f;
		CFont intlFont;
		intlFont.CreateFont( 540, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Arial");
		CPen integralPen, selectedPen;
		COLORREF oldColor;
		UINT oldAlign = pDC->SetTextAlign(TA_TOP|TA_BOTTOM|TA_NOUPDATECP);
		CFont *oldFont = pDC->SelectObject( &intlFont );
		integralPen.CreatePen(PS_SOLID, 0, theApp.Color[integColor]);
		selectedPen.CreatePen(PS_SOLID, 0, theApp.Color[intehColor]);
		oldPen = pDC->SelectObject( &integralPen );
		oldColor = pDC->SetTextColor(theApp.Color[integColor]);
		float intydif = 0.0;
		if (integralmax - integralmin)
			intydif = 14000.0f / (integralmax - integralmin);
		const int xdiff = zoom.right - zoom.left;
		POSITION pos = pDoc->intregions.GetHeadPosition();
		while (pos) {
			IREGION *p = pDoc->intregions.GetNext(pos);
			pDC->SelectObject(p->selected ? &selectedPen : &integralPen);
			pDC->SetTextColor(p->selected ? theApp.Color[intehColor] : theApp.Color[integColor]);
			bool pendown = false;
			if (! (int(p->stop)<zoom.left || int(p->start)>zoom.right)) {
				UINT n = p->stop - p->start;
				for (UINT i=0; i<n; i++) {
					int x = int(p->start + i);
					if (zoom.left<=x && x<zoom.right) {
						x = MulDiv(x-zoom.left, 20000, xdiff);
						int y = int((p->integral[i]-integralmin) * intydif) + 1000;
						if (pendown)
							pDC->LineTo(x, y);
						else {
							pDC->MoveTo(x, y);
							pendown = true;
						}
						if (i == n-1) {
							CString str;
							str.Format("%.2f", IntegralConst * p->total);
							pDC->TextOut(x, y, str);
						}
					}
				}
			}
		}
		pDC->SetTextAlign(oldAlign);
		pDC->SelectObject(oldFont);
		pDC->SetTextColor( oldColor );
		pDC->SelectObject( oldPen );
	}
	if ( axisUnits!=noUnits && axisUnits!=nullUnits ) {
		CAxis axis;
		axis.Update(pDC, 20000, width, axisStart, axisStop, axisLabel, logpixelsx);
	}

	if (theApp.viewfileinfo && (theApp.disMode==normalMode || theApp.disMode==shelfMode)) {
		CFont fileFont;
		fileFont.CreateFont(440, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Times New Roman");
		pDC->SetTextColor(theApp.Color[titleColor]);
		CString str;
		CString pstr;
		CFont font;
		UINT oldalign = pDC->SetTextAlign(TA_TOP|TA_LEFT|TA_NOUPDATECP);
		CFont *oldfont = pDC->SelectObject(&fileFont);
		str = pDoc->fullpathname;
		int y = WindowExtent.cy + WindowOrigin.y - 500;
		pDC->TextOut(0, y, str);
		if ( pDoc->GetFlexHeaderItem("date", pstr)) 
			str.Format("Acquired on %s", pstr);
		else
			str.Empty();
		if (pDoc->GetFlexHeaderItem("user", pstr)) {
			if (str.IsEmpty())
				str.Format("For %s", pstr);
			else {
				str += " for ";
				str += pstr;
			}
		}
		if (! str.IsEmpty()) {
			pDC->TextOut(0, y-500, str);
			pDC->SetTextAlign(TA_TOP | TA_RIGHT | TA_NOUPDATECP);
		}
		if (pDoc->GetFlexHeaderItem("sample", pstr))
			str = pstr;
		else
			str.Empty();
		if (pDoc->GetFlexHeaderItem("comment", pstr)) {
			if (str.IsEmpty())
				str = pstr;
			else {
				str += " ";
				str += pstr;
			}
		}
		int x = WindowExtent.cx + WindowOrigin.x - 1000;
		if (! str.IsEmpty())
			pDC->TextOut(x, y, str);
		if (pDoc->GetFlexHeaderItem("workorder", pstr))
			str.Format("WO# %s", pstr);
		else
			str.Empty();
		if (pDoc->GetFlexHeaderItem("submitted", pstr)) {
			if (str.IsEmpty())
				str.Format("(%s)", pstr);
			else {
				str += " (";
				if (pstr.IsEmpty())
					str += "unknown";
				else
					str += pstr;
				str += ")";
			}
		}
		if (pDoc->GetFlexHeaderItem("project", pstr)) {
			if (str.IsEmpty())
				str.Format("Prj# %s", pstr);
			else {
				str += " Prj# ";
				str += pstr;
			}
		}
		if (theApp.DataClassification.Count()) {
			if (! str.IsEmpty())
				str += " ";
			if (! pDoc->GetFlexHeaderItem("dataclass", pstr))
				pstr = "x";
			str += theApp.DataClassification.Lookup(pstr);
		}
		if ( ! str.IsEmpty() )
			pDC->TextOut( x, y-500, str );
		pDC->SetTextAlign( oldalign );
		pDC->SelectObject( oldfont );
		font.DeleteObject();
		//pDC->RestoreDC( -1 );
	}
	if (theApp.viewparameters && (theApp.disMode==normalMode || theApp.disMode==shelfMode)) {
		CParam param(WindowExtent.cy + WindowOrigin.y);
		param.Draw(pDC, pDoc);
	}
	pDC->SetBkMode( oldbkmode );
}


// CNet12NMRView printing


void CNet12NMRView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CNet12NMRView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CNet12NMRView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CNet12NMRView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CNet12NMRView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CNet12NMRView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	UINT menuid;
	if (phaseMode == theApp.disMode) {
		menuid = IDR_PHASEMENU;
		ReleaseCapture();
		ShowCursor(TRUE);
		CMenu menu;
		menu.LoadMenu(menuid);
		CMenu *popup = menu.GetSubMenu(phaseonleft ? 0 : 1);
		menuid = theApp.GetContextMenuManager()->TrackPopupMenu(popup->m_hMenu, point.x, point.y, this );
		ShowCursor(FALSE);
		CMainFrame *pMain = STATIC_DOWNCAST( CMainFrame, AfxGetMainWnd() );
		SetCursorPos( pMain->midscreen.x, pMain->midscreen.y );
		SetCapture();
		switch (menuid) {
		case ID_LEFTPHASE_COMPLETE:
		case ID_RIGHTPHASE_COMPLETE:
			theApp.GlobalPhaseA += phaseA;
			theApp.GlobalPhaseB += phaseB;
			OnAtcursorbutton();
			break;
		case ID_LEFTPHASE_RIGHT:
		case ID_RIGHTPHASE_LEFT:
			phaseonleft = !phaseonleft;
			Invalidate();
			break;
		case ID_LEFTPHASE_UP:
		case ID_RIGHTPHASE_UP:
			maxdtphasemode *= 0.5f;
			Invalidate();
			break;
		case ID_LEFTPHASE_DOWN:
		case ID_RIGHTPHASE_DOWN:
			maxdtphasemode *= 2.0f;
			Invalidate();
			break;
		case ID_LEFTPHASE_ABORT:
		case ID_RIGHTPHASE_ABORT:
			OnAtcursorbutton();
			break;
		}
	} else {
		CursorY = CursorPosition.y;
		menuid = IDR_POPUP_EDIT;
		CRect r(0, 0, 20001, 16001);
		if (r.PtInRect(CursorPosition)) {
			switch (theApp.disMode) {
			case baselineMode:
				menuid = IDR_BASELINEMENU;
				break;
			case integralMode:
				menuid = IDR_INTEGRALMENU;
				break;
			case peakpickMode:
				menuid = IDR_PEAKMENU;
				break;
			}
		}
		theApp.GetContextMenuManager()->ShowPopupMenu(menuid, point.x, point.y, AfxGetMainWnd());
	}
#endif
}


// CNet12NMRView diagnostics

#ifdef _DEBUG
void CNet12NMRView::AssertValid() const
{
	CView::AssertValid();
}

void CNet12NMRView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CNet12NMRDoc* CNet12NMRView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CNet12NMRDoc)));
	return (CNet12NMRDoc*)m_pDocument;
}
#endif //_DEBUG

void CNet12NMRView::FindMinMax(void)
{
	CNet12NMRDoc *pDoc = GetDocument();
	float max = pDoc->dt[zoom.left].real();
	float min = max;
	for (int i=zoom.left; i<zoom.right; i++) {
		float re = pDoc->dt[i].real();
		if (re > max)
			max = re;
		else {
			if (re < min)
				min = re;
		}
		if (false == pDoc->sp.ftflag) {
			float im = pDoc->dt[i].imag();
			if (im > max)
				max = im;
			else {
				if (im < min)
					min = im;
			}
		}
	}
	dmax = max;
	dmin = min;
	if (dmax == dmin)
		dmax = dmin + 1.0f;
	if (pDoc->intregions.GetCount())
		FindIntegralMax();
}



void CNet12NMRView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (WindowExtent.cx <= 0)
		return;
	CMainFrame *mw = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if (phaseMode == theApp.disMode) {
		mw->MessageToBar("", IDS_STATUS_PANE2);
		ClientToScreen(&point);
		CNet12NMRDoc *pDoc = GetDocument();
		CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
		float inc = (2.0f*maxdtphasemode) / (dmax-dmin);
		if (inc > 1.0f)
			inc = 1.0f;
		else if (inc < 0.0625f)
			inc = 0.0625f;
		inc *= float(pMain->midscreen.y - point.y);
		if (nFlags & MK_CONTROL)
			inc *= 0.1f;
		else if (nFlags & MK_SHIFT)
			inc *= 10.0f;
		if (inc) {
			int size = int(vdt.size());
			float a, b = inc*float(size)/float(zoom.left-zoom.right);
			if (phaseonleft) {
				b *= -1.0;
				a = float(float(size/2-index.right)*b/float(size));
			} else
				a = float(float(size/2-index.left)*b/float(size));
			phaseA += a;
			phaseB += b;
			while (phaseA < 0.0f)
				phaseA += 360.0f;
			while (phaseA >= 360.0f)
				phaseA -= 360.0f;
			if (phaseA || phaseB) {
				for (int i=zoom.left; i<zoom.right; i++)
				{
					float theta = 1.74532925199e-2f * (phaseA + float(i-size/2) * phaseB / float(size));
					vdt[i] = polar(abs(pDoc->dt[i]), arg(pDoc->dt[i]) - theta);
				}
			} else {
				for (int i=zoom.left; i<zoom.right; i++)
					vdt[i] = pDoc->dt[i];
			}
			ReleaseCapture();
			SetCursorPos(pMain->midscreen.x, pMain->midscreen.y);
			SetCapture();
			Invalidate();
		}
		CView::OnMouseMove(nFlags, point);
		return;
	}
	CDC *pDC = GetDC();
	if (pDC) {
		CPen crsrPen;
		crsrPen.CreatePen(PS_SOLID, 0, theApp.Color[crossColor]^theApp.Color[backgColor]);
		CRect r;
		pDC->SetMapMode(MM_ANISOTROPIC);
		pDC->SetWindowExt(WindowExtent);
		pDC->SetWindowOrg(WindowOrigin);
		pDC->SetViewportExt(ViewportExtent);
		pDC->SetViewportOrg(ViewportOrigin);
		int oldmode = pDC->SetROP2(R2_XORPEN);
		CPen *oldPen = pDC->SelectObject(&crsrPen);
		if (CursorPosition.x>=0 && CursorPosition.x<=20000 && CursorPosition.y>=0 && CursorPosition.y<=16000) {
			pDC->MoveTo(CursorPosition.x, 0);
			pDC->LineTo(CursorPosition.x, 16000);
			pDC->MoveTo(0, CursorPosition.y);
			pDC->LineTo(20000, CursorPosition.y);
		}
		CursorPosition = point;
		pDC->DPtoLP(&CursorPosition);
		if (mw) {
			mw->MessageToBar(GetCursorDifference(), IDS_STATUS_PANE2);
			if (CursorPosition.x>=0 && CursorPosition.x<=20000 && CursorPosition.y>=0 && CursorPosition.y<=16000) {
				pDC->MoveTo(CursorPosition.x, 0);
				pDC->LineTo(CursorPosition.x, 16000);
				pDC->MoveTo(0, CursorPosition.y);
				pDC->LineTo(20000, CursorPosition.y);
				if (axisUnits!=noUnits) {
					CNet12NMRDoc *pDoc = GetDocument();
					double ydif = double(dmax - dmin);
					double xdif = axisStop - axisStart;
					CString str;
					str.Format("X=%.2f Y=%.2f", 
						5.00e-5 * xdif * double(CursorPosition.x) + axisStart,
						6.25e-5 * ydif * double(CursorPosition.y) + double(dmin));
					if (theApp.disMode==integralMode && pDoc->sp.ftflag) {
						int x = zoom.left + MulDiv(CursorPosition.x, zoom.right-zoom.left, 20000);
						POSITION pos = pDoc->intregions.GetHeadPosition();
						while (pos) {
							IREGION *p = pDoc->intregions.GetNext(pos);
							if (int(p->start)<=x && x<int(p->stop)) {
								CString istr;
								istr.Format(" Integral=%.2f", p->integral[x-p->start] * 
									(pDoc->sp.bintegralconst ? pDoc->sp.integralconst : theApp.GlobalIntegralConst));
								str += istr;
							}
						}
					}
					mw->MessageToBar(str, IDS_STATUS_PANE1);
				}  else
					mw->MessageToBar("", IDS_STATUS_PANE1);
			}
		}
	
		pDC->SetROP2(oldmode);
		pDC->SelectObject(oldPen);
		ReleaseDC(pDC);
	}
	CView::OnMouseMove(nFlags, point);
}


BOOL CNet12NMRView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (zDelta) {
		float f = abs(float(zDelta))/float(WHEEL_DELTA);
		f *= 1.189207115f; // Fourth root of 2
		if (zDelta > 0)
			f = 1.0f/f;
		if (phaseMode == theApp.disMode) {
			if (nFlags & MK_CONTROL) {
				float offsetdelta = 0.01f * maxdtphasemode;
				if (zDelta < 0)
					offsetdelta = -offsetdelta;
				offsetdtphasemode += offsetdelta;
			} else
				maxdtphasemode *= f;
			Invalidate();
		} else
			OnViewY(f);
	}
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}


void CNet12NMRView::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (phaseMode == theApp.disMode) {
		phaseonleft = !phaseonleft;
		Invalidate();
	} else {
		CDC *pDC = GetDC();
		pDC->SetMapMode(MM_ANISOTROPIC);
		pDC->SetWindowExt(WindowExtent);
		pDC->SetWindowOrg(WindowOrigin);
		pDC->SetViewportExt(ViewportExtent);
		pDC->SetViewportOrg(ViewportOrigin);
		//TRACE1("OnLButtonDown() point.y=%d\n", point.y);
		pDC->DPtoLP(&point);
		CursorY = point.y;
		//TRACE1("OnLButtonDown() CursorY=%d\n", CursorY);
		CPen zoomPen;
		zoomPen.CreatePen(PS_SOLID, 0, theApp.Color[zoomcColor]^theApp.Color[backgColor]);
		CPen *oldPen = pDC->SelectObject(&zoomPen);
		int oldMode = pDC->SetROP2(R2_XORPEN);
		if (cursor.right >= 0) {
			pDC->MoveTo(cursor.right, 0);
			pDC->LineTo(cursor.right, 16000);
			cursor.right = -1;
		}
		if (cursor.left >= 0) {
			pDC->MoveTo(cursor.left, 0);
			pDC->LineTo(cursor.left, 16000);
		}
		cursor.left = point.x;
		if (cursor.left < 0)
			cursor.left = 0;
		else if (cursor.left > 20000)
			cursor.left = 20000;
		pDC->MoveTo(cursor.left, 0);
		pDC->LineTo(cursor.left, 16000);
		pDC->SetROP2(oldMode);
		pDC->SelectObject(oldPen);
		ReleaseDC(pDC);
		SetCapture();
	}
	CView::OnLButtonDown(nFlags, point);
}


void CNet12NMRView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (phaseMode != theApp.disMode) {
		if (cursor.left >= 0 && cursor.right == -1) {
			CDC *pDC = GetDC();
			pDC->SetMapMode(MM_ANISOTROPIC);
			pDC->SetWindowExt(WindowExtent);
			pDC->SetWindowOrg(WindowOrigin);
			pDC->SetViewportExt(ViewportExtent);
			pDC->SetViewportOrg(ViewportOrigin);
			pDC->DPtoLP(&point);
			CPen zoomPen;
			zoomPen.CreatePen( PS_SOLID, 0, theApp.Color[zoomcColor]^theApp.Color[backgColor]);
			CPen *oldPen = pDC->SelectObject(&zoomPen);
			int oldMode = pDC->SetROP2(R2_XORPEN);
			cursor.right = point.x;
			if (cursor.right < 0)
				cursor.right = 0;
			else if (cursor.right > 20000)
				cursor.right = 20000;
			pDC->MoveTo(cursor.right, 0);
			pDC->LineTo(cursor.right, 16000);
			if (cursor.left == cursor.right)
				cursor.left = cursor.right = -1;
			else if (cursor.left > cursor.right) {
				int i = cursor.left;
				cursor.left = cursor.right;
				cursor.right = i;
			}
			pDC->SetROP2(oldMode);
			pDC->SelectObject(oldPen);
			ReleaseDC(pDC);
			ReleaseCapture();
			switch (theApp.disMode) {
			case baselineMode:
				if (nFlags & MK_SHIFT)
					OnBaseline(true);
				else if (nFlags & MK_CONTROL)
					OnBaseline(false);
				break;
			case integralMode:
				if (nFlags & MK_SHIFT)
					OnIntegral(true);
				else if (nFlags & MK_CONTROL)
					OnIntegral(false);
				break;
			case peakpickMode:
				if (nFlags & MK_SHIFT) {
					float f = dmin + ((dmax - dmin) * float(CursorY) / 16000.0f);
					OnThreshold(f);
				} else if (nFlags & MK_CONTROL)
					OnThreshold(0.0f);
				break;
			}
		}
	}
	CView::OnLButtonUp(nFlags, point);
}


void CNet12NMRView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (phaseMode == theApp.disMode) {
		switch (nChar) {
		case 'c':
		case 'C':
			theApp.GlobalPhaseA += phaseA;
			theApp.GlobalPhaseB += phaseB;
		case 'a':
		case 'A':
			OnAtcursorbutton();
			break;
		case 'd':
		case 'D':
			maxdtphasemode *= 2.0f;
			Invalidate();
			break;
		case 'l':
		case 'L':
			if (! phaseonleft) {
				phaseonleft = true;
				Invalidate();
			}
			break;
		case 'r':
		case 'R':
			if (phaseonleft) {
				phaseonleft = false;
				Invalidate();
			}
			break;
		case 'u':
		case 'U':
			maxdtphasemode *= 0.5f;
			Invalidate();
			break;
		}
	} else {
		switch (nChar) {
		case VK_LEFT:
			if (cursor.left > 0)
				MoveZoomCursors(-1);
			break;
		case VK_RIGHT:
			if (cursor.left >= 0)
				MoveZoomCursors(1);
			break;
		case 'h':
		case 'H':
			SplitZoomCursors();
			break;
		}
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CNet12NMRView::SplitZoomCursors()
{
	if (cursor.left < 0)
		return;
	int dif = cursor.right - cursor.left;
	if (dif < 2)
		return;
	CDC *pDC = GetDC();
	pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetWindowExt(WindowExtent);
	pDC->SetWindowOrg(WindowOrigin);
	pDC->SetViewportExt(ViewportExtent);
	pDC->SetViewportOrg(ViewportOrigin);
	CPen zoomPen;
	zoomPen.CreatePen(PS_SOLID, 0, theApp.Color[zoomcColor]^theApp.Color[backgColor]);
	CPen *oldPen = pDC->SelectObject(&zoomPen);
	int oldMode = pDC->SetROP2(R2_XORPEN);
	pDC->MoveTo(cursor.right, 0);
	pDC->LineTo(cursor.right, 16000);
	cursor.right = cursor.left + (dif / 2);
	pDC->MoveTo(cursor.right, 0);
	pDC->LineTo(cursor.right, 16000);
	pDC->SetROP2(oldMode);
	pDC->SelectObject(oldPen);
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	pMain->MessageToBar(GetCursorDifference(), IDS_STATUS_PANE2);
}

void CNet12NMRView::MoveZoomCursors(int x)
{
	x = MulDiv(x, WindowExtent.cx, ViewportExtent.cx);
	if ((cursor.left<=0 && x<0) || (cursor.right>=20000 && x>0))
		return;
	if (x > 0) {
		if (cursor.right + x > 20000)
			x = 20000 - cursor.right;
	} else if (x < 0) {
		if (cursor.left + x < 0)
			x = cursor.left;
	}
	CDC *pDC = GetDC();
	pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetWindowExt(WindowExtent);
	pDC->SetWindowOrg(WindowOrigin);
	pDC->SetViewportExt(ViewportExtent);
	pDC->SetViewportOrg(ViewportOrigin);
	CPen zoomPen;
	zoomPen.CreatePen(PS_SOLID, 0, theApp.Color[zoomcColor]^theApp.Color[backgColor]);
	CPen *oldPen = pDC->SelectObject(&zoomPen);
	int oldMode = pDC->SetROP2(R2_XORPEN);
	pDC->MoveTo(cursor.left, 0);
	pDC->LineTo(cursor.left, 16000);
	cursor.left += x;
	pDC->MoveTo(cursor.left, 0);
	pDC->LineTo(cursor.left, 16000);
	pDC->MoveTo(cursor.right, 0);
	pDC->LineTo(cursor.right, 16000);
	cursor.right += x;
	pDC->MoveTo(cursor.right, 0);
	pDC->LineTo(cursor.right, 16000);
	pDC->SetROP2(oldMode);
	pDC->SelectObject(oldPen);
}

BOOL CNet12NMRView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if ( CursorPosition.x>=0 && CursorPosition.x<=20000 &&
		 CursorPosition.y>=0 && CursorPosition.y<=16000 ) {
		SetCursor( NULL );
		return TRUE;
	}
	return CView::OnSetCursor(pWnd, nHitTest, message);
}


BOOL CNet12NMRView::OnEraseBkgnd(CDC* pDC)
{
    // Set brush to desired background color.
    CBrush backBrush(theApp.Color[backgColor]);
    // Save old brush.
    CBrush* pOldBrush = pDC->SelectObject(&backBrush);
    CRect rect;
    pDC->GetClipBox(&rect);     // Erase the area needed.
    pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
    pDC->SelectObject(pOldBrush);
    return TRUE;
}


void CNet12NMRView::OnViewY(float factor)
{
	if (yMode == fromMiddle) {
		float y = fabsf(dmax);
		if (AutoYScale && fabsf(dmin) > y)
			y = fabsf(dmin);
		dmax = float(factor) * y;
		dmin = -dmax;
	} else {
		float ydif = dmax - dmin;
		if (yMode == fromTop)
			dmin = dmax - float(factor) * ydif;
		else
			dmax = dmin + float(factor) * ydif;
	}
	AutoYScale = false;
	Invalidate();
}

double CNet12NMRView::PtoH(unsigned int n)
{
	CNet12NMRDoc *pDoc = GetDocument();
	double hz = pDoc->sp.of + (0.5 - double(n)/double(pDoc->dt.size()-1)) * pDoc->sp.sw;
	return hz;
}

void CNet12NMRView::SetAxisRange(void)
{
	double start, stop;
	char *label;
	CNet12NMRDoc *pDoc = GetDocument();
	switch ( axisUnits ) {
	case pointsUnits:
		start = double( zoom.left );
		stop = double( zoom.right );
		label = "Points";
		break;
	case timeUnits:
		start = 1.0e-6 * pDoc->sp.de + pDoc->sp.dw * zoom.left;
		stop =  1.0e-6 * pDoc->sp.de + pDoc->sp.dw * zoom.right;
		if ( stop < 2.0 ) {
			start *= 1000.0;
			stop *= 1000.0;
			if ( stop < 2.0 ) {
				start *= 1000.0;
				stop *= 1000.0;
				label = "usec";
			} else
				label = "msec";
		} else
			label = "sec";
		break;
	default:
		start = PtoH( zoom.left );
		stop = PtoH( zoom.right );
		if ( axisUnits == ppmUnits ) {
			start /= pDoc->sp.sf;
			stop /= pDoc->sp.sf;
			label = "PPM";
		} else {
			if ( fabs(start) > 20000.0 || fabs(stop) > 20000.0 ) {
				start *= 0.001;
				stop *= 0.001;
				label = "Kilohertz";
			} else
				label = "Hertz";
		}
		break;
	}
	axisStart = start;
	axisStop = stop;
	axisLabel = label;
}

void CNet12NMRView::OnUpdateAutoybutton(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(AutoYScale ? TRUE : FALSE);
}

void CNet12NMRView::OnAutoybutton()
{
	AutoYScale = !AutoYScale;
	if (AutoYScale)
		FindMinMax();
	Invalidate();
}


void CNet12NMRView::OnUpybutton()
{
	OnViewY(0.5);
}


void CNet12NMRView::OnDownybutton()
{
	OnViewY(2.0);
}


void CNet12NMRView::OnViewYTop()
{
	yMode = fromTop;
}

void CNet12NMRView::OnUpdateViewYTop(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(yMode==fromTop ? 1 : 0);
}


void CNet12NMRView::OnViewYCenter()
{
	yMode = fromMiddle;
}

void CNet12NMRView::OnUpdateViewYCenter(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(yMode==fromMiddle ? 1 : 0);
}

void CNet12NMRView::OnViewYBottom()
{
	yMode = fromBottom;
}


void CNet12NMRView::OnUpdateViewYBottom(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(yMode==fromBottom ? 1 : 0);
}

void CNet12NMRView::EnableIfFTflag(CCmdUI *pCmdUI, BOOL value)
{
	bool bvalue = (value==FALSE) ? false : true;
	pCmdUI->Enable((GetDocument()->sp.ftflag==bvalue) ? 1 : 0);
}

void CNet12NMRView::OnUpdateTruncatebutton(CCmdUI *pCmdUI)
{
	EnableIfFTflag(pCmdUI, FALSE);
}

void CNet12NMRView::OnTruncatebutton()
{
	GetDocument()->Truncate();
	OnEditFull();
}

void CNet12NMRView::OnUpdateDclevelbutton(CCmdUI *pCmdUI)
{
	EnableIfFTflag(pCmdUI, FALSE);
}


void CNet12NMRView::OnDclevelbutton()
{
	GetDocument()->DCLevelCorrect();
	if (AutoYScale)
		FindMinMax();
	Invalidate();
}


void CNet12NMRView::OnUpdateQcbutton(CCmdUI *pCmdUI)
{
	EnableIfFTflag(pCmdUI, FALSE);
}


void CNet12NMRView::OnQcbutton()
{
	GetDocument()->QuadratureCorrection();
	if (AutoYScale)
		FindMinMax();
	Invalidate();
}

void CNet12NMRView::OnUpdateLeftshiftbutton(CCmdUI *pCmdUI)
{
	EnableIfFTflag(pCmdUI, FALSE);
}

void CNet12NMRView::OnLeftshiftbutton()
{
	GetDocument()->LeftShift();
	if (AutoYScale)
		FindMinMax();
	Invalidate();
}

void CNet12NMRView::OnUpdateRightshiftbutton(CCmdUI *pCmdUI)
{
	EnableIfFTflag(pCmdUI, FALSE);
}

void CNet12NMRView::OnRightshiftbutton()
{
	GetDocument()->RightShift();
	if (AutoYScale)
		FindMinMax();
	Invalidate();
}

void CNet12NMRView::OnUpdateApodizebutton(CCmdUI *pCmdUI)
{
	EnableIfFTflag(pCmdUI, FALSE);
}

void CNet12NMRView::OnApodizebutton()
{
	GetDocument()->Apodize();
	if (AutoYScale)
		FindMinMax();
	Invalidate();
}

void CNet12NMRView::OnUpdateZerofillbutton(CCmdUI *pCmdUI)
{
	EnableIfFTflag(pCmdUI, FALSE);
}

void CNet12NMRView::OnZerofillbutton()
{
	GetDocument()->ZeroFill();
	OnEditFull();
}

void CNet12NMRView::OnUpdateFtbutton(CCmdUI *pCmdUI)
{
	EnableIfFTflag(pCmdUI, FALSE);
}

void CNet12NMRView::OnFtbutton()
{
	GetDocument()->FourierTransform();
	axisUnits = ppmUnits;
	SetAxisRange();
	AutoYScale = true;
	FindMinMax();
	Invalidate();
}

void CNet12NMRView::OnUpdateMagnitudebutton(CCmdUI *pCmdUI)
{
	EnableIfFTflag(pCmdUI, TRUE);
}

void CNet12NMRView::OnMagnitudebutton()
{
	GetDocument()->Magnitude();
	UpdateIntegralList();
	UpdatePeakList();
	if (AutoYScale)
		FindMinMax();
	else
		FindIntegralMax();
	Invalidate();
}

void CNet12NMRView::OnUpdateZerophasebutton(CCmdUI *pCmdUI)
{
	EnableIfFTflag(pCmdUI, TRUE);
}

void CNet12NMRView::OnZerophasebutton()
{
	theApp.GlobalPhaseA = theApp.GlobalPhaseB = 0.0f;
	GetDocument()->GlobalPhase();
	UpdateIntegralList();
	UpdatePeakList();
	if (AutoYScale)
		FindMinMax();
	else
		FindIntegralMax();
	Invalidate();
}

void CNet12NMRView::OnUpdateAutophasebutton(CCmdUI *pCmdUI)
{
	EnableIfFTflag(pCmdUI, TRUE);
}

void CNet12NMRView::OnAutophasebutton()
{
	GetDocument()->AutoPhase();
	UpdateIntegralList();
	UpdatePeakList();
	if (AutoYScale)
		FindMinMax();
	else
		FindIntegralMax();
	Invalidate();
}

void CNet12NMRView::OnUpdateAutophaseaonlybutton(CCmdUI *pCmdUI)
{
	EnableIfFTflag(pCmdUI, TRUE);
}

void CNet12NMRView::OnAutophaseaonlybutton()
{
	GetDocument()->AutoPhaseAOnly();
	UpdateIntegralList();
	UpdatePeakList();
	if (AutoYScale)
		FindMinMax();
	else
		FindIntegralMax();
	Invalidate();
}

void CNet12NMRView::OnUpdateFlipabutton(CCmdUI *pCmdUI)
{
	EnableIfFTflag(pCmdUI, TRUE);
}

void CNet12NMRView::OnFlipabutton()
{
	theApp.GlobalPhaseA += 180.0f;
	GetDocument()->GlobalPhase();
	UpdateIntegralList();
	UpdatePeakList();
	if (AutoYScale)
		FindMinMax();
	else
		FindIntegralMax();
	Invalidate();
}


void CNet12NMRView::OnUpdateIncrementbutton(CCmdUI *pCmdUI)
{
	EnableIfFTflag(pCmdUI, TRUE);
}

void CNet12NMRView::OnIncrementbutton()
{
	PhaseDelta(true);
}

void CNet12NMRView::PhaseDelta(bool isIncrement)
{
	float inc;
	switch (theApp.phaseIncrement) {
	case tenPhaseIncrement:
		inc = 10.0f;
		break;
	case onePhaseIncrement:
		inc = 1.0f;
		break;
	default:
		inc = 0.1f;
		break;
	}
	if (! isIncrement)
		inc *= -1.0f;
	if (constPhaseTerm == theApp.phaseTerm)
		theApp.GlobalPhaseA += inc;
	else
		theApp.GlobalPhaseB += inc;
	GetDocument()->GlobalPhase();
	UpdateIntegralList();
	UpdatePeakList();
	if (AutoYScale)
		FindMinMax();
	else
		FindIntegralMax();
	Invalidate();
}

void CNet12NMRView::OnUpdateDecrementbutton(CCmdUI *pCmdUI)
{
	EnableIfFTflag(pCmdUI, TRUE);
}

void CNet12NMRView::OnDecrementbutton()
{
	PhaseDelta(false);
}

void CNet12NMRView::OnUpdateFiltercompbutton(CCmdUI *pCmdUI)
{
	EnableIfFTflag(pCmdUI, TRUE);
}

void CNet12NMRView::OnFiltercompbutton()
{
	GetDocument()->FilterComp();
	UpdateIntegralList();
	UpdatePeakList();
	if (AutoYScale)
		FindMinMax();
	else
		FindIntegralMax();
	Invalidate();
}

void CNet12NMRView::OnUpdateEditZoom(CCmdUI *pCmdUI)
{
	pCmdUI->Enable((cursor.left==cursor.right) ? FALSE : TRUE);
}

void CNet12NMRView::OnEditZoom()
{
	int r = zoom.right - zoom.left;
	int i = zoom.left + MulDiv(cursor.right, r, 20000);
	int j = zoom.left + MulDiv(cursor.left, r, 20000);
	if ( i > j + 1 ) {
		if ( zoom.right > i ) zoom.right = i;
		zoom.left = j;
		cursor.left = cursor.right = -1;
		if (AutoYScale)
			FindMinMax();
		else
			FindIntegralMax();
		SetAxisRange();
		Invalidate();
	} else {
		MessageBox("Zoom region must be contain at least two points!", NULL, MB_ICONERROR);
	}
}

void CNet12NMRView::OnUpdateEditFull(CCmdUI *pCmdUI)
{
	unsigned int size = GetDocument()->dt.size();
	pCmdUI->Enable((0==zoom.left && size==zoom.right) ? FALSE : TRUE);
}

void CNet12NMRView::OnEditFull()
{
	cursor.left = cursor.right = -1;
	zoom.left = 0;
	zoom.right = int(GetDocument()->dt.size());
	if (AutoYScale)
		FindMinMax();
	else
		FindIntegralMax();
	SetAxisRange();
	Invalidate();
}

void CNet12NMRView::OnUpdateEditSetoffset(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(CursorPosition.x>=0 && CursorPosition.x<=20000 &&
		CursorPosition.y>=0 && CursorPosition.y<=16000 &&
		GetDocument()->sp.ftflag);
}

void CNet12NMRView::SetPeakOffset(PEAK *p)
{
	CNet12NMRDoc *pDoc = GetDocument();
	double oldoffset = theApp.useparabolicfit ? pDoc->ntoppm(p->nfit) : pDoc->ntoppm(p->n);
	oldoffset *= pDoc->sp.sf;
	SetOffsetCore(oldoffset);
}

void CNet12NMRView::SetOffsetCore(double oldoffset)
{
	CNet12NMRDoc *pDoc = GetDocument();
	CSetOffsetDlg dlg;
	dlg.m_Units = (axisUnits==hertzUnits) ? 0 : 1;
	dlg.m_Value.Format("%.4f", oldoffset / ( (axisUnits==ppmUnits) ? pDoc->sp.sf : 1.0 ) );
	if ( IDOK == dlg.DoModal() ) {
		double newoffset = atof(dlg.m_Value);
		if (dlg.m_Units)
			newoffset *= pDoc->sp.sf;
		pDoc->sp.of += newoffset - oldoffset;
		UpdateIntegralList(false);
		UpdatePeakList(false);
		SetAxisRange();
		pDoc->SetModifiedFlag();
		Invalidate();
	}
}

void CNet12NMRView::OnEditSetoffset()
{
	CNet12NMRDoc *pDoc = GetDocument();
	double oldoffset = double(zoom.left) + double(CursorPosition.x) * double(zoom.right-zoom.left) * 5.0e-5;
	oldoffset = pDoc->sp.of + (0.5 - oldoffset / double(pDoc->dt.size() - 1)) * pDoc->sp.sw;
	SetOffsetCore(oldoffset);
}

void CNet12NMRView::OnUpdateAtcursorbutton(CCmdUI *pCmdUI)
{
	//CNet12NMRDoc *pDoc = GetDocument();
	BOOL enable = GetDocument()->sp.ftflag ? TRUE : FALSE;
	if (enable) {
		if (cursor.left<0 || cursor.right<0)
			enable = FALSE;
	}
	pCmdUI->Enable(enable);
	if (enable)
		pCmdUI->SetCheck(phaseMode == theApp.disMode);
	else
		pCmdUI->SetCheck(FALSE);
}

void CNet12NMRView::OnAtcursorbutton()
{
	static bool prevAutoYScale;
	static float prevdmin, prevdmax;
	CNet12NMRDoc *pDoc = GetDocument();
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	pDoc->GlobalPhase();
	FindMinMax();
	if (phaseMode == theApp.disMode) {
		theApp.disMode = normalMode;
		AutoYScale = prevAutoYScale;
		ReleaseCapture();
		ShowCursor(TRUE);
		pDoc->RecalcIntegrals();
		pDoc->RecalcPeaks();
		if (AutoYScale)
			FindMinMax();
		else {
			dmax = prevdmax;
			dmin = prevdmin;
			FindIntegralMax();
		}
	} else {
		theApp.disMode = phaseMode;
		prevAutoYScale = AutoYScale;
		prevdmin = dmin;
		prevdmax = dmax;
		if (vdt.size() < pDoc->dt.size())
			vdt.resize(pDoc->dt.size());
		for (int i=zoom.left; i<zoom.right; i++)
			vdt[i] = pDoc->dt[i];
		ShowCursor(FALSE);
		CRect r;
		pMain->GetWindowRect(&r);
		pMain->midscreen = r.CenterPoint();
		SetCursorPos(pMain->midscreen.x, pMain->midscreen.y);
		SetCapture();
		phaseonleft = true;
		phaseA = phaseB = 0.0f;
		maxdtphasemode = abs(dmax);
		if (abs(dmin) > maxdtphasemode)
			maxdtphasemode = abs(dmin);
		maxdtphasemode *= 0.25f;
		offsetdtphasemode = 0.0f;
		index.left = zoom.left + MulDiv(cursor.left, zoom.right-zoom.left, 20000);
		index.right = zoom.left + MulDiv(cursor.right, zoom.right-zoom.left, 20000);
	}
	Invalidate();
}

void CNet12NMRView::OnUpdateAbslevelbutton(CCmdUI *pCmdUI)
{
	EnableIfFTflag(pCmdUI, TRUE);
}

void CNet12NMRView::OnAbslevelbutton()
{
	CNet12NMRDoc *pDoc = GetDocument();
	pDoc->Level(zoom.left, zoom.right);
	UpdateIntegralList();
	UpdatePeakList();
	if (AutoYScale)
		FindMinMax();
	else
		FindIntegralMax();
	Invalidate();
}

void CNet12NMRView::OnUpdateAbsresetbutton(CCmdUI *pCmdUI)
{
	CNet12NMRDoc *pDoc = GetDocument();
	pCmdUI->Enable((pDoc->sp.ftflag && pDoc->baseregions.GetCount()) ? TRUE : FALSE);
}

void CNet12NMRView::OnAbsresetbutton()
{
	CNet12NMRDoc *pDoc = GetDocument();
	UINT size = pDoc->dt.size();
	for (UINT i=0; i<size; i++) {
		pDoc->bbaseline[i] = false;
		pDoc->fbaseline[i] = 0.0f;
	}
	while (pDoc->baseregions.GetCount())
		delete pDoc->baseregions.RemoveHead();
	pDoc->SetModifiedFlag();
	Invalidate();
}

void CNet12NMRView::OnUpdateAbsfindbutton(CCmdUI *pCmdUI)
{
	EnableIfFTflag(pCmdUI, TRUE);
}

void CNet12NMRView::OnAbsfindbutton()
{
	CNet12NMRDoc *pDoc = GetDocument();
	OnAbcwidthedit();	// update the ribbon values to theApp, see the routine below
	OnAbcsnedit();
	OnAbsresetbutton();	// reset {f,b}baseline vectors and clear pDoc->baseregions
	pDoc->FindBaseline(theApp.m_nABCWidth, theApp.m_fABCSN, zoom.left, zoom.right);
	pDoc->SaveState();
	pDoc->SetModifiedFlag();
	Invalidate();
}

void CNet12NMRView::OnAbcwidthedit()
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CMyRibbonBar *pRibbon = DYNAMIC_DOWNCAST(CMyRibbonBar, pMain->GetRibbonBar());
	CMFCRibbonEdit *pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, pRibbon->FindByID(ID_ABCWIDTHEDIT));
	theApp.m_nABCWidth = atoi(pEdit->GetEditText());
}

void CNet12NMRView::OnAbcsnedit()
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CMyRibbonBar *pRibbon = DYNAMIC_DOWNCAST(CMyRibbonBar, pMain->GetRibbonBar());
	CMFCRibbonEdit *pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, pRibbon->FindByID(ID_ABCSNEDIT));
	theApp.m_fABCSN = float(atof(pEdit->GetEditText()));
}

void CNet12NMRView::OnBaseline(bool bval)
{
	// add in new section
	CNet12NMRDoc *pDoc = GetDocument();
	int size = int(pDoc->dt.size());
	int from = zoom.left + MulDiv(cursor.left, zoom.right-zoom.left, 20000);
	if (from < 0)
		from = 0;
	else if (from >=size)
		from = size;
	int to = zoom.left + MulDiv(cursor.right, zoom.right-zoom.left, 20000);
	if (to < 0)
		to = 0;
	else if (to > size)
		to = size;
	if (from > to) {
		int tmp = from;
		from = to;
		to = tmp;
	}
	while ( from < to )
		pDoc->bbaseline[from++] = bval;
	pDoc->SaveState();
	// update user
	pDoc->SetModifiedFlag();
	Invalidate();
}

void CNet12NMRView::AddIntegral(UINT start, UINT stop, bool value)
{
	CString str;
	// add a first region
	CNet12NMRDoc *pDoc = GetDocument();
	if (0 == pDoc->intregions.GetCount()) {
		if (value) {
			IREGION *p = new IREGION;
			p->selected = false;
			p->start = start;
			p->stop = stop;
			pDoc->Integrate(p);
			pDoc->intregions.AddTail(p);
		}
		return;
	}

	IREGION *p = pDoc->intregions.GetTail();
	if (p->stop <= start) {
		// new region is after the last region in the list
		if (value) {
			IREGION *pNew = new IREGION;
			pNew->selected = false;
			pNew->start = start;
			pNew->stop = stop;
			pDoc->Integrate(pNew);
			pDoc->intregions.AddTail(pNew);
		}
		return;
	}

	p = pDoc->intregions.GetHead();
	if (stop <= p->start) {
		// new region is before the first region in the list
		if (value) {
			IREGION *pNew = new IREGION;
			pNew->selected = false;
			pNew->start = start;
			pNew->stop = stop;
			pDoc->Integrate(pNew);
			pDoc->intregions.AddHead(pNew);
		}
		return;
	}

	POSITION pos = pDoc->intregions.GetHeadPosition();
	pDoc->intregions.GetNext(pos); // we're now at the second position
	while (pos) {
		IREGION *pNext = pDoc->intregions.GetAt(pos);
		if (p->stop <= start && stop <= pNext->start) {
			// the new region is inbetween p and pNext (i.e., just before pos)
			if (value) {
				IREGION *pNew = new IREGION;
				pNew->selected = false;
				pNew->start = start;
				pNew->stop = stop;
				pDoc->Integrate(pNew);
				pDoc->intregions.InsertBefore(pos, pNew);
			}
			return;
		}
		// increment the two pointers
		p = pNext;
		pDoc->intregions.GetNext(pos);
	}
	// we got here, so there must be some sort of overlap with one or more peakregions
	// here's the hard part!
	// new region:					|-------|
	// current region_________________________________
	// case 1:					|---+-------+---|
	// case 2:						|-------+---|
	// case 3:						|	|---+---|
	// case 4:					|---+-------|
	// case 5:						|-------|
	// case 6:						|	|---|
	// case 7:					|---+---|   |
	// case 8:						|---|   |
	// case 9:						| |-|   |
	// Each of these 9 cases has two conditions depending on the incoming value (true or false).
	// starting from the head and working our way through all the regions means that cases 7, 8 and 9 are
	// extra tough because we have to see if the new region affects subsequent regions in the intregions
	// list.
	pos = pDoc->intregions.GetHeadPosition();
	while (pos) {
		IREGION *p = pDoc->intregions.GetAt(pos);
		if (start < p->stop && p->start < stop) {
			if (stop < p->stop) {
				// case 1, 2 or 3
				if (value) {
					if (start < p->start)
						p->start = start;
				} else {
					if (p->start < start) {
						IREGION *pNew = new IREGION;
						pNew->selected = false;
						pNew->start = p->start;
						pNew->stop = start;
						pDoc->intregions.InsertBefore(pos, pNew);
						pDoc->Integrate(pNew);
					}
					p->start = stop;
				}
				pDoc->Integrate(p);
				return;
			} else if (stop == p->stop) {
				// case 4, 5 or 6
				if (value) {
					if (start < p->start)
						p->start = start;
				} else {
					if (p->start < start)
						p->stop = start;
					else {
						delete p;
						pDoc->intregions.RemoveAt(pos);
						return;
					}
				}
				pDoc->Integrate(p);
				return;
			} else {
				// case 7, 8 or 9
				if (value) {
					if (start < p->start)
						p->start = start;
					p->stop = stop;
					pDoc->Integrate(p);
					pDoc->intregions.GetNext(pos);
				} else {
					if (p->start < start) {
						p->stop = start;
						pDoc->Integrate(p);
						pDoc->intregions.GetNext(pos);
					} else {
						POSITION deadpos = pos;
						delete pDoc->intregions.GetNext(pos);
						pDoc->intregions.RemoveAt(deadpos);
					}
				}
				// now take care of regions after here
				// first, delete any that are within the new region
				while (pos) {
					p = pDoc->intregions.GetAt(pos);
					if (p->stop <= stop) {
						POSITION deadpos = pos;
						delete pDoc->intregions.GetNext(pos);
						pDoc->intregions.RemoveAt(deadpos);
					} else
						break;
				}
				if (pos) {
					p = pDoc->intregions.GetAt(pos);
					if (p->start < stop) {
						if (value) {
							POSITION back = pos;
							pDoc->intregions.GetPrev(back);
							IREGION *pPrev = pDoc->intregions.GetAt(back);
							pPrev->stop = p->stop;
							delete pDoc->intregions.GetAt(pos);
							pDoc->intregions.RemoveAt(pos);
							pDoc->Integrate(pPrev);
						} else {
							p->start = stop;
							pDoc->Integrate(p);
						}
					}
				}
				return;
			}
		}
		if (pos)
			pDoc->intregions.GetNext(pos);
	}
}

void CNet12NMRView::OnIntegral(bool bval)
{
	CNet12NMRDoc *pDoc = GetDocument();
	UINT size = pDoc->dt.size();
	int from = zoom.left + MulDiv(cursor.left,  zoom.right-zoom.left, 20000);
	int to   = zoom.left + MulDiv(cursor.right, zoom.right-zoom.left, 20000);
	AddIntegral(from, to, bval);
	UpdateIntegralList();	// update the list
	FindIntegralMax();		// and find the integral max (in the Zoom region)
	pDoc->SetModifiedFlag();
	Invalidate();
}

void CNet12NMRView::EnableIfZoomAndFT(CCmdUI *pCmdUI)
{
	pCmdUI->Enable((cursor.left!=cursor.right && GetDocument()->sp.ftflag) ? TRUE : FALSE);
}

void CNet12NMRView::OnAddbaselinebutton()
{
	OnBaseline(false);
}

void CNet12NMRView::OnRemovebaselinebutton()
{
	OnBaseline(true);
}

void CNet12NMRView::OnPolycheck()
{
	theApp.polybaseline = true;
}

void CNet12NMRView::OnUpdatePolycheck(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(theApp.polybaseline ? TRUE : FALSE);
}

void CNet12NMRView::OnMovecheck()
{
	theApp.polybaseline = false;
}

void CNet12NMRView::OnUpdateMovecheck(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(theApp.polybaseline ? FALSE : TRUE);
}

void CNet12NMRView::OnUpdateCalcbasebutton(CCmdUI *pCmdUI)
{
	EnableIfFTflag(pCmdUI, TRUE);
}

void CNet12NMRView::OnCalcbasebutton()
{
	CWaitCursor wait;
	CNet12NMRDoc *pDoc = GetDocument();
	OnPolyordercombo();
	OnMvavedit();
	pDoc->CalcBaseline(zoom.left, zoom.right);
	Invalidate();
}

void CNet12NMRView::OnPolyordercombo()
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CMyRibbonBar *pRibbon = DYNAMIC_DOWNCAST(CMyRibbonBar, pMain->GetRibbonBar());
	CMFCRibbonComboBox *pCombo = DYNAMIC_DOWNCAST(CMFCRibbonComboBox, pRibbon->FindByID(ID_POLYORDERCOMBO));
	theApp.m_nPolyorder = pCombo->GetCurSel() + 2;
}

void CNet12NMRView::OnUpdatePolyordercombo(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(theApp.polybaseline ? TRUE : FALSE);
}

void CNet12NMRView::OnMvavedit()
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CMyRibbonBar *pRibbon = DYNAMIC_DOWNCAST(CMyRibbonBar, pMain->GetRibbonBar());
	CMFCRibbonEdit *pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, pRibbon->FindByID(ID_MVAVEDIT));
	theApp.m_nMAWidth = atoi(pEdit->GetEditText());
}

void CNet12NMRView::OnUpdateMvavedit(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(theApp.polybaseline ? FALSE : TRUE);
}

void CNet12NMRView::OnApplybasebutton()
{
	CNet12NMRDoc *pDoc = GetDocument();
	for (int i=zoom.left; i<zoom.right; i++) {
		pDoc->dt[i] -= complex<float>(pDoc->fbaseline[i], GetDocument()->dt[i].imag());
		pDoc->fbaseline[i] = 0.0f;
	}
	pDoc->sp.baseline = true;
	pDoc->sp.polybaseline = theApp.polybaseline;
	UpdateIntegralList();
	UpdatePeakList();
	if (AutoYScale)
		FindMinMax();
	else
		FindIntegralMax();
	pDoc->SetModifiedFlag();
	Invalidate();
}

void CNet12NMRView::OnUpdateApplybasebutton(CCmdUI *pCmdUI)
{
	EnableIfFTflag(pCmdUI, TRUE);
}

void CNet12NMRView::OnBasetoshelfbutton()
{
	theApp.shelf.BaseRegionToShelf(GetDocument());
}

void CNet12NMRView::OnUpdateBasetoshelfbutton(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDocument()->baseregions.GetCount() ? TRUE : FALSE);
}

void CNet12NMRView::OnShelftobasebutton()
{
	// copy down the new regions
	theApp.shelf.ShelfToBaseRegion(GetDocument());
	Invalidate();
}

void CNet12NMRView::OnUpdateShelftobasebutton(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(theApp.shelf.HasBaseline() ? TRUE : FALSE);
}

void CNet12NMRView::OnAddintegralbutton()
{
	OnIntegral(true);
}

void CNet12NMRView::OnRemoveintegralbutton()
{
	OnIntegral(false);
}

void CNet12NMRView::OnUpdateClearintegralbutton(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDocument()->intregions.GetCount() ? TRUE : FALSE);
}

void CNet12NMRView::OnClearintegralbutton()
{
	CNet12NMRDoc *pDoc = GetDocument();
	while (pDoc->intregions.GetCount())
		delete pDoc->intregions.RemoveHead();
	pDoc->LoadState();
	pDoc->RecalcIntegrals();
	integralmax = integralmin = 0.0f;
	pDoc->SetModifiedFlag();
	Invalidate();
}

void CNet12NMRView::OnUpdateInttoshelfbutton(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDocument()->intregions.GetCount() ? TRUE : FALSE);
}

void CNet12NMRView::OnInttoshelfbutton()
{
	theApp.shelf.IntegralRegionToShelf(GetDocument());
}

void CNet12NMRView::OnUpdateShelftointbutton(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(theApp.shelf.HasIntegrals() ? TRUE : FALSE);
}

void CNet12NMRView::OnShelftointbutton()
{
	// copy down the new regions
	CNet12NMRDoc *pDoc = GetDocument();
	theApp.shelf.ShelfToIntegralRegion(pDoc);
	UpdateIntegralList();
	FindIntegralMax();
	Invalidate();
}


void CNet12NMRView::OnUpdateBreakintegralbutton(CCmdUI *pCmdUI)
{
	CNet12NMRDoc *pDoc = GetDocument();
	BOOL inRegion = FALSE;
	if (pDoc->sp.ftflag && pDoc->intregions.GetCount()) {
		if (CursorPosition.x>=0 && CursorPosition.x<20000) {
			int x = zoom.left + MulDiv(CursorPosition.x, zoom.right-zoom.left, 20000);
			if (x < 0)
				x=0;
			else if (x >= int(pDoc->dt.size()))
				x = (int)pDoc->dt.size() - 1;
			POSITION pos = pDoc->intregions.GetHeadPosition();
			while (pos) {
				IREGION *p = pDoc->intregions.GetNext(pos);
				if (int(p->start)<=x && x<int(p->stop)) {
					inRegion = TRUE;
					break;
				}
			}
		}
	}
	pCmdUI->Enable(inRegion);
}


void CNet12NMRView::OnBreakintegralbutton()
{
	CNet12NMRDoc *pDoc = GetDocument();
	int x = zoom.left + MulDiv( CursorPosition.x, zoom.right-zoom.left, 20000 );
	if (x < 0)
		x = 0;
	else if (x >= (int)pDoc->dt.size())
		x = (int)pDoc->dt.size() - 1;
	AddIntegral(x, x, false);
	UpdateIntegralList();	// update the list
	FindIntegralMax();		// and find the integral max (in the zoom region)
	GetDocument()->SetModifiedFlag();
	Invalidate();
}

void CNet12NMRView::OnSetintegralbutton()
{
	CNet12NMRDoc *pDoc = GetDocument();
	UINT size = pDoc->dt.size();
	int x = zoom.left + MulDiv(CursorPosition.x, zoom.right-zoom.left, 20000);
	if (x < 0)
		x = 0;
	else if (x >= (int)size)
		x = (int)size - 1;
	bool notfound = true;
	IREGION *p;
	POSITION pos = pDoc->intregions.GetHeadPosition();
	while (pos) {
		p = pDoc->intregions.GetNext(pos);
		if (int(p->start) <= x && x < int(p->stop)) {
			notfound = false;
			break;
		}
	}
	if (notfound) {
		AfxMessageBox("Integral Region not found, command aborted!", MB_OK|MB_ICONSTOP );
		return;
	} else
		SetIntegral(p);
}

void CNet12NMRView::SetIntegral(IREGION *p)
{
	CSetIntegralDlg dlg;
	CNet12NMRDoc *pDoc = GetDocument();
	float IntegralConst = pDoc->sp.bintegralconst ? pDoc->sp.integralconst : theApp.GlobalIntegralConst;
	float total = 0.0f;
	if (p) {
		double from = pDoc->ntoppm(p->start);
		double to = pDoc->ntoppm(p->stop);
		switch (axisUnits) {
			case hertzUnits:
				dlg.m_strIntegralPrompt.Format("Integral from %.1f Hz to %.1f Hz", from*pDoc->sp.sf, to*pDoc->sp.sf);
				break;
			case ppmUnits:
				dlg.m_strIntegralPrompt.Format("Integral from %.2f PPM to %.2f PPM", from, to);
				break;
			default:
				dlg.m_strIntegralPrompt.Format("Inregral from Point %i to %i", p->start, p->stop-1);
				break;
		}
		total = p->total;
	} else {
		int count = 0;
		POSITION pos = pDoc->intregions.GetHeadPosition();
		while (pos) {
			IREGION *pInt = pDoc->intregions.GetNext(pos);
			if (pInt->selected) {
				p = pInt;
				count++;
				total += pInt->total;
			}
		}
		dlg.m_strIntegralPrompt.Format("%d Integral total", count);
	}
	dlg.m_strIntegralValue.Format("%.2f", total * IntegralConst);
	if (IDOK == dlg.DoModal()) {
		float newint = float(abs(atof(dlg.m_strIntegralValue)));
		if (newint) {
			if (p) {
				pDoc->setintegralvalue = abs(atof(dlg.m_strIntegralValue));
				pDoc->setintegralindex = p->stop;
			}
			CString istr;
			IntegralConst = newint / total;
			istr.Format("%.12e", IntegralConst);
			if (pDoc->sp.bintegralconst) {
				pDoc->SetModifiedFlag();
				pDoc->sp.integralconst = IntegralConst;
			} else
				theApp.GlobalIntegralConst = IntegralConst;
			Invalidate();
			UpdateIntegralList(false);
		}
	}
}

void CNet12NMRView::UpdateIntegralList(bool recalc)
{
	CNet12NMRDoc *pDoc = GetDocument();
	if (recalc)
		pDoc->RecalcIntegrals();
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	pMain->m_wndOutput.ClearIntegrals();
	if (pDoc->intregions.GetCount()) {
		CString str;
		str.Format("%10.10s\t%10.10s\t%10.10s", "From(PPM)", "To(PPM)", "Integral");
		pMain->m_wndOutput.AddIntegral(str, NULL);
		float IntegralConst = pDoc->sp.bintegralconst ? pDoc->sp.integralconst : theApp.GlobalIntegralConst;
		POSITION pos = pDoc->intregions.GetHeadPosition();
		while (pos) {
			IREGION *p = pDoc->intregions.GetNext(pos);
			double value = IntegralConst * p->total;
			str.Format("%10.2f\t%10.2f\t%10.2f", pDoc->ntoppm(p->start), pDoc->ntoppm(p->stop), value);
			pMain->m_wndOutput.AddIntegral(str, p);
		}
	}
}

void CNet12NMRView::UpdatePeakList(bool recalc)
{
	CNet12NMRDoc *pDoc = GetDocument();
	if (recalc)
		pDoc->RecalcPeaks();
	// clear output window
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	pMain->m_wndOutput.ClearPeaks();
	// fill peak output window
	if (pDoc->peaks.GetCount()) {
		CString str1, str2, fmt;
		str1.LoadString(theApp.labelisppm ? IDC_DIGITSPPM : IDC_DIGITSHZ);
		int sigfig = theApp.GetProfileInt(theApp.settings, str1, 2);
		str1.Format("%s(%s)", theApp.labelisppm ? "PPM" : "Hertz", theApp.useparabolicfit ? "fit" : "raw");
		str2.Format("Height(%s)", theApp.useparabolicfit ? "fit" : "raw");
		fmt.Format("%10.10s\t%16.16s", str1, str2);
		pMain->m_wndOutput.AddPeak(fmt, NULL);
		POSITION pos = pDoc->peaks.GetHeadPosition();
		while (pos) {
			PEAK *peak = pDoc->peaks.GetNext(pos);
			double h = theApp.useparabolicfit ? peak->heightfit : peak->height;
			double x = theApp.useparabolicfit ? pDoc->ntoppm(peak->nfit) : pDoc->ntoppm(peak->n);
			if (! theApp.labelisppm)
				x *= pDoc->sp.sf;
			str1.Format("%10.*f\t%16.1f", sigfig, x, h);
			pMain->m_wndOutput.AddPeak(str1, peak);
		}
	}
}
void CNet12NMRView::AddThreshold(UINT start, UINT stop, float threshold)
{
	// add a first region
	CNet12NMRDoc *pDoc = GetDocument();
	if (0 == pDoc->peakregions.GetCount()) {
		if (threshold) {
			PREGION *p = new PREGION;
			p->start = start;
			p->stop = stop;
			p->threshold = threshold;
			pDoc->peakregions.AddTail(p);
		}
		return;
	}

	PREGION *p = pDoc->peakregions.GetTail();
	if (p->stop <= start) {
		// the new region is after the last peakregion
		if (threshold >= 0.0f) {
			PREGION *pNew = new PREGION;
			pNew->start = start;
			pNew->stop = stop;
			pNew->threshold = threshold;
			pDoc->peakregions.AddTail(pNew);
		}
		return;
	}

	p = pDoc->peakregions.GetHead();
	if (stop <= p->start) {
		// new region is before the first peakregion
		if (threshold > 0.0f) {
			PREGION *pNew = new PREGION;
			pNew->start = start;
			pNew->stop = stop;
			pNew->threshold = threshold;
			pDoc->peakregions.AddHead(pNew);
		}
		return;
	}

	POSITION pos = pDoc->peakregions.GetHeadPosition();
	pDoc->peakregions.GetNext(pos);
	while (pos) {
		PREGION *pNext = pDoc->peakregions.GetAt(pos);
		if (p->stop <= start && stop <= pNext->start) {
			// the new region is inbetween p and pNext (i.e., before pos)
			if (threshold > 0.0f) {
				PREGION *pNew = new PREGION;
				pNew->start = start;
				pNew->stop = stop;
				pNew->threshold = threshold;
				pDoc->peakregions.InsertBefore(pos, pNew);
			}
			return;
		}
		// increment pointers
		p = pNext;
		pDoc->peakregions.GetNext(pos);
	}

	// we got here, so there must be some sort of overlap
	// here's the hard part!
	// new region:					|-------|
	// current region_________________________________
	// case 1:					|---+-------+---|
	// case 2:						|-------+---|
	// case 3:						|	|---+---|
	// case 4:					|---+-------|
	// case 5:						|-------|
	// case 6:						|	|---|
	// case 7:					|---+---|   |
	// case 8:						|---|   |
	// case 9:						| |-|   |
	// These 9 cases are further complicated for peak thresholds by the value of the new threhold:
	// zero, equal to a intersecting region or some new value. Each of these threshold values must be
	// addressed.
	// starting from the head and working our way through all the regions means that cases 7, 8 and 9 are
	// extra tough because we have to see if the new region affects subsequent regions in the peakregions
	// list.
	pos = pDoc->peakregions.GetHeadPosition();
	while (pos) {
		PREGION *p = pDoc->peakregions.GetAt(pos);
		if (start < p->stop && p->start < stop) {
			if (stop < p->stop) {
				// case 1, 2 or 3
				if (p->threshold == threshold) {
					if (start < p->start)
						p->start = start;
					return;
				}
				if (p->start < start) {
					// case 1, with a new threshold
					PREGION *pNew = new PREGION;
					pNew->start = stop;
					pNew->stop = p->stop;
					pNew->threshold = p->threshold;
					pDoc->peakregions.InsertAfter(pos, pNew);
					p->stop = start;
					if (threshold) {
						p = new PREGION;
						p->start = start;
						p->stop = stop;
						p->threshold = threshold;
						pDoc->peakregions.InsertAfter(pos, p);
					}
				} else {
					// case 2 or 3, with a new threshold
					p->start = stop;
					if (threshold) {
						p = new PREGION;
						p->start = start;
						p->stop = stop;
						p->threshold = threshold;
						pDoc->peakregions.InsertBefore(pos, p);
					}
				}
				return;
			} else if (stop == p->stop) {
				// case 4, 5 or 6
				if (p->threshold == threshold) {
					if (start < p->start)
						p->start = start;
					return;
				}
				if (p->start < start) {
					// case 4
					p->stop = start;
					if (threshold) {
						p = new PREGION;
						p->start = start;
						p->stop = stop;
						p->threshold = threshold;
						pDoc->peakregions.InsertAfter(pos, p);
					}
					return;
				} else {
					// case 5 and 6
					if (threshold) {
						p->threshold = threshold;
						if (start < p->start)
							p->start = start;	// case 6
					} else {
						delete p;
						pDoc->peakregions.RemoveAt(pos);
					}
					return;
				}
			} else {
				// case 7, 8 or 9
				if (p->threshold == threshold) {
					p->stop = stop;
					if (start < p->start)
						p->start = start;	// case 9
					pDoc->peakregions.GetNext(pos);
				} else if (threshold) {
					if (p->start < start) {
						PREGION *pNew = new PREGION;
						pNew->start = p->start;
						pNew->stop = start;
						pNew->threshold = p->threshold;
						pDoc->peakregions.InsertBefore(pos, pNew);
					}
					p->start = start;
					p->stop = stop;
					p->threshold = threshold;
					pDoc->peakregions.GetNext(pos);
				} else { // 0 == threshold
					if (p->start < start) {
						p->stop = start;	// case 7
						pDoc->peakregions.GetNext(pos);
					} else {
						POSITION deadpos = pos;
						delete pDoc->peakregions.GetNext(pos);
						pDoc->peakregions.RemoveAt(deadpos);
					}
				}
				// now take care of regions after here
				// first, delete any that are within the new region
				while (pos) {
					p = pDoc->peakregions.GetAt(pos);
					if (p->stop <= stop) {
						POSITION deadpos = pos;
						delete pDoc->peakregions.GetNext(pos);
						pDoc->peakregions.RemoveAt(deadpos);
					} else
						break;
				}
				if (pos) {
					p = pDoc->peakregions.GetAt(pos);
					if (p->start < stop) {
						if (p->threshold == threshold) {
							POSITION back = pos;
							pDoc->peakregions.GetPrev(back);
							PREGION *pPrev = pDoc->peakregions.GetAt(back);
							ASSERT(pPrev->threshold == threshold);
							pPrev->stop = p->stop;
							delete pDoc->peakregions.GetAt(pos);
							pDoc->peakregions.RemoveAt(pos);
						} else
							p->start = stop;
					}
				}
				return;
			}
		}
		if (pos)
			pDoc->peakregions.GetNext(pos);
	}
}

void CNet12NMRView::OnThreshold(float y)
{
	CNet12NMRDoc *pDoc = GetDocument();
	UINT size = pDoc->dt.size();
	// add in new section
	int from = zoom.left + MulDiv(cursor.left, zoom.right-zoom.left, 20000);
	if (from < 2 && y)
		from = 2;
	int to = zoom.left + MulDiv(cursor.right, zoom.right-zoom.left, 20000);
	if (to > int(size)-2 && y)
		to = size - 2;
	AddThreshold(from, to, y);
	UpdatePeakList();
	Invalidate();
	pDoc->SetModifiedFlag();
}

void CNet12NMRView::OnUpdateClearpeakregionbutton(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDocument()->peakregions.GetCount() ? TRUE : FALSE);
}

void CNet12NMRView::OnClearpeakregionbutton()
{
	CNet12NMRDoc *pDoc = GetDocument();
	UINT size = pDoc->dt.size();
	while (pDoc->peaks.GetCount())
		delete pDoc->peaks.RemoveHead();
	while (pDoc->peakregions.GetCount()) {
		PREGION *p = pDoc->peakregions.RemoveHead();
		delete p;
	}
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	pMain->m_wndOutput.ClearPeaks();
	Invalidate();
	pDoc->SetModifiedFlag();
}

void CNet12NMRView::OnUpdateSpreadsheetbutton(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDocument()->peaks.GetCount()>1?TRUE:FALSE);
}

void CNet12NMRView::OnSpreadsheetbutton()
{
	CNet12NMRDoc *pDoc = GetDocument();
	CStringList strList;
	int size = 0;
	POSITION pos = pDoc->peaks.GetHeadPosition();
	while (pos) {
		PEAK *peak = pDoc->peaks.GetNext(pos);
		if (zoom.left<=int(peak->n) && int(peak->n)<=zoom.right) {
			CString str;
			double h = theApp.useparabolicfit ? peak->heightfit : peak->height;
			double x = theApp.useparabolicfit ? pDoc->ntoppm(peak->nfit) : pDoc->ntoppm(peak->n);
			str.Format("%g\t%g\r\n", x, h);
			strList.AddTail(str);
			size += str.GetLength();
		}
	}
	if (size) {
		CString str;
		str.Format("%d\t%.7f\r\n", strList.GetCount(), GetDocument()->sp.sf);
		strList.AddHead(str);
		size += str.GetLength() + 1;
		if (OpenClipboard()) {
			if (EmptyClipboard()) {
				GLOBALHANDLE hgText = GlobalAlloc(GHND, size);
				if (hgText) {
					char *pgmem = (char *)GlobalLock(hgText);
					if (pgmem) {
						while (strList.GetCount()) {
							str = strList.RemoveHead();
							strcat_s(pgmem, size, str);
						}
						GlobalUnlock(hgText);
						SetClipboardData(CF_TEXT, hgText);
						str = "start excel \"";
						str += theApp.m_pszHelpFilePath;
						size = str.ReverseFind('\\') + 1;
						str = str.Mid(0, size) + "PeakAnalysis.xlsm\"";
						system(str);
					}
				}
			}
			CloseClipboard();
		}
		strList.RemoveAll();
	}
}

void CNet12NMRView::OnUpdateParapeakcheck(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(theApp.useparabolicfit?1:0);
}

void CNet12NMRView::OnParapeakcheck()
{
	theApp.useparabolicfit = !theApp.useparabolicfit;
	CString entry;
	entry.LoadString(IDC_PARFITCHECK);
	theApp.WriteProfileInt(theApp.settings, entry, theApp.useparabolicfit?1:0);
	if (GetDocument()->peaks.GetCount()) {
		UpdatePeakList(false);
		Invalidate();
	}
}

void CNet12NMRView::OnUpdatePpmpeakcheck(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(theApp.labelisppm?1:0);
}

void CNet12NMRView::OnPpmpeakcheck()
{
	theApp.labelisppm = ! theApp.labelisppm;
	CString entry;
	entry.LoadString(IDC_PEAKPPM);
	theApp.WriteProfileInt(theApp.settings, entry, theApp.labelisppm?1:0);
	if (GetDocument()->peaks.GetCount()) {
		UpdatePeakList(false);
		Invalidate();
	}
}

void CNet12NMRView::OnUpdateHzpeakcheck(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(theApp.labelisppm?0:1);
}

void CNet12NMRView::OnPpmpeakcombo()
{
	CMFCRibbonBar *pRibbon = ((CMDIFrameWndEx *)AfxGetMainWnd())->GetRibbonBar();
	ASSERT_VALID(pRibbon);
	CMFCRibbonComboBox *pCombo = DYNAMIC_DOWNCAST(CMFCRibbonComboBox, pRibbon->FindByID(ID_PPMPEAKCOMBO));
	if (pCombo) {
		CString entry;
		entry.LoadString(IDC_DIGITSPPM);
		theApp.digitsPPM = pCombo->GetCurSel();
		theApp.WriteProfileInt(theApp.settings, entry, theApp.digitsPPM);
	}
	if (GetDocument()->peaks.GetCount() && theApp.labelisppm) {
		UpdatePeakList(false);
		Invalidate();
	}
}

void CNet12NMRView::OnHzpeakcombo()
{
	CMFCRibbonBar *pRibbon = ((CMDIFrameWndEx *)AfxGetMainWnd())->GetRibbonBar();
	ASSERT_VALID(pRibbon);
	CMFCRibbonComboBox *pCombo = DYNAMIC_DOWNCAST(CMFCRibbonComboBox, pRibbon->FindByID(ID_HZPEAKCOMBO));
	if (pCombo) {
		CString entry;
		entry.LoadString(IDC_DIGITSHZ);
		theApp.digitsHz = pCombo->GetCurSel();
		theApp.WriteProfileInt(theApp.settings, entry, theApp.digitsHz);
	}
	if (GetDocument()->peaks.GetCount() && ! theApp.labelisppm) {
		UpdatePeakList(false);
		Invalidate();
	}
}

void CNet12NMRView::OnPeaksetthreshold()
{
	float h = dmin + ((dmax - dmin) * float(CursorY) / 16000.0f);
	OnThreshold(h);
}

void CNet12NMRView::OnPeakremovethreshold()
{
	OnThreshold(0.0f);
}

void CNet12NMRView::OnUpdatePeaktoshelfbutton(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDocument()->peaks.GetCount() ? TRUE : FALSE);
}

void CNet12NMRView::OnPeaktoshelfbutton()
{
	theApp.shelf.PeakRegionToShelf(GetDocument());
}

void CNet12NMRView::OnUpdateShelftopeakbutton(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(theApp.shelf.HasPeaks() ? TRUE : FALSE);
}

void CNet12NMRView::OnShelftopeakbutton()
{
	// copy down the new regions
	CNet12NMRDoc *pDoc = GetDocument();
	while (pDoc->peakregions.GetCount())
		delete pDoc->peakregions.RemoveHead();
	theApp.shelf.ShelfToPeakRegion(pDoc);
	pDoc->SaveState();
	UpdatePeakList();	// regenerate peak list
	pDoc->SetModifiedFlag();
	Invalidate();
}

void CNet12NMRView::OnUpdateHertzbutton(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDocument()->sp.ftflag ? 1 : 0);
}

void CNet12NMRView::OnHertzbutton()
{
	axisUnits = hertzUnits;
	SetAxisRange();
	Invalidate();
}

void CNet12NMRView::OnPpmbutton()
{
	axisUnits = ppmUnits;
	SetAxisRange();
	Invalidate();
}

void CNet12NMRView::OnUpdateTimebutton(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDocument()->sp.ftflag ? 0 : 1);
}

void CNet12NMRView::OnTimebutton()
{
	axisUnits = timeUnits;
	SetAxisRange();
	Invalidate();
}

void CNet12NMRView::OnPointsbutton()
{
	axisUnits = pointsUnits;
	SetAxisRange();
	Invalidate();
}

void CNet12NMRView::OnNoaxisbutton()
{
	axisUnits = noUnits;
	Invalidate();
}

void CNet12NMRView::OnUpdateReloadbutton(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(0 == GetDocument()->sp.groupnum ? TRUE : FALSE);
}

void CNet12NMRView::OnReloadbutton()
{
	// clear everything out
	CNet12NMRDoc *pDoc = GetDocument();
	pDoc->DeleteAllNewData();
	bool success = false;
	if (shared==pDoc->leaf.s.location && net12nmrfile==pDoc->leaf.s.leaftype) {
		CFile file;
		CString path = pDoc->fullpathname;
		if (file.Open(pDoc->fullpathname, CFile::modeRead)) {
			CArchive ar(&file, CArchive::load);
			pDoc->Serialize(ar);
			pDoc->fullpathname = path;
			ar.Close();
			file.Close();
			pDoc->SetModifiedFlag(FALSE);
			success = true;
		}
	} else {
		CTypedPtrList<CPtrList, TMPSPECTRUM *> spectrum;
		CString path = pDoc->fullpathname;
		if (served == pDoc->leaf.s.location) {
			int i = path.Find(':')  + 1;
			if (i>0)
				path = pDoc->fullpathname.Right(pDoc->fullpathname.GetLength() - i);
		}
		if (theApp.GetTmpSpectra(path, pDoc->leaf, spectrum)) {
			if (1 == spectrum.GetCount()) {
				TMPSPECTRUM *s = spectrum.RemoveHead();
				theApp.MoveTmpToDoc(s, pDoc);
				delete s;
				success = true;
			}
		}
		while (spectrum.GetCount()) {
			TMPSPECTRUM *s = spectrum.RemoveHead();
			POSITION pos = s->fh.GetStartPosition();
			while (pos) {
				CString type;
				s->fh.GetNextAssoc(pos, type, s->fhmap);
			}
			s->fh.RemoveAll();
			while (s->bregion.GetCount())
				delete s->bregion.RemoveHead();
			while (s->pregion.GetCount())
				delete s->pregion.RemoveHead();
			while (s->iregion.GetCount())
				delete s->iregion.RemoveHead();
			s->dt.resize(0);
			delete s;
		}
	}
	if (success) {
		zoom.left = 0;
		zoom.right = pDoc->dt.size();
		UpdateIntegralList();
		UpdatePeakList();
		FindIntegralMax();
		pDoc->SetModifiedFlag(FALSE);
		if (false==pDoc->sp.ftflag)
			axisUnits = timeUnits;
		SetAxisRange();
		if (AutoYScale)
			FindMinMax();
		Invalidate();
		CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
		theApp.shelf.RibbonUpdate(pMain->m_wndRibbonBar);
	} else {
		AfxMessageBox("Trouble reloading file!", MB_ICONSTOP|MB_OK);
		pDoc->OnCloseDocument();
	}
}

void CNet12NMRView::OnUpdateZerodatabutton(CCmdUI *pCmdUI)
{
	pCmdUI->Enable((cursor.left>=0 && cursor.right>cursor.left) ? TRUE : FALSE);
}

void CNet12NMRView::OnZerodatabutton()
{
	CNet12NMRDoc *pDoc = GetDocument();
	int size = int(pDoc->dt.size());
	int from = zoom.left + MulDiv(cursor.left, zoom.right-zoom.left, 20000);
	if (from < 0)
		from = 0;
	else if (from >=size)
		from = size;
	int to = zoom.left + MulDiv(cursor.right, zoom.right-zoom.left, 20000);
	if (to < 0)
		to = 0;
	else if (to > size)
		to = size;
	if (from > to) {
		int tmp = from;
		from = to;
		to = tmp;
	}
	while ( from < to )
		pDoc->dt[from++] = complex<float>(0.0f,0.0f);
	// update user
	UpdatePeakList();
	UpdateIntegralList();
	if (AutoYScale)
		FindMinMax();
	else
		FindIntegralMax();
	Invalidate();
	pDoc->SetModifiedFlag();
}

void CNet12NMRView::OnEditCopy()
{
	CMetaFileDC mfDC;
	mfDC.CreateEnhanced(NULL, NULL, NULL, "NetNMR\0NetNMR Application\0\0");
	mfDC.SetMapMode(MM_HIMETRIC);
	CNet12NMRDoc *pDoc = GetDocument();
	OnDrawCore(pDoc, &mfDC, 1000);
	HENHMETAFILE hMF = mfDC.CloseEnhanced();
	if (hMF) {
		if (OpenClipboard()) {
			if (EmptyClipboard()) {
				SetClipboardData(CF_ENHMETAFILE, hMF);
			}
			// now put the x, y values on the clipboard in text mode
			CMemFile memfile;
			for (int i = zoom.left; i<zoom.right; i++) {
				float y = pDoc->dt[i].real();
				CString buf;
				if (axisUnits!=hertzUnits && axisUnits!=ppmUnits) {
					buf.Format("%d\t%g\r\n", i, y);
				} else {
					double x = PtoH(i);
					if (axisUnits==ppmUnits && pDoc->sp.f1freq)
						x /= pDoc->sp.f1freq;
					buf.Format("%g\t%g\r\n", x, y);
				}
				memfile.Write(buf, buf.GetLength());
			}
			char anull = 0;
			memfile.Write(&anull, 1);
			memfile.SeekToBegin();
			int len = (int)memfile.GetLength() + 1;
			GLOBALHANDLE hGText = GlobalAlloc(GHND, len);
			if (hGText) {
				LPVOID pGText = GlobalLock(hGText);
				if (pGText) {
					memfile.Read(pGText, len);
					GlobalUnlock(hGText);
					SetClipboardData(CF_TEXT, hGText);
				}
			}
		}
		CloseClipboard();
	}
}

LPCTSTR CNet12NMRView::GetCursorDifference()
{
	double diff = fabs(axisStart - axisStop);
	if (axisUnits == noUnits)
		return "";
	static CString str, wstr;
	if (cursor.left >= 0) {
		if (cursor.right > 0)
			diff *= double(1 + cursor.right - cursor.left) / 20000.0;
		else
			diff *= double(1 + abs(CursorPosition.x - cursor.left)) / 20000.0;
		wstr = "Diff";
	} else
		wstr = "Width";
	switch (axisUnits) {
		case hertzUnits:
			str.Format("%s = %.3fPPM (%.2fHz)", wstr, diff / GetDocument()->sp.sf, diff);
			break;
		case ppmUnits:
			str.Format("%s = %.3fPPM (%.2fHz)", wstr, diff, diff * GetDocument()->sp.sf);
			break;
		default:
			str.Format("%s = %.2f", wstr, diff);
			break;
	}
	return str;
}

void CNet12NMRView::OnUpdateParfitcheck(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(theApp.useparabolicfit ? 1 : 0);
}

void CNet12NMRView::OnParfitcheck()
{
	theApp.useparabolicfit = !theApp.useparabolicfit;
	UpdatePeakList(false);
	Invalidate();
}

void CNet12NMRView::OnUpdatePlaybutton(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDocument()->sp.ftflag ? 0 : 1);
}

void CNet12NMRView::OnPlaybutton()
{
	int i;
	CNet12NMRDoc *pDoc = GetDocument();
	int size = int(pDoc->dt.size());
	struct wav_tag {
		char ChunkID[4];
		int ChunkSize;
		char Format[4];
		char Subchunk1ID[4];
		int Subchunk1Size;
		short int AudioFormat;
		short int NumChannels;
		int SampleRate;
		int ByteRate;
		short int BlockAlign;
		short int BitsPerSample;
		char SubChunk2ID[4];
		int Subchunk2Size;
	} sWavHeader;
	memcpy(sWavHeader.ChunkID, "RIFF", 4);
	sWavHeader.ChunkSize = sizeof(struct wav_tag) + size * 4;
	memcpy(sWavHeader.Format, "WAVE", 4);
	memcpy(sWavHeader.Subchunk1ID, "fmt ", 4);
	sWavHeader.Subchunk1Size = 16;
	sWavHeader.AudioFormat = 1;
	sWavHeader.NumChannels = 2;
	if (0.0 == pDoc->sp.dw)
		return;
	sWavHeader.SampleRate = int(1.0 / pDoc->sp.dw);
	sWavHeader.ByteRate = 4 * sWavHeader.SampleRate;
	sWavHeader.BlockAlign = 4;
	sWavHeader.BitsPerSample = 16;
	memcpy(sWavHeader.SubChunk2ID, "data", 4);
	sWavHeader.Subchunk2Size = 4 * size;
	float ymax = 0.0f;
	for (i=0; i<size; i++) {
		float y = fabs(pDoc->dt[i].real());
		if (y > ymax) ymax = y;
		y = fabs(pDoc->dt[i].imag());
		if (y > ymax) ymax = y;
	}
	if (ymax <= 0.0f)
		return;
	CFile wavfile;
	char fname[MAX_PATH];
	int l = GetTempPath(MAX_PATH, fname);
	if (l<=0 || l>MAX_PATH-8)
		return;
	strcat_s(fname, "NMR.wav");
	if (wavfile.Open(fname, CFile::modeCreate | CFile::modeWrite)) {
		wavfile.Write(&sWavHeader, sizeof(wav_tag));
		ymax = 32000.0f / ymax;
		for (i=0; i<size; i++) {
			short int d[2];
			d[0] = (short int)(ymax * pDoc->dt[i].real());
			d[1] = (short int)(ymax * pDoc->dt[i].imag());
			wavfile.Write(d, 2*sizeof(short int));
		}
		wavfile.Close();
		PlaySound(fname, NULL, SND_FILENAME);
	}
}

void CNet12NMRView::OnEditZoomto()
{
	CZoomToDlg dlg;
	if ( IDOK == dlg.DoModal() ) {
		if ( dlg.m_ValueA.GetLength() && dlg.m_ValueB.GetLength() ) {
			CNet12NMRDoc *pDoc = GetDocument();
			double a = atof( dlg.m_ValueA );
			double b = atof( dlg.m_ValueB );
			int cb = int( pDoc->dt.size() );
			if ( ! dlg.m_bPTS ) {
				if ( dlg.m_bTime ) {
					double dw = pDoc->sp.dw;
					a /= dw;
					b /= dw;
				} else {
					if ( dlg.m_bPPM ) {
						double sf = pDoc->sp.sf;
						a *= sf;
						b *= sf;
					}
					double sw = pDoc->sp.sw;
					double of = pDoc->sp.of;
					a = cb * ( of + sw / 2.0 - a ) / sw;
					b = cb * ( of + sw / 2.0 - b ) / sw;
				}
			}
			int i = int( a + 0.5 );
			int j = int( b + 0.5 );
			if ( i < 0 )
				i = 0;
			else if ( i > cb )
				i = cb;
			if ( j < 0 )
				j = 0;
			else if ( j > cb )
				j = cb;
			if ( i > j ) {
				int tmp = i;
				i = j;
				j = tmp;
			}
			if ( j > i+2 ) {
				cursor.left = cursor.right = -1;
				zoom.left = i;
				zoom.right = j;
				if ( AutoYScale )
					FindMinMax();
				else
					FindIntegralMax();
				SetAxisRange();
				Invalidate();
			} else
				MessageBox("Zoom region must be contain at least two points!", NULL, MB_ICONERROR);
		}
	}
}

void CNet12NMRView::OnGlobalintegralcheck()
{
	CNet12NMRDoc *pDoc = GetDocument();
	pDoc->sp.bintegralconst = !pDoc->sp.bintegralconst;
	pDoc->SetModifiedFlag();
	Invalidate();
	UpdateIntegralList(false);
}

void CNet12NMRView::OnUpdateGlobalintegralcheck(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(GetDocument()->sp.bintegralconst ? FALSE : TRUE);
}

void CNet12NMRView::OnUpdateLocaltoglobalbutton(CCmdUI *pCmdUI)
{
	BOOL bOn = GetDocument()->sp.bintegralconst ? TRUE : FALSE;
	if (GetDocument()->sp.integralconst == theApp.GlobalIntegralConst)
		bOn = FALSE;
	pCmdUI->Enable(bOn);
}

void CNet12NMRView::OnLocaltoglobalbutton()
{
	CNet12NMRDoc *pDoc = GetDocument();
	theApp.GlobalIntegralConst = pDoc->sp.integralconst;
	pDoc->sp.bintegralconst = false;
}

void CNet12NMRView::OnUpdateGlobaltolocalbutton(CCmdUI *pCmdUI)
{
	BOOL bOn = GetDocument()->sp.bintegralconst ? FALSE : TRUE;
	if (GetDocument()->sp.integralconst == theApp.GlobalIntegralConst)
		bOn = FALSE;
	pCmdUI->Enable(bOn);
}

void CNet12NMRView::OnGlobaltolocalbutton()
{
	CNet12NMRDoc *pDoc = GetDocument();
	pDoc->sp.integralconst = theApp.GlobalIntegralConst;
	pDoc->sp.bintegralconst = true;
	pDoc->SetModifiedFlag();
}

void CNet12NMRView::OnUpdateViewPeaklabel(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(ViewPeakLabels ? TRUE : FALSE);
}

void CNet12NMRView::OnViewPeaklabel()
{
	ViewPeakLabels = !ViewPeakLabels;
	if (peakpickMode != theApp.disMode)
		Invalidate();
}

void CNet12NMRView::OnUpdateViewIntegrals(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(ViewIntegral ? TRUE : FALSE);
}

void CNet12NMRView::OnViewIntegrals()
{
	ViewIntegral = !ViewIntegral;
	if (integralMode != theApp.disMode) {
		FindIntegralMax();
		Invalidate();
	}
}

void CNet12NMRView::FindIntegralMax()
{
	integralmax = integralmin = 0.0f;
	CNet12NMRDoc *pDoc = GetDocument();
	POSITION pos = pDoc->intregions.GetHeadPosition();
	while (pos) {
		IREGION *p = pDoc->intregions.GetNext(pos);
		// if the integral region is not outside the zoomed spectrum
		if (! (zoom.left>int(p->stop) || zoom.right<int(p->start))) {
			UINT size = int(p->stop - p->start);
			for (UINT i=0; i<size; i++) {
				int x = int(p->start + i);
				if (zoom.left<=x && x<zoom.right) {
					float y = p->integral[i];
					if (y > integralmax)
						integralmax = y;
					else if (y < integralmin)
						integralmin = y;
				}
			}
		}
	}
}

void CNet12NMRView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	if (bActivate) {
		if (pActivateView == this) {
			CNet12NMRDoc *pDoc = GetDocument();
			if (pDoc->dt.size()) {
				pDoc->LoadState();
				UpdateIntegralList(false);
				UpdatePeakList(false);
				//TRACE1("Activating %s\n", pDoc->fullpathname);
			}
		}
	//} else {
	//	if (pDeactiveView == this) {
	//		CNet12NMRDoc *pDoc = GetDocument();
	//		if (pDoc->dt.size())
	//			TRACE1("Deactivate %s\n", pDoc->fullpathname);
	//	}
	}
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

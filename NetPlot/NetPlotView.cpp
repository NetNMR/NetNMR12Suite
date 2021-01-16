// NetPlotView.cpp : implementation of the CNetPlotView class
//

#include "stdafx.h"
#include "..\NMRDataTypes.h"
#include "NetPlot.h"
#include "Spectrum.h"
#include "NetPlotDoc.h"
#include "NetPlotView.h"
#include "MainFrame.h"
#include "ProcessWnd.h"
#include "Axis.h"
#include "OffsetDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CNetPlotView

IMPLEMENT_DYNCREATE(CNetPlotView, CView)

BEGIN_MESSAGE_MAP(CNetPlotView, CView)
	// Standard printing commands
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_CONTEXTMENU()
	ON_COMMAND          (ID_FILE_PRINT_DIRECT,           &CView::OnFilePrint)
	ON_COMMAND          (ID_FILE_PRINT_PREVIEW,          &CView::OnFilePrintPreview)
	ON_COMMAND          (ID_FILE_PRINT,                  &CView::OnFilePrint)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT,                  &CNetPlotView::OnUpdateFilePrint)
	ON_COMMAND          (ID_VIEW_ZOOM,                   &CNetPlotView::OnViewZoom)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM,                   &CNetPlotView::OnUpdateViewZoom)
	ON_COMMAND          (ID_VIEW_FULL,                   &CNetPlotView::OnViewFull)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FULL,                   &CNetPlotView::OnUpdateViewFull)
	ON_COMMAND          (ID_INTEGRAL_CLEARALL,           &CNetPlotView::OnIntegralClearall)
	ON_UPDATE_COMMAND_UI(ID_INTEGRAL_CLEARALL,           &CNetPlotView::OnUpdateIntegralClearall)
	ON_COMMAND          (ID_INTEGRAL_ADDINTEGRAL,        &CNetPlotView::OnIntegralAddintegral)
	ON_UPDATE_COMMAND_UI(ID_INTEGRAL_ADDINTEGRAL,        &CNetPlotView::OnUpdateIntegralAddintegral)
	ON_COMMAND          (ID_INTEGRAL_REMOVEINTEGRAL,     &CNetPlotView::OnIntegralRemoveintegral)
	ON_UPDATE_COMMAND_UI(ID_INTEGRAL_REMOVEINTEGRAL,     &CNetPlotView::OnUpdateIntegralRemoveintegral)
	ON_COMMAND          (ID_INTEGRAL_FITINTEGRALS,       &CNetPlotView::OnIntegralFitintegrals)
	ON_UPDATE_COMMAND_UI(ID_INTEGRAL_FITINTEGRALS,       &CNetPlotView::OnUpdateIntegralFitintegrals)
	ON_COMMAND          (ID_INTEGRAL_ILTINTEGRALS,       &CNetPlotView::OnIntegralILTintegrals)
	ON_UPDATE_COMMAND_UI(ID_INTEGRAL_ILTINTEGRALS,       &CNetPlotView::OnUpdateIntegralFitintegrals)
	ON_COMMAND          (ID_Y_DOWN,                      &CNetPlotView::OnYDown)
	ON_UPDATE_COMMAND_UI(ID_Y_DOWN,                      &CNetPlotView::OnUpdateYDown)
	ON_COMMAND          (ID_Y_UP,                        &CNetPlotView::OnYUp)
	ON_UPDATE_COMMAND_UI(ID_Y_UP,                        &CNetPlotView::OnUpdateYUp)
	ON_COMMAND          (ID_Y_X1,                        &CNetPlotView::OnYX1)
	ON_UPDATE_COMMAND_UI(ID_Y_X1,                        &CNetPlotView::OnUpdateYX1)
	ON_COMMAND          (ID_VIEW_AUTOMATIC,              &CNetPlotView::OnViewAutomatic)
	ON_UPDATE_COMMAND_UI(ID_VIEW_AUTOMATIC,              &CNetPlotView::OnUpdateViewAutomatic)
	ON_UPDATE_COMMAND_UI(ID_CONTEXT_SINGLEPOINTINTEGRAL, &CNetPlotView::OnUpdateContextSinglepointintegral)
	ON_COMMAND          (ID_CONTEXT_SETOFFSETHERE,       &CNetPlotView::OnContextSetoffsethere)
	ON_COMMAND			(ID_VIEW_GLOBAL,                 &CNetPlotView::OnGlobalView)
	ON_UPDATE_COMMAND_UI(ID_VIEW_GLOBAL,                 &CNetPlotView::OnUpdateGlobalView)
	ON_COMMAND(ID_VIEW_DATACLASS, &CNetPlotView::OnViewDataclass)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DATACLASS, &CNetPlotView::OnUpdateViewDataclass)
END_MESSAGE_MAP()

// CNetPlotView construction/destruction

CNetPlotView::CNetPlotView()
: disMode(normalMode)
, oneSpec(NULL)
, iLeft(IDC_LEFTPHASEBUTTON)
{
	mainCursor = zoomCursor.left = zoomCursor.right = -1;
	leftcurpos = rightcurpos = 0.0f;
	zoomPen.CreatePen(PS_SOLID, 0,
		theApp.arrColors[theApp.RegistryInt(ReadInt, BackgroundColor, 8)].value^
		theApp.arrColors[theApp.RegistryInt(ReadInt, ZoomCursorColor, 3)].value);
	cursorPen.CreatePen(PS_SOLID, 0,
		theApp.arrColors[theApp.RegistryInt(ReadInt, BackgroundColor, 8)].value^
		theApp.arrColors[theApp.RegistryInt(ReadInt, MainCursorColor, 1)].value);
}

CNetPlotView::~CNetPlotView()
{
	zoomPen.DeleteObject();
	cursorPen.DeleteObject();
}

BOOL CNetPlotView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), (HBRUSH)::GetStockObject(LTGRAY_BRUSH), NULL);

	return CView::PreCreateWindow(cs);
}

// CNetPlotView drawing

void CNetPlotView::OnDraw(CDC* pDC)
{
	CNetPlotDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	PRINTDLG pd;
	pd.lStructSize=(DWORD)sizeof(PRINTDLG);
	if (theApp.GetPrinterDeviceDefaults(&pd)) {
		// protect memory handle with ::GlobalLock and ::GlobalUnlock
		DEVMODE FAR *pDevMode=(DEVMODE FAR *)::GlobalLock(pd.hDevMode);
		// set orientation to landscape
		switch (pDevMode->dmPaperSize) {
			case DMPAPER_LEGAL:
				page = CSize(21590, 35560);
				break;
			case DMPAPER_10X14:
				page = CSize(25400, 35560);
				break;
			case DMPAPER_11X17:
				page = CSize(27940, 43180);
				break;
			case DMPAPER_A3:
				page = CSize(29700, 42000);
				break;
			case DMPAPER_A4:
				page = CSize(21000, 29700);
				break;
			case DMPAPER_A5:
				page = CSize(14800, 21000);
				break;
			case DMPAPER_B5:
				page = CSize(18200, 25700);
				break;
			case DMPAPER_EXECUTIVE:
				page = CSize(18415, 26416);
				break;
			case DMPAPER_LETTER:
			default:
				page = CSize(21590, 27940);
				break;
		}
		if (pDoc->IsLandscape) {
			pDevMode->dmOrientation = DMORIENT_LANDSCAPE;
			int tmp = page.cx;
			page.cx = page.cy;
			page.cy = tmp;
		} else
			pDevMode->dmOrientation = DMORIENT_PORTRAIT;
		::GlobalUnlock(pd.hDevMode);
	} else {
		if (pDoc->IsLandscape)
			page = CSize(27940, 21590);
		else
			page = CSize(21590, 27940);
	}
	CRect cr;
	GetClientRect(cr);
	pDC->SetMapMode( MM_ISOTROPIC );
	windExt = CSize(page.cx, -page.cy);
	pDC->SetWindowExt(windExt);
	if (pDC->IsPrinting()) {
		int x = pDC->GetDeviceCaps(PHYSICALWIDTH);
		int y = pDC->GetDeviceCaps(PHYSICALHEIGHT);
		pDC->SetViewportExt(x, y);
		pDC->SetViewportOrg(0, y);
	} else {
		viewExt = CSize(cr.Width(), cr.Height());
		pDC->SetViewportExt(viewExt);
		viewOrg = CPoint(0, cr.Height());
		if (float(cr.Height())/float(cr.Width()) > float(page.cy)/float(page.cx))
			viewOrg.y = (cr.Height() + MulDiv(cr.Width(), page.cy, page.cx)) / 2;
		else
			viewOrg.x = (cr.Width() - MulDiv(cr.Height(), page.cx, page.cy)) / 2;
		pDC->SetViewportOrg(viewOrg);
		if (phaseMode != disMode) {
			COLORREF color = theApp.arrColors[theApp.RegistryInt(ReadInt, BackgroundColor, 8)].value;
			pDC->FillSolidRect( 0, 0, page.cx, page.cy, color);
		}
	}
	if (phaseMode == disMode && oneSpec)
		PaintPhase(pDC);
	else
		PaintCore(pDC);
}

void CNetPlotView::PaintPhase(CDC *pDC)
{
	CPen specPen(PS_SOLID, 0, theApp.arrColors[theApp.RegistryInt(ReadInt, SpectraColor, 0)].value);
	CPen *oldPen = pDC->SelectObject(&specPen);
	pDC->OffsetWindowOrg(0, 0);
	int sms = oneSpec->Stop() - oneSpec->Start();
	pDC->MoveTo(MulDiv(oneSpec->from-oneSpec->Start(), page.cx, sms),
		oneSpec->Translate(page.cy, oneRange, oneMin, oneSpec->GetAt(oneSpec->from)));
	for (int i=oneSpec->from+1; i<=oneSpec->to; i++)
		pDC->LineTo(MulDiv(i-oneSpec->Start(), page.cx, sms),
			oneSpec->Translate(page.cy, oneRange, oneMin, oneSpec->GetAt(i)));
	int ropmode = pDC->SetROP2(R2_XORPEN);
	if (zoomCursor.left >= 0) {
		pDC->SelectObject(&zoomPen);
		if (IDC_LEFTPHASEBUTTON == iLeft) {
			int x = MulDiv(zoomCursor.left, page.cx, specSize.cx);
			pDC->MoveTo(x, 0);
			pDC->LineTo(x, page.cy);
		} else {
			int x = MulDiv(zoomCursor.right, page.cx, specSize.cx);
			pDC->MoveTo(x, 0);
			pDC->LineTo(x, page.cy);
		}
	}
	pDC->SelectObject(oldPen);
}

void CNetPlotView::PaintCore(CDC *pDC)
{
	CWaitCursor wait;
	CNetPlotDoc *pDoc = GetDocument();
	if (0 == pDoc->Spectra.GetCount())
		return;
	int count = (int)pDoc->Spectra.GetCount();
	int h = int(1000.0 * pDoc->Height);
	int w = int(1000.0 * pDoc->Width);
	int oldBkMode = pDC->SetBkMode(TRANSPARENT);
	COLORREF oldColor = pDC->SetTextColor(theApp.arrColors[theApp.RegistryInt(ReadInt, AxisColor, 0)].value);
	if (pDoc->YExpansion != 1.0f) {
		CFont expFont;
		expFont.CreateFont( 400, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Symbol" );
		CFont *origFont = pDC->SelectObject( &expFont );
		pDC->SetTextAlign( TA_RIGHT | TA_BOTTOM );
		CString s;
		s.Format( "%c%g", 180, pDoc->YExpansion );
		pDC->TextOut( page.cx-1000, 1000, s );
		pDC->SelectObject( origFont );
		expFont.DeleteObject();
		pDC->SetTextAlign( TA_LEFT | TA_BOTTOM );
	}
	if (theApp.DataClass.Count() && pDoc->ShowClass) {
		int highestdataclass = -1;
		POSITION pos = pDoc->Spectra.GetHeadPosition();
		while (pos) {
			CSpectrum *pSpec = pDoc->Spectra.GetNext(pos);
			if (pSpec->GetClass() > highestdataclass)
				highestdataclass = pSpec->GetClass();
		}
		pDC->OffsetWindowOrg(0, 0);
		CFont classFont;
		classFont.CreateFont(400, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Arial");
		CFont *origFont = pDC->SelectObject( &classFont );
		pDC->SetTextColor(theApp.arrColors[theApp.RegistryInt(ReadInt, TitleColor, 0)].value);
		pDC->TextOut(1000, 1000, theApp.DataClass.GetAt(highestdataclass));
		pDC->SelectObject( origFont );
		classFont.DeleteObject();
	}

	specSize = CSize(
		int(1000.0f*(pDoc->Width +float(count-1)*fabs(pDoc->OffsetX))),
		int(1000.0f*(pDoc->Height+float(count-1)*fabs(pDoc->OffsetY))));
	windOrg = CPoint(
		-int((page.cx-specSize.cx)/2.0),
		-int((page.cy-specSize.cy)/2.0));
	pDC->OffsetWindowOrg(
		-int((page.cx-w-1000.0*(count-1)*pDoc->OffsetX)/2.0),
		-int((page.cy-h+1000.0*(count-1)*pDoc->OffsetY)/2.0));
	if ( ! pDoc->Title.IsEmpty() ) {
		CFont titleFont;
		titleFont.CreateFont( MulDiv(pDoc->nTitleSize, 100, 3), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Arial" );
		CFont *origFont = pDC->SelectObject( &titleFont );
		pDC->SetTextColor(theApp.arrColors[theApp.RegistryInt(ReadInt, TitleColor, 0)].value);
		pDC->SetTextAlign( TA_CENTER | TA_BOTTOM );
		pDC->TextOut( w/2, h+1720, pDoc->Title );
		pDC->SetTextAlign( TA_LEFT | TA_BOTTOM );
		pDC->SelectObject( origFont );
		titleFont.DeleteObject();
	}
	CFont labelFont;
	labelFont.CreateFont( MulDiv(pDoc->nLabelSize, 100, 3), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Times New Roman" );
	CPen integralpen(PS_SOLID, 0, theApp.arrColors[theApp.RegistryInt(ReadInt, IntegralColor, 5)].value);
	CPen basepen(PS_SOLID, 0, theApp.arrColors[theApp.RegistryInt(ReadInt, BaselineColor, 6)].value);
	CFont *origFont = pDC->SelectObject( &labelFont );
	POSITION pos = pDoc->Spectra.GetHeadPosition();
	int scount = 0;
	while ( pos ) {
		CSpectrum *pSpec = pDoc->Spectra.GetNext( pos );
		CPen spectrapen(PS_SOLID, 0, theApp.arrColors[pSpec->color].value);
		CPen *oldPen = pDC->SelectObject(&spectrapen);
		pDC->SetTextColor(theApp.arrColors[pSpec->color].value);
		int sms = pSpec->Stop() - pSpec->Start();
		float yrange, ymin;
		if ( pDoc->GlobalScaling ) {	// GLOBAL SCALING IS ON
			ymin = pDoc->ymin;
			yrange = (pDoc->ymax - ymin) / (pDoc->YExpansion * pSpec->GetYExpansion());
			if (pDoc->ymin < 0.0f)
				ymin /= pDoc->YExpansion * pSpec->GetYExpansion();
		} else {						// GLOBAL SCALING IS OFF
			yrange = pSpec->GetYRange() / (pDoc->YExpansion * pSpec->GetYExpansion());
			if (abs(pSpec->Ymax()) >= abs(pSpec->Ymin()))
				ymin = pSpec->Ymin();
			else
				ymin = pSpec->Ymax() - yrange;
		}
		if ( pSpec->to-pSpec->from && yrange ) {
			if ( ! pDoc->Label.IsEmpty() && ! pSpec->label.IsEmpty() ) {
				int x = int( 1000.0 * pDoc->LabelX );
				int y = int( 1000.0 * pDoc->LabelY );
				if (0.0f == pDoc->OffsetY)
					y += pDoc->nLabelSize * 40 * scount++;
				pDC->TextOut( x, y, pSpec->label );
			}
			pDC->MoveTo( MulDiv(pSpec->from-pSpec->Start(), w, sms),
				pSpec->Translate(h, yrange, ymin, pSpec->GetAt(pSpec->from)) );
			if (subtractMode != disMode) {
				for ( int i=pSpec->from+1; i<=pSpec->to; i++ ) {
					if (pDoc->AllTheSame && pDoc->bIntegral.size())
						pDC->SelectObject(pDoc->bIntegral[i]?&integralpen:&spectrapen);
					pDC->LineTo(MulDiv(i-pSpec->Start(), w, sms),
						pSpec->Translate(h, yrange, ymin, pSpec->GetAt(i)));
				}
			}
			if (baseMode==disMode || subtractMode==disMode) {
				if (baseMode==disMode)
					pDC->SelectObject(&basepen);
				pDC->MoveTo( MulDiv(pSpec->from-pSpec->Start(), w, sms),
					pSpec->Translate(h, yrange, ymin, pSpec->GetBaseAt(pSpec->from)) );
				for ( int i=pSpec->from+1; i<=pSpec->to; i++ ) {
					pDC->LineTo(MulDiv(i-pSpec->Start(), w, sms),
						pSpec->Translate(h, yrange, ymin, pSpec->GetBaseAt(i)));
				}
			}
					
		}
		if ( pos ) {
			pDC->OffsetWindowOrg( -int(1000.0*pDoc->OffsetX), int(1000.0*pDoc->OffsetY) );
		} else {
			CAxis Axis;
			Axis.Update(pDC, w, h, theApp.arrColors[theApp.RegistryInt(ReadInt, AxisColor, 0)].value );
		}
		pDC->SelectObject(oldPen);
		spectrapen.DeleteObject();
	}

	if (! pDC->IsPrinting()) {
		int ropmode = pDC->SetROP2(R2_XORPEN);
		if (zoomCursor.left >= 0) {
			CPen *oldpen = pDC->SelectObject(&zoomPen);
			pDC->MoveTo(zoomCursor.left, 0);
			pDC->LineTo(zoomCursor.left, specSize.cy);
			pDC->MoveTo(zoomCursor.right, 0);
			pDC->LineTo(zoomCursor.right, specSize.cy);
			pDC->SelectObject(oldpen);
		}
		if (mainCursor >= 0) {
			CPen *oldpen = pDC->SelectObject(&cursorPen);
			pDC->MoveTo(mainCursor, 0);
			pDC->LineTo(mainCursor, specSize.cy);
			pDC->SelectObject(oldpen);
		}
		pDC->SetROP2(ropmode);
	}
	pDC->SelectObject(origFont);
	pDC->SetTextColor(oldColor);
	pDC->SetBkMode(oldBkMode);
	labelFont.DeleteObject();
	integralpen.DeleteObject();
}

// CNetPlotView printing

BOOL CNetPlotView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CNetPlotView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CNetPlotView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CNetPlotView diagnostics

#ifdef _DEBUG
void CNetPlotView::AssertValid() const
{
	CView::AssertValid();
}

void CNetPlotView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CNetPlotDoc* CNetPlotView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CNetPlotDoc)));
	return (CNetPlotDoc*)m_pDocument;
}
#endif //_DEBUG


// CNetPlotView message handlers

void CNetPlotView::CopytoClipboard() 
{
	CMetaFileDC mfDC;
	mfDC.CreateEnhanced(NULL, NULL, NULL, "NetPlot\01D NMR StackPlot\0\0");
	mfDC.SetMapMode(MM_HIMETRIC);
	PaintCore(&mfDC);
	HENHMETAFILE hMF = mfDC.CloseEnhanced();
	if ( hMF ) {
		if ( OpenClipboard() ) {
			if ( EmptyClipboard() ) {
				SetClipboardData( CF_ENHMETAFILE, hMF );
			}
			CloseClipboard();
		}
	}
}

void CNetPlotView::OnUpdateFilePrint(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDocument()->Spectra.GetCount()>0);
}

void CNetPlotView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CNetPlotDoc *pDoc = GetDocument();
	if (pDoc->Spectra.GetCount() && (pDoc->AllTheSame||pDoc->Range!=RangeFull) && 0.0f==pDoc->OffsetX) {
		leftcurpos = curcurpos;
		CDC *pDC = GetDC();
		pDC->SetMapMode(MM_ISOTROPIC);
		pDC->SetWindowExt(windExt);
		pDC->SetViewportExt(viewExt);
		pDC->SetViewportOrg(viewOrg);
		pDC->SetWindowOrg(windOrg);
		pDC->DPtoLP(&point);
		CPen *oldPen = pDC->SelectObject(&zoomPen);
		int oldMode = pDC->SetROP2(R2_XORPEN);
		if (zoomCursor.right >= 0) {
			pDC->MoveTo(zoomCursor.right, 0);
			pDC->LineTo(zoomCursor.right, specSize.cy);
			zoomCursor.right = -1;
		}
		if (zoomCursor.left >= 0) {
			pDC->MoveTo(zoomCursor.left, 0);
			pDC->LineTo(zoomCursor.left, specSize.cy);
		}
		zoomCursor.left = point.x;
		if (zoomCursor.left < 0)
			zoomCursor.left = 0;
		else if (zoomCursor.left > specSize.cx)
			zoomCursor.left = specSize.cx;
		pDC->MoveTo(zoomCursor.left, 0);
		pDC->LineTo(zoomCursor.left, specSize.cy);
		pDC->SetROP2(oldMode);
		pDC->SelectObject(oldPen);
		ReleaseDC(pDC);
		SetCapture();
	}
	CView::OnLButtonDown(nFlags, point);
}

void CNetPlotView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (zoomCursor.left >= 0 && zoomCursor.right == -1) {
		rightcurpos = curcurpos;
		CDC *pDC = GetDC();
		pDC->SetMapMode(MM_ISOTROPIC);
		pDC->SetWindowExt(windExt);
		pDC->SetViewportExt(viewExt);
		pDC->SetViewportOrg(viewOrg);
		pDC->SetWindowOrg(windOrg);
		pDC->DPtoLP(&point);
		CPen *oldPen = pDC->SelectObject(&zoomPen);
		int oldMode = pDC->SetROP2(R2_XORPEN);
		zoomCursor.right = point.x;
		if (zoomCursor.right < 0)
			zoomCursor.right = 0;
		else if (zoomCursor.right > specSize.cx)
			zoomCursor.right = specSize.cx;
		pDC->MoveTo(zoomCursor.right, 0);
		pDC->LineTo(zoomCursor.right, specSize.cy);
		if (zoomCursor.left == zoomCursor.right) {
			zoomCursor.left = zoomCursor.right = -1;
			leftcurpos = rightcurpos = 0.0f;
		} else if (zoomCursor.left > zoomCursor.right) {
			int i = zoomCursor.left;
			zoomCursor.left = zoomCursor.right;
			zoomCursor.right = i;
			float f = rightcurpos;
			rightcurpos = leftcurpos;
			leftcurpos = f;
		}
		pDC->SetROP2(oldMode);
		pDC->SelectObject(oldPen);
		ReleaseDC(pDC);
		ReleaseCapture();
	}	
	CView::OnLButtonUp(nFlags, point);
}

void CNetPlotView::OnMouseMove(UINT nFlags, CPoint point)
{
	CNetPlotDoc *pDoc = GetDocument();
	if (pDoc->Spectra.GetCount() && windExt.cx>0) {
		if (pDoc->Range!=RangeFull && 0.0f==pDoc->OffsetX) {
			CDC *pDC = GetDC();
			pDC->SetMapMode(MM_ISOTROPIC);
			pDC->SetWindowExt(windExt);
			pDC->SetViewportExt(viewExt);
			pDC->SetViewportOrg(viewOrg);
			pDC->SetWindowOrg(windOrg);
			pDC->DPtoLP(&point);
			CPen *oldPen = pDC->SelectObject(&cursorPen);
			int oldMode = pDC->SetROP2(R2_XORPEN);
			if (mainCursor >= 0) {
				pDC->MoveTo(mainCursor, 0);
				pDC->LineTo(mainCursor, specSize.cy);
			}
			CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, theApp.m_pMainWnd);
			if (point.x>=0 && point.x<=specSize.cx && point.y>=0 && point.y<=specSize.cy) {
				curcurpos = pDoc->RangeFrom - float(point.x)*(pDoc->RangeFrom-pDoc->RangeTo)/float(specSize.cx);
				CString msg;
				msg.Format("%.2f %s", curcurpos, pDoc->Range==RangePPM?"PPM":"Hz");
				pMain->MessagetoBar(msg);
				mainCursor = point.x;
				pDC->MoveTo(mainCursor, 0);
				pDC->LineTo(mainCursor, specSize.cy);
			} else {
				pMain->MessagetoBar("");
				mainCursor = -1;
			}
			pDC->SetROP2(oldMode);
			pDC->SelectObject(oldPen);
			ReleaseDC(pDC);
		}
	}
	CView::OnMouseMove(nFlags, point);
}

void CNetPlotView::OnViewZoom()
{
	CNetPlotDoc *pDoc = GetDocument();
	if (RangeFull == pDoc->Range) {
		pDoc->RangeFrom = pDoc->PPMFrom;
		pDoc->RangeTo = pDoc->PPMTo;
		pDoc->Range = RangePPM;
	}
	float d = pDoc->RangeFrom - pDoc->RangeTo;
	float f = pDoc->RangeFrom - d * float(zoomCursor.left)  / float(specSize.cx);
	float t = pDoc->RangeFrom - d * float(zoomCursor.right) / float(specSize.cx);
	pDoc->RangeFrom = f;
	pDoc->RangeTo   = t;
	POSITION pos = pDoc->Spectra.GetHeadPosition();
	while( pos ) {
		CSpectrum *pSpec = pDoc->Spectra.GetNext( pos );
		pSpec->CalcStartStop();
		pSpec->CalcToFrom();
		pSpec->FindMinMax();
	}
	pDoc->SetMinMax();
	mainCursor = zoomCursor.left = zoomCursor.right = -1;
	Invalidate(TRUE);
	pDoc->SetModifiedFlag();
}

void CNetPlotView::OnUpdateViewZoom(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(zoomCursor.left>=0 && zoomCursor.right>=0);
}

void CNetPlotView::OnViewFull()
{
	CNetPlotDoc *pDoc = GetDocument();
	pDoc->RangeFrom = (pDoc->Range==RangeHertz) ? pDoc->HertzFrom : pDoc->PPMFrom;
	pDoc->RangeTo   = (pDoc->Range==RangeHertz) ? pDoc->HertzTo   : pDoc->PPMTo;
	if (pDoc->Range==RangeFull)
		pDoc->Range = RangePPM;
	POSITION pos = pDoc->Spectra.GetHeadPosition();
	while( pos ) {
		CSpectrum *pSpec = pDoc->Spectra.GetNext( pos );
		pSpec->CalcStartStop();
		pSpec->CalcToFrom();
		pSpec->FindMinMax();
	}
	pDoc->SetMinMax();
	mainCursor = zoomCursor.left = zoomCursor.right = -1;
	Invalidate(TRUE);
	pDoc->SetModifiedFlag();
}

void CNetPlotView::OnUpdateViewFull(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(CanDoFull());
}

BOOL CNetPlotView::CanDoFull()
{
	CNetPlotDoc *pDoc = GetDocument();
	if (pDoc->Spectra.GetCount()) {
		switch (pDoc->Range) {
			case RangePPM:
				return (pDoc->PPMFrom!=pDoc->RangeFrom || pDoc->PPMTo!=pDoc->RangeTo);
				break;
			case RangeHertz:
				return (pDoc->HertzFrom!=pDoc->RangeFrom || pDoc->HertzTo!=pDoc->RangeTo);
				break;
			default:
				return TRUE;
				break;
		}
	} else
		return FALSE;
}

void CNetPlotView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CMenu menu;
	if (0 == GetDocument()->Spectra.GetCount())
		return;
	if (menu.LoadMenu(IDR_CONTEXTMENU)) {
		CMenu *popup = menu.GetSubMenu(0);
		if (popup) {
			popup->EnableMenuItem(ID_VIEW_ZOOM, (zoomCursor.left>=0&&zoomCursor.right>=0)?MF_ENABLED:MF_GRAYED);
			popup->EnableMenuItem(ID_VIEW_FULL, CanDoFull()?MF_ENABLED:MF_GRAYED);
			popup->EnableMenuItem(ID_CONTEXT_SETOFFSETHERE, (IDC_RANGEFULL==GetDocument()->Range||GetDocument()->OffsetX)?MF_GRAYED:MF_ENABLED);
			popup->EnableMenuItem(ID_CONTEXT_SINGLEPOINTINTEGRAL, mainCursor>=0?MF_ENABLED:MF_GRAYED);
			popup->EnableMenuItem(ID_INTEGRAL_ADDINTEGRAL, OnUpdateIntegral()?MF_ENABLED:MF_GRAYED);
			popup->EnableMenuItem(ID_INTEGRAL_REMOVEINTEGRAL, OnUpdateIntegral()?MF_ENABLED:MF_GRAYED);
			popup->EnableMenuItem(ID_INTEGRAL_CLEARALL, OnUpdateClearAll()?MF_ENABLED:MF_GRAYED);
			popup->EnableMenuItem(ID_INTEGRAL_FITINTEGRALS, OnUpdateFitIntegrals()?MF_ENABLED:MF_GRAYED);
			popup->EnableMenuItem(ID_Y_UP, MF_ENABLED);
			popup->EnableMenuItem(ID_Y_DOWN, MF_ENABLED);
			popup->EnableMenuItem(ID_Y_X1, (1.0f==GetDocument()->YExpansion)?MF_GRAYED:MF_ENABLED);
			popup->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, pWnd);
		}
	}
}

void CNetPlotView::OnIntegralClearall()
{
	CNetPlotDoc *pDoc = GetDocument();
	if (pDoc->bIntegral.size()) {
		for (UINT i=0; i<pDoc->bIntegral.size(); i++)
			pDoc->bIntegral[i] = false;
		if (baseMode == disMode)
			pDoc->CalculateBaseline();
		else
			Invalidate();
	}
}

void CNetPlotView::OnUpdateIntegralAddintegral(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(OnUpdateIntegral());
}

void CNetPlotView::OnUpdateIntegralRemoveintegral(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(OnUpdateIntegral());
}

BOOL CNetPlotView::OnUpdateIntegral()
{
	CNetPlotDoc *pDoc = GetDocument();
	return (pDoc->Spectra.GetCount() && pDoc->bIntegral.size()>0 && pDoc->AllTheSame && 
		zoomCursor.left>=0 && zoomCursor.right>=0);
}

void CNetPlotView::OnIntegralAddintegral()
{
	OnIntegral(true);
}

void CNetPlotView::OnIntegralRemoveintegral()
{
	OnIntegral(false);
}

void CNetPlotView::OnIntegral(bool value)
{
	CNetPlotDoc *pDoc = GetDocument();
	CSpectrum *pSpec = pDoc->Spectra.GetHead();
	int start = pSpec->Start();
	int stop = pSpec->Stop();
	int diff = stop - start;
	int from = int(float(start) + float(diff) * float(zoomCursor.left ) / float(specSize.cx) + 0.5f);
	int to   = int(float(start) + float(diff) * float(zoomCursor.right) / float(specSize.cx) + 0.5f);
	for (int i=from; i<=to; i++)
		if (i>=0 && i<(int)pDoc->bIntegral.size())
			pDoc->bIntegral[i] = value;
	pDoc->SetModifiedFlag();
	if (baseMode == disMode)
		pDoc->CalculateBaseline();
	else
		Invalidate();
}

void CNetPlotView::OnUpdateIntegralClearall(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(OnUpdateClearAll());
}

void CNetPlotView::OnUpdateContextSinglepointintegral(CCmdUI *pCmdUI)
{
	CNetPlotDoc *pDoc = GetDocument();
	CSpectrum *pSpec = pDoc->Spectra.GetHead();
	int start = pSpec->Start();
	int diff = pSpec->Stop() - start;
	int pt = int(float(start+1) + float(diff) * float(mainCursor) / float(specSize.cx));
	if (pt>=0 && pt<=(int)pDoc->bIntegral.size()) {
		if (! pDoc->bIntegral[pt]) {
			pDoc->bIntegral[pt] = true;
			pDoc->SetModifiedFlag();
			Invalidate();
		}
	}
}

BOOL CNetPlotView::OnUpdateClearAll()
{
	CNetPlotDoc *pDoc = GetDocument();
	if (0 == pDoc->Spectra.GetCount())
		return FALSE;
	BOOL retval = FALSE;
	for (UINT i=0; i<pDoc->bIntegral.size(); i++) {
		if (pDoc->bIntegral[i]) {
			retval = TRUE;
			break;
		}
	}
	return retval;
}

void CNetPlotView::OnUpdateIntegralFitintegrals(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(OnUpdateFitIntegrals());
}

BOOL CNetPlotView::OnUpdateFitIntegrals()
{
	CNetPlotDoc *pDoc = GetDocument();
	if (0 == pDoc->Spectra.GetCount())
		return FALSE;
	BOOL retval = FALSE;
	CSpectrum *aSpec = pDoc->Spectra.GetHead();
	for (int i=aSpec->Start(); i<=aSpec->Stop(); i++) {
		if (i>=0 && i<(int)pDoc->bIntegral.size()) {
			if (pDoc->bIntegral[i]) {
				retval = TRUE;
				break;
			}
		}
	}
	return retval;
}

void CNetPlotView::OnIntegralILTintegrals()
{
	// FIRST: DON'T GO IF THERE IS NO CURRENT DOCUMENT PATH!
	CNetPlotDoc *pDoc = GetDocument();
	if (3 > strlen(pDoc->GetPathName())) {
		AfxMessageBox("To establish a file path, first save the *.npl document!", MB_ICONHAND);
		return;
	}
	// SECOND: GET INTEGRAL RANGES TO BE CALCULATED
	typedef struct rangetag {
		int from, to;
		double ppmfrom, ppmto;
	} RANGE;
	CTypedPtrList <CPtrList, RANGE *> range;
	CSpectrum *aSpec = pDoc->Spectra.GetHead();
	double sw = aSpec->SweepWidth();
	double sf = aSpec->Frequency();
	double of = aSpec->Offset();
	double cb = double(aSpec->Size());
	bool prev = false;
	for (int i=aSpec->Start(); i<=aSpec->Stop(); i++) {
		if (i>=0 && i<(int)pDoc->bIntegral.size()) {
			if (pDoc->bIntegral[i] != prev) {
				if (pDoc->bIntegral[i]) {	// HERE COMES A NEW INTEGRAL
					RANGE *newrange = new RANGE;
					newrange->from = i;
					newrange->ppmfrom = (of + sw * (0.5 - double(i) / cb)) / sf;
					range.AddTail(newrange);
				} else {					// OKAY THIS INTEGRAL IS DONE
					range.GetTail()->to = i - 1;
					range.GetTail()->ppmto = (of + sw * (0.5 - double(i - 1) / cb)) / sf;
				}
				prev = pDoc->bIntegral[i];
			}
			// CLOSE THE FINAL INTEGRAL, IF NEEDED
			if (i==(int)pDoc->bIntegral.size()-1 && pDoc->bIntegral[i]) {
				range.GetTail()->to = i;
				range.GetTail()->ppmto = (of + sw * (0.5 - double(i) / cb)) / sf;
			}
		}
	}

	// THIRD: CREATE TEXT FILE
	CString txtpath = pDoc->GetPathName();
	if (0 == txtpath.Right(4).CompareNoCase(".npl"))
		txtpath = txtpath.Left(int(strlen(txtpath)) - 4);
	txtpath += "-integrals.txt";
	CStdioFile txtfile(txtpath, CFile::modeCreate|CFile::modeWrite|CFile::typeText);
	if (txtfile.m_hFile != CFile::hFileNull) {
		CString line, value;
		if (aSpec->FHGet("dosygamma", value)) {
			line.Format("# gamma=%s\n", value);
			txtfile.WriteString(line);
		}
		if (aSpec->FHGet("timecubed", value)) {
			line.Format("# tcubed=%s\n", value);
			txtfile.WriteString(line);
		} else if (aSpec->FHGet("dosytimecubed", value)) {
			line.Format("# tcubed=%s\n", value);
			txtfile.WriteString(line);
		}
		if (aSpec->FHGet("DAC_to_G", value)) {
			line.Format("# dactog=%s\n", value);
			txtfile.WriteString(line);
		}
		line = pDoc->Label;
		POSITION pos = range.GetHeadPosition();
		while (pos) {
			RANGE *r = range.GetNext(pos);
			if (r->ppmfrom == r->ppmto)
				value.Format("\t%.3f PPM", r->ppmfrom);
			else
				value.Format("\t%.3f to %.3f PPM", r->ppmfrom, r->ppmto);
			line += value;
		}
		txtfile.WriteString(line + '\n');
		pos = pDoc->Spectra.GetHeadPosition();
		while (pos) {
			CSpectrum *pSpec = pDoc->Spectra.GetNext(pos);
			line = pSpec->label;
			POSITION rpos = range.GetHeadPosition();
			while (rpos) {
				RANGE *r = range.GetNext(rpos);
				double sum = 0.0;
				for (int i=r->from; i<=r->to; i++)
					sum += double(pSpec->GetAt(i));
				value.Format("\t%g", sum);
				line += value;
			}
			txtfile.WriteString(line + '\n');
		}
		txtfile.Close();
	}

	// FOURTH: START ILT
	CString cmd = theApp.m_pszHelpFilePath;
	cmd = cmd.Left(cmd.ReverseFind('\\') + 1);
	cmd += "ilt.exe";
	if (cmd.Find(' '))
		cmd = '\"' + cmd + '\"';
	cmd = "start \"Launch ILT\" " + cmd;	// "Launch ILT" is a required title
	cmd += " \"" + txtpath + '\"';
	system(cmd);

	// LAST: CLEAN-UP TEMPORARY STORAGE
	while (range.GetCount())
		delete range.RemoveHead();
}

void CNetPlotView::OnIntegralFitintegrals()
{
	// FIRST: GET INTEGRAL RANGES TO BE CALCULATED
	typedef struct rangetag {
		int from, to;
		double ppmfrom, ppmto;
	} RANGE;
	CNetPlotDoc *pDoc = GetDocument();
	CTypedPtrList <CPtrList, RANGE *> range;
	CSpectrum *aSpec = pDoc->Spectra.GetHead();
	double sw = aSpec->SweepWidth();
	double sf = aSpec->Frequency();
	double of = aSpec->Offset();
	double cb = double(aSpec->Size());
	bool prev = false;
	for (int i=aSpec->Start(); i<=aSpec->Stop(); i++) {
		if (i>=0 && i<(int)pDoc->bIntegral.size()) {
			if (pDoc->bIntegral[i] != prev) {
				if (pDoc->bIntegral[i]) {	// HERE COMES A NEW INTEGRAL
					RANGE *newrange = new RANGE;
					newrange->from = i;
					newrange->ppmfrom = (of + sw * (0.5 - double(i) / cb)) / sf;
					range.AddTail(newrange);
				} else {					// OKAY THIS INTEGRAL IS DONE
					range.GetTail()->to = i - 1;
					range.GetTail()->ppmto = (of + sw * (0.5 - double(i - 1) / cb)) / sf;
				}
				prev = pDoc->bIntegral[i];
			}
			// CLOSE THE FINAL INTEGRAL, IF NEEDED
			if (i==(int)pDoc->bIntegral.size()-1 && pDoc->bIntegral[i]) {
				range.GetTail()->to = i;
				range.GetTail()->ppmto = (of + sw * (0.5 - double(i) / cb)) / sf;
			}
		}
	}

	// SECOND: CALCULATE ALL INTEGRALS
	CTypedPtrList <CPtrList, double *> integrals;
	POSITION rpos = range.GetHeadPosition();
	int count = int(pDoc->Spectra.GetCount());
	while (rpos) {
		RANGE *r = range.GetNext(rpos);
		double *ilist = new double[count];
		POSITION spos = pDoc->Spectra.GetHeadPosition();
		for (int i=0; i<count; i++) {
			CSpectrum *pSpec = pDoc->Spectra.GetNext(spos);
			double sum = 0.0;
			// INTEGRATE!
			for (int j=r->from; j<=r->to; j++)
				sum += double(pSpec->GetAt(j));
			ilist[i] = sum;
		}
		integrals.AddTail(ilist);
	}

	// THIRD: CREATE TEXT
	CString text;
	// CODE UP THE DOSY PARAMETERS AND PUT THEM IN THE FIRST CELL, IF THEY EXIST
	if (aSpec->FHGet("dosygamma", text)) {
		text = "gamma=" + text;
		CString value;
		if (aSpec->FHGet("timecubed", value)) {
			text += " tcubed=" + value;
			if (aSpec->FHGet("DAC_to_G", value)) {
				text += " dactog=" + value;
			} else {
				text.Empty();
			}
		} else {
			text.Empty();
		}
	}

	if (text.IsEmpty())	// THERE WAS NO DOSY DATA, SO THE FILENAME WILL GO IN THE FIRST CELL
		text = pDoc->GetTitle();
	// NOW ADD THE SPECTRA LABELS. THESE LABELS WILL BECOME THE SPECTRA NAMES (PARAMETER VALUES)
	POSITION pos = pDoc->Spectra.GetHeadPosition();
	while (pos) {
		CSpectrum *pSpec = pDoc->Spectra.GetNext(pos);
		text += "\t" + pSpec->label;
	}
	text += "\tShape\tWidth\tShift\r\n";	// NEEDED TO SATISFY nmr curve fit.xlsm
	rpos = range.GetHeadPosition();
	POSITION ipos = integrals.GetHeadPosition();
	while (rpos) {
		RANGE *r = range.GetNext(rpos);
		double *ilist = integrals.GetNext(ipos);
		CString str;
		if (r->ppmfrom == r->ppmto)
			str.Format("\"%.3f PPM \"", r->ppmfrom);
		else
			str.Format("\"%.3f to %.3f PPM\"", r->ppmfrom, r->ppmto);
		text += str;
		for (int i=0; i<count; i++) {
			str.Format("\t%g", ilist[i]);
			text += str;
		}
		str.Format("\tX\t%g\t%g\n", r->ppmfrom-r->ppmto, 0.5*(r->ppmfrom+r->ppmto));
		text += str;
	}
	// DO IT AGAIN TO SATISFY THE nmr curve fit.xlsm MACRO
	rpos = range.GetHeadPosition();
	ipos = integrals.GetHeadPosition();
	while (rpos) {
		RANGE *r = range.GetNext(rpos);
		double *ilist = integrals.GetNext(ipos);
		CString str;
		if (r->ppmfrom == r->ppmto)
			str.Format("\"%.3f PPM\"", r->ppmfrom);
		else
			str.Format("\"%.3f to %.3f PPM\"", r->ppmfrom, r->ppmto);
		text += str;
		for (int i=0; i<count; i++) {
			str.Format("\t%g", ilist[i]);
			text += str;
		}
		text += "\r\n";
	}

	// FORTH: COPY TO CLIPBOARD
	int length = text.GetLength();
	if (OpenClipboard()) {
		if (EmptyClipboard()) {
			GLOBALHANDLE hgText = GlobalAlloc(GHND, length+1);
			if (hgText) {
				char *pgmem = (char *)GlobalLock(hgText);
				if (pgmem) {
					strncpy_s(pgmem, length+1, text, length);
					GlobalUnlock(hgText);
					SetClipboardData(CF_TEXT, hgText);
				}
			}
		}
		CloseClipboard();
	}

	// FIFTH: START "nmr curve fit.xlsm"
	text = "start excel \"";
	text += theApp.m_pszHelpFilePath;
	int i = text.ReverseFind('\\') + 1;
	text = text.Mid(0, i) + "nmr curve fit.xlsm\"";
	system(text);

	// LAST: CLEAN-UP TEMPORARY STORAGE
	while (range.GetCount())
		delete range.RemoveHead();
	while (integrals.GetCount())
		delete integrals.RemoveHead();
}

void CNetPlotView::OnWheel(float delta)
{
	CNetPlotDoc *pDoc = GetDocument();
	if (pDoc->Spectra.GetCount()) {
		if (yexpandMode == disMode) {
			oneSpec->SetYExpansion(oneSpec->GetYExpansion()*delta);
			CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
			pMain->dialogFrame->UpdateContent();
		} else
			pDoc->YExpansion *= delta;
		Invalidate(TRUE);
		GetDocument()->SetModifiedFlag();
	}
}

BOOL CNetPlotView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (zDelta) {
		double f = abs(double(zDelta))/double(WHEEL_DELTA);
		f *= (nFlags & MK_CONTROL) ?  1.04427378242741384 : 1.189207115; // Fourth or sixteenth root of 2
		if (zDelta < 0)
			f = 1.0/f;

		OnWheel((float)f);
	}
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CNetPlotView::OnYDown()
{
	OnWheel(0.5f);
}

void CNetPlotView::OnUpdateYDown(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDocument()->Spectra.GetCount()>0);
}

void CNetPlotView::OnYUp()
{
	OnWheel(2.0f);
}

void CNetPlotView::OnUpdateYUp(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDocument()->Spectra.GetCount()>0);
}

void CNetPlotView::OnYX1()
{
	GetDocument()->YExpansion = 1.0f;
	Invalidate();
}

void CNetPlotView::OnUpdateYX1(CCmdUI *pCmdUI)
{
	CNetPlotDoc *pDoc = GetDocument();
	pCmdUI->Enable(pDoc->Spectra.GetCount() && pDoc->YExpansion!=1.0f);
}

void CNetPlotView::AutoArrange(float &width, float &height, float &offsetx, float &offsety, float &labelx, float &labely)
{
	CNetPlotDoc *pDoc = GetDocument();
	int count = int(pDoc->Spectra.GetCount());
	float pw = 0.001f * float(page.cx);
	float ph = 0.001f * float(page.cy);
	offsetx = 0.0f;
	width = pw - 5.08f;
	if (count > 1) {
		if (pDoc->ymin < 0.0f && pDoc->ymax > 0.0f && -3.0f*pDoc->ymin >= pDoc->ymax) {
			height = (ph - 7.0f) / (0.5f * float(count+1));
			offsety = 0.5f * height;
			labelx = pw - 4.9f;
			labely = height * ((0.0f-pDoc->ymin) / (pDoc->ymax-pDoc->ymin));
		} else {
			offsety = (ph - 7.0f) / float(count);
			height = offsety - 0.1f;
			labelx = 0.0f;
			labely = 0.5f * height;
		}
	} else {
		height = ph - 7.0f;
		offsety = 4.0f;
		labelx = 0.0f;
		labely = 0.5f * height;
	}
}
void CNetPlotView::OnViewAutomatic()
{
	CNetPlotDoc *p = GetDocument();
	AutoArrange(p->Width, p->Height, p->OffsetX, p->OffsetY, p->LabelX, p->LabelY);
	p->SetModifiedFlag();
	Invalidate();
}

void CNetPlotView::OnUpdateViewAutomatic(CCmdUI *pCmdUI)
{
	CNetPlotDoc *p = GetDocument();
	pCmdUI->Enable(p->Spectra.GetCount() > 0);
}

void CNetPlotView::GetZoomCursors(int &left, int &right)
{
	left = zoomCursor.left;
	right = zoomCursor.right;
}
void CNetPlotView::OnContextSetoffsethere()
{
	COffsetDlg dlg;
	float start = curcurpos;
	dlg.m_floatOffset = start;
	if (IDOK == dlg.DoModal()) {
		CNetPlotDoc *pDoc = GetDocument();
		double difference = double(dlg.m_floatOffset - start);
		if (difference) {
			bool isfirst = true;
			POSITION pos = pDoc->Spectra.GetHeadPosition();
			while (pos) {
				CSpectrum *p = pDoc->Spectra.GetNext(pos);
				p->SetOffset(p->Offset() + difference * (IDC_RANGEPPM==GetDocument()->Range ? p->Frequency() : 1.0));
				p->CalcStartStop();
				p->CalcToFrom();
				p->FindMinMax();
				if (isfirst) {
					p->GetFromToRange(pDoc->PPMFrom, pDoc->PPMTo);
					pDoc->HertzFrom = pDoc->PPMFrom * float(p->Frequency());
					pDoc->HertzTo = pDoc->PPMTo * float(p->Frequency());
					isfirst = false;
				} else {
					float ppmfrom, ppmto;
					p->GetFromToRange(ppmfrom, ppmto);
					float hzfrom = ppmfrom * float(p->Frequency());
					float hzto = ppmto * float(p->Frequency());
					if (ppmfrom > pDoc->PPMFrom)
						pDoc->PPMFrom = ppmfrom;
					if (ppmto < pDoc->PPMTo)
						pDoc->PPMTo = ppmto;
					if (hzfrom > pDoc->HertzFrom)
						pDoc->HertzFrom = hzfrom;
					if (hzto < pDoc->HertzTo)
						pDoc->HertzTo = hzto;
				}
			}
			pDoc->SetMinMax();
			pDoc->SetModifiedFlag();
			Invalidate();
		}
	}
}

void CNetPlotView::OnGlobalView()
{
	GetDocument()->GlobalScaling = !GetDocument()->GlobalScaling;
	Invalidate();
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if (pMain && pMain->dialogFrame)
		pMain->dialogFrame->UpdateContent();
}

void CNetPlotView::OnUpdateGlobalView(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDocument()->Spectra.GetCount() > 0);
	pCmdUI->SetCheck(GetDocument()->GlobalScaling?1:0);
}

float CNetPlotView::GetCursorDifference()
{
	if (zoomCursor.left < 0)
		return(0.0f);
	if (zoomCursor.right < 0)
		return(leftcurpos - curcurpos);
	return(leftcurpos - rightcurpos);
}

void CNetPlotView::OnViewDataclass()
{
	CNetPlotDoc *pDoc = GetDocument();
	pDoc->ShowClass = ! pDoc->ShowClass;
	Invalidate();
}


void CNetPlotView::OnUpdateViewDataclass(CCmdUI *pCmdUI)
{
	CNetPlotDoc *pDoc = GetDocument();
	pCmdUI->SetCheck((pDoc->ShowClass && theApp.DataClass.Count()>0 && pDoc->Spectra.GetCount()>0) ? 1 : 0);
	pCmdUI->Enable(theApp.DataClass.Count()>0 && pDoc->Spectra.GetCount()>0);
}

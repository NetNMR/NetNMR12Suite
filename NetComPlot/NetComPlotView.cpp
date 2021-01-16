// NetComPlotView.cpp : implementation of the CNetComPlotView class
//

#include "stdafx.h"
#include "NetComPlot.h"
#include "SpecDim.h"
#include "OneDSpec.h"
#include "TwoDSpec.h"
#include "NetComPlotDoc.h"
#include "NetComPlotView.h"
#include "Axis.h"
#include "SetOffsetDlg.h"
#include "IntegralEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CNetComPlotView

IMPLEMENT_DYNCREATE(CNetComPlotView, CView)

BEGIN_MESSAGE_MAP(CNetComPlotView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_COMMAND(ID_EDIT_COPY, &CNetComPlotView::OnEditCopy)
	ON_COMMAND(ID_VIEW_ZOOM, &CNetComPlotView::OnViewZoom)
	ON_COMMAND(ID_VIEW_FULL, &CNetComPlotView::OnViewFull)
	ON_COMMAND(ID_VIEW_LAND, &CNetComPlotView::OnViewLand)
	ON_COMMAND(ID_VIEW_PORT, &CNetComPlotView::OnViewPort)
	ON_COMMAND(ID_VIEW_INTEGRAL, &CNetComPlotView::OnViewIntegral)
	ON_COMMAND(ID_VIEW_PREVIOUS, &CNetComPlotView::OnViewPrevious)
	ON_COMMAND(ID_SETOFFSETHERE, &CNetComPlotView::OnSetoffsethere)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &CNetComPlotView::OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM, &CNetComPlotView::OnUpdateViewZoom)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FULL, &CNetComPlotView::OnUpdateViewFull)
	ON_UPDATE_COMMAND_UI(ID_VIEW_INTEGRAL, &CNetComPlotView::OnUpdateViewIntegral)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PREVIOUS, &CNetComPlotView::OnUpdateViewPrevious)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LAND, &CNetComPlotView::OnUpdateViewLand)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PORT, &CNetComPlotView::OnUpdateViewPort)
	ON_WM_MOUSEMOVE()
	ON_WM_CONTEXTMENU()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_VIEW_INFORMATION, &CNetComPlotView::OnViewInformation)
	ON_UPDATE_COMMAND_UI(ID_VIEW_INFORMATION, &CNetComPlotView::OnUpdateViewInformation)
END_MESSAGE_MAP()

// CNetComPlotView construction/destruction

CNetComPlotView::CNetComPlotView()
: nbmapsize(0)
, bmap(NULL)
{
	cpCursor[0].x = 0;
	zoomRect = CRect(0,0,0,0);
	cp2DOffset = zoomStart = CPoint(0,0);
}

CNetComPlotView::~CNetComPlotView()
{
	if (nbmapsize) {
		if (bmap)
			delete[] bmap;
		nbmapsize = 0;
	}
}

BOOL CNetComPlotView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	// return CView::PreCreateWindow(cs);
	if (!CView::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), (HBRUSH)::GetStockObject(LTGRAY_BRUSH), NULL);

	return CView::PreCreateWindow(cs);
}

// CNetComPlotView drawing

void CNetComPlotView::OnDraw(CDC *pDC)
{
	CNetComPlotDoc *pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (fullRange==pDoc->param.hRange || fullRange==pDoc->param.vRange)
		OnViewFull();
	PRINTDLG pd;
	pd.lStructSize=(DWORD)sizeof(PRINTDLG);
	theApp.GetPageSize(DMPAPER_LETTER);
	if (theApp.GetPrinterDeviceDefaults(&pd)) {
		// protect memory handle with ::GlobalLock and ::GlobalUnlock
		DEVMODE FAR *pDevMode=(DEVMODE FAR *)::GlobalLock(pd.hDevMode);
		// set orientation to landscape
		pDevMode->dmPaperSize = pDoc->param.pagesize;
		pDevMode->dmOrientation = pDoc->param.dmorient;
		theApp.GetPageSize(pDoc->param.pagesize);
		if (pDevMode->dmOrientation == DMORIENT_LANDSCAPE) {
			int tmp = theApp.csPageSize.cx;
			theApp.csPageSize.cx = theApp.csPageSize.cy;
			theApp.csPageSize.cy = tmp;
		}
		::GlobalUnlock(pd.hDevMode);
	}
	pDC->SetMapMode(MM_ISOTROPIC);
	pDC->SetWindowExt(theApp.csPageSize.cx, -theApp.csPageSize.cy);
	if (pDC->IsPrinting()) {
		int x = pDC->GetDeviceCaps((pDoc->param.dmorient==DMORIENT_LANDSCAPE)?PHYSICALHEIGHT:PHYSICALWIDTH);
		int y = pDC->GetDeviceCaps((pDoc->param.dmorient==DMORIENT_LANDSCAPE)?PHYSICALWIDTH:PHYSICALHEIGHT);
		pDC->SetViewportExt(x, y);
		pDC->SetViewportOrg(0, y);
	} else {
		CRect cr;
		GetClientRect(cr);
		pDC->SetViewportExt(cr.Width(), cr.Height());
		CPoint pt(0, cr.Height());
		if (float(cr.Height())/float(cr.Width()) > float(theApp.csPageSize.cy)/float(theApp.csPageSize.cx))
			pt.y = (cr.Height() + MulDiv(cr.Width(), theApp.csPageSize.cy, theApp.csPageSize.cx)) / 2;
		else
			pt.x = (cr.Width() - MulDiv(cr.Height(), theApp.csPageSize.cx, theApp.csPageSize.cy)) / 2;
		pDC->SetViewportOrg(pt);
		pDC->FillSolidRect(0, 0, theApp.csPageSize.cx, theApp.csPageSize.cy, theApp.arrColors[pDoc->param.nPageColor].value);
	}
	cpHInitOffset = CSize(0, -(int)(1000.0f*pDoc->param.f2DSize[1])-400);
	cpHOffset = CSize(0, -(int)(1000.0f*pDoc->param.f1DOffset));
	cpVInitOffset = CSize(400, 0);
	cpVOffset = CSize((int)(1000.0f*pDoc->param.f1DOffset), 0);
	int oldBkMode = pDC->SetBkMode(TRANSPARENT);
	OnDrawCore(pDC, pDoc);
	// MAKE THE SPECTRUM RECTANGLES IN DEVICE COORDINATES FOR CURSOR CONVENIENCE
	if (! pDC->IsPrinting()) {
		if (cpCursor[0].x)
			DrawCursor();
		int i = (int)floorf(1000.0f*pDoc->param.f2DSize[0]+0.5f);
		int j = (int)floorf(1000.0f*pDoc->param.f2DSize[1]+0.5f);
		int h = (int)floorf(1000.0f*pDoc->param.f1DHeight+0.5f);
		scrRect = CRect(0, 0, i, j);
		pDC->LPtoDP(scrRect);
		scrRect.NormalizeRect();
		POSITION pos = pDoc->HOneDSpec.GetTailPosition();
		if (pos) {
			int saveddc = pDC->SaveDC();
			pDC->OffsetWindowOrg(cpHInitOffset.x, cpHInitOffset.y);
			while (pos) {
				COneDSpec *p = pDoc->HOneDSpec.GetPrev(pos);
				p->scrRect = CRect(0, 0, i, h);
				pDC->LPtoDP(p->scrRect);
				p->scrRect.NormalizeRect();
				pDC->OffsetWindowOrg(cpHOffset.x, cpHOffset.y);
			}
			pDC->RestoreDC(saveddc);
		}
		pos = pDoc->VOneDSpec.GetTailPosition();
		if (pos) {
			int saveddc = pDC->SaveDC();
			pDC->OffsetWindowOrg(cpVInitOffset.x, cpVInitOffset.y);
			while (pos) {
				COneDSpec *p = pDoc->VOneDSpec.GetPrev(pos);
				p->scrRect = CRect(-h, 0, 0, j);
				pDC->LPtoDP(p->scrRect);
				p->scrRect.NormalizeRect();
				pDC->OffsetWindowOrg(cpVOffset.x, cpVOffset.y);
			}
			pDC->RestoreDC(saveddc);
		}
		zoomRect = CRect(0,0,0,0);
	}
	pDC->SetBkMode(oldBkMode);
}

void CNetComPlotView::Annotate(CDC *pDC, CNetComPlotDoc *pDoc)
{
	CFont font;
	int y = theApp.csPageSize.cy - 400;
	int x = 400;
	font.CreateFont(300, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Arial");
	CFont *oldFont = pDC->SelectObject(&font);
	int oldBkMode = pDC->SetBkMode(TRANSPARENT);
	COLORREF oldColor = pDC->SetTextColor(theApp.arrColors[pDoc->param.nInfoColor].value);
	POSITION pos = pDoc->TwoDSpec.GetHeadPosition();
	while (pos) {
		CTwoDSpec *s = pDoc->TwoDSpec.GetNext(pos);
		int start = 0;
		CString str = s->strInfo.Tokenize("\r\n", start);
		if (theApp.DataClass.Count()) {
			str += " ";
			if (s->strDataClass.IsEmpty())
				s->strDataClass = "x";
			str += theApp.DataClass.Lookup(s->strDataClass);
		}
		pDC->TextOut(x, y, str);
		y -= 360;
	}
	pDC->SelectObject(oldFont);
	pDC->SetBkMode(oldBkMode);
	pDC->SetTextColor(oldColor);
}

void CNetComPlotView::OnDrawCore(CDC *pDC, CNetComPlotDoc *pDoc)
{
	if (pDoc->TwoDSpec.GetCount() < 1)
		return;
	// Annotate
	if (pDoc->param.iShowInfo)
		Annotate(pDC, pDoc);
	// Calculate the page offset according to the datasets and pagesize
	cp2DOffset = CPoint(theApp.csPageSize.cx/2, theApp.csPageSize.cy/2); // Start in the middle of the page
	cp2DOffset = cp2DOffset - CSize(int(pDoc->param.f2DSize[0]*500.0f), int(pDoc->param.f2DSize[1]*500.0f)); // 2D Spectra
	cp2DOffset = cp2DOffset + CSize(-750, 500);		// Axis
	float x = 0.0f;
	float y = 0.0f;
	int i = int(pDoc->VOneDSpec.GetCount());
	if (i) {
		x = pDoc->param.f1DHeight;
		if (i > 1)
			x += --i * pDoc->param.f1DOffset;
	}
	i = int(pDoc->HOneDSpec.GetCount());
	if (i) {
		y = pDoc->param.f1DHeight;
		if (i > 1)
			y += --i * pDoc->param.f1DOffset;
	}

	cp2DOffset = cp2DOffset + CSize(int(500.0f * x), -int(500.0f * y));
	pDC->OffsetWindowOrg(-cp2DOffset.x, -cp2DOffset.y);

	CAxis axis(pDoc);
	axis.Draw(pDC);

	POSITION pos = pDoc->TwoDSpec.GetTailPosition();
	while (pos) {
		CTwoDSpec *p = pDoc->TwoDSpec.GetPrev(pos);
		if (p->param.mode != negMode) {
			for (int i=0; i<p->param.contour; i++)
				p->arrContour[i] = p->posContour[i];
			CPen pen(PS_SOLID, 0, theApp.arrColors[p->param.poscolor].value);
			CPen *oldPen = pDC->SelectObject(&pen);
			Draw2DContour(pDC, p);
			pDC->SelectObject(oldPen);
		}
		if (p->param.mode != posMode) {
			for (int i=0; i<p->param.contour; i++)
				p->arrContour[i] = p->negContour[i];
			CPen pen(PS_SOLID, 0, theApp.arrColors[p->param.negcolor].value);
			CPen *oldPen = pDC->SelectObject(&pen);
			Draw2DContour(pDC, p);
			pDC->SelectObject(oldPen);
		}
	}
	pos = pDoc->HOneDSpec.GetTailPosition();
	if (pos) {
		int saveddc = pDC->SaveDC();
		pDC->OffsetWindowOrg(cpHInitOffset.x, cpHInitOffset.y);
		while (pos) {
			COneDSpec *p = pDoc->HOneDSpec.GetPrev(pos);
			CPen pen(PS_SOLID, 0, theApp.arrColors[p->param.color].value);
			CPen *oldPen = pDC->SelectObject(&pen);
			Draw1DSpectrum(pDC, p, TRUE);
			pDC->SelectObject(oldPen);
			pDC->OffsetWindowOrg(cpHOffset.x, cpHOffset.y);
		}
		pDC->RestoreDC(saveddc);
	}
	pos = pDoc->VOneDSpec.GetTailPosition();
	if (pos) {
		int saveddc = pDC->SaveDC();
		pDC->OffsetWindowOrg(cpVInitOffset.x, cpVInitOffset.y);
		while (pos) {
			COneDSpec *p = pDoc->VOneDSpec.GetPrev(pos);
			CPen pen(PS_SOLID, 0, theApp.arrColors[p->param.color].value);
			CPen *oldPen = pDC->SelectObject(&pen);
			Draw1DSpectrum(pDC, p, FALSE);
			pDC->SelectObject(oldPen);
			pDC->OffsetWindowOrg(cpVOffset.x, cpVOffset.y);
		}
		pDC->RestoreDC(saveddc);
	}
}


// CNetComPlotView printing

BOOL CNetComPlotView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CNetComPlotView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CNetComPlotView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CNetComPlotView diagnostics

#ifdef _DEBUG
void CNetComPlotView::AssertValid() const
{
	CView::AssertValid();
}

void CNetComPlotView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CNetComPlotDoc* CNetComPlotView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CNetComPlotDoc)));
	return (CNetComPlotDoc*)m_pDocument;
}
#endif //_DEBUG


// CNetComPlotView message handlers

void CNetComPlotView::fillzero()
{
	for (int i=0; i<nbmapsize; i++)
		bmap[i] = 0;
}

void CNetComPlotView::Draw2DContour(CDC *pDC, CTwoDSpec *pspec)
{
	CWaitCursor wait;
	CPoint point;

	// makesure bitmap is big enough
	int reqsize = pspec->param.contour * pspec->HSpecDim.Size() * pspec->VSpecDim.Size()/16+1;
	if (reqsize > nbmapsize) {
		if (bmap) {
			delete[] bmap;
			nbmapsize = 0;
		}
		bmap = new u_long[reqsize];
		if (bmap)
			nbmapsize = reqsize;
	}
	//V.W. Snyder, Algorithm 531 - Contour Plotting
	int iCV, ii, jj, ks;
	int l1[4], l2[4], ij[2];
	int i1[2], i2[2], i3[6];
	float xint[4], xy[2];
	int z1, z2;
	l1[2] = l1[3] = -1;
	i1[0]=1; i1[1]=0; i2[0]=1; i2[1]=-1; i3[0]=i3[3]=i3[4]=1; i3[1]=i3[2]=i3[5]=0;
	int nx = l1[0] = pspec->VSpecDim.to-pspec->VSpecDim.from;
	int ny = l1[1] = pspec->HSpecDim.to-pspec->HSpecDim.from;
	if (nx<4 || ny<4) return;	//TOO FEW DATA POINTS TO PLOT!
	xy[0] = 1.0f;
	xy[1] = 1.0f;
	int icur = max(1,min((int)(xy[0]), l1[0]));
	int jcur = max(1,min((int)(xy[1]), l1[1]));
	fillzero();
	int ibkey = 0;
L10:	ij[0] = icur;
	ij[1] = jcur;
L20:	l2[0] = ij[0];
	l2[2] = 0 - ij[0];
	l2[1] = ij[1];
	l2[3] = 0 - ij[1];
	int idir = 0;
L30:	int nxidir = idir + 1;
	int k = nxidir;
	if (nxidir > 3) nxidir = 0;
L40:	ij[0] = abs(ij[0]);
	ij[1] = abs(ij[1]);
	int l = 1;
L50:	if (ij[l-1] >= l1[l-1]) goto L130;
	int jump = 100;
L60:	int ix = 1;
	if (ij[2-l] == 1) {
		ii = ij[0] + i1[2-l];
		jj = ij[1] + i1[l];
	} else {
		ii = ij[0] + i2[l-1];
		jj = ij[1] + i2[2-l];
		ix = 0;
		if (ij[2-l] >= l1[2-l])
			ix += 2;
	}
	if (jump==280) goto L280;
	if ((3!=ix) && (0!=ix+ibkey)) {
		ii = ij[0] + i1[l-1];
		jj = ij[1] + i1[2-l];
		z1 = pspec->dt[pspec->VSpecDim.from+ij[0]-1][pspec->HSpecDim.from+ij[1]-1];
		z2 = pspec->dt[pspec->VSpecDim.from+ii-1][pspec->HSpecDim.from+jj-1];
		for (iCV=0; iCV<pspec->param.contour; iCV++) {
			int offset = 2*(nx*(ny*iCV+ij[1]-1)+ij[0]-1)+l-1;
			if (!GetBit(offset)) {
				if ((pspec->arrContour[iCV]>min(z1, z2)) && (pspec->arrContour[iCV]<=max(z1, z2)))
					goto L190;
				MarkBitmap(offset);
			}
		}
	}
L130:	if (++l <= 2) goto L50;
	l = idir % 2 + 1;
	if (l1[k-1] >= 0)
		ij[l-1] = abs(ij[l-1]);
	else
		ij[l-1] = 0-abs(ij[l-1]);
	while (1) {
		if (ij[l-1] < l1[k-1]) {
			ij[l-1]++;
			if (ij[l-1] <= l2[k-1]) goto L40;
			l2[k-1] = ij[l-1];
			idir = nxidir;
			goto L30;
		}
		if (idir != nxidir) {
			nxidir++;
			ij[l-1] = l1[k-1];
			k = nxidir;
			l = 3 - l;
			ij[l-1] = l2[k-1];
			if (nxidir > 3) nxidir = 0;
		} else
			break;
	}
	if (ibkey != 0) return;
	ibkey = 1;
	goto L10;
L190:	int iedge = l;
	if (ix != 1) iedge += 2;
	int iflag = 2 + ibkey;
	xint[iedge-1] = (float)(pspec->arrContour[iCV]-z1)/(float)(z2-z1);
L200:	xy[l-1] = (float)(ij[l-1]) + xint[iedge-1];
	xy[2-l] = (float)(ij[2-l]);
	MarkBitmap(2*(nx*(ny*iCV+ij[1]-1)+ij[0]-1)+l-1);
	point = pspec->Translate(xy[1], xy[0]);
	if (iflag == 2 || iflag == 3)
		pDC->MoveTo(point);
	else
		pDC->LineTo(point);
	if (iflag >= 4) {
		icur = ij[0];
		jcur = ij[1];
		goto L20;
	}
	int ni = 1;
	if (iedge >= 3) {
		ij[0] -= i3[iedge-1];
		ij[1] -= i3[iedge+1];
	}
	for (k=1; k<=4; k++) {
		if (k == iedge) continue;
		ii = ij[0] + i3[k-1];
		jj = ij[1] + i3[k];
		z1 = pspec->dt[pspec->VSpecDim.from+ii-1][pspec->HSpecDim.from+jj-1];
		ii = ij[0] + i3[k];
		jj = ij[1] + i3[k+1];
		z2 = pspec->dt[pspec->VSpecDim.from+ii-1][pspec->HSpecDim.from+jj-1];
 		if (pspec->arrContour[iCV] <= min(z1, z2) || pspec->arrContour[iCV] > max(z1, z2)) continue;
		if ((k==1) || (k==4)) {
			int zz = z1;
			z1 = z2;
			z2 = zz;
		}
		xint[k-1] = (float)(pspec->arrContour[iCV]-z1)/(float)(z2-z1);
		ni++;
		ks = k;
	}
	if (ni != 2) {
		ks = 5 - iedge;
		if (xint[2] >= xint[0]) {
			ks = 3 - iedge;
			if (ks <= 0) ks += 4;
		}
	}
	l = ks;
	iflag = 1;
	jump = 280;
	if (ks >= 3) {
		ij[0] += i3[ks-1];
		ij[1] += i3[ks+1];
		l = ks - 2;
	}
	if (! GetBit(2*(nx*(ny*iCV+ij[1]-1)+ij[0]-1)+l-1)) goto L60;
	iflag = 5;
	goto L290;
L280:	if (ix != 0) iflag = 4;
L290:	iedge = ks + 2;
	if (iedge > 4) iedge -= 4;
	xint[iedge-1] = xint[ks-1];
	goto L200;
}

void CNetComPlotView::MarkBitmap(int offset)
{
	u_long bit = (1u << (offset % 32));
	bmap[offset/32] |= bit;
}

BOOL CNetComPlotView::GetBit(int offset)
{
	u_long bit = (1u << (offset % 32));
	bit = bmap[offset/32] & bit;
	return bit!=0u;
}

void CNetComPlotView::Draw1DSpectrum(CDC *pDC, COneDSpec *p, BOOL bIsHoriz)
{
	int i = p->SpecDim.from;
	pDC->MoveTo(p->Tranform(i, p->dt[i], bIsHoriz));
	for (++i; i<=p->SpecDim.to; i++)
		pDC->LineTo(p->Tranform(i, p->dt[i], bIsHoriz));
}

void CNetComPlotView::OnUpdateEditCopy(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDocument()->TwoDSpec.GetCount()>0);
}

void CNetComPlotView::OnEditCopy()
{
	CMetaFileDC mfDC;
	mfDC.CreateEnhanced(NULL, NULL, NULL, "NetComPlot\02D NMR Plot\0\0");
	mfDC.SetMapMode(MM_HIMETRIC);
	OnDrawCore(&mfDC, GetDocument());
	HENHMETAFILE hMF = mfDC.CloseEnhanced();
	if (hMF) {
		if (OpenClipboard()) {
			if (EmptyClipboard()) {
				SetClipboardData(CF_ENHMETAFILE, hMF);
			}
			CloseClipboard();
		}
	}	
}

void CNetComPlotView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (nFlags & MK_LBUTTON && GetDocument()->TwoDSpec.GetCount()) {
		CDC *pDC = GetDC();
		if (pDC) {
			CRect *oldRect = &zoomRect;
			CSize csOne(1,1);
			CRect newRect(zoomStart.x, zoomStart.y, point.x, point.y);
			newRect.NormalizeRect();
			if (! newRect.IsRectEmpty())
				pDC->DrawDragRect(&newRect, csOne, oldRect, csOne);
			zoomRect = newRect;
			ReleaseDC(pDC);
		}
	}
	strHUnits = strVUnits = NULL;
	pHSpecDim = pVSpecDim = NULL;
	if (cpCursor[0].x) {
		DrawCursor();
		cpCursor[0].x = 0;
	}
	if (1 > GetDocument()->TwoDSpec.GetCount())
		return;
	CRect *pRect = NULL;
	CString msg;
	static char *hz = "Hz";
	static char *ppm = "PPM";
	if (scrRect.PtInRect(point)) {
		pRect = &scrRect;
		float fFr, fSW;
		CTwoDSpec *p = GetDocument()->TwoDSpec.GetHead();
		pHSpecDim = & p->HSpecDim;
		pVSpecDim = & p->VSpecDim;
		fFr = GetDocument()->param.fFrom[0];
		fSW = fFr - GetDocument()->param.fTo[0];
		strHUnits = (hertzRange == GetDocument()->param.hRange) ? hz : ppm;
		fHContext = fFr - fSW * ((float)(point.x-scrRect.left)/(float)scrRect.Width());
		fFr = GetDocument()->param.fFrom[1];
		fSW = fFr - GetDocument()->param.fTo[1];
		strVUnits = (hertzRange == GetDocument()->param.vRange) ? hz : ppm;
		fVContext = fFr - fSW * ((float)(scrRect.bottom-point.y)/(float)scrRect.Height());
		msg.Format("F2=%.3f %s F1=%.3f %s", fHContext, strHUnits, fVContext, strVUnits);
	} else {
		POSITION pos = GetDocument()->HOneDSpec.GetTailPosition();
		while (pos) {
			COneDSpec *p = GetDocument()->HOneDSpec.GetPrev(pos);
			if (p->scrRect.PtInRect(point)) {
				pRect = &p->scrRect;
				float fFr, fSW;
				fFr = GetDocument()->param.fFrom[0];
				fSW = fFr - GetDocument()->param.fTo[0];
				fHContext = fFr - fSW * ((float)(point.x-pRect->left)/(float)pRect->Width());
				strHUnits = (hertzRange == GetDocument()->param.hRange) ? hz : ppm;
				msg.Format("%.3f %s", fHContext, strHUnits);
				pHSpecDim = & p->SpecDim;
				break;
			}
		}
		if (NULL == pRect) {
			pos = GetDocument()->VOneDSpec.GetTailPosition();
			while (pos) {
				COneDSpec *p = GetDocument()->VOneDSpec.GetPrev(pos);
				if (p->scrRect.PtInRect(point)) {
					pRect = &p->scrRect;
					float fFr, fSW;
					fFr = GetDocument()->param.fFrom[1];
					fSW = fFr - GetDocument()->param.fTo[1];
					fVContext = fFr - fSW * ((float)(pRect->bottom-point.y)/(float)pRect->Height());
					strVUnits = (hertzRange == GetDocument()->param.vRange) ? hz : ppm;
					msg.Format("%.3f %s", fVContext, strVUnits);
					pVSpecDim = & p->SpecDim;
					break;
				}
			}
		}
	}
	if (pRect) {
		SetCursor(NULL);
		cpCursor[0].y = cpCursor[1].y = point.y;
		cpCursor[0].x = pRect->left;
		cpCursor[1].x = pRect->right;
		cpCursor[2].x = cpCursor[3].x = point.x;
		cpCursor[2].y = pRect->top;
		cpCursor[3].y = pRect->bottom;
		DrawCursor();
	} else
		msg = "";
	theApp.Message(msg);
}

void CNetComPlotView::DrawCursor()
{
	CDC *pDC = GetDC();
	if (pDC) {
		CPen pen(PS_SOLID, 0, RGB(0,255,255));
		int oldDrawMode = pDC->SetROP2( R2_XORPEN );
		CPen *oldPen = pDC->SelectObject(&pen);
		pDC->MoveTo(cpCursor[0]);
		pDC->LineTo(cpCursor[1]);
		pDC->MoveTo(cpCursor[2]);
		pDC->LineTo(cpCursor[3]);
		pDC->SelectObject(oldPen);
		pDC->SetROP2(oldDrawMode);
		ReleaseDC(pDC);
	}
}

void CNetComPlotView::OnSetoffsethere()
{
	BOOL bH = (NULL != pHSpecDim);
	BOOL bV = (NULL != pVSpecDim);
	float fHN, fVN;
	if (bH)
		fHN = (0==strcmp(strHUnits, "PPM")) ? pHSpecDim->PPM2N(fHContext) : pHSpecDim->Hertz2N(fHContext);
	if (bV)
		fVN = (0==strcmp(strVUnits, "PPM")) ? pVSpecDim->PPM2N(fVContext) : pVSpecDim->Hertz2N(fHContext);
	CSetOffsetDlg dlg;
	dlg.m_strHValue.Format("%g", fHContext);
	dlg.m_strVValue.Format("%g", fVContext);
	CString strIniHValue = dlg.m_strHValue;
	CString strIniVValue = dlg.m_strVValue;
	dlg.m_strHUnits = strHUnits;
	dlg.m_strVUnits = strVUnits;
	if (IDOK == dlg.DoModal()) {
		BOOL bChanged = FALSE;
		if (bH && dlg.m_strHValue.Compare(strIniHValue)) {
			bChanged = TRUE;
			float value = (float)atof(dlg.m_strHValue);
			if (0==strcmp(strHUnits, "PPM"))
				value *= pHSpecDim->Frequency();
			pHSpecDim->ChangeOffset(value, fHN);
		}
		if (bV && dlg.m_strVValue.Compare(strIniVValue)) {
			bChanged = TRUE;
			float value = (float)atof(dlg.m_strVValue);
			if (0==strcmp(strVUnits, "PPM"))
				value *= pVSpecDim->Frequency();
			pVSpecDim->ChangeOffset(value, fVN);
		}
		if (bChanged) {
			GetDocument()->SetModifiedFlag();
			GetDocument()->InitAllTransforms();
			Invalidate();
		}
	}
}

void CNetComPlotView::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	CMenu menu;
	if(menu.LoadMenu(IDR_CONTEXTMENU)) {
		CMenu *popup = menu.GetSubMenu(0);
		if (popup) {
			BOOL bShow = (pHSpecDim || pVSpecDim);
			BOOL bFullShow = TRUE;
			popup->EnableMenuItem(ID_VIEW_FULL, bFullShow?MF_ENABLED:MF_GRAYED);
			popup->EnableMenuItem(ID_VIEW_ZOOM, zoomRect.IsRectEmpty()?MF_GRAYED:MF_ENABLED);
			popup->EnableMenuItem(ID_VIEW_PREVIOUS, GetDocument()->History.IsEmpty()?MF_GRAYED:MF_ENABLED);
			popup->EnableMenuItem(ID_SETOFFSETHERE, MF_BYCOMMAND|(bShow?MF_ENABLED:MF_GRAYED));
			popup->EnableMenuItem(ID_VIEW_INTEGRAL, zoomRect.IsRectEmpty()?MF_GRAYED:MF_ENABLED);
			popup->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, this);
		}
	}
}

void CNetComPlotView::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (GetDocument()->TwoDSpec.GetCount()) {
		CRect newRect(point, point);
		if (! zoomRect.IsRectEmpty()) {
			CDC *pDC = GetDC();
			if (pDC) {
				CSize csOne(1,1);
				pDC->DrawDragRect(newRect, csOne, &zoomRect, csOne);
				ReleaseDC(pDC);
			}
		}
		zoomRect = newRect;
		zoomStart = point;
	} else
		zoomRect = CRect(0,0,0,0);
}

void CNetComPlotView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (GetDocument()->TwoDSpec.GetCount()) {
		CRect newRect;
		newRect.IntersectRect(scrRect, zoomRect);
		CDC *pDC = GetDC();
		if (pDC) {
			CSize csOne(1,1);
			pDC->DrawDragRect(newRect, csOne, zoomRect, csOne);
			ReleaseDC(pDC);
		}
		zoomRect = newRect;
	}
}

void CNetComPlotView::OnUpdateViewIntegral(CCmdUI *pCmdUI)
{
	// THERE HAS TO BE A ZOOM BOX AND NEITHER DIMENSION CAN BE IN FULL MODE
	pCmdUI->Enable(! zoomRect.IsRectEmpty());
}

void CNetComPlotView::OnViewIntegral()
{
	float fF, fW, fhStart, fhStop, fvStart, fvStop;
	DOCPARAM &param = GetDocument()->param;
	fF = param.fFrom[0];
	fW = fF - param.fTo[0];
	fhStart = fF - fW * (float)(zoomRect.left-scrRect.left) / (float)scrRect.Width();
	fhStop = fF - fW * (float)(zoomRect.right-scrRect.left) / (float)scrRect.Width();
	fF = param.fFrom[1];
	fW = fF - param.fTo[1];
	fvStart = fF - fW * (float)(scrRect.bottom-zoomRect.bottom) / (float)scrRect.Height();
	fvStop = fF - fW * (float)(scrRect.bottom-zoomRect.top) / (float)scrRect.Height();
	CString istr;
	istr.Format("HStart\t%g\r\nHStop\t%g\r\nVStart\t%g\r\nVStop\t%g\r\n", fhStart, fhStop, fvStart, fvStop);
	POSITION pos = GetDocument()->TwoDSpec.GetHeadPosition();
	while (pos) {
		CTwoDSpec *pSpec = GetDocument()->TwoDSpec.GetNext(pos);
		int ihStart, ihStop, ivStart, ivStop;
		if (hertzRange == param.hRange) {
			ihStart = (int)floor(pSpec->HSpecDim.Hertz2N(fhStart));
			ihStop = (int)ceil(pSpec->HSpecDim.Hertz2N(fhStop));
		} else {
			ihStart = (int)floor(pSpec->HSpecDim.PPM2N(fhStart));
			ihStop = (int)ceil(pSpec->HSpecDim.PPM2N(fhStop));
		}
		if (hertzRange == param.vRange) {
			ivStart = (int)floor(pSpec->VSpecDim.Hertz2N(fvStart));
			ivStop = (int)ceil(pSpec->VSpecDim.Hertz2N(fvStop));
		} else {
			ivStart = (int)floor(pSpec->VSpecDim.PPM2N(fvStart));
			ivStop = (int)ceil(pSpec->VSpecDim.PPM2N(fvStop));
		}
		int count = 0;
		double integral = 0.0;
		for (int j=ivStart; j<=ivStop; j++) {
			if (j>=0 && j<pSpec->VSpecDim.Size()) {
				for (int i=ihStart; i<ihStop; i++) {
					if (i>=0 && i<pSpec->HSpecDim.Size()) {
						count++;
						integral += (double)pSpec->dt[j][i];
					}
				}
			}
		}
		CString intstr;
		integral *= 1.0e-6;
		if (GetDocument()->TwoDSpec.GetCount() > 1) {
			int i = 0;
			intstr.Format("%s(%d)\t%g\r\n", pSpec->strInfo.Tokenize("\r\n", i), count, integral);
		} else {
			intstr.Format("#Points\t%d\r\n", count);
			istr += intstr;
			intstr.Format("Integral\t%g\r\n", integral);
		}
		istr += intstr;
	}
	CIntegralEdit dlg;
	dlg.m_integralEdit = istr;
	dlg.DoModal();
}

void CNetComPlotView::OnUpdateViewZoom(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(! zoomRect.IsRectEmpty());
}

void CNetComPlotView::OnViewZoom()
{
	float fF, fW, fhStart, fhStop, fvStart, fvStop;
	CTwoDSpec *p = GetDocument()->TwoDSpec.GetHead();
	DOCPARAM &param = GetDocument()->param;
	fF = param.fFrom[0];
	fW = fF - param.fTo[0];
	fhStart = fF - fW*(float)(zoomRect.left-scrRect.left)/(float)scrRect.Width();
	fhStop = fF - fW*(float)(zoomRect.right-scrRect.left)/(float)scrRect.Width();
	float w;
	if (hertzRange == param.hRange)
		w = p->HSpecDim.Hertz2N(fhStop) - p->HSpecDim.Hertz2N(fhStart);
	else
		w = p->HSpecDim.PPM2N(fhStop) - p->HSpecDim.PPM2N(fhStart);
	if (w < 5.0f) {
		AfxMessageBox("Horizontal zoom region is too small!", MB_OK|MB_ICONERROR);
		return;
	}

	fF = param.fFrom[1];
	fW = fF - param.fTo[1];
	fvStart = fF - fW*(float)(scrRect.bottom-zoomRect.bottom)/(float)scrRect.Height();
	fvStop = fF - fW*(float)(scrRect.bottom-zoomRect.top)/(float)scrRect.Height();
	if (hertzRange == param.vRange)
		w = p->VSpecDim.Hertz2N(fvStop) - p->VSpecDim.Hertz2N(fvStart);
	else
		w = p->VSpecDim.PPM2N(fvStop) - p->VSpecDim.PPM2N(fvStart);
	if (w < 5.0f) {
		AfxMessageBox("Vertical zoom region is too small!", MB_OK|MB_ICONERROR);
		return;
	}
	
	// SAVE THE REGION IN HISTORY
	DOCPARAM *newparam = new DOCPARAM;
	memcpy(newparam, &param, sizeof(DOCPARAM));
	GetDocument()->History.AddHead(newparam);

	param.fFrom[0] = fhStart;
	param.fTo[0] = fhStop;
	param.fFrom[1] = fvStart;
	param.fTo[1] = fvStop;
	GetDocument()->InitAllTransforms();
	GetDocument()->SetModifiedFlag();
	Invalidate();
}

void CNetComPlotView::OnUpdateViewFull(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(true);
}

void CNetComPlotView::OnViewFull()
{
	CNetComPlotDoc *pDoc = GetDocument();
	DOCPARAM par;
	memcpy(&par, &pDoc->param, sizeof(DOCPARAM));
	if (fullRange == pDoc->param.hRange)
		pDoc->param.hRange = ppmRange;
	pDoc->SetFullHRange(pDoc->param);
	if (fullRange == pDoc->param.vRange)
		pDoc->param.vRange = ppmRange;
	pDoc->SetFullVRange(pDoc->param);
	if (par.fFrom[0]!=pDoc->param.fFrom[0] || par.fFrom[1]!=pDoc->param.fFrom[1] ||
		par.fTo[0]!=pDoc->param.fTo[0] || par.fTo[1]!=pDoc->param.fTo[1] ||
		par.hRange!=pDoc->param.hRange || par.vRange!=pDoc->param.vRange) {
			DOCPARAM *newparam = new DOCPARAM;
			memcpy(newparam, &par, sizeof(DOCPARAM));
			pDoc->History.AddHead(newparam);
	}
	pDoc->InitAllTransforms();
	pDoc->SetModifiedFlag();
	Invalidate();
}
void CNetComPlotView::OnUpdateViewPrevious(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(! GetDocument()->History.IsEmpty());
}

void CNetComPlotView::OnViewPrevious()
{
	CNetComPlotDoc *pDoc = GetDocument();
	DOCPARAM *p = pDoc->History.RemoveHead();
	memcpy(&pDoc->param, p, sizeof(DOCPARAM));
	delete p;
	pDoc->InitAllTransforms();
	pDoc->SetModifiedFlag();
	Invalidate();
}

void CNetComPlotView::OnViewLand()
{
	CNetComPlotDoc *pDoc = GetDocument();
	pDoc->param.dmorient = DMORIENT_LANDSCAPE;
	pDoc->SetModifiedFlag();
	Invalidate();
}

void CNetComPlotView::OnViewPort()
{
	CNetComPlotDoc *pDoc = GetDocument();
	pDoc->param.dmorient = DMORIENT_PORTRAIT;
	pDoc->SetModifiedFlag();
	Invalidate();
}

void CNetComPlotView::OnUpdateViewLand(CCmdUI *pCmdUI)
{
	CNetComPlotDoc *pDoc = GetDocument();
	pCmdUI->SetCheck(pDoc->param.dmorient==DMORIENT_LANDSCAPE ? 1 : 0);
}
void CNetComPlotView::OnUpdateViewPort(CCmdUI *pCmdUI)
{
	CNetComPlotDoc *pDoc = GetDocument();
	pCmdUI->SetCheck(pDoc->param.dmorient==DMORIENT_PORTRAIT ? 1 : 0);
}
void CNetComPlotView::OnViewInformation()
{
	CNetComPlotDoc *pDoc = GetDocument();
	pDoc->param.iShowInfo = pDoc->param.iShowInfo ? 0 : 1;
	Invalidate();
}

void CNetComPlotView::OnUpdateViewInformation(CCmdUI *pCmdUI)
{
	CNetComPlotDoc *pDoc = GetDocument();
	pCmdUI->SetCheck(pDoc->param.iShowInfo);
}

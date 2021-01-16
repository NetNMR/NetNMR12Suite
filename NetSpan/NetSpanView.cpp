// NetSpanView.cpp : implementation of the CNetSpanView class
//

#include "stdafx.h"
#include "NetSpan.h"

#include "NetSpanDoc.h"
#include "NetSpanView.h"
#include "MainFrame.h"
#include "TabbedDlg.h"
#include "AddSpectraDlg.h"
#include "LSquare.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CNetSpanView

IMPLEMENT_DYNCREATE(CNetSpanView, CView)

BEGIN_MESSAGE_MAP(CNetSpanView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_COMMAND(ID_FILE_ADDSPECTRA, &CNetSpanView::OnFileAddspectra)
	ON_COMMAND(ID_EDIT_COPY, &CNetSpanView::OnEditCopy)
	ON_COMMAND(ID_CONTEXT_NEWLINEHERE, &CNetSpanView::OnContextNewlinehere)
	ON_COMMAND(ID_CONTEXT_ZOOM, &CNetSpanView::OnContextZoom)
	ON_COMMAND(ID_CONTEXT_FULL, &CNetSpanView::OnContextFull)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &CNetSpanView::OnUpdateEditCopy)
	ON_WM_MOUSEMOVE()
	ON_WM_CONTEXTMENU()
	ON_WM_KILLFOCUS()
	ON_WM_NCMOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CNetSpanView construction/destruction

CNetSpanView::CNetSpanView()
{
	myView = viewAll;
	for (int i=0; i<4; i++)
		cpCursor[i] = CPoint(0,0);
	zoomCursor.left = zoomCursor.right = -1;
	pRect = NULL;
	serr = 0.0;
	labelFont.CreateFont(1000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Times New Roman");
	cursorPen.CreatePen(PS_SOLID, 0,
		theApp.arrColors[theApp.RegistryInt(ReadInt, BkgrColor, 0)].value^RGB(255,0,0));
	zoomPen.CreatePen(PS_SOLID, 0,
		theApp.arrColors[theApp.RegistryInt(ReadInt, BkgrColor, 0)].value^RGB(0,0,255));
}

CNetSpanView::~CNetSpanView()
{
	labelFont.DeleteObject();
	cursorPen.DeleteObject();
	zoomPen.DeleteObject();
	textList.RemoveAll();
}

BOOL CNetSpanView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

bool CNetSpanView::ShowSpectrum(int index)
{
	bool retvalue = false;
	int count = GetDocument()->cols;
	if (count <= 8)
		return true;
	switch (myView) {
		case viewAll:
			retvalue = true;
			break;
		case viewTop:
			if (count-index <= 8)
				retvalue = true;
			break;
		case viewBottom:
			if (index < 8)
				retvalue = true;
			break;
		case viewSpaced:
			int skip = count / 8;
			if (0==index%skip && index/skip<8)
				retvalue = true;
			break;
	}
	return retvalue;
}

// CNetSpanView drawing

void CNetSpanView::OnDraw(CDC* pDC)
{
	CPen calcPen, diffPen, specPen, singPen;
	calcPen.CreatePen(PS_SOLID, 0, theApp.arrColors[theApp.RegistryInt(ReadInt, CalcColor, 8)].value);
	diffPen.CreatePen(PS_SOLID, 0, theApp.arrColors[theApp.RegistryInt(ReadInt, DiffColor, 1)].value);
	specPen.CreatePen(PS_SOLID, 0, theApp.arrColors[theApp.RegistryInt(ReadInt, SpecColor, 2)].value);
	singPen.CreatePen(PS_SOLID, 0, theApp.arrColors[theApp.RegistryInt(ReadInt, SingColor, 6)].value);
	CNetSpanDoc *pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc->cols < 1) return;
	COLORREF oldColor = pDC->SetTextColor(theApp.arrColors[theApp.RegistryInt(ReadInt, LablColor, 7)].value);
	CFont *oldFont = pDC->SelectObject( &labelFont );
	CPen *oldPen = pDC->SelectObject(&specPen);
	int w = 24000;
	int h = pDoc->cols;
	if (h>8 && myView!=viewAll)
		h = 8;
	h = 24000/h;
	int p = pDoc->to - pDoc->from + 1;
	double ymin = pDoc->ymin;
	double range = pDoc->ymax - ymin;
	POSITION pos = pDoc->Spectra.GetHeadPosition();
	pDC->SetBkMode(TRANSPARENT);

	int count = 0;
	while (pos) {
		CSpectrum *s = pDoc->Spectra.GetNext(pos);
		if (ShowSpectrum(count++)) {
			s->scrRect = CRect(0, 0, w, h);
			pDC->LPtoDP(s->scrRect);
			s->scrRect.NormalizeRect();
			pDC->TextOut(0, h, s->label);
			pDC->MoveTo(0, (int)((double)h * (s->obs[pDoc->from] - ymin) / range));
			for (int i=pDoc->from+1; i<=pDoc->to; i++)
				pDC->LineTo(MulDiv(i-pDoc->from, w, p), (int)((double)h * (double)(s->obs[i] - ymin) / range));
			if (theApp.pCurrentLine) {
				pDC->SelectObject(&singPen);
				pDC->MoveTo(0, (int)((double)h * (s->singleline[pDoc->from] - ymin) / range));
				for (int i=pDoc->from+1; i<=pDoc->to; i++)
					pDC->LineTo(MulDiv(i-pDoc->from, w, p), (int)((double)h * (double)(s->singleline[i] - ymin) / range));
			}
			pDC->SelectObject(&calcPen);
			pDC->MoveTo(0, (int)((double)h * (s->calc[pDoc->from] - ymin) / range));
			for (int i=pDoc->from+1; i<=pDoc->to; i++)
				pDC->LineTo(MulDiv(i-pDoc->from, w, p), (int)((double)h * (double)(s->calc[i] - ymin) / range));
			pDC->SelectObject(&diffPen);
			pDC->MoveTo(0, (int)((double)h * (s->diff[pDoc->from] - ymin) / range));
			for (int i=pDoc->from+1; i<=pDoc->to; i++)
				pDC->LineTo(MulDiv(i-pDoc->from, w, p), (int)((double)h * (double)(s->diff[i] - ymin) / range));
			pDC->SelectObject(&specPen);
			pDC->OffsetWindowOrg(0, -h);
		}
	}

	if (zoomCursor.left > -1) {
		pDC->SetWindowOrg(windOrg);
		pDC->SelectObject(zoomPen);
		int oldmode = pDC->SetROP2(R2_XORPEN);
		pDC->MoveTo(zoomCursor.left, 0);
		pDC->LineTo(zoomCursor.left, 24000);
		pDC->MoveTo(zoomCursor.right, 0);
		pDC->LineTo(zoomCursor.right, 24000);
		pDC->SetROP2(oldmode);
	}
	pDC->SelectObject(oldFont);
	pDC->SelectObject(oldPen);
	pDC->SetTextColor(oldColor);
	specPen.DeleteObject();
	diffPen.DeleteObject();
	calcPen.DeleteObject();
	if (! pDC->IsPrinting()) {
		if (cpCursor[0].x)
			DrawCursor();
	}
}


// CNetSpanView printing

BOOL CNetSpanView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	pInfo->SetMaxPage(1);
	return DoPreparePrinting(pInfo);
}

void CNetSpanView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	prlabelFont.CreateFont(100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Times New Roman");
}

void CNetSpanView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	prlabelFont.DeleteObject();
}


// CNetSpanView diagnostics

#ifdef _DEBUG
void CNetSpanView::AssertValid() const
{
	CView::AssertValid();
}

void CNetSpanView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CNetSpanDoc* CNetSpanView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CNetSpanDoc)));
	return (CNetSpanDoc*)m_pDocument;
}
#endif //_DEBUG


// CNetSpanView message handlers

void CNetSpanView::DrawCursor(void)
{
	CDC *pDC = GetDC();
	if (pDC && pRect) {
		int oldDrawMode = pDC->SetROP2(R2_XORPEN);
		CPen *oldPen = pDC->SelectObject(&cursorPen);
		pDC->MoveTo(cpCursor[0]);
		pDC->LineTo(cpCursor[1]);
		pDC->MoveTo(cpCursor[2]);
		pDC->LineTo(cpCursor[3]);
		pDC->SelectObject(oldPen);
		pDC->SetROP2(oldDrawMode);
		ReleaseDC(pDC);
	}
}

void CNetSpanView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (cpCursor[0].x)
		DrawCursor();
	pRect = NULL;
	POSITION pos = GetDocument()->Spectra.GetHeadPosition();
	while (pos) {
		CSpectrum *pSpec = GetDocument()->Spectra.GetNext(pos);
		if (pSpec->scrRect.PtInRect(point)) {
			if (GetDocument()->cols > 1)
				theApp.SetPaneText(1, pSpec->label);
			pRect = &pSpec->scrRect;
			break;
		}
	}
	if (pRect) {
		CRect scrRect;
		GetClientRect(scrRect);
		SetCursor(NULL);
		cpCursor[0].x = pRect->left;
		cpCursor[0].y = cpCursor[1].y = point.y;
		cpCursor[1].x = pRect->right;
		cpCursor[2].x = cpCursor[3].x = point.x;
		cpCursor[2].y = scrRect.top;
		cpCursor[3].y = scrRect.bottom;
		DrawCursor();
		index = MulDiv(GetDocument()->to-GetDocument()->from, point.x-pRect->left, pRect->Width());
		index += GetDocument()->from;
		CString str;
		str.Format("%.3f PPM", GetDocument()->ppm[index]);
		theApp.SetPaneText(0, str);
		
	} else {
		cpCursor[0].x = 0;
		theApp.SetPaneText(1, "LABEL");
		theApp.SetPaneText(0, "");
	}
}

void CNetSpanView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if (cpCursor[0].x) {
		CMenu menu;
		if(menu.LoadMenu(IDR_CONTEXTMENU)) {
			CMenu *popup = menu.GetSubMenu(0);
			popup->EnableMenuItem(ID_CONTEXT_ZOOM, CanZoom());
			popup->EnableMenuItem(ID_CONTEXT_FULL, CanFull());
			if (popup) {
				popup->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, pWnd);
			}
		}
	}
}

void CNetSpanView::OnContextNewlinehere()
{
	// "index" IS WHERE THE CURSOR IS
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, theApp.m_pMainWnd);
	CNetSpanDoc *pDoc = GetDocument();
	// GET A POINTER TO THE SPECTRUM WITH THE LARGEST diff INTENSITY AT CURSOR
	POSITION pos = pDoc->Spectra.GetHeadPosition();
	CSpectrum *pMax = pDoc->Spectra.GetNext(pos);
	while (pos) {
		CSpectrum *pSpec = pDoc->Spectra.GetNext(pos);
		if (fabs(pSpec->diff[index]) > fabs(pMax->diff[index]))
			pMax = pSpec;
	}
	// LOOK FOR WHERE diff HAS DROPPED TO 0.5 OF MAX
	int offset = 0;
	do {
		if (index-offset<0 || index+offset>=pDoc->rows)
			break;
		if (index-++offset >= 0) {
			if (fabs(pMax->diff[index-offset]) < 0.5*fabs(pMax->diff[index]))
				break;
		}
		if (index+offset < pDoc->rows) {
			if (fabs(pMax->diff[index+offset]) < 0.5*fabs(pMax->diff[index]))
				break;
		}
	} while (offset);
	double width = pDoc->xnorm[index+offset]-pDoc->xnorm[index-offset];
	
	pDoc->SetModifiedFlag();
	// Assign a default name
	CString newname;
	for (int i=1; newname.IsEmpty(); i++) {
		newname.Format("Line %d", i);
		pos = pDoc->Lines.GetHeadPosition();
		while (pos) {
			CLine *pTest = pDoc->Lines.GetNext(pos);
			if (0 == newname.Compare(pTest->name)) {
				newname.Empty();
				break;
			}
		}
	}
	CLine *newLine = new CLine(0.5, width, pDoc->xnorm[index], pDoc->rows, pDoc, newname);
	// PUT IT IN CHEMICAL SHIFT ORDER
	pos = pDoc->Lines.GetHeadPosition();
	while (pos) {
		CLine *pLine = pDoc->Lines.GetAt(pos);
		if (newLine->GetShift() < pLine->GetShift()) {
			pDoc->Lines.InsertBefore(pos, newLine);
			break;
		}
		pDoc->Lines.GetNext(pos);
	}
	if (! pos)
		pDoc->Lines.AddTail(newLine);
	// Set intensities for each spectrum
	pos = pDoc->Spectra.GetHeadPosition();
	while (pos) {
		CSpectrum *pSpec = pDoc->Spectra.GetNext(pos);
		pSpec->intensity[newLine->id] = pSpec->diff[index];
	}
	pDoc->ReCalculate();
	Invalidate();
	pMain->tabDlg->ShowAllPages(TRUE);
}

void CNetSpanView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	CNetSpanDoc *pDoc = GetDocument();
	CView::OnPrepareDC(pDC, pInfo);
	if (pDC->IsPrinting()) {
		int w = pDC->GetDeviceCaps(HORZRES);
		int h = pDC->GetDeviceCaps(VERTRES);
		pDC->SetMapMode(MM_ISOTROPIC);
		pDC->SetWindowExt(w, h);
		pDC->SetViewportOrg(w/10, MulDiv(h, 15, 16));
	} else {
		pDC->SetMapMode(MM_ANISOTROPIC);
		windExt = CSize(26000, 26000);
		pDC->SetWindowExt(windExt);
		windOrg = CPoint(-1000, -1000);
		pDC->SetWindowOrg(windOrg);
		CRect cr;
		GetClientRect(cr);
		viewExt = CSize(cr.Width(), -cr.Height());
		pDC->SetViewportExt(viewExt);
		viewOrg = CPoint(0, cr.Height());
		pDC->SetViewportOrg(viewOrg);
	}
	pDC->SetBkMode(TRANSPARENT);
}

void CNetSpanView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	CPen calcPen, diffPen, specPen;
	calcPen.CreatePen(PS_SOLID, 0, theApp.arrColors[theApp.RegistryInt(ReadInt, CalcColor, 8)].value);
	diffPen.CreatePen(PS_SOLID, 0, theApp.arrColors[theApp.RegistryInt(ReadInt, DiffColor, 1)].value);
	specPen.CreatePen(PS_SOLID, 0, theApp.arrColors[theApp.RegistryInt(ReadInt, SpecColor, 2)].value);
	CNetSpanDoc *pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc->cols < 1) return;
	COLORREF oldColor = pDC->SetTextColor(theApp.arrColors[theApp.GetProfileInt("Settings", "AxisColor", 0)].value);
	CFont *oldFont = pDC->SelectObject(&prlabelFont);
	CPen *oldPen = pDC->SelectObject(&specPen);
	int w = MulDiv(pDC->GetDeviceCaps(HORZRES), 7, 8);
	int h = MulDiv(pDC->GetDeviceCaps(VERTRES), 7, 8*pDoc->cols);
	int p = pDoc->to - pDoc->from + 1;
	double ymin = pDoc->ymin;
	double range = pDoc->ymax - ymin;
	POSITION pos = pDoc->Spectra.GetHeadPosition();
	while (pos) {
		CSpectrum *s = pDoc->Spectra.GetNext(pos);
		pDC->TextOut(0, h, s->label);
		pDC->MoveTo(0, (int)((double)h * (s->obs[pDoc->from] - ymin) / range));
		for (int i=pDoc->from+1; i<=pDoc->to; i++)
			pDC->LineTo(MulDiv(i-pDoc->from, w, p), (int)((double)h * (double)(s->obs[i] - ymin) / range));
		pDC->SelectObject(&calcPen);
		pDC->MoveTo(0, (int)((double)h * (s->calc[pDoc->from] - ymin) / range));
		for (int i=pDoc->from+1; i<=pDoc->to; i++)
			pDC->LineTo(MulDiv(i-pDoc->from, w, p), (int)((double)h * (double)(s->calc[i] - ymin) / range));
		pDC->SelectObject(&diffPen);
		pDC->MoveTo(0, (int)((double)h * (s->diff[pDoc->from] - ymin) / range));
		for (int i=pDoc->from+1; i<=pDoc->to; i++)
			pDC->LineTo(MulDiv(i-pDoc->from, w, p), (int)((double)h * (double)(s->diff[i] - ymin) / range));
		pDC->SelectObject(&specPen);
		pDC->OffsetWindowOrg(0, -h);
	}
	pDC->SelectObject(oldFont);
	pDC->SelectObject(oldPen);
	pDC->SetTextColor(oldColor);
	calcPen.DeleteObject();
	specPen.DeleteObject();
	diffPen.DeleteObject();
}

int CNetSpanView::FillText()
{
	int length = 0;
	CNetSpanDoc *pDoc = GetDocument();
	if (0==pDoc->Spectra.GetCount() || 0==pDoc->Lines.GetCount())
		return 0;
	// CLEAR THE LIST
	textList.RemoveAll();
	// BUILD AND SAVE FIRST ROW
	CString str = "#";
	if (! pDoc->sfstr.IsEmpty())
		str += ' ' + pDoc->sfstr;
	if (! pDoc->gammastr.IsEmpty())
		str += ' ' + pDoc->gammastr;
	if (! pDoc->tcubedstr.IsEmpty())
		str += ' ' + pDoc->tcubedstr;
	if (! pDoc->dactogstr.IsEmpty())
		str += ' ' + pDoc->dactogstr;
	str.Trim();
	if (1 == str.GetLength())
		str = pDoc->GetTitle();
	POSITION pos = pDoc->Spectra.GetHeadPosition();
	while (pos) {
		CSpectrum *pSpec = pDoc->Spectra.GetNext(pos);
		str += '\t' + pSpec->label;
	}
	if (pDoc->sf==1.0)
		str += "\tf-Loren\tWidth(PPM)\tShift\r\n";
	else
		str += "\tf-Loren\tWidth(Hz)\tShift\r\n";
	length += str.GetLength();
	textList.AddTail(str);
	// BUILD AND SAVE EACH ROW, INCLUDING SHAPE, WIDTH and SHIFT
	pos = pDoc->Lines.GetHeadPosition();
	while (pos) {
		CString val;
		CLine *pLine = pDoc->Lines.GetNext(pos);
		str = pLine->name;
		POSITION spos = pDoc->Spectra.GetHeadPosition();
		while (spos) {
			CSpectrum *pSpec = pDoc->Spectra.GetNext(spos);
			val.Format("\t%g", pDoc->ynorm*pSpec->intensity[pLine->id]);
			str += val;
		}
		val.Format("\t%g\t%g\t%g\r\n", pLine->GetShape(), pLine->GetWidth()*pDoc->sf*pDoc->xsw, pDoc->xoffset-pDoc->xsw*pLine->GetShift());
		str += val;
		textList.AddTail(str);
		length += str.GetLength();
	}
	// BUILD AND SAVE INTEGRALS FOR EACH LINE AT EACH VALUE
	//pos = pDoc->Lines.GetHeadPosition();
	//while (pos) {
	//	CString val;
	//	CLine *pLine = pDoc->Lines.GetNext(pos);
	//	str = "Integral of ";
	//	str += pLine->name;
	//	POSITION spos = pDoc->Spectra.GetHeadPosition();
	//	while (spos) {
	//		CSpectrum *pSpec = pDoc->Spectra.GetNext(spos);
	//		val.Format("\t%g", pDoc->ynorm * pSpec->intensity[pLine->id]
	//			* pDoc->sf * pDoc->xsw * pLine->GetWidth()
	//			* (1.0 + pLine->GetShape() * 0.47566462663560588938881769));
	//		str += val;
	//	}
	//	str += "\r\n";
	//	textList.AddTail(str);
	//	length += str.GetLength();
	//}
	// THE INTEGRALS WILL BE EXPRESSED AS A FORMULA OF INTENSTY, WIDTH and SHAPE
	int nl = int(pDoc->Lines.GetCount());
	for (int i=1; i<=nl; i++) {
		str.Format("=\"Integral of \" & a%d", i+1);
		int ns = int(pDoc->Spectra.GetCount());
		for (int j=1; j<=ns; j++) {
			CString val, c1, c2, c3;
			ExcelColumn(c1, j+1);
			ExcelColumn(c2, ns+3);
			ExcelColumn(c3, ns+2);
			val.Format("\t=%s%d*%s%d*(1+%s%d*(sqrt(pi()*ln(2))-1))", c1, i+1, c2, i+1, c3, i+1);
			str += val;
		}
		str += "\r\n";
		textList.AddTail(str);
		length += str.GetLength();
	}
	return length;
}

void CNetSpanView::ExcelColumn(CString &str, int col)
{
	str.Empty();
	int i = -1;
	while (col > 26) {
		i++;
		col -= 26;
	}
	if (i >= 0)
		str.Format("%c%c", 'a'+i, 'a'+col-1);
	else
		str.Format("%c", 'a'+col-1);
}

void CNetSpanView::EraseCursor()
{
	if (cpCursor[0].x) {
		DrawCursor();
		cpCursor[0].x = 0;
	}
}

void CNetSpanView::OnEditCopy()
{
	CMetaFileDC mfDC;
	mfDC.CreateEnhanced(NULL, NULL, NULL, "NetSpan\0NetSpan StackPlot\0\0");
	mfDC.SetMapMode(MM_HIMETRIC);
	CClientDC clientDC(this);
	mfDC.SetAttribDC(clientDC.m_hDC);
	OnDraw(&mfDC);
	mfDC.ReleaseAttribDC();
	HENHMETAFILE hMF = mfDC.CloseEnhanced();
	if (hMF) {
		if (OpenClipboard()) {
			if (EmptyClipboard()) {
				SetClipboardData(CF_ENHMETAFILE, hMF);
				int size = FillText() + 1;
				if (size) {
					GLOBALHANDLE hgText = GlobalAlloc(GHND, size);
					if (hgText) {
						char *pgmem = (char *)GlobalLock(hgText);
						if (pgmem) {
							while (textList.GetCount()) {
								CString line = textList.RemoveHead();
								strcat_s(pgmem, size, line);
							}
							GlobalUnlock(hgText);
							SetClipboardData(CF_TEXT, hgText);
						}
					}
				}
			}
			CloseClipboard();
		}
	}
	Invalidate();
}

void CNetSpanView::OnUpdateEditCopy(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDocument()->Spectra.GetCount()>0 && GetDocument()->Lines.GetCount()>0);
}

void CNetSpanView::OnFileAddspectra()
{
	CAddSpectraDlg dlg;
	if (IDOK == dlg.DoModal()) {
		if (dlg.Spectra.GetCount()) {
			GetDocument()->SetModifiedFlag();
			CNetSpanDoc *pDoc = GetDocument();
			pDoc->sumsq = 0.0;
			if (pDoc->rows) {
				POSITION pos = dlg.Spectra.GetHeadPosition();
				while (pos) {
					CSpectrum *pSpec = dlg.Spectra.GetNext(pos);
					for (int i=0; i<dlg.np; i++)
						pSpec->obs[i] /= pDoc->ynorm;
				}
			} else {
				pDoc->rows = dlg.np;
				pDoc->ppm.SetSize(dlg.np);
				pDoc->xnorm.SetSize(dlg.np);
				pDoc->xoffset = dlg.offset;
				pDoc->xsw = dlg.sw;
				pDoc->sf = dlg.sf;
				pDoc->from = 0;
				pDoc->to = dlg.np - 1;
				for (int i=0; i<dlg.np; i++) {
					pDoc->xnorm[i] = double(i)/double(pDoc->to);
					pDoc->ppm[i] = dlg.offset - dlg.sw * pDoc->xnorm[i];
				}
				POSITION pos = dlg.Spectra.GetHeadPosition();
				CSpectrum *pSpec = dlg.Spectra.GetNext(pos);
				pSpec->FindMinMax(pDoc->from, pDoc->to);
				double ymax = pSpec->GetMax();
				double ymin = pSpec->GetMin();
				while (pos) {
					pSpec = dlg.Spectra.GetNext(pos);
					pSpec->FindMinMax(pDoc->from, pDoc->to);
					if (pSpec->GetMax() > ymax)
						ymax = pSpec->GetMax();
					if (pSpec->GetMin() < ymin)
						ymin = pSpec->GetMin();
				}
				pDoc->ynorm = (fabs(ymin) > fabs(ymax)) ? fabs(ymin) : fabs(ymax);
				pos = dlg.Spectra.GetHeadPosition();
				while (pos) {
					CSpectrum *pSpec = dlg.Spectra.GetNext(pos);
					for (int i=0; i<dlg.np; i++)
						pSpec->obs[i] /= pDoc->ynorm;
				}
			}
			while (dlg.Spectra.GetCount()) {
				CSpectrum *pSpec = dlg.Spectra.RemoveHead();
				POSITION pos = pDoc->Lines.GetHeadPosition();
				while (pos) {
					CLine *pLine = pDoc->Lines.GetNext(pos);
					pSpec->eintensity[pLine->id] = pSpec->intensity[pLine->id] = 0.0;
				}
				pDoc->Spectra.AddTail(pSpec);
				pDoc->cols = (int)pDoc->Spectra.GetCount();
			}
			CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, theApp.m_pMainWnd);
			pMain->RefillSpecList();
			pDoc->FindMinMax();
			pDoc->ReCalculate();
			if (pDoc->Lines.GetCount()) {
				CLSquare lsquare;
				if (lsquare.Create(false))
					lsquare.Optimize(0.01, 0, false);
			}
			Invalidate();
		}
	}
}

void CNetSpanView::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (GetDocument()->cols*GetDocument()->rows) {
		CDC *pDC = GetDC();
		pDC->SetMapMode(MM_ANISOTROPIC);
		pDC->SetWindowExt(windExt);
		pDC->SetWindowOrg(windOrg);
		pDC->SetViewportExt(viewExt);
		pDC->SetViewportOrg(viewOrg);
		pDC->DPtoLP(&point);
		CPen *oldPen = pDC->SelectObject(&zoomPen);
		int oldMode = pDC->SetROP2(R2_XORPEN);
		if (zoomCursor.right >= 0) {
			pDC->MoveTo(zoomCursor.right, 0);
			pDC->LineTo(zoomCursor.right, 24000);
			zoomCursor.right = -1;
		}
		if (zoomCursor.left >= 0) {
			pDC->MoveTo(zoomCursor.left, 0);
			pDC->LineTo(zoomCursor.left, 24000);
		}
		zoomCursor.left = point.x;
		if (zoomCursor.left < 0)
			zoomCursor.left = 0;
		else if (zoomCursor.left > 24000)
			zoomCursor.left = 24000;
		pDC->MoveTo(zoomCursor.left, 0);
		pDC->LineTo(zoomCursor.left, 24000);
		pDC->SetROP2(oldMode);
		pDC->SelectObject(oldPen);
		ReleaseDC(pDC);
		SetCapture();
	}
	CView::OnLButtonDown(nFlags, point);
}

void CNetSpanView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (zoomCursor.left >= 0 && zoomCursor.right == -1) {
		CDC *pDC = GetDC();
		pDC->SetMapMode(MM_ANISOTROPIC);
		pDC->SetWindowExt(windExt);
		pDC->SetWindowOrg(windOrg);
		pDC->SetViewportExt(viewExt);
		pDC->SetViewportOrg(viewOrg);
		pDC->DPtoLP(&point);
		CPen *oldPen = pDC->SelectObject(&zoomPen);
		int oldMode = pDC->SetROP2(R2_XORPEN);
		zoomCursor.right = point.x;
		if (zoomCursor.right < 0)
			zoomCursor.right = 0;
		else if (zoomCursor.right > 24000)
			zoomCursor.right = 20000;
		pDC->MoveTo(zoomCursor.right, 0);
		pDC->LineTo(zoomCursor.right, 24000);
		if (zoomCursor.left == zoomCursor.right)
			zoomCursor.left = zoomCursor.right = -1;
		else if (zoomCursor.left > zoomCursor.right) {
			int i = zoomCursor.left;
			zoomCursor.left = zoomCursor.right;
			zoomCursor.right = i;
		}
		pDC->SetROP2(oldMode);
		pDC->SelectObject(oldPen);
		ReleaseDC(pDC);
		ReleaseCapture();
	}	
	CView::OnLButtonUp(nFlags, point);
}

void CNetSpanView::ClearZoomCursor()
{
	zoomCursor.left = zoomCursor.right = -1;
}

void CNetSpanView::OnContextZoom()
{
	CNetSpanDoc *pDoc = GetDocument();
	int sw = pDoc->to - pDoc->from + 1;
	pDoc->to = pDoc->from + MulDiv(sw, zoomCursor.right, 24000);
	pDoc->from += MulDiv(sw, zoomCursor.left, 24000);
	ClearZoomCursor();
	pDoc->ReCalculate();
	pDoc->FindMinMax();
	Invalidate(TRUE);
}

UINT CNetSpanView::CanZoom()
{
	if (zoomCursor.left<0)
		return MF_GRAYED;
	CNetSpanDoc *pDoc = GetDocument();
	double sw = pDoc->ppm[pDoc->from] - pDoc->ppm[pDoc->to];
	double left = pDoc->ppm[pDoc->from] - sw * zoomCursor.left / 24000;
	double right = pDoc->ppm[pDoc->from] - sw * zoomCursor.right / 24000;
	POSITION pos = pDoc->Lines.GetHeadPosition();
	while (pos) {
		CLine *pLine = pDoc->Lines.GetNext(pos);
		double shift = pDoc->xoffset - pDoc->xsw * pLine->GetShift();
		if (shift>left || right>shift)
			return MF_GRAYED;
	}
	return MF_ENABLED;
}

void CNetSpanView::OnContextFull()
{
	ClearZoomCursor();
	CNetSpanDoc *pDoc = GetDocument();
	pDoc->from = 0;
	pDoc->to = pDoc->rows - 1;
	pDoc->ReCalculate();
	pDoc->FindMinMax();
	Invalidate(TRUE);
}

UINT CNetSpanView::CanFull()
{
	CNetSpanDoc *pDoc = GetDocument();
	return (pDoc->from==0 && pDoc->to==pDoc->rows-1) ? MF_GRAYED : MF_ENABLED;
}

BOOL CNetSpanView::OnEraseBkgnd(CDC* pDC)
{
    // Set brush to desired background color.
    CBrush backBrush(theApp.arrColors[theApp.RegistryInt(ReadInt, BkgrColor, 0)].value);
    // Save old brush.
    CBrush* pOldBrush = pDC->SelectObject(&backBrush);
    CRect rect;
    pDC->GetClipBox(&rect);     // Erase the area needed.
    pDC->PatBlt(rect.left, rect.top, rect.Width(), 
    rect.Height(), PATCOPY);
    pDC->SelectObject(pOldBrush);
	return true; //CView::OnEraseBkgnd(pDC);
}
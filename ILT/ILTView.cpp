// ILTView.cpp : implementation of the CILTView class
//

#include "stdafx.h"
#include "ILT.h"

#include "Decay.h"
#include "ILTDoc.h"
#include "ILTView.h"
#include "FitTextDlg.h"
#include "SelectDecayDlg.h"
#include "jama_lu.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CILTView

IMPLEMENT_DYNCREATE(CILTView, CView)

BEGIN_MESSAGE_MAP(CILTView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_VIEW_TRANSFORM, &CILTView::OnViewTransform)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TRANSFORM, &CILTView::OnUpdateViewTransform)
	ON_COMMAND(ID_VIEW_TEXT, &CILTView::OnViewText)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TEXT, &CILTView::OnUpdateViewText)
	ON_COMMAND(ID_EDIT_COPY, &CILTView::OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &CILTView::OnUpdateEditCopy)
	ON_COMMAND(ID_VIEW_EXPONENTIAL, &CILTView::OnViewExponential)
	ON_UPDATE_COMMAND_UI(ID_VIEW_EXPONENTIAL, &CILTView::OnUpdateViewExponential)
	ON_COMMAND(ID_VIEW_NEXT, &CILTView::OnViewNext)
	ON_UPDATE_COMMAND_UI(ID_VIEW_NEXT, &CILTView::OnUpdateViewNext)
	ON_COMMAND(ID_VIEW_PREVIOUS, &CILTView::OnViewPrevious)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PREVIOUS, &CILTView::OnUpdateViewPrevious)
	ON_COMMAND_RANGE(ID_CONFIDENCEINTERVAL_1, ID_CONFIDENCEINTERVAL_6, &CILTView::OnConfidence)
	ON_UPDATE_COMMAND_UI_RANGE(ID_CONFIDENCEINTERVAL_1, ID_CONFIDENCEINTERVAL_6, &CILTView::OnUpdateConfidenceInterval)
	ON_COMMAND_RANGE(ID_CONFIDENCEINTERVAL_SIMPLE, ID_CONFIDENCEINTERVAL_JOINT, &CILTView::OnCIType)
	ON_UPDATE_COMMAND_UI_RANGE(ID_CONFIDENCEINTERVAL_SIMPLE, ID_CONFIDENCEINTERVAL_JOINT, &CILTView::OnUpdateCIType)
END_MESSAGE_MAP()

// CILTView construction/destruction

CILTView::CILTView()
: viewDecay(true)
, viewSimpleCI(true)
{
	for (int i=0; i<128; i++) {
		double theta = M_PI * double(i) / 64.0;
		costheta[i] = cos(theta);
		sintheta[i] = sin(theta);
	}
}

CILTView::~CILTView()
{
}

BOOL CILTView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CILTView drawing

void CILTView::DrawAxes(CDC *pDC, double xmin, double xmax, double ymin, double ymax, LPCTSTR label)
{
	CILTDoc *pDoc = GetDocument();
	// modify DC
	CPen axisPen(PS_SOLID, 1, theApp.Colors[6]);
	CPen *origPen = pDC->SelectObject(&axisPen);
	CFont axisFont;
	axisFont.CreateFont(400, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Arial");
	CFont *origFont = pDC->SelectObject(&axisFont);
	COLORREF origColor = pDC->SetTextColor(theApp.Colors[6]);
	int origBkMode = pDC->SetBkMode(TRANSPARENT);

	// time axis first
	const int yoff = -100;
	// baseline
	pDC->MoveTo(0, yoff);
	pDC->LineTo(15000, yoff);
	// tic marks
	int expon = int(log10(xmin));
	double mant = 1.0;
	while (xmin < mant * pow(10.0, expon))
		expon--;
	while (true) {
		double curtic = mant * pow(10.0, expon);
		if (curtic > xmax)
			break;
		if (curtic >= xmin) {
			int x = int(15000.0 * log(curtic/xmin) / log(xmax/xmin));
			pDC->MoveTo(x, yoff);
			if (1.0 == mant) {
				pDC->LineTo(x, yoff-200);
				// big tic mark labels
				CString val;
				val.Format("%g", pow(10.0, expon));
				CRect rect(x-100, yoff-220, x+100, yoff-440);
				pDC->DrawText(val, &rect, DT_TOP|DT_CENTER|DT_NOCLIP);
			} else
				pDC->LineTo(x, yoff-100);
		}
		if (8.5 > mant)
			mant += 1.0;
		else {
			mant = 1.0;
			expon++;
		}
	}
	// label
	CString strLabel;
	strLabel.Format("%s", label);
	CRect rect(7000, yoff-500, 8000, yoff-1000);
	pDC->DrawText(strLabel, &rect, DT_TOP|DT_CENTER|DT_NOCLIP);

	// vertical axis next
	// baseline
	pDC->MoveTo(yoff, 0);
	pDC->LineTo(yoff, 10000);
	// tic marks
	double inc = 0.25 * (ymax - ymin);
	expon = int(floor(log10(inc)));
	inc /= pow(10.0, expon);
	int ilabel;
	if (inc < 2.0) {
		inc = 1.0;
		ilabel = 2;
	} else if (inc < 5.0) {
		inc = 2.0;
		ilabel = 2;
	} else {
		inc = 5.0;
		ilabel = 5;
	}
	// inc * pow(10,expon) is the spacing between major tic marks
	// we'll start on a major tic mark
	double starty = ceil(ymax / (inc * pow(10.0, expon)));
	starty *= inc*pow(10.0, expon);
	// change the increment to the minor tic mark spacing
	if (1.0 == inc)
		inc = 0.5 * pow(10.0, expon);
	else
		inc = pow(10.0, expon);
	int places = 0;
	if (ilabel*inc < 1.0)
		places = int(-floor(log10(ilabel*inc)));
	int i = 0;
	double currenty = starty;
	while (currenty >= ymin) {	// as long as we haven't gone past ymin
		if (currenty<=ymax) {	// make the tic mark if the currenty is in range
			int y = int(10000.0*(currenty-ymin)/(ymax-ymin));
			pDC->MoveTo(yoff, y);
			if (i % ilabel)
				pDC->LineTo(yoff-100, y);
			else {
				pDC->LineTo(yoff-200, y);
				if (places) {
					CString fstr;
					fstr.Format("%%.%df", places);
					strLabel.Format(fstr, currenty);
				} else
					strLabel.Format("%g", currenty);
				CRect r(yoff-1000, y+400, yoff-300, y-400);
				pDC->DrawText(strLabel, &r, DT_SINGLELINE|DT_VCENTER|DT_RIGHT|DT_NOCLIP);
			}
		}
		currenty = starty - double(++i) * inc;
		if (-1.0e-14<currenty && currenty<1.0e-14)
			currenty = 0.0;
	}

	// restore DC
	pDC->SelectObject(origPen);
	pDC->SelectObject(origFont);
	pDC->SetTextColor(origColor);
	pDC->SetBkMode(origBkMode);
}

void CILTView::OnDraw(CDC* pDC)
{
	CILTDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	if (pDoc->Decays.IsEmpty())
		return;
	if (NULL == theApp.pDecay)
		theApp.pDecay = pDoc->Decays.GetHeadPosition();
	if (pDoc->Decays.GetAt(theApp.pDecay)->Terms.IsEmpty())
		viewDecay = true;
	CRect rect;
	GetClientRect(&rect);
	pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetWindowExt(20000, 15000);
	pDC->SetWindowOrg(-2500, -2500);
	pDC->SetViewportExt(rect.Width(), -rect.Height());
	//if (3*rect.Width() > 4*rect.Height())
	//	pDC->SetViewportOrg((rect.Width()-MulDiv(rect.Height(), 4, 3))/2, rect.Height());
	//else
	//	pDC->SetViewportOrg(0, rect.Height()-(rect.Height()-MulDiv(rect.Width(), 3, 4))/2);
	pDC->SetViewportOrg(0, rect.Height());
	OnDrawCore(pDC);
}

void CILTView::OnDrawCore(CDC *pDC)
{
	CILTDoc *pDoc = GetDocument();
	if (NULL == pDoc)
		return;
	CDecay *pDecay = pDoc->Decays.GetAt(theApp.pDecay);

	bool noMC = true;
	for (int i=0; i<10; i++) {
		if (0 < pDecay->mcterms[i].GetCount()) {
			noMC = false;
			break;
		}
	}

	if (viewDecay) {
		if (pDoc->Xvalue[0] <= 0.0)
			return;

		// draw left and bottom axes
		int n = pDoc->Xvalue.dim();
		double xmin = pDoc->Xvalue[0];
		double xmax = pDoc->Xvalue[n - 1];
		double ydiff = pDecay->Diff();
		double ymin  = pDecay->Min();
		double ymax  = pDecay->Max();
		if (typeDosy == pDoc->kernelType)
			DrawAxes(pDC, xmin, xmax, ymin, ymax, "Grad. Area * 10^10");
		else
			DrawAxes(pDC, xmin, xmax, ymin, ymax, pDoc->strTime);

		// difference, calculated and observe data
		CPen dataPen(PS_SOLID, 0, theApp.Colors[0]);
		CPen calcPen(PS_SOLID, 0, theApp.Colors[1]);
		CPen diffPen(PS_SOLID, 0, theApp.Colors[2]);
		CFont axisFont;
		axisFont.CreateFont(400, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Arial");
		CFont *origFont = pDC->SelectObject(&axisFont);
		CPen *origPen = pDC->SelectObject(&dataPen);
		COLORREF origColor = pDC->SetTextColor(theApp.Colors[2]);
		int origBkMode = pDC->SetBkMode(TRANSPARENT);
		if (pDecay->IsCalculated()) {
			// difference (right) axis
			double diffdiff = pDecay->DiffDiff();
			double diffmin = pDecay->DiffMin();
			double diffmax = pDecay->DiffMax();
			pDC->SelectObject(&diffPen);
			pDC->MoveTo(15100, 0);
			pDC->LineTo(15100, 10000);
			double inc = 0.25 * diffdiff;
			int expon = int(floor(log10(inc)));
			inc /= pow(10.0, expon);
			int ilabel;
			if (inc < 2.0) {
				inc = 1.0;
				ilabel = 2;
			} else if (inc  < 5.0) {
				inc = 2.0;
				ilabel = 2;
			} else {
				inc = 5.0;
				ilabel = 5;
			}
			double starty = ceil(diffmax / (inc * pow(10.0, expon)));
			starty *= inc * pow(10.0, expon);
			if (1.0 == inc)
				inc = 0.5 * pow(10.0, expon);
			else
				inc = pow(10.0, expon);
			int places = 0;
			if (ilabel*inc < 1.0)
				places = int(-floor(log10(ilabel*inc)));
			int i = 0;
			double currenty = starty;
			while (currenty >= diffmin) {
				if (currenty <= diffmax) {
					int y = int(10000.0 * (currenty-diffmin)/diffdiff);
					pDC->MoveTo(15100, y);
					if (i % ilabel)
						pDC->LineTo(15200, y);
					else {
						pDC->LineTo(15300, y);
						CString label;
						if (places) {
							CString fstr;
							fstr.Format("%%.%df", places);
							label.Format(fstr, currenty);
						} else
							label.Format("%g", currenty);
						CRect r(15400, y+400, 16500, y-400);
						pDC->DrawText(label, &r, DT_SINGLELINE|DT_VCENTER|DT_LEFT|DT_NOCLIP);
					}
				}
				currenty = starty - double(++i) * inc;
				if (-1.0e-14<currenty && currenty<1.0e-14)
					currenty = 0.0;
			}

			// difference points
			for (int i=0; i<n; i++) {
				int x = int(15000.0 * log(pDoc->Xvalue[i]/xmin) / log(xmax/xmin));
				int y = int(10000.0 * (pDecay->GetDiff(i) - diffmin) / diffdiff);
				pDC->MoveTo(x, y-50);
				pDC->LineTo(x, y+50);
				pDC->MoveTo(x-50, y);
				pDC->LineTo(x+50, y);
			}
			// calculated line
			pDC->SelectObject(&calcPen);
			pDC->MoveTo(0, int(10000.0 * (pDecay->GetCalc(0) - ymin) / ydiff));
			for (int i=30; i<=15000; i+=30) {
				double xvalue = xmin * pow(xmax/xmin, double(i)/15000.0);
				int y = int(10000.0 * (pDecay->CalcValue(xvalue) - ymin) / ydiff);
				pDC->LineTo(i, y);
			}
		}
		// observed data
		pDC->SelectObject(&dataPen);
		for (int i=0; i<n; i++) {
			int x = int(15000.0 * log(pDoc->Xvalue[i]/xmin) / log(xmax/xmin));
			int y = int(10000.0 * (pDecay->GetAt(i) - ymin) / ydiff);
			pDC->MoveTo(x, y-50);
			pDC->LineTo(x, y+50);
			pDC->MoveTo(x-50, y);
			pDC->LineTo(x+50, y);
		}
		pDC->SelectObject(origPen);
		pDC->SelectObject(origFont);
		pDC->SetTextColor(origColor);
		pDC->SetBkMode(origBkMode);
	} else {
		// density plot
		LPCTSTR title;
		switch (pDoc->kernelType) {
			case typeDosy:
				title = "D * 10^10";
				break;
			case typeT2:
				title = "T2";
				break;
			case typeT1:
				title = "T1";
				break;
		}

		if (noMC) {
			// find Xvalue min and max
			double xmin = pDoc->dblFrom;
			double xmax = pDoc->dblTo;
			double ltt = log(xmax / xmin);

			// initialize amax
			double amax = 0.0;

			int nrows = 0;
			Array2D<double> ellipses;

			if (pDecay->bOptimize) {
				// storage for the ellipses
				nrows = 2 * (int(pDecay->Terms.GetCount()) - 1);
				ellipses = Array2D<double>(nrows, 128);

				// build all the ellipses
				POSITION pos = pDecay->Terms.GetHeadPosition();
				for (int i=0; i<nrows; i+=2) {
					CTerm *term = pDecay->Terms.GetNext(pos);
					if (term->la) {
						double a, b, cosp, sinp;

						// calculate the major and minor axis and the angle of the ellipse
						if (viewSimpleCI) {
							a = sqrt(ChiSq(true) * term->la);
							b = sqrt(ChiSq(true) * term->lc);
							cosp = 1.0;
							sinp = 0.0;
						} else {
							Array2D<double> C(2, 2);
							C[0][0] = term->la;
							C[1][1] = term->lc;
							C[0][1] = C[1][0] = term->lb;
							LU<double> lu(C);
							if (lu.isNonsingular()) {
								Array2D<double> I(2, 2);
								I[0][0] = I[1][1] = 1.0;
								I[1][0] = I[0][1] = 0.0;
								Array2D<double> invC = lu.solve(I);
								double la = invC[0][0];
								double lb = invC[0][1];
								double lc = invC[1][1];
								double phi = atan(2.0 * lb / (lc - la));
								cosp = double(cos(phi));
								sinp = double(sin(phi));
								double tmp = sqrt(1.0 + 4.0 * lb * lb / pow(la - lc, 2));
								a = sqrt(-ChiSq(false) * 2.0 / ((lc - la) * tmp - (la + lc)));
								b = sqrt(-ChiSq(false) * 2.0 / ((la - lc) * tmp - (la + lc)));
							} else {
								// this shouldn't happen!
								a = b = cosp = sinp = 0.0;
							}
						}

						// calculate the a and r of the ellipses
						for (int j=0; j<128; j++) {
							double amp = term->a + a * sintheta[j] * cosp + b * costheta[j] * sinp;
							double r   = term->r + b * costheta[j] * cosp - a * sintheta[j] * sinp;
							if (amp > amax)
								amax = amp;
							ellipses[i][j] = amp;
							// take care of the bad case where r <= 0
							if (typeDosy == pDoc->kernelType) {
								if (r <= 0.0)
									r = 0.01 * xmin;
							} else {
								if (r <= 0.0)
									r = 100.0 * xmax;
								else
									r = 1.0 / r;
							}
							//if (r > xmax)
							//	xmax = r;
							//else if (r < xmin)
							//	xmin = r;
							ellipses[i+1][j] = r;
						}
					}
				}
			} else {
				POSITION pos = pDecay->Terms.GetHeadPosition();
				while (pos) {
					CTerm *term = pDecay->Terms.GetNext(pos);
					if (term->a > amax)
						amax = term->a;
				}
			}
			//for (int j=0; j<128; j++)
			//	for (int i=0; i<nrows; i++)
			//		TRACE2("%g%c", ellipses[i][j], (i==nrows-1)?'\n':'\t');

			// finally ready to draw the axes
			DrawAxes(pDC, xmin, xmax, 0.0, amax, title);

			// and the terms...
			CPen ciPen(PS_SOLID, 10, theApp.Colors[4]);
			CPen *origPen = pDC->SelectObject(&ciPen);
			CBrush eBrush(theApp.Colors[4]);
			CBrush *origBrush = pDC->SelectObject(&eBrush);
			int oldFillMode = pDC->SetPolyFillMode(WINDING);

			// first draw the error ellipses
			if (pDecay->bOptimize) {
				for (int i=0; i<nrows; i+=2) {
					CPoint ellipse[128];
					for (int j=0; j<128; j++) {
						ellipse[j].x = int(15000.0 * log(ellipses[i+1][j] / xmin) / ltt);
						if (ellipse[j].x > 15000)
							ellipse[j].x = 15000;
						else if (ellipse[j].x < 0)
							ellipse[j].x = 0;
						ellipse[j].y = int(10000.0 * ellipses[i][j] / amax);
						if (ellipse[j].y > 10000)
							ellipse[j].y = 10000;
						else if (ellipse[j].y < 0)
							ellipse[j].y = 0;
					}
					pDC->Polygon(ellipse, 128);
				}
			}

			// then draw the delta function terms
			CPen specPen(PS_SOLID|PS_ENDCAP_FLAT, 30, theApp.Colors[3]);
			pDC->SelectObject(&specPen);
			POSITION pos = pDecay->Terms.GetHeadPosition();
			while (pos) {
				CTerm *term = pDecay->Terms.GetNext(pos);
				if (pos) {
					double r = term->r;
					if (typeDosy != pDoc->kernelType)
						r = 1.0 / r;
					int x = int(15000.0 * log(r / xmin) / ltt);
					int y = int(10000.0 * term->a / amax);
					pDC->MoveTo(x, 0);
					pDC->LineTo(x, y);
				}
			}
			pDC->SelectObject(origPen);
			pDC->SelectObject(origBrush);
			pDC->SetPolyFillMode(oldFillMode);
		} else {
			// Monte Carlo simulation
			// draw most abundent terms first
			
			// calculate abundance for each model
			int size[10];
			for (int i=0; i<10; i++)
				size[i] = int(pDecay->mcterms[i].GetCount())/(i+1);
			int max;

			do {	// while there are models that haven't been drawn

				// find the most most abundent set
				max = -1;
				for (int i=0; i<10; i++) {
					if (size[i] > 0) {
						if (max >= 0) {
							if (size[i] > size[max])
								max = i;
						} else
							max = i;
					}
				}
				if (max >= 0) {
					// draw the most abundent set
					//TRACE1("max=%d\n", max);
					POSITION pos = pDecay->mcterms[max].GetHeadPosition();
					while (pos) {
						SMCTerm *term = pDecay->mcterms[max].GetNext(pos);
						pDC->SetPixelV(term->x.i, term->y.i, theApp.Colors[8+max]);
					}
					size[max] = 0;	// and zero the set so that it won't be drawn again
				}
			} while (max >= 0);
			DrawAxes(pDC, pDoc->dblFrom, pDoc->dblTo, 0.0, double(pDoc->mcymax), title);
		}
	}

	// now do the text at the top of the graph
	CFont textFont;
	textFont.CreateFont(300, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Arial");
	CFont *origFont = pDC->SelectObject(&textFont);
	COLORREF origColor = pDC->SetTextColor(theApp.Colors[5]);
	int origBkMode = pDC->SetBkMode(TRANSPARENT);
	CString str;
	str.Format("Exponential: %s", pDecay->strTitle);
	pDC->TextOut(0, 11000, str);
	double ssq, rssq;
	pDecay->GetStats(ssq, rssq);
	str.Format("Sum of Sq. = %g", ssq);
	pDC->TextOut(0, 10600, str);

	if (! pDecay->Terms.IsEmpty() && noMC) {
		str.Format("Resid. Sum of Sq. = %g", rssq);
		pDC->TextOut(5000, 11000, str);
		str.Format("R-Squared = %g", 1.0 - rssq/ssq);
		pDC->TextOut(5000, 10600, str);
		int nterms = 2 * int(pDecay->Terms.GetCount()) - 1;
		int nobs = pDoc->Xvalue.dim();
		if (nobs > nterms) {
			str.Format("Std. Error = %g", sqrt(rssq/(nobs-nterms)));
			pDC->TextOut(10000, 11000, str);
		}
	}
	pDC->SetBkMode(origBkMode);
	pDC->SetTextColor(origColor);
	pDC->SelectObject(origFont);
}


// CILTView printing

BOOL CILTView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CILTView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CILTView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CILTView diagnostics

#ifdef _DEBUG
void CILTView::AssertValid() const
{
	CView::AssertValid();
}

void CILTView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CILTDoc* CILTView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CILTDoc)));
	return (CILTDoc*)m_pDocument;
}
#endif //_DEBUG


// CILTView message handlers

BOOL CILTView::OnEraseBkgnd(CDC* pDC)
{
    // Set brush to desired background color.
    CBrush backBrush(theApp.Colors[7]);
    // Save old brush.
    CBrush* pOldBrush = pDC->SelectObject(&backBrush);
    CRect rect;
    pDC->GetClipBox(&rect);     // Erase the area needed.
    pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
    pDC->SelectObject(pOldBrush);
	return true; //CView::OnEraseBkgnd(pDC);
}

void CILTView::OnViewTransform()
{
	viewDecay = ! viewDecay;
	Invalidate();
}

void CILTView::OnUpdateViewTransform(CCmdUI *pCmdUI)
{
	if (theApp.pDecay) {
		if (GetDocument()->Decays.GetAt(theApp.pDecay)->Terms.IsEmpty()) {
			pCmdUI->SetCheck(FALSE);
			pCmdUI->Enable(FALSE);
		} else {
			pCmdUI->SetCheck(viewDecay?FALSE:TRUE);
			pCmdUI->Enable(TRUE);
		}
	} else {
		pCmdUI->SetCheck(FALSE);
		pCmdUI->Enable(FALSE);
	}
}

void CILTView::OnViewText()
{
	CFitTextDlg dlg;
	dlg.chisq = ChiSq(true);
	dlg.DoModal();
}

void CILTView::OnUpdateViewText(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDocument()->Decays.IsEmpty()?FALSE:TRUE);
}

void CILTView::OnEditCopy()
{
	CMetaFileDC mfDC;
	mfDC.CreateEnhanced(NULL, NULL, NULL, "ILT\0Inverse Laplace Transform\0\0");
	mfDC.SetMapMode(MM_HIMETRIC);
	OnDrawCore(&mfDC);
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

void CILTView::OnUpdateEditCopy(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDocument()->Decays.IsEmpty()?FALSE:TRUE);
}

void CILTView::OnViewExponential()
{
	CSelectDecayDlg dlg;
	dlg.DoModal();
}

void CILTView::OnUpdateViewExponential(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDocument()->Decays.GetCount() > 1);
}

void CILTView::OnViewNext()
{
	GetDocument()->Decays.GetNext(theApp.pDecay);
	Invalidate();
}

void CILTView::OnUpdateViewNext(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(theApp.pDecay!=GetDocument()->Decays.GetTailPosition());
}

void CILTView::OnViewPrevious()
{
	GetDocument()->Decays.GetPrev(theApp.pDecay);
	Invalidate();
}

void CILTView::OnUpdateViewPrevious(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(theApp.pDecay!=GetDocument()->Decays.GetHeadPosition());
}

void CILTView::OnConfidence(UINT uID)
{
	GetDocument()->iConfidenceInterval = uID - ID_CONFIDENCEINTERVAL_1;
	if (! viewDecay)
		Invalidate(TRUE);
}

void CILTView::OnUpdateConfidenceInterval(CCmdUI *pCmdUI)
{
	int ci = GetDocument()->iConfidenceInterval;
	pCmdUI->SetCheck((ci+ID_CONFIDENCEINTERVAL_1==pCmdUI->m_nID) ? 1 : 0);
}

void CILTView::OnCIType(UINT uID)
{
	viewSimpleCI = (uID == ID_CONFIDENCEINTERVAL_SIMPLE);
	if (! viewDecay)
		Invalidate();
}

void CILTView::OnUpdateCIType(CCmdUI *pCmdUI)
{
	int ans;
	if (pCmdUI->m_nID == ID_CONFIDENCEINTERVAL_SIMPLE)
		ans = viewSimpleCI ? 1 : 0;
	else
		ans = viewSimpleCI ? 0 : 1;
	pCmdUI->SetCheck(ans);
}

double CILTView::ChiSq(bool bIsSimple)
{
	if (bIsSimple) {
		switch(GetDocument()->iConfidenceInterval) {
			default: return 1.0742;
			case 1: return 2.7056;
			case 2: return 3.8415;
			case 3: return 6.635;
			case 4: return 10.8285;
			case 5: return 15.145;
		}
	} else {
		switch (GetDocument()->iConfidenceInterval) {
			default: return 2.4079;
			case 1: return 4.6052;
			case 2: return 5.9915;
			case 3: return 9.2106;
			case 4: return 13.8169;
			case 5: return 18.4247;
		}
	}
}
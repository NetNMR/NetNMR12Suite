#include "StdAfx.h"
#include "NetSpan.h"
#include "lsquare.h"
#include "MainFrame.h"
#include "NetSpanView.h"
#include "NetSpanDoc.h"


CLSquare::CLSquare(void)
: nspec(0)
, nline(0)
, ndim(0)
, prtdrv(NULL)
, b(NULL)
, borig(NULL)
, indx(NULL)
{
	pMain = STATIC_DOWNCAST(CMainFrame, theApp.m_pMainWnd);
	pView = STATIC_DOWNCAST(CNetSpanView, pMain->GetActiveView());
	pDoc = STATIC_DOWNCAST(CNetSpanDoc, pMain->GetActiveDocument());
}

CLSquare::~CLSquare(void)
{
	if (prtdrv)
		delete []prtdrv;
	if (b)
		delete []b;
	if (borig)
		delete []borig;
	if (indx)
		delete []indx;
}

BOOL CLSquare::Create(bool fullset)
{
	nspec = (int)pDoc->Spectra.GetCount();
	nline = (int)pDoc->Lines.GetCount();
	if (0==nspec || 0==nline) return FALSE;
	ndim = nspec*pDoc->nbase + nspec*nline;
	if (fullset) {
		POSITION pos = pDoc->Lines.GetHeadPosition();
		while (pos) {
			CLine *pLine = pDoc->Lines.GetNext(pos);
			ndim += pLine->ParCount();
		}
	}
	if (ndim >= nspec * (pDoc->to - pDoc->from + 1)) {
		AfxMessageBox("Parameters exceed points!", MB_ICONEXCLAMATION);
		return FALSE;
	}
	if (! a.SetSize(ndim, ndim) || ! aorig.SetSize(ndim, ndim))
		return FALSE;
	prtdrv = new double[ndim];
	b = new double[ndim];
	borig = new double[ndim];
	indx = new int[ndim];
	if (!prtdrv || !b || !borig || !indx) return FALSE;
	return TRUE;
}

void CLSquare::Build(bool fullset)
{
	a = 0.0;
	for (int i=0; i<ndim; i++)
		b[i] = 0.0;
	POSITION posa = pDoc->Spectra.GetHeadPosition();
	// THIS IS CARRIED OUT FOR EVERY POINT OF DISPLAYED DATA
	// SO WE LOOP OVER EACH SPECTRUM ...
	while (posa) {
		CSpectrum *pSpeca = pDoc->Spectra.GetNext(posa);
		// AND EVERY DISPLAYED POINT IN THAT SPECTRUM!
		for (int i=pDoc->from; i<=pDoc->to; i++) {
			// NOW CALCULATE EACH PARTIAL DERIVATIVE FOR EVERY PARAMETER
			int index = 0;
			// FOR EACH SPECTRUM
			POSITION posb = pDoc->Spectra.GetHeadPosition();
			while (posb) {
				CSpectrum *pSpecb = pDoc->Spectra.GetNext(posb);
				// nbase BASELINE PARAMETERS
				prtdrv[index] = (pSpeca==pSpecb) ? 1.0 : 0.0;
				b[index] += prtdrv[index]*pSpeca->diff[i];
				if (pDoc->nbase > 1) {
					double w = (double)i/(double)pDoc->rows;
					prtdrv[index+1] = prtdrv[index] * w;
					b[index+1] += prtdrv[index+1]*pSpeca->diff[i];
					if (pDoc->nbase > 2) {
						prtdrv[index+2] = prtdrv[index+1] * w;
						b[index+2] += prtdrv[index+2]*pSpeca->diff[i];
						if (pDoc->nbase > 3) {
							prtdrv[index+3] = prtdrv[index+2] * w;
							b[index+3] += prtdrv[index+3]*pSpeca->diff[i];
						}
					}
				}
				index += pDoc->nbase;
				// nline INTENSITES
				POSITION lpos = pDoc->Lines.GetHeadPosition();
				while (lpos) {
					CLine *pLine = pDoc->Lines.GetNext(lpos);
					prtdrv[index] = (pSpeca==pSpecb) ? pLine->line[i] : 0.0;
					b[index] += prtdrv[index]*pSpeca->diff[i];
					index++;
				}
			}
			// FOR EACH LINE: SHAPE, WIDTH AND SHIFT
			if (fullset) {
				posb = pDoc->Lines.GetHeadPosition();
				while (posb) {
					CLine *pLine = pDoc->Lines.GetNext(posb);
					double d = pDoc->xnorm[i] - pLine->GetShift();
					double w = pLine->GetWidth();
					double x = 2.0 * d / w;
					double y = x * x;
					double y1 = y + 1.0;
					double y2 = y1 * y1;
					double z = x / w;
					double fl = pLine->GetShape();
					if (pLine->bshape) {
						prtdrv[index] = pSpeca->intensity[pLine->id]*(1.0/y1-exp(-0.693147180559945309417*y));
						b[index] += prtdrv[index]*pSpeca->diff[i];
						index++;
					}
					if (pLine->bwidth) {
						prtdrv[index] = pSpeca->intensity[pLine->id]*
							((fl*4.0*x*d/(w*w*y2))+
							(1.0-fl)*(4.0*0.693147180559945309417*x*d*exp(-0.693147180559945309417*y)/(w*w)));
						b[index] += prtdrv[index]*pSpeca->diff[i];
						index++;
					}
					if (pLine->bshift) {
						prtdrv[index] = pSpeca->intensity[pLine->id]*
							((fl*4.0*x/(w*y2))+
							(1.0-fl)*(4.0*0.693147180559945309417*x*exp(-0.693147180559945309417*y)/w));
						b[index] += prtdrv[index]*pSpeca->diff[i];
						index++;
					}
				}
			}
			ASSERT(index==ndim);
			for (int r=0; r<ndim; r++) {
				for (int c=r; c<ndim; c++)
					if (prtdrv[r] && prtdrv[c])
						a[r][c] += prtdrv[r]*prtdrv[c];
			}
		}
	}
	for (int i=1; i<ndim; i++) {
		for (int j=0; j<i; j++)
			a[i][j] = a[j][i];
	}
}

void CLSquare::Evaluate(bool fullset)
{
	int index = 0;
	POSITION pos = pDoc->Spectra.GetHeadPosition();
	while (pos) {
		CSpectrum *pSpec = pDoc->Spectra.GetNext(pos);
		double dc, slope, curve, cube;
		pSpec->GetBaseline(dc, slope, curve, cube);
		dc += b[index++];
		if (pDoc->nbase > 1) {
			slope += b[index++];
			if (pDoc->nbase > 2) {
				curve += b[index++];
				if (pDoc->nbase > 3) {
					cube += b[index++];
				}
			}
		}
		pSpec->SetBaseline(dc, slope, curve, cube);
		POSITION lpos = pDoc->Lines.GetHeadPosition();
		while (lpos) {
			CLine *pLine = pDoc->Lines.GetNext(lpos);
			pSpec->intensity[pLine->id] += b[index++];
		}
	}
	int diminish = 0;
	if (fullset) {
		double xinc = (pDoc->xnorm[pDoc->to] - pDoc->xnorm[pDoc->from]) / 50.0;
		pos = pDoc->Lines.GetHeadPosition();
		while (pos) {
			CLine *pLine = pDoc->Lines.GetNext(pos);
			if (pLine->bshape) {
				double newshape = b[index++];
				if (newshape > 0.05)
					newshape = 0.05;
				else if (newshape < -0.05)
					newshape = -0.05;
				newshape += pLine->GetShape();
				if (newshape != pLine->ChangeShape(newshape)) {
					pLine->bshape = FALSE;
					diminish++;
				}
			}
			if (pLine->bwidth) {
				double newwidth = b[index++];
				if (newwidth > xinc)
					newwidth = xinc;
				else if (newwidth < -xinc)
					newwidth = -xinc;
				newwidth += pLine->GetWidth();
				if (newwidth != pLine->ChangeWidth(newwidth)) {
					pLine->bwidth = FALSE;
					diminish++;
				}
			}
			if (pLine->bshift) {
				double newshift = b[index++];
				if (newshift > xinc)
					newshift = xinc;
				else if (newshift < -xinc)
					newshift = -xinc;
				newshift += pLine->GetShift();
				if (newshift != pLine->ChangeShift(newshift)) {
					pLine->bshift = FALSE;
					diminish++;
				}
			}
		}
	}
	ASSERT(ndim==index);
	pDoc->ReCalculate();
	serr = sqrt(pDoc->sumsq/(double)(nspec*(pDoc->to-pDoc->from+1)-ndim));
	ndim -= diminish;
}

BOOL CLSquare::Optimize(double tolerance, int itermax, bool fullset)
{
	pView->serr = 0.0;
	double olderr = 0.0;
	int itercount = 0;
	double unused;
	while (1) {
		Build(fullset);
		aorig = a;
		for (int i=0; i<ndim; i++)
			borig[i] = b[i];
		// SOLVE FOR X
		a.ludcmp(ndim, indx, prtdrv, unused);
		a.lubksb(ndim, indx, b);
		// IMPROVE B TO MACHINE PRECISION
		for (int i=0; i<ndim; i++) {
			double adp = -borig[i];
			for (int j=0; j<ndim; j++)
				adp += aorig[i][j] * b[j];
			prtdrv[i] = adp;
		}
		a.lubksb(ndim, indx, prtdrv);
		for (int i=0; i<ndim; i++)
			b[i] -= prtdrv[i];
		// APPLY CORRECTION AND UPDATE GRAPHICS
		Evaluate(fullset);
		pView->Invalidate();
		pView->UpdateWindow();
		double tol = fabs(olderr - serr) / serr;
		CString str;
		str.Format("iteration: %d tolerance: %.6f stderr: %.6g\n", ++itercount, tol, serr);
		pMain->SetText(0, str);
		// CHECK FOR BEING DONE?
		if (itermax < 1)
			return true;
		if (tol <= tolerance) {
			pView->serr = serr;
			CalculateErrors(fullset);
			return TRUE;
		}
		if (itercount>=itermax) {
			AfxMessageBox("Could not converge!", MB_ICONEXCLAMATION);
			break;
		}
		olderr = serr;
	}
	return FALSE;
}

void CLSquare::CalculateErrors(bool fullset)
{
	CDblMatrix d;
	Build(fullset);
	d.SetSize(ndim, ndim);
	d = a;
//#ifdef _DEBUG
//	d.Dump(ndim, ndim);
//#endif
	double jay = d.Determinant(ndim, indx, prtdrv);
	d.SetSize(ndim-1, ndim-1);
	int index = 0;
	POSITION pos = pDoc->Spectra.GetHeadPosition();
	while (pos) {
		CSpectrum *pSpec = pDoc->Spectra.GetNext(pos);
		d.Minor(a, index, index);
		pSpec->eBaseline[0] = serr * sqrt(d.Determinant(ndim-1, indx, prtdrv) / jay);
		index++;
		if (pDoc->nbase > 1) {
			d.Minor(a, index, index);
			pSpec->eBaseline[1] = serr * sqrt(d.Determinant(ndim-1, indx, prtdrv) / jay);
			index++;
			if (pDoc->nbase > 2) {
				d.Minor(a, index, index);
				pSpec->eBaseline[2] = serr * sqrt(d.Determinant(ndim-1, indx, prtdrv) / jay);
				index++;
				if (pDoc->nbase > 3) {
					d.Minor(a, index, index);
					pSpec->eBaseline[3] = serr * sqrt(d.Determinant(ndim-1, indx, prtdrv) / jay);
					index++;
				}
			}
		}
		POSITION lpos = pDoc->Lines.GetHeadPosition();
		while (lpos) {
			CLine *pLine = pDoc->Lines.GetNext(lpos);
			d.Minor(a, index, index);
			pSpec->eintensity[pLine->id] = serr * sqrt(d.Determinant(ndim-1, indx, prtdrv) / jay);
			index++;
		}
	}
	pos = pDoc->Lines.GetHeadPosition();
	while (pos) {
		CLine *pLine = pDoc->Lines.GetNext(pos);
		if (pLine->bshape) {
			d.Minor(a, index, index);
			pLine->eshape = serr * sqrt(d.Determinant(ndim-1, indx, prtdrv) / jay);
			index++;
		}	
		if (pLine->bwidth) {
			d.Minor(a, index, index);
			pLine->ewidth = serr * sqrt(d.Determinant(ndim-1, indx, prtdrv) / jay);
			index++;
		}	
		if (pLine->bshift) {
			d.Minor(a, index, index);
			pLine->eshift = serr * sqrt(d.Determinant(ndim-1, indx, prtdrv) / jay);
			index++;
		}	
	}
}
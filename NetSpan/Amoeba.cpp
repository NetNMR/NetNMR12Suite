// Amoeba.cpp : implementation file
//

#include "stdafx.h"
#include "NetSpan.h"
#include "Amoeba.h"
#include "MainFrame.h"
#include "NetSpanView.h"
#include "NetSpanDoc.h"

// CAmoeba

CAmoeba::CAmoeba()
: p(NULL)
, y(NULL)
, psum(NULL)
, ptry(NULL)
, ndim(0)
, nline(0)
, nspec(0)
{
	pMain = STATIC_DOWNCAST(CMainFrame, theApp.m_pMainWnd);
	pView = STATIC_DOWNCAST(CNetSpanView, pMain->GetActiveView());
	pDoc = STATIC_DOWNCAST(CNetSpanDoc, pMain->GetActiveDocument());
}

CAmoeba::~CAmoeba()
{
	if (p) {
		for (int i=0; i<=ndim; i++) {
			if (p[i])
				delete []p[i];
		}
		delete []p;
		p = NULL;
	}
	if (y) {
		delete []y;
		y = NULL;
	}
	if (psum) {
		delete []psum;
		psum = NULL;
	}
	if (ptry) {
		delete []ptry;
		ptry = NULL;
	}
}

// CAmoeba member functions

BOOL CAmoeba::Create()
{
	nspec = (int)pDoc->Spectra.GetCount();
	nline = (int)pDoc->Lines.GetCount();
	if (0==nspec || 0==nline) return FALSE;
	ndim = nspec * pDoc->nbase + nspec * nline;
	POSITION pos = pDoc->Lines.GetHeadPosition();
	while (pos) {
		CLine *pLine = pDoc->Lines.GetNext(pos);
		ndim += pLine->ParCount();
	}
	if (ndim >= nspec * (pDoc->to-pDoc->from+1)) {
		AfxMessageBox("Paramaters exceed points!", MB_ICONEXCLAMATION);
		return FALSE;
	}
	y = new double[ndim+1];
	if (NULL == y) return FALSE;
	psum = new double[ndim];
	if (NULL == psum) return FALSE;
	ptry = new double[ndim];
	if (NULL == ptry) return FALSE;
	p = new double*[ndim+1];
	if (p) {
		for (int i=0; i<=ndim; i++)
			p[i] = NULL;
		for (int i=0; i<=ndim; i++) {
			p[i] = new double[ndim];
			if (NULL == p[i])
				return FALSE;
		}
	} else
		return FALSE;
	int index = 0;
	pos = pDoc->Spectra.GetHeadPosition();
	while (pos) {
		CSpectrum *pSpec = pDoc->Spectra.GetNext(pos);
		if (pDoc->nbase) {
			double dc, sl, cr, cb;
			pSpec->GetBaseline(dc, sl, cr, cb);
			p[0][index] = dc;
			psum[index++] = 5.0;
			if (pDoc->nbase > 1) {
				p[0][index] = sl;
				psum[index++] = 0.5;
				if (pDoc->nbase > 2) {
					p[0][index] = cr;
					psum[index++] = 0.05;
					if (pDoc->nbase > 3) {
						p[0][index] = cb;
						psum[index++] = 0.005;
					}
				}
			}
		}
		POSITION lpos = pDoc->Lines.GetHeadPosition();
		while (lpos) {
			CLine *pLine = pDoc->Lines.GetNext(lpos);
			p[0][index] = pSpec->intensity[pLine->id];
			psum[index++] = 0.05 * (pDoc->ymax - pDoc->ymin);
		}
	}
	double sw = pDoc->xnorm[pDoc->to] - pDoc->xnorm[pDoc->from];
	pos = pDoc->Lines.GetHeadPosition();
	while (pos) {
		CLine *pLine = pDoc->Lines.GetNext(pos);
		if (pLine->bshape) {
			p[0][index] = pLine->GetShape();
			if (p[0][index] + 0.05 < 1.0)
				psum[index++] = 0.05;
			else
				psum[index++] = -0.05;
		}
		if (pLine->bwidth) {
			p[0][index] = pLine->GetWidth();
			psum[index++] = 0.005 * sw;
		}
		if (pLine->bshift) {
			p[0][index] = pLine->GetShift();
			psum[index++] = 0.005 * sw;
		}
	}
	ASSERT(index == ndim);
	double ytmp = pDoc->sumsq;
	for (int i=1; i<=ndim; i++) {
		for (int j=0; j<ndim; j++) {
			p[i][j] = p[0][j];
			if (i-1 == j)
				p[i][j] += psum[j];
		}
		y[i] = Evaluate(p[i]);
	}
	y[0] = sqrt(ytmp/(double)(nspec*(pDoc->to-pDoc->from+1)-ndim));
	return TRUE;
}

double CAmoeba::Evaluate(double *param)
{
	int index = 0;
	POSITION pos = pDoc->Spectra.GetHeadPosition();
	while (pos) {
		CSpectrum *pSpec = pDoc->Spectra.GetNext(pos);
		switch (pDoc->nbase) {
			case 1:
				pSpec->SetBaseline(param[index], 0.0, 0.0, 0.0);
				break;
			case 2:
				pSpec->SetBaseline(param[index], param[index+1], 0.0, 0.0);
				break;
			case 3:
				pSpec->SetBaseline(param[index], param[index+1], param[index+2], 0.0);
				break;
			case 4:
				pSpec->SetBaseline(param[index], param[index+1], param[index+2], param[index+3]);
				break;
		}
		index += pDoc->nbase;
		POSITION lpos = pDoc->Lines.GetHeadPosition();
		while (lpos) {
			CLine *pLine = pDoc->Lines.GetNext(lpos);
			pSpec->intensity[pLine->id] = param[index++];
		}
	}
	pos = pDoc->Lines.GetHeadPosition();
	while (pos) {
		CLine *pLine = pDoc->Lines.GetNext(pos);
		if (pLine->bshape) {
			param[index] = pLine->ChangeShape(param[index]);
			index++;
		}
		if (pLine->bwidth) {
			param[index] = pLine->ChangeWidth(param[index]);
			index++;
		}
		if (pLine->bshift) {
			param[index] = pLine->ChangeShift(param[index]);
			index++;
		}
	}
	pDoc->ReCalculate();
	return sqrt(pDoc->sumsq/(double)(nspec*(pDoc->to-pDoc->from+1)-ndim));
}

void CAmoeba::CalcPSum()
{
	for (int j=0; j<ndim; j++) {
		double sum = 0.0;
		for (int i=0; i<=ndim; i++)
			sum += p[i][j];
		psum[j] = sum;
	}
}

bool CAmoeba::Optimize(double tolerance, int itermax)
{
	int itercount = 0;
	CalcPSum();
	CTime lt = CTime::GetCurrentTime();
	while (1) {
		int lo=0, hi, nhi;
		if (y[0] > y[1]) {
			hi = 0;
			nhi = 1;
		} else {
			hi = 1;
			nhi = 0;
		}
		for (int i=0; i<=ndim; i++) {
			if (y[i] < y[lo])
				lo = i;
			if (y[i] > y[hi]) {
				nhi = hi;
				hi = i;
			} else if (y[i] > y[nhi]) {
				if (i != hi)
					nhi = i;
			}
		}
		double tol = 2.0*fabs(y[hi]-y[lo])/(fabs(y[hi])+fabs(y[lo]));
		CString str;
		str.Format("iteration:%d tolerance:%.6f stderr:%.6g\n", itercount, tol, y[lo]);
		pMain->SetText(0, str);
		if (tol<=tolerance) {
			pView->serr = Evaluate(p[lo]);
			pView->Invalidate();
			return true;
		}
		if (itercount >= itermax) {
			AfxMessageBox("Could not converge!", MB_ICONEXCLAMATION);
			return false;
		}	
		double ytry = Try(-1.0, hi, itercount);
		if (ytry <= y[lo]) {
			//TRACE0("EXPAND\n");
			CTime ct = CTime::GetCurrentTime();
			CTimeSpan span = ct - lt;
			if (span.GetTotalSeconds() >= (LONGLONG)10) {
				lt = ct;
				pView->Invalidate();
				pView->UpdateWindow();
			}
			ytry = Try(2.0, hi, itercount);
		} else if (ytry >= y[nhi]) {
			double ysave = y[hi];
			//TRACE0("CONTRACT\n");
			ytry = Try(0.5, hi, itercount);
			if (ytry >= ysave) {
				for (int i=0; i<=ndim; i++) {
					if (i != lo) {
						for (int j=0; j<ndim; j++) {
							psum[j] = 0.5 * (p[i][j] + p[lo][j]);
							p[i][j] = psum[j];
						}
						//TRACE0("TRUNCATE\n");
						y[i] = Evaluate(psum);
					}
				}
				itercount += ndim;
				CalcPSum();
			}
		}
	}
}

double CAmoeba::Try(double fac, const int hi, int &iter)
{
	double fac1 = (1.0 - fac)/(double)ndim;
	double fac2 = fac1 - fac;
	for (int j=0; j<ndim; j++)
		ptry[j] = psum[j]*fac1 - p[hi][j]*fac2;
	double ytry = Evaluate(ptry);
	iter++;
	if (ytry < y[hi]) {
		y[hi] = ytry;
		for (int j=0; j<ndim; j++) {
			psum[j] += ptry[j]-p[hi][j];
			p[hi][j] = ptry[j];
		}
	}
	return ytry;
}
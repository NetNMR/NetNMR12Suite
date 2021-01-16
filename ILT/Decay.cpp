// Decay.cpp : implementation file
//

#include "stdafx.h"
#include "ILT.h"
#include "Decay.h"
#include "MainFrm.h"
#include "ILTDoc.h"
#include "tnt_array2d_utils.h"
#include "jama_qr.h"
#include "jama_lu.h"
#include "jama_eig.h"
//#include "jama_svd.h"

// CDecay

CDecay::CDecay()
 : ssq(0.0)
 , rssq(0.0)
 , iymin(0)
 , iymax(0)
 , isJoint(false)
{
}

CDecay::CDecay(CILTDoc *pDocument, int size, LPCTSTR t)
{
	pDoc = pDocument;
	strTitle = t;
	if (size > 1) {
		obs = Array1D<double>(size);
		calc = Array1D<double>(size);
		diff = Array1D<double>(size);
	}
}

CDecay::~CDecay()
{
	while (! Terms.IsEmpty())
		delete Terms.RemoveTail();
	for (int i=0; i<10; i++)
		while (! mcterms[i].IsEmpty())
			delete mcterms[i].RemoveTail();
}

// CDecay member functions

CTerm *CDecay::NewTerm()
{
	CTerm *term;
	switch (pDoc->kernelType) {
		case typeDosy:
		case typeT2:
			term = new CDTerm;
			break;
		case typeT1:
			term = new CRTerm;
			break;
	}
	return term;
}


bool CDecay::IsCalculated()
{
	return Terms.IsEmpty() ? false : true;
}

double CDecay::GetCalc(int i)
{
	if (0<=i && i<calc.dim() && Terms.GetCount())
		return calc[i];
	else
		return 0.0;
}

double CDecay::GetDiff(int i)
{
	if (0<=i && i<diff.dim() && Terms.GetCount())
		return diff[i];
	else
		return 0.0;
}

void CDecay::SetAt(int i, double val)
{
	if (0<=i && i<obs.dim())
		obs[i] = val;
}

double CDecay::GetAt(int i)
{
	if (0<=i && i<obs.dim())
		return obs[i];
	else
		return 0.0;
}

void CDecay::Calculate()
{
	// calculate the new spectrum
	CWaitCursor wait;
	int m = obs.dim();
	rssq = 0.0;
	idmin = idmax = 0;
	for (int i=0; i<m; i++) {
		double c = 0.0;
		double xvalue = pDoc->Xvalue[i];
		POSITION pos = Terms.GetHeadPosition();
		while (pos) {
			CTerm *term = Terms.GetNext(pos);
			if (pos)
				c += term->Evaluate(xvalue);
			else
				c += term->a;
		}
		calc[i] = c;
		c = obs[i] - c;
		rssq += pow(c, 2);
		if (c < diff[idmin])
			idmin = i;
		else if (c > diff[idmax])
			idmax = i;
		diff[i] = c;
	}
}

double CDecay::CalcValue(double x)
{
	CWaitCursor wait;
	double y = 0;
	POSITION pos = Terms.GetHeadPosition();
	while (pos) {
		CTerm *term = Terms.GetNext(pos);
		if (pos)
			y += term->Evaluate(x);
		else
			y += term->a;
	}
	return y;
}

double CDecay::SSQ()
{
	CWaitCursor wait;
	// besides calculating the sum of squares,
	// also find the index to the min and max Y-values
	iymin = iymax = 0;
	ssq = 0.0;
	for (int i=0; i<obs.dim(); i++) {
		ssq += pow(obs[i], 2);
		if (obs[i] > obs[iymax])
			iymax = i;
		else if (obs[i] < obs[iymin])
			iymin = i;
	}
	//CString msg;
	//msg.Format("Max obs[%d]=%g : Min obs[%d]=%g", iymax, obs[iymax], iymin, obs[iymin]);
	//AfxMessageBox(msg);
	return ssq;
}

double CDecay::Max()
{
	return obs[iymax];
}

double CDecay::Min()
{
	return obs[iymin];
}

double CDecay::Diff()
{
	return Max() - Min();
}

double CDecay::DiffMax()
{
	return diff[idmax];
}

double CDecay::DiffMin()
{
	return diff[idmin];
}

double CDecay::DiffDiff()
{
	return DiffMax() - DiffMin();
}

Array1D<double> CDecay::nnls(Array2D<double> &A, Array1D<double> &b)
{
	// From the method originally published by Lawson & Hanson in
	// "Solving Least Squares Problems", Prentice-Hall, 1974.
	// Solves for x in Ax=b, where all x[i] >= 0.0.

	// Designed from a PowerPoint presentation by Yi-Gang (Jeremy) Tai at U. of Texas, San Antonio.
	// http://www.cs.utsa.edu/~rcl/meeting/past/f05-11-nnls.ppt

	// Modified by TAE to allow the final solution element, x[n-1], to have any real value.

	CWaitCursor wait;
	// CHECK ARRAY SIZES
	int m = A.dim1();
	int n = A.dim2();
	Array1D<double> x(n, 0.0);
	if (m != b.dim()) {
		return x;
	}

	// Let the user know what's going on
	theApp.SetPaneText(0, "Performing NNLS (Inverse Laplace Transform)");
	// INITIALIZE DATA
	bool *IsP = new bool[n];
	int nP = 1;								// always include the constant
	for (int i=0; i<n; i++) {
		x[i] = 0.0;
		IsP[i] = (i==n-1) ? true : false;	// always include the constant
	}
	while (1) { // OUTER LOOP
		// IF Z IS EMPTY WE HAVE A SOLUTION
		if (nP == n)
			break;

		// CALCULATE w = A^T * (b - Ax)
		Array1D<double> w = matmult(transpose(A), b - matmult(A, x));

		// IF ALL w[i] <= 0, WE HAVE A SOLUTION
		bool all_le_zero = true;
		int maxwi = -1;
		for (int i=0; i<n-1; i++) {
			if (! IsP[i]) {
				if (w[i] > 0.0)
					all_le_zero = false;
				if (maxwi < 0)
					maxwi = i;
				else if (w[i] > w[maxwi])
					maxwi = i;
			}
		}
		if (all_le_zero)
			break;

		// MOVE THE BIGGEST w[i] INDEX FROM Z TO P
		IsP[maxwi] = true;
		nP++;
		// redundancy check
		int nremove=0, iremove=-1;
		while (1) { // INNER LOOP

			// FORM Apos , a SUBSET OF A WHERE InP[j] IS TRUE
			Array2D<double> Apos(m, nP);
			Array1D<int> Aindex(nP);
			int k = 0;
			for (int j=0; j<n; j++) {
				if (IsP[j]) {
					for (int i=0; i<m; i++)
						Apos[i][k] = A[i][j];
					//TRACE3("nP=%d: Aindex[%d]=%d\n", nP, k, j);
					Aindex[k++] = j;
				}
			}

			// SOLVE FOR z in: Apos z = b
			QR<double> qr(Apos);
			Array1D<double> zp = qr.solve(b);

			// IF ALL z[i] WITH INDICIES IN P ARE > 0, WE HAVE A TRIAL SOLUTION
			// THERE IS AN EXPECTION FOR THE CONSTANT
			bool all_ge_zero = true;
			for (int i=0; i<nP-1; i++) {
				if (zp[i] < 0.0) {
					all_ge_zero = false;
					break;
				}
			}
			if (all_ge_zero) {
				for (int i=0; i<nP; i++) {
					x[Aindex[i]] = zp[i];
				}
				break;
			}

			// AT LEAST ONE z[i] WITH P INDEX IS negative
			// FIND MINIMUM x[i]/(x[i]-z[i]) for any negative z[i]
			Array1D<double> z(n, 0.0);
			for (int i=0; i<nP; i++)
				z[Aindex[i]] = zp[i];
			double minimum = DBL_MAX;
			for (int i=0; i<n-1; i++) {
				if (z[i] < 0.0) {
					double newa = x[i] / (x[i] - z[i]);
					if (newa < minimum)
						minimum = newa;
				}
			}

			// check for redundancy
			nremove = 0;
			iremove = -1;
			// compute x += minimum*(z-x)
			for (int i=0; i<n; i++) {
				x[i] += minimum * (z[i] - x[i]);
				// move index zero x[i] from P to Z
				if (i<n-1 && IsP[i] && abs(x[i])<1.0e-15) {
					nremove++;
					iremove = i;
					x[i] = 0.0;
					IsP[i] = false;
					nP--;
				}
			}

		}	// INNER LOOP
		// we deleted the term we just added: Time to Quit!
		if (1==nremove && maxwi==iremove)
			break;
	}		// OUTER LOOP
	delete[] IsP;
	return x;
}

void CDecay::SIRT(const int itercount, const double tolerance, const double from, const double to, const int n)
{
	// From "A new method for multi-exponential inversion of NMR relaxation measurements" by
	// Zhongdong Wang, Lizhi Xiao and Tangyan Liu in Science in China Series G Physics,
	// Mechanics & Astronomy 47, 265(2004).
	//
	// This produces the broadest possible distribution that fits the exponential.
	// SIRT stand for "simultaneous iterative reconstruction technique".
	CWaitCursor wait;
	const int m = obs.dim();
	while (! Terms.IsEmpty())
		delete Terms.RemoveHead();
	Array1D<double> lambda(m, 0.0);
	Array1D<int> LA(n+1, 0);
	Array1D<double> R(n);
	Array2D<double> A(m, n+1);
	Array1D<double> x(n+1, 0.0);
	Array1D<double> xnew(n+1);
	
	// create the constant term
	for (int i=0; i<m; i++)
		A[i][n] = 1.0;

	// now the other terms
	for (int i=0; i<n; i++) {
		R[i] = from * pow(to/from, double(i)/double(n-1));
		if (typeDosy != pDoc->kernelType)
			R[i] = 1.0 / R[i];
		Array1D<double> col = makedecay(R[i]);
		for (int j=0; j<m; j++)
			A[j][i] = col[j];
	}
	// build LA
	for (int i=0; i<=n; i++)
		for (int j=0; j<m; j++)
			if (A[j][i] > 0.0)
				LA[i]++;
	// build lambda
	for (int i=0; i<m; i++) {
		for (int j=0; j<=n; j++)
			lambda[i] += pow(A[i][j], 2);
		lambda[i] = 1.0 / lambda[i];
	}
	// SIRT ITERATION
	double pssq = DBL_MAX;
	for (int count=0; count<itercount; count++) {
		double ssq = 0.0;
		// new calculation
		for (int i=0; i<m; i++) {
			calc[i] = x[n];
			for (int j=1; j<n; j++)
				calc[i] += x[j] * A[i][j];
			diff[i] = obs[i] - calc[i];
			ssq += pow(diff[i], 2);
		}
		double tol = (pssq - ssq) / pssq;
		CString msg;
		msg.Format("Iteration=%d tolerance=%g", count+1, tol);
		if (tol <= dblSIRTTolerance)
			break;
		pssq = ssq;
		theApp.SetPaneText(0, msg);
		for (int i=0; i<=n; i++) {
			xnew[i] = 0.0;
			for (int j=0; j<m; j++)
				xnew[i] += lambda[j] * diff[j] * A[j][i];
			xnew[i] /= LA[i];
			x[i] += xnew[i];
			if (x[i]<0.0 && i<n)
				x[i] = 0.0;
		}
	}
	CTerm *newterm = NewTerm();
	newterm->a = x[n];
	newterm->Insert(Terms);
	for (int i=0; i<n; i++) {
		newterm = NewTerm();
		newterm->r = R[i];
		newterm->a = x[i];
		newterm->Insert(Terms);
	}
	Calculate();
	return;
}

void CDecay::ILT(const double alpha, const double from, const double to, const int n)
{
	CWaitCursor wait;
	const int m = obs.dim();
	// delete the previous solution
	while (! Terms.IsEmpty())
		delete Terms.RemoveHead();
	// build new x and A
	Array1D<double> x;
	Array1D<double> R(n);
	Array2D<double> A(m, n+1);
	for (int i=0; i<=n; i++) {
		if (i==n) {
			// make the constant column
			for (int j=0; j<m; j++)
				A[j][i] = 1.0;
		} else {
			// partial derivatives of term with respect to amp is = kernel
			R[i] = from * pow(to/from, double(i)/double(n-1));
			if (typeDosy != pDoc->kernelType)
				R[i] = 1.0 / R[i];
			Array1D<double> col = makedecay(R[i]);
			for (int j=0; j<m; j++)
				A[j][i] = col[j];
		}
	}
	if (0.0 == alpha) {
		x = nnls(A, obs);
	} else {
		Array2D<double> Aa(n+1, n+1);
		Array1D<double> b(n+1);
		for (int i=0; i<=n; i++) {
			b[i] = 0.0;
			for (int k=0; k<m; k++)
				b[i] += (obs[k] * A[k][i]);
			for (int j=i; j<=n; j++) {
				Aa[i][j] = 0.0;
				for (int k=0; k<m; k++)
					Aa[i][j] += A[k][i] * A[k][j];
				if (i == j) {
					//if (i < n)
						Aa[i][j] += alpha;
				} else
					Aa[j][i] = Aa[i][j];
			}
		}
		x = nnls(Aa, b);
	}
	if (bSimplify) {
		// save the const
		CTerm *constterm = NewTerm();
		constterm->a = x[n];
		constterm->Insert(Terms);
		// reset the last x to insure we can harvest all the peaks properly
		x[n] = 0.0;
		
		double sum=0.0, cm=0.0;	// the harvester accumulators
		if (x[0]>0.0) {	// new peak at x[0]
			if (x[1] <= 0.0) {
				// x[0] is an isolated peak, create and add the new term
				CTerm *term = NewTerm();
				term->a = x[0];
				term->r = R[0];
				term->Insert(Terms);
			} else {	// there's more to come, so accumulate the peak
				sum = x[0];
				cm = x[0] * pDoc->Xvalue[0];
			}
		}

		// harvester
		for (int i=1; i<=n; i++) {
			if (x[i]) {
				// go up the side of the peak
				while (x[i-1] < x[i]) {
					sum += x[i];
					cm += x[i] * R[i];
					// we don't have to watch for the end because x[n]=0
					i++;
				}
				if (i <= n) { // as long as we aren't at the end of x[]
					// then go down the other side
					// stop when x[i] starts increasing
					while (x[i-1]>=x[i]) {
						if (x[i] > 0.0) {
							sum += x[i];
							cm += x[i] * R[i];
						} else
							break;
						if (++i > n)
							break; // no more data
					}
				}
				// save the accumulated peak
				if (sum) {
					CTerm *term = NewTerm();
					term->a = sum;
					term->r = cm / sum;
					term->Insert(Terms);
					sum = cm = 0.0;
				}
				// do we still have data
				if (i < n) {
					// if were not at the end, we must be staring a new peak
					// i.e., we went through a valley that didn't touch 0
					// so reset the accumulators
					sum = x[i];
					cm = x[i] * R[i];
				}
			}
		}
		x[n] = constterm->a;
	} else { // do not simplify, just save the non-zero terms and the constant!
		// first, save the constant
		CTerm *term = NewTerm();
		term->a = x[n];
		term->Insert(Terms);
		// then all the other terms
		for (int i=0; i<n; i++) {
			// if alpha!=0 save all terms
			// if alpha==0 save only nonzero terms
			if (x[i]>0 || 0.0!=dblAlpha) {
				term = NewTerm();
				term->r = R[i];
				term->a = x[i];
				term->Insert(Terms);
			}
		}
	}

	Calculate();
	return;
}

Array1D<double> CDecay::makedecay(const double relaxrate)
{
	CWaitCursor wait;
	int n = pDoc->Xvalue.dim();
	if (n<=0 || relaxrate<=0.0)
		return Array1D<double>();
	Array1D<double> d(n);
	// make a dummy term to access the function
	CTerm *term = NewTerm();
	term->r = relaxrate;
	for (int i=0; i<n; i++)
		d[i] = term->PartDerA(pDoc->Xvalue[i]);
	delete term;
	return d;
}

void CDecay::GetStats(double &sumsq, double &rsumsq)
{
	sumsq = ssq;
	rsumsq = rssq;
}

int CDecay::CombinePairs()
{
	CWaitCursor wait;
	int dropped = 0;
	POSITION pos = Terms.GetHeadPosition();
	while (pos) {
		CTerm *term = Terms.GetAt(pos);
		POSITION nextpos = pos;
		Terms.GetNext(nextpos);
		if (nextpos) {
			CTerm *nextterm = Terms.GetAt(nextpos);
			if (nextterm->r > 0.0) {
				if (term->r/nextterm->r < 1.05) {
					double a = term->a + nextterm->a;
					double R = nextterm->r - term->r;
					CString msg;
					if (pDoc->kernelType == typeDosy)
						msg.Format("Combining terms at D = %g and %g", term->r, nextterm->r);
					else
						msg.Format("Combining terms at T = %g and %g", 1.0/term->r, 1.0/nextterm->r);
					theApp.SetPaneText(0, msg);
					//TRACE0(msg+'\n');
					R *= nextterm->a / a;
					term->a = a;
					term->r += R;
					delete nextterm;
					Terms.RemoveAt(nextpos);
					dropped++;
				}
			}
		}
		Terms.GetNext(pos);
	}
	Calculate();
	return dropped;
}

// return true if a term is discarded
// return false if no term is discarded
bool CDecay::DiscardTerm(double chisq, double from, double to)
{
	// find the least significant term and kill it
	CWaitCursor wait;
	if (Terms.GetCount() < 2)
		return false;
	double min = 0.0;
	POSITION dpos = NULL;
	POSITION pos = Terms.GetHeadPosition();
	while (pos) {
		CTerm *term = Terms.GetAt(pos);
		if (term->r) {	// don't mess with the constant!
			double a = term->a - sqrt(chisq * term->la);
			if (a < min) {
				min = a;
				dpos = pos;
			}
		}
		Terms.GetNext(pos);
	}
	if (dpos) {
		CTerm *term = Terms.GetAt(dpos);
		CString msg;
		if (pDoc->kernelType == typeDosy)
			msg.Format("Deleting Null Hypothesis term at D = %g, intensity interval contains zero.", term->r);
		else
			msg.Format("Deleting Null Hypothesis term at T = %g, intensity interval contains zero.", 1.0/term->r);
		theApp.SetPaneText(0, msg);
		//TRACE0(msg + '\n');
		delete term;
		Terms.RemoveAt(dpos);
		Calculate();
		return true;

	}
	// Any low rate terms?
	pos = Terms.GetHeadPosition();
	while (pos) {
		dpos = pos;
		CTerm *term = Terms.GetNext(pos);
		if (pos) {	// don't test the constant!
			double r = term->r - sqrt(chisq * term->lc);
			if (r <= 0.0) {
				CString msg;
				if (pDoc->kernelType == typeDosy)
					msg.Format("Deleting Null Hypothesis term at D = %g, rate interval contains zero", term->r);
				else
					msg.Format("Deleting Null Hypothesis term at T = %g, rate interval contains zero", 1.0/term->r);
				theApp.SetPaneText(0, msg);
				//TRACE0(msg + '\n');
				delete term;
				Terms.RemoveAt(dpos);
				RedoConstant();
				return true;
			}
		}
	}
	// remove all terms that have an out of range rate
	bool foundbad = false;
	pos = Terms.GetHeadPosition();
	while (pos) {
		dpos = pos;
		CTerm *term = Terms.GetNext(pos);
		if (pos) {	// only test non-constant terms!
			double comp = term->r;
			if (pDoc->kernelType != typeDosy)
				comp = 1.0 / comp;
			if (comp < from || to < comp) {
				CString msg;
				msg.Format("Deleting out of range term at %g", comp);
				theApp.SetPaneText(0, msg);
				//TRACE0(msg + '\n');
				delete term;
				Terms.RemoveAt(dpos);
				RedoConstant();
				foundbad = true;
			}
		}
	}
	return foundbad;
}

Array2D<double> CDecay::gradientintegral()
{
	CWaitCursor wait;
	const int m = obs.dim();
	const int npars = 2 * int(Terms.GetCount()) - 1;
	// calculate the partial derrivatives
	Array2D<double> pder(m, npars);
	POSITION pos = Terms.GetHeadPosition();
	for (int j=0; j<npars; j+=2) {
		CTerm *term = Terms.GetNext(pos);
		for (int i=0; i<m; i++) {
			double xvalue = pDoc->Xvalue[i];
			if (pos) {
				pder[i][j] = term->PartDerA(xvalue);
				pder[i][j+1] = term->PartDerR(xvalue);
			} else
				pder[i][j] = 1.0;
		}
	}

	// calculate the A Array2D
	Array2D<double> A(npars, npars);
	for (int i=0; i<npars; i++) {
		for (int j=i; j<npars; j++) {
			A[i][j] = 0.0;
			for (int k=0; k<m; k++)
				A[i][j] += pder[k][i] * pder[k][j];
			if (i != j)
				A[j][i] = A[i][j];
		}
	}

	return A;
}

double CDecay::amofunc(const Array1D<double> &p)
{
	CWaitCursor wait;
	double r = 0.0;
	int n = (p.dim() - 1) / 2;
	// create an empty term to use Evaluate()
	CTerm *term = NewTerm();
	for (int i=0; i<pDoc->Xvalue.dim(); i++) {
		double calc = 0.0;
		for (int j=0; j<n; j++) {
			term->a = p[2*j];
			term->r = p[2*j+1];
			calc += term->Evaluate(pDoc->Xvalue[i]);
		}
		calc += p[2*n];
		r += pow(obs[i]-calc, 2);
	}
	delete term;
	return r;
}

double CDecay::amotry(Array2D<double> &p, Array1D<double> &y, Array1D<double> &psum, const int ihi, int &iterations, double fac)
{
	CWaitCursor wait;
	int ndim = p.dim2();
	Array1D<double> ptry(ndim);
	double fac1 = (1.0 - fac) / double(ndim);
	double fac2 = fac1 - fac;
	for (int i=0; i<ndim; i++) {
		ptry[i] = psum[i] * fac1 - p[ihi][i] * fac2;
		if (ptry[i]<0.0 && i<ndim-1)
			ptry[i] = 0.0;
	}
	double ytry = amofunc(ptry);
	++iterations;
	if (ytry < y[ihi]) {
		y[ihi] = ytry;
		for (int j=0; j<ndim; j++) {
			psum[j] += ptry[j] - p[ihi][j];
			p[ihi][j] = ptry[j];
		}
	}
	return ytry;
}

bool CDecay::Amoeba()
{
	// returns true if Amoeba dropped terms (a or r <= 0)
	CWaitCursor wait;
	// initialization
	int icount = 0;
	const int npars = 2 * int(Terms.GetCount()) - 1;
	if (npars == 1) {
		CTerm *term = Terms.GetHead();
		term->a = obs[0];
		for (int i=1; i<obs.dim(); i++)
			term->a += obs[i];
		term->a /= double(obs.dim());
		Calculate();
		return false;
	}
	Array1D<double> y(npars+1);
	Array2D<double> p(npars+1, npars);
	Array1D<double> psum(npars);
	// need to find the range of data for setting up the const parameter increment
	double ymin, ymax;
	ymin = ymax = obs[0];
	for (int i=1; i<obs.dim(); i++)
		if (obs[i] > ymax)
			ymax = obs[i];
		else if (obs[i] < ymin)
			ymin = obs[i];

	// initialize p[][]
	// first the starting point, p[0][], is set to current position
	POSITION pos = Terms.GetHeadPosition();
	for (int i=0; i<npars; i+=2) {
		CTerm *term = Terms.GetNext(pos);
		p[0][i] = term->a;
		if (pos)
			p[0][i+1] = term->r;
	}
	// for each subsequent point, increment each parameter by 1%
	for (int i=0; i<npars; i++) {
		for (int j=0; j<npars; j++)
			p[i+1][j] = (i==j) ? 1.01 * p[0][j] : p[0][j];
		psum[i] = p[0][i] * (0.01 + double(npars+1));
	}
	// a special case for the constant, 1% of the max diff in the observed values
	p[npars][npars-1] = p[0][npars-1] + 0.01 * (ymax - ymin);
	psum[npars-1] = double(npars) * p[0][npars-1] + p[npars][npars-1];

	// calculate y[]
	for (int i=0; i<=npars; i++) {
		Array1D<double> ptry(npars);
		for (int j=0; j<npars; j++)
			ptry[j] = p[i][j];
		y[i] = amofunc(ptry);
	}

	// now the fun begins, the minimization loop
	double amoebabest = DBL_MAX;
	int ilo;
	while (true) {

		// find low, high and next highest
		int ihi, inhi;
		ilo = 0;
		if (y[0] > y[1]) {
			ihi = 0;
			inhi = 1;
		} else {
			ihi = 1;
			inhi = 0;
		}
		for (int i=0; i<=npars; i++) {
			if (y[i] < y[ilo])
				ilo = i;
			if (y[i] > y[ihi]) {
				inhi = ihi;
				ihi = i;
			} else if (y[i] > y[inhi])
				if (i != ihi)
					inhi = i;
		}

		// let the user know about improvements
		if (y[ilo] < amoebabest) {
			amoebabest = y[ilo];
			CString msg;
			msg.Format("Optimization best rssq = %g", amoebabest);
			theApp.SetPaneText(0, msg);
		}

		// are we done?
		double rtol = 2.0 * (y[ihi] - y[ilo]) / (y[ihi] + y[ilo]);
		if (rtol < 1.0e-10)
			break;
		if (icount > 10000)
			break;

		CString msg;
		double ytry = amotry(p, y, psum, ihi, icount, -1.0);
		if (ytry <= y[ilo]) {
			ytry = amotry(p, y, psum, ihi, icount, 2.0);
		} else if (ytry >= y[inhi]) {
			double ysave = y[ihi];
			ytry = amotry(p, y, psum, ihi, icount, 0.5);
			if (ytry >= ysave) {
				for (int i=0; i<=npars; i++) {
					if (i != ilo) {
						for (int j=0; j<npars; j++) {
							psum[j] = 0.5 * (p[i][j] + p[ilo][j]);
							p[i][j] = psum[j];
						}
						y[i] = amofunc(psum);
					}
				}
				icount += npars;
				// recompute the sum
				for (int j=0; j<npars; j++) {
					psum[j] = 0.0;
					for (int i=0; i<=npars; i++)
						psum[j] += p[i][j];
				}
			}
		}
	}

	// return value
	bool termdropped = false;
	// build new TERMS (amoeba may have put them out of order)
	while (! Terms.IsEmpty())
		delete Terms.RemoveHead();
	CTerm *constterm = NewTerm();	// make the const term
	constterm->a = p[ilo][npars-1];
	constterm->Insert(Terms);
	for (int i=0; i<npars; i+=2) {
		if (i+2 <= npars-1) {
			if (p[ilo][i]>0.0 && p[ilo][i+1]>0.0) {	// term->a and term->r must be > 0.0
				CTerm *newterm = NewTerm();
				newterm->a = p[ilo][i];
				newterm->r = p[ilo][i+1];
				newterm->Insert(Terms);
			} else
				termdropped = true;
		}
	}
	Calculate();
	return termdropped;
}

void CDecay::ParamErrors()
{
	CWaitCursor wait;
	int n = 2 * int(Terms.GetCount()) - 1;
	Array2D<double> A = gradientintegral();
	double isigsq = double(obs.dim())/rssq;
	for (int i=0; i<n; i++)
		for (int j=0; j<n;j++)
			A[i][j] *= isigsq;
	LU<double> lu(A);
	if (lu.isNonsingular()) {
		Array2D<double> I(n, n, 0.0);
		for (int i=0; i<n; i++)
			I[i][i] = 1.0;
		Array2D<double> invA = lu.solve(I);
		POSITION pos = Terms.GetHeadPosition();
		for (int i=0; i<n; i+=2) {
			CTerm *term = Terms.GetNext(pos);
			term->la = invA[i][i];
			if (pos) {
				term->lb = invA[i][i+1];
				term->lc = invA[i+1][i+1];
			}
		}
	}
}

void CDecay::WEGL()
{
	// alogrithm orignally published:
	//
	// "Exponential-Sum Fitting of Radiative Transmission Functions" by Wiscombe and Evans,
	// in J. of Comput. Phys., vol 24, pp. 416 (1977), and
	//
	// "On Least Squares Exponential Sum Approximation with Positive Coefficients", by Evans,
	// Gragg and LeVeque, in Mathematics of Computation, vol 34, pp. 203 (1980).
	//
	// Alogrithm modified by TAE:
	// * Fit just constant term first before a new value is generated.
	// * Use golden section search to find next term.
	// * Use nnls to manage removal of terms no longer needed.

	CString msg;
	CWaitCursor wait;
	int m = obs.dim();
	while (! Terms.IsEmpty())
		delete Terms.RemoveHead();
	CTerm *term = NewTerm();
	for (int i=0; i<obs.dim(); i++)
		term->a += obs[i];
	term->a /= double(obs.dim());
	term->Insert(Terms);
	Calculate();
	while (true) {		// Keep adding new T2 values until we get a good fit.
		// find the next T2 value to add
		double newr = findnextr();
		if (newr <= 0.0) {
			theApp.SetPaneText(0, "WEGL is done!");
			return; // nothing more to add!
		}
		if (typeDosy == pDoc->kernelType)
			msg.Format("adding term D=%g", newr);
		else
			msg.Format("adding term T=%g", 1.0/newr);
		theApp.SetPaneText(0, msg);

		// Does the term already exist?
		POSITION pos = Terms.GetHeadPosition();
		while (pos) {
			CTerm *term = Terms.GetNext(pos);
			if (term->r == newr) {
				theApp.SetPaneText(0, "Terminating with duplicate term!");
				return;
			}
		}
		CTerm *newterm = NewTerm();
		newterm->r = newr;
		newterm->Insert(Terms);

		// recalculate the decay coefficients and put them in TERMS.a
		nnlswrapper();

		// remove terms that have zero coefficient
		pos = Terms.GetHeadPosition();
		while (pos) {
			POSITION cpos = pos;
			CTerm *term = Terms.GetNext(pos);
			if (term->r!=0.0 && 0.0>=term->a) {
				if (typeDosy == pDoc->kernelType)
					msg.Format("Removing term D=%g", term->r);
				else
					msg.Format("Removing term T=%g", 1.0/term->r);
				theApp.SetPaneText(0, msg);
				delete term;
				Terms.RemoveAt(cpos);
			}
		}
		Calculate();
	}
}

void CDecay::nnlswrapper()
{
	CWaitCursor wait;
	// make the A Array2D and solve for x
	int n = int(Terms.GetCount());
	int m = obs.dim();
	Array2D<double> A(m, n);
	int j = 0;
	POSITION pos = Terms.GetHeadPosition();
	while (pos) {
		CTerm *term = Terms.GetNext(pos);
		Array1D<double> col = (term->r > 0.0) ? makedecay(term->r) : Array1D<double>(m, 1.0);
		for (int i=0; i<m; i++)
			A[i][j] = col[i];
		j++;
	}

	// solve for x using non-negative least squares 
	Array1D<double> x = nnls(A, obs);

	// move solution to TERMS list
	pos = Terms.GetHeadPosition();
	for (int i=0; i<n; i++) {
		CTerm *term = Terms.GetNext(pos);
		term->a = x[i];
	}
}

double CDecay::PEalpha(const double xvalue)
{
	double x = xvalue;
	if (typeDosy != pDoc->kernelType)
		x = 1.0 / x;
	return PEalpha(makedecay(x));
}

double CDecay::PEalpha(const Array1D<double> &Ealpha)
{
	CWaitCursor wait;
	int n = Ealpha.dim();
	if ( n!=obs.dim() || n!=calc.dim() )
		return 0.0;
	double PE = 0.0;
	for (int i=0; i<n; i++)
		PE += (calc[i] - obs[i]) * Ealpha[i];
	return 2.0 * PE;
}

double CDecay::findnextr()
{
	CWaitCursor wait;
	int n = pDoc->Xvalue.dim();
	Array1D<double> pe(n);
	for (int i=0; i<n; i++)
		pe[i] = PEalpha(pDoc->Xvalue[i]);
	// return true if sucessful, false if not
	// FIND THE MINIMUM PsubE(alpha)
	double min = pe[0];
	int imin = 0;
	for (int i=1; i<n; i++) {
		if (pe[i] < min) {
			imin = i;
			min = pe[i];
		}
	}
	if (min >= 0.0)
		return 0.0; // nothing more to do!
	double x0, x1, x2, x3, f0, f1, f2,f3;
	int i0, i3;
	x0 = x3 = pDoc->Xvalue[imin];
	i0 = i3 = imin;

	// simple search for bounds
	int count = 0;
	do {
		if (i0 > 0)
			x0 = pDoc->Xvalue[--i0];
		else
			x0 *= 0.95;
		f0 = PEalpha(x0);
	} while (f0<=min && 100>count++);
	count = 0;
	do {
		if (i3 < n-1)
			x3 = pDoc->Xvalue[++i3];
		else
			x3 *= 1.05;
		f3 = PEalpha(x3);
	} while (f3<=min && 100>count++);

	// Golden Section search
	const double R = 0.61803399;
	const double C = 1.0 - R;
	if (fabs(x3-pDoc->Xvalue[imin]) > fabs(pDoc->Xvalue[imin]-x0)) {
		x1 = pDoc->Xvalue[imin];
		f1 = min;
		x2 = x1 + C * (x3 - x1);
		f2 = PEalpha(x2);
	} else {
		x2 = pDoc->Xvalue[imin];
		f2 = min;
		x1 = x2 - C * (x2 - x0);
		f1 = PEalpha(x1);
	}
	while (x0-x3 > 1.0e-8*(x1+x2) ) {
		if (f2 < f1) {
			x0 = x1;
			f0 = f1;
			x1 = x2;
			f1 = f2;
			x2 = R*x1 + C*x3;
			f2 = PEalpha(x2);
		} else {
			x3 = x2;
			f3 = f2;
			x2 = x1;
			f2 = f1;
			x1 = R*x2 + C*x3;
			f1 = PEalpha(x1);
		}
	}
	double relaxrate = (f1 < f2) ? x1 : x2;
	if (typeDosy != pDoc->kernelType)
		relaxrate = 1.0 / relaxrate;
	return relaxrate;
}

void CDecay::DoProcess(int method, BOOL simplify, BOOL optimize, const double alpha, const int sirtcount, const double sirttolerance, const double from, const double to, const int n)
{
	// this is the process that will be applied when the user clicks "okay"
	// on the ILT Process dialog
	double chisq = 1.0742;
	switch (pDoc->iConfidenceInterval) {
		case 1:
			chisq = 2.7056;
			break;
		case 2:
			chisq = 3.8415;
			break;
		case 3:
			chisq = 6.635;
			break;
		case 4:
			chisq = 10.8285;
			break;
		case 5:
			chisq = 15.145;
			break;
	}
	iMethod = method;
	bSimplify = simplify;
	bOptimize = optimize;
	dblAlpha = alpha;
	iSIRTCount = sirtcount;
	dblSIRTTolerance = sirttolerance;
	switch (method) {
		case IDC_ILTMETHODRADIO:
			// bSimplify is available to ITL(), so we don't have to do anything else
			// for ILT, "Simplify" means to coalese neighboring, non-zero points in the ILT x[] array.
			ILT(alpha, from, to, n);
			break;
		case IDC_EGLMETHODRADIO:
			WEGL();
			if (simplify)
				// WEGL() is likely to produce closely spaced pairs
				CombinePairs();
			break;
		case IDC_SIRTMETHODRADIO:
			SIRT(sirtcount, sirttolerance, from, to, n);
			break;
	}
	if (optimize) {
		do {
			do {
				// do Amoeba optimization until no peaks are lost (negative a or r)
				while (Amoeba())
					RedoConstant();
			} while (CombinePairs());	// continue until there are no peaks within 1.05 of each other
			ParamErrors();				// have to calculate errors for DiscardTerm()
		} while (DiscardTerm(chisq, from, to));	// continue until all a[] values can reject null hypothesis
	}
}

void CDecay::RedoConstant()
{
	CTerm *constant = Terms.GetTail();
	constant->a = 0.0;
	Calculate();
	int size = obs.dim();
	for (int i=0; i<size; i++)
		constant->a += diff[i];
	constant->a /= double(size);
	Calculate();
}

void CDecay::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{	// storing code
		ar << iMethod-IDC_EGLMETHODRADIO << bOptimize << bSimplify << dblAlpha;
		ar << iSIRTCount << dblSIRTTolerance;
		ar << obs.dim();
		for (int i=0; i<obs.dim(); i++)
			ar << obs[i];
		int n = int(Terms.GetCount());
		ar << n;
		POSITION pos = Terms.GetHeadPosition();
		while (pos) {
			CTerm *term = Terms.GetNext(pos);
			term->Serialize(ar);
		}
	} else {	// loading code
		int m, n;
		ar >> iMethod >> bOptimize >> bSimplify >> dblAlpha;
		if (iMethod > 2)
			iMethod = 1;
		iMethod += IDC_EGLMETHODRADIO;
		ar >> iSIRTCount >> dblSIRTTolerance;
		ar >> m;
		obs = Array1D<double>(m);
		calc = Array1D<double>(m);
		diff = Array1D<double>(m);
		for (int i=0; i<m; i++)
			ar >> obs[i];
		ar >> n;
		for (int i=0; i<n; i++) {
			CTerm *term = NewTerm();
			term->Serialize(ar);
			Terms.AddTail(term);
		}
		SSQ();
		Calculate();
	}
}

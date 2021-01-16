#include "StdAfx.h"
#include "dblmatrix.h"

CDblMatrix::CDblMatrix(void)
{
	data = NULL;
	rows = cols = 0;
}

BOOL CDblMatrix::SetSize(int r, int c)
{
	ASSERT(r>0 && c>0);
	if (rows)
		Delete();
	BOOL allocated = TRUE;
	data = new double*[r];
	ASSERT(data);
	if (data) {
		for (int i=0; i<r; i++) {
			data[i] = new double[c];
			ASSERT(data[i]);
			if (! data[i])
				allocated = FALSE;
		}
	} else
		allocated = FALSE;
	if (allocated) {
		rows = r;
		cols = c;
	} else
		Delete();
	return allocated;
}

void CDblMatrix::Delete()
{
	if (data) {
		for (int i=0; i<rows; i++) {
			if (data[i])
				delete []data[i];
		}
		delete []data;
		data = NULL;
	}
	rows = cols = 0;
}

CDblMatrix::~CDblMatrix(void)
{
	Delete();
}

int CDblMatrix::Rows()
{
	return rows;
}

int CDblMatrix::Cols()
{
	return cols;
}

CDblMatrix &CDblMatrix::operator =(double x)
{
	for (int i=0; i<rows; i++) {
		for (int j=0; j<cols; j++)
			data[i][j] = x;
	}
	return *this;
}

CDblMatrix &CDblMatrix::operator =(CDblMatrix &s)
{
	if (rows!=s.Rows() || cols!=s.Cols()) {
		if (rows)
			Delete();
		SetSize(s.Rows(), s.Cols());
	}
	ASSERT(rows==s.Rows() && cols==s.Cols());
	for (int i=0; i<rows; i++) {
		for (int j=0; j<cols; j++)
			data[i][j] = s[i][j];
	}
	return *this;
}

double *CDblMatrix::operator [](const int r)
{
	ASSERT(r>=0 && r<rows);
	return data[r];
}

BOOL CDblMatrix::ludcmp(int n, int *indx, double *vv, double &d)
{
	ASSERT(rows==cols);
	ASSERT(n<=rows);
	d=1.0;
	for (int i=0; i<n; i++) {
		double big = 0.0;
		for (int j=0; j<n; j++) {
			double temp = fabs(data[i][j]);
			if (temp > big)
				big=temp;
		}
		if (big == 0.0) 
			return FALSE;
		vv[i] = 1.0 / big;
	}
	for (int j=0; j<n; j++) {
		for (int i=0; i<j; i++) {
			double sum = data[i][j];
			for (int k=0; k<i; k++)
				sum -= data[i][k] * data[k][j];
			data[i][j] = sum;
		}
		double big = 0.0;
		int imax;
		for (int i=j; i<n; i++) {
			double sum = data[i][j];
			for (int k=0; k<j; k++)
				sum -= data[i][k] * data[k][j];
			data[i][j] = sum;
			double dum = vv[i]*fabs(sum);
			if (dum >= big) {
				big = dum;
				imax = i;
			}
		}
		if (j != imax) {
			for (int k=0; k<n; k++) {
				double dum = data[imax][k];
				data[imax][k] = data[j][k];
				data[j][k] = dum;
			}
			d = -d;
			vv[imax] = vv[j];
		}
		indx[j] = imax;
		if (data[j][j] == 0.0)
			data[j][j] = 1.0e-30;
		if (j != n-1) {
			double dum = 1.0 / data[j][j];
			for (int i=j+1; i<n; i++)
				data[i][j] *= dum;
		}
	}
	return TRUE;
}

void CDblMatrix::lubksb(int n, int *indx, double *b)
{
	ASSERT(n<=rows);
	int ii = 0;
	for (int i=0; i<n; i++) {
		int ip = indx[i];
		double sum = b[ip];
		b[ip] = b[i];
		if (ii) {
			for (int j=ii; j<=i-1; j++)
				sum -= data[i][j] * b[j];
		} else if (sum)
			ii = i;
		b[i] = sum;
	}
	for (int i=n-1; i>=0; i--) {
		double sum = b[i];
		for (int j=i+1; j<n; j++) 
			sum -= data[i][j] * b[j];
		b[i] = sum / data[i][i];
	}
}

void CDblMatrix::Minor(CDblMatrix &s, int xrow, int xcol)
{
	ASSERT(xrow>=0 && xrow<s.Rows());
	ASSERT(xcol>=0 && xcol<s.Cols());
	if (rows+1 != s.Rows() || cols+1 != s.Cols()) {
		SetSize(s.Rows()-1, s.Cols()-1);
	}
	int i, j, icount, jcount;
	for (i=0, icount=0; i<s.Rows(); i++) {
		if (i != xrow) {
			for (j=0, jcount=0; j<s.Cols(); j++) {
				if (j != xcol)
					data[icount][jcount++] = s[i][j];
			}
			icount++;
		}
	}
}

double CDblMatrix::Determinant(int n, int *indx, double *vv)
{
	ASSERT(rows == cols);
	ASSERT(n <= rows);
	double d;
	ludcmp(n, indx, vv, d);
	for (int i=0; i<n; i++)
		d *= data[i][i];
	return d;
}

#ifdef _DEBUG
void CDblMatrix::Dump(int r, int c)
{
	CStdioFile file;
	if (file.Open("c:\\documents and settings\\early\\desktop\\matrixA.txt", CFile::modeCreate|CFile::modeWrite|CFile::typeText)) {
		for (int i=0; i<r; i++) {
			for (int j=0; j<c; j++) {
				if (j) file.WriteString("\t");
				CString s;
				s.Format("%g", data[i][j]);
				file.WriteString(s);
			}
			file.WriteString("\n");
		}
		file.Close();
	}
}
#endif
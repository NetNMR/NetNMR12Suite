#pragma once

class CDblMatrix
{
private:
	int rows, cols;
	double **data;
	void Delete(void);
public:
	int Rows(void);
	int Cols(void);
	CDblMatrix(void);
	BOOL SetSize(int r, int c);
	~CDblMatrix(void);
	CDblMatrix &operator=(CDblMatrix &s);
	CDblMatrix &operator=(double x);
	double *operator[](const int r);
	BOOL ludcmp(int n, int *indx, double *vv, double &d);
	void lubksb(int n, int *indx, double *b);
	void Minor(CDblMatrix &s, int xrow, int xcol);
	double Determinant(int n, int *indx, double *vv);
#ifdef _DEBUG
	void Dump(int r, int c);
#endif
};

#pragma once

// CLine command target

class CNetSpanDoc;

class CLine : public CObject
{
public:
	CLine(double l, double w, double s, int size, CNetSpanDoc *pDocument, LPCTSTR newname);
	virtual ~CLine();
private:
	double shape, shift, width;
	CNetSpanDoc *pDoc;
public:
	UINT id;
	CString name;
	BOOL bshape, bshift, bwidth;
	double eshape, eshift, ewidth;
	CArray<double, double>line;
	int ParCount(void);
	double GetShape(void);
	double GetWidth(void);
	double GetShift(void);
	double ChangeShape(double newvalue);
	double ChangeWidth(double newvalue);
	double ChangeShift(double newvalue);
	void ReCalculate(void);
};



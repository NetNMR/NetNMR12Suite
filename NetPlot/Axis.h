// Axis.h: interface for the CAxis class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#define LARGETIC 0
#define SMALLTIC 1
#define NORMAL 0
#define REVERSE 1

class CNetPlotDoc;

class CAxis  
{
private:
	CNetPlotDoc *pDoc;
	void DrawTics(CDC *pDC, int size, double inc );
	void InitAxis();
	double anint( double x );
	double delta, xdif, xmin, xmax, ticratio;
	int sense, ww, wh;
	CString buf;
	CFont tickFont, axisFont;
public:
	void Update(CDC *pDC, int width, int height, COLORREF color );
	CAxis();
	virtual ~CAxis();
};

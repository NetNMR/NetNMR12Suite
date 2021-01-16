// Axis.cpp: implementation of the CAxis class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\NMRDataTypes.h"
#include "Spectrum.h"
#include "netplot.h"
#include "MainFrame.h"
#include "ChildFrame.h"
#include "NetPlotDoc.h"
#include "Axis.h"

#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAxis::CAxis()
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CChildFrame *pChild = STATIC_DOWNCAST(CChildFrame, pMain->MDIGetActive());
	pDoc = STATIC_DOWNCAST(CNetPlotDoc, pChild->GetActiveDocument() );
	tickFont.CreateFont( MulDiv(pDoc->nAxisSize, 100, 3), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Arial"	);
	axisFont.CreateFont( MulDiv(pDoc->nAxisSize, 100, 3), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Times New Roman" );
}

CAxis::~CAxis()
{
	tickFont.DeleteObject();
	axisFont.DeleteObject();
}

double CAxis::anint(double x)
{
	double ipart;
	double fract = modf( x, &ipart );
	if ( fabs( fract ) >= 0.5 ) {
		if ( x > 0.0 ) ipart++;
		else ipart--;
	}
	return( ipart );
}

void CAxis::InitAxis()
{
	CWaitCursor wait;
	/* axisfactor used if log10(delta) < 0.0 */
	double axisfactor = 0.0;
	
	/* first, estimate common log of delta */
	delta = log10(xdif / anint(0.25 * (double)pDoc->Width));

	if ( delta < 0.0 ) {
		/* move delta to positive number and remember necessary factor */
		axisfactor = floor( delta );
		delta -= axisfactor;
	}

	/* calculate mantissa of delta */
	double inc = pow( 10.0, modf( delta, &delta ) );

	/* round mantissa to 1, 2 or 5 and set up large/small tic ratio */
	ticratio = 5.0;
	if ( inc < 2.0 ) {
		inc = 1.0;
	} else {
		if ( inc < 5.0 ) {
			inc = 2.0;
			ticratio = 4.0;
		} else
			inc = 5.0;
	}

	/* attach exponent to mantissa to generate exact delta */
	delta = inc * pow( 10.0, delta );
	
	/* and attach factor */
	if ( axisfactor < 0.0 ) delta *= pow( 10.0, axisfactor );
}

void CAxis::DrawTics(CDC *pDC, int size, double inc)
{
	CWaitCursor wait;
	double axstart, axstop;
	/* set up parameters according to arguments */
	int ticlength = ( size == SMALLTIC ) ? 200 : 300;
	double axislength = (double)( ww - 1 );
	int places = 0;
	if (inc < 1.0)
		places = int(-floor(log10(inc)));
	if ( sense == NORMAL ) { axstart = xmin; axstop = xmax; }
	else { axstart = xmax; axstop = xmin; }

	/* initialize the value of the first tic mark */
	double lvalue = axstart / inc;
	if ( sense == NORMAL ) lvalue = ceil( lvalue );
	else lvalue = floor( lvalue );
	lvalue *= inc;

	/* calculate the relative pixel position of first tic mark */
	double cpos = fabs( axislength * ( axstart - lvalue ) / ( axstart - axstop ) );

	/* calculate pixel increment for subsequent marks */
	double axinc = axislength * inc / fabs( axstart - axstop );

	/* loop for all the tics */
	while ( cpos <=  axislength ) {

		int x = int(cpos);
		int y = wh;
		/* paint tic */
		pDC->MoveTo( x, y );
		y -= ticlength;
        pDC->LineTo( x, y );

		/* how about a label? */
		if ( size == LARGETIC ) {
			
			/* print label value in memory and paint it*/
			if (places) {
				CString fstr;
				fstr.Format("%%.%df", places);
				buf.Format(fstr, lvalue);
			} else
				buf.Format( "%.10g", lvalue );
			if ( ! buf.Compare( "-0" ) ) buf = "0";
			pDC->TextOut( x, y, buf );
		}
		/* increment label value, depending on sense of axis */
		if ( sense == NORMAL )
			lvalue += delta;
		else
			lvalue -= delta;
			
		/* round "fuzz" off label value, otherwise,
								zero might print as x.xxxxe-17 */
		lvalue = inc * anint( lvalue / inc );
		
		/* increment relative tic position */
		cpos += axinc;
	}
}

void CAxis::Update(CDC *pDC, int width, int height, COLORREF color)
{	// width is the width in logical units;
	// wwidth is the window width in device units;
	if (0==width || pDoc->RangeFrom==pDoc->RangeTo)
		return;
	CWaitCursor wait;
	CString str = pDoc->Range==RangeHertz ? "Hertz": "PPM";
	if ( pDoc->RangeFrom > pDoc->RangeTo ) {
		xmin = pDoc->RangeTo;
		xmax = pDoc->RangeFrom;
		xdif = xmax - xmin;
		sense = REVERSE;
	} else {
		xmin = pDoc->RangeFrom;
		xmax = pDoc->RangeTo;
		xdif = xmax - xmin;
		sense = NORMAL;
	}
	ww = width;
	wh = -150;
	CFont *oldFont = pDC->SelectObject( &tickFont );
	CPen basePen, tickPen;
	basePen.CreatePen( PS_SOLID, 50, color );
	tickPen.CreatePen( PS_SOLID,  0, color );
	COLORREF oldColor = pDC->SetTextColor(color);
	CPen *oldPen = pDC->SelectObject( &basePen );
	UINT oldAlign = pDC->SetTextAlign(TA_TOP|TA_CENTER|TA_NOUPDATECP);
	pDC->MoveTo( 0, wh );
	pDC->LineTo( width, wh );
	InitAxis();
	pDC->SelectObject( &tickPen );
	DrawTics( pDC, LARGETIC, delta );
	DrawTics( pDC, SMALLTIC, delta/ticratio );
	pDC->SelectObject( &axisFont );
	pDC->TextOut( ww/2, wh-pDoc->nAxisSize*50, str );
	pDC->SetTextAlign( oldAlign );
	pDC->SetTextColor(oldColor);
	pDC->SelectObject( oldFont );
	pDC->SelectObject( oldPen );
}

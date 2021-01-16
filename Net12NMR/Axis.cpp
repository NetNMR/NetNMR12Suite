#include "StdAfx.h"
#include "resource.h"
#include "MainFrame.h"
#include "Axis.h"
#include "Net12NMR.h"
#include ".\Axis.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAxis::CAxis()
{
	tickFont.CreateFont( 600, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Arial"	);
	axisFont.CreateFont( 600, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Times New Roman" );
}

CAxis::~CAxis()
{
	tickFont.DeleteObject();
	axisFont.DeleteObject();
}

// anint() returns the nearest integral value to x
// halfway case are rounded to the value larger in magnitude
double CAxis::anint( double x )
{
	double ipart;
	double fract = modf( x, &ipart );
	if ( fabs( fract ) >= 0.5 ) {
		if ( x > 0.0 ) ipart++;
		else ipart--;
	}
	return( ipart );
}

/* calculates delta, increment of tic marks in user units	*/
/* also sets ratio of number of small tics to large tics	*/
void CAxis::InitAxis( int size, int length )
{
	/* axisfactor used if log10(delta) < 0.0 */
	double axisfactor = 0.0;
	
	/* first, estimate common log of delta */
	delta = log10( xdif / anint( (double)size / (double)length + 0.5 ) );

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

	return;
}

void CAxis::DrawTics( CDC *pDC, int size, double inc )
{
	double axstart, axstop;
	/* set up parameters according to arguments */
	int places = 0;
	if (inc < 1.0)
		places = int(-floor(log10(inc)));
	int ticlength = ( size == SMALLTIC ) ? 200 : 300;
	double axislength = (double)( ww - 1 );
	if ( sense == NORMAL ) { axstart = xmin; axstop = xmax; }
	else { axstart = xmax; axstop = xmin; }

	/* initialize the value of the first tic mark */
	double lvalue = axstart / inc;
	if ( sense == NORMAL )
		lvalue = ceil( lvalue );
	else
		lvalue = floor( lvalue );
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
				buf.Format("%.10g", lvalue );
			if ( ! buf.Compare("-0") ) buf = "0";
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
	return;
}

void CAxis::Update(CDC *pDC, int width, int wwidth, double start, double stop, LPCTSTR label, int hint)
{	// width is the width in logical units;
	// wwidth is the window width in device units;
	COLORREF oldColor;
	CString str = label;
	if ( start > stop ) {
		xmin = stop;
		xmax = start;
		xdif = start - stop;
		sense = REVERSE;
	} else {
		xmin = start;
		xmax = stop;
		xdif = stop - start;
		sense = NORMAL;
	}
	//int hint = pDC->GetDeviceCaps(LOGPIXELSX);
	ww = width;
	wh = -200;
	CFont *oldFont = pDC->SelectObject( &tickFont );
	CPen basePen, tickPen;
	basePen.CreatePen( PS_SOLID, 50, theApp.Color[axis_Color]);
	tickPen.CreatePen( PS_SOLID,  0, theApp.Color[axis_Color]);
	CPen *oldPen = pDC->SelectObject( &basePen );
	oldColor = pDC->SetTextColor(theApp.Color[axis_Color]);             // Axis Color
	int oldMode = pDC->SetBkMode( TRANSPARENT );
	UINT oldAlign = pDC->SetTextAlign(TA_TOP|TA_CENTER|TA_NOUPDATECP);
	pDC->MoveTo( 0, wh );
	pDC->LineTo( width, wh );
	InitAxis( width, MulDiv( hint, 3 * width, wwidth ) );
	pDC->SelectObject( &tickPen );
	DrawTics( pDC, LARGETIC, delta );
	DrawTics( pDC, SMALLTIC, delta/ticratio );
	pDC->SelectObject( &axisFont );
	pDC->TextOut( 10000, -1100, str );
	pDC->SetTextAlign( oldAlign );
	pDC->SetBkMode( oldMode );
	pDC->SetTextColor( oldColor );
	pDC->SelectObject( oldFont );
	pDC->SelectObject( oldPen );
	return;
}
#pragma once

class CAxis
{
public:
	CAxis(CNetComPlotDoc *pDoc);
	~CAxis(void);
	void Draw(CDC *pDC);
private:
	CFont font;
	double dSize[2];
	double dFrom[2], dTo[2];
	double dDelta[2], dTicratio[2];
	void Tics(CDC *pDC, BOOL bLabel, int i);
	COLORREF color;
};

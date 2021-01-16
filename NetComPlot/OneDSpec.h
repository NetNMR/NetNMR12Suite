#pragma once
#include "specdim.h"
#include "atltypes.h"

typedef struct onedparamtag {
	double floor, ceiling;
	int color;
} ONEDPARAM;

class COneDSpec
{
public:
	ONEDPARAM param;
	COneDSpec(void);
	~COneDSpec(void);
	CSpecDim SpecDim;
	int *dt;
	CString strInfo;
	BOOL Initialize(void);
	int iMin, iMax;
	void GetRange(void);
	void InitTransform(float fSize, float fHeight, float fFrom, float fTo, int nRange);
	CPoint Tranform(int x, int y, BOOL bIsHoriz);
	void Archive(CArchive & ar);
	CRect scrRect;
private:
	float fSlope, fYint, fYSlope;
	int nYmin, nYmax;
};

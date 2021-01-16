#pragma once
#include "specdim.h"

typedef enum emodetag {
	posMode = 0,
	negMode = 1,
	bothMode = 2
} EMODE;

typedef enum espacingtag {
	logSpacing = 0,
	linSpacing = 1
} ESPACING;

typedef struct twodparamtag {
	EMODE mode;
	int contour;
	ESPACING spacing;
	double floor, ceiling;
	int poscolor, negcolor;
} TWODPARAM;

class CTwoDSpec
{
public:
	TWODPARAM param;
	CTwoDSpec(void);
	~CTwoDSpec(void);
	CSpecDim HSpecDim;
	CSpecDim VSpecDim;
	int **dt;
	CString strInfo, strDataClass;
	double iMax;
	int arrContour[9], posContour[9], negContour[9];
	void GetRange(void);
	BOOL Initialize(void);
	void InitTransform(float *f2DSize, float *fFrom, float *fTo, int hRange, int vRange);
	CPoint Translate(float x, float y);
private:
	float fHSlope, fVSlope, fHYint, fVYint;
public:
	void Archive(CArchive & ar);
	void Symmetrize(UINT nID);
};

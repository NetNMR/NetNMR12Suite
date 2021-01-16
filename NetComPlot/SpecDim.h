#pragma once



// CSpecDim command target

class CSpecDim
{
public:
	CSpecDim();
	virtual ~CSpecDim();
private:
	float freq;
	float sw;
	float of;
	int np;
	float hzperpt, yint;
public:
	float Frequency(void);
	float SpecWidth(void);
	float Offset(void);
	int Size(void);
	void Initialize(float f, float s, float o, int n);
	void ChangeOffset(float newoffset, float atn);
	float N2Hertz(float n);
	float N2PPM(float n);
	int from;
	int to;
	float Hertz2N(float hertz);
	float PPM2N(float ppm);
	void Archive(CArchive & ar);
};



#pragma once

#define LARGETIC 0
#define SMALLTIC 1
#define NORMAL 0
#define REVERSE 1


class CAxis {
private:
	double delta, xdif, xmin, xmax, ticratio;
	int sense, ww, wh;
	CString buf;
	CFont tickFont, axisFont;

	void InitAxis(int size, int length);
	void DrawTics(CDC *pDC, int size, double inc);
	double anint(double x);
public:
	CAxis();
	virtual ~CAxis();
	void Update(CDC *pDC, int width, int height, double start, double stop, LPCTSTR label, int hint);
};

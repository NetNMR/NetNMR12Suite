#pragma once

#include "stdafx.h"

class CNormalRandom
{
private:
	// data
	bool iset;
	static const int M1 = 25900;
	static const int M2 = 134456;
	static const int M3 = 243000;
	static const int IA1 = 7141;
	static const int IA2 = 8121;
	static const int IA3 = 4561;
	static const int IC1 = 54773;
	static const int IC2 = 28411;
	static const int IC3 = 51349;

	int idum;
	int ix1, ix2, ix3;
	double r[98];
	bool iff;
	
	double gset;

	double Rand()
	{
		if (! idum || ! iff) {
			iff = true;
			ix1 = (IC1 - idum) % M1;
			ix1 = (IA1*ix1+IC1) % M1;
			ix2 = ix1 % M2;
			ix1 = (IA1 * ix1 + IC1) % M1;
			ix3 = ix1 % M3;
			for (int j=1; j<=97; j++) {
				ix1 = (IA1 * ix1 + IC1) % M1;
				ix2 = (IA2 * ix2 + IC2) % M2;
				r[j] = (ix1 + ix2 * (1.0 / M2)) * (1.0 / M1);
			}
			idum = 1;
		}
		ix1 = (IA1 * ix1 + IC1) % M1;
		ix2 = (IA2 * ix2 + IC2) % M2;
		ix3 = (IA3 * ix3 + IC3) % M3;
		int j = 1 + ((97 *ix3) / M3);
		if (j>97 || j<1)
			AfxMessageBox("Rand() ERROR: r index j out of range!\n", MB_ICONERROR);
		double temp = (double)r[j];
		r[j] = (ix1 + ix2 * (1.0 / M2)) * (1.0 / M1);
		return temp;
	}

public:
	CNormalRandom(int seed)
	{
		if (seed == 0)
			seed = -1;
		if (seed > 0)
			seed = -seed;
		idum = seed;
		iff = false;
		iset = false;
	}

	~CNormalRandom() {}

	double get()
	{
		if (! iset) {
			double v1, v2, r;
			do {
				v1 = 2.0 * Rand() - 1.0;
				v2 = 2.0 * Rand() - 1.0;
				r = v1 * v1 + v2 * v2;
			} while (r >= 1.0);
			double fac = sqrt(-2.0 * log(r) / r);
			gset = v1 * fac;
			iset = true;
			return v2 * fac;
		} else {
			iset = false;
			return gset;
		}
	}
};
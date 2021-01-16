// Spectrum.h: interface for the CSpectrum class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

class CNMRServer;

class CSpectrum  
{
public:
	// methods
	CSpectrum();
	virtual ~CSpectrum();
	void Add(vector<complex<float>>&);
	void AutoPhase();
	void AutoPhaseAOnly();
	void Apodize(int type, double value, int units);
	void CalcBaseline(int order);
	void CalcStartStop();
	void CalcToFrom();
	void Copy(CSpectrum *);
	void DCCorrect();
	void FFT();
	void FHSet(LPCTSTR key, LPCTSTR type, LPCTSTR value);
	bool FHGet(LPCTSTR key, CString &value);
	void FHRemove(LPCTSTR key);
	void FHGetNames(CStringList &list);
	void File(CFile &file);
	void FileX(CFile &file);
	void FindBaseline(int width, float sn, int start, int stop);
	void FindMinMax(bool usedata=true);
	double Frequency();
	float GetAt(int i);
	float GetBaseAt(int i);
	int GetClass();
	void GetFromToRange(float &RangeFrom, float &RangeTo);
	void GetLabel();
	float GetYExpansion();
	float GetYRange();
	void GlobalPhase();
	void Magnitude();
	double Offset();
	void Serialize(CArchive &ar);
	bool Server(CNMRServer &server, LPCTSTR str);
	void SetGlobalPhase();
	void SetOffset(double offset);
	void SetYExpansion(float y);
	UINT Size();
	int Start();
	int Stop();
	void SubtractBaseline(int order);
	void SubtractData(vector<complex<float>>&, float scale);
	void SubtractRaw(vector<complex<float>>&, float scale);
	double SweepWidth();
	int Translate(int h, float range, float ym, float yf);
	float Ymax();
	float Ymin();
	void ZeroFill(bool bigger);
	// values
	int from, to, color;
	CString pathname, label;
private:
	// data
	vector<complex<float>> raw;
	vector<float> data;
	vector<float> baseline;
	CTypedPtrMap<CMapStringToOb, CString, CMapStringToString *> fh;
	double sweepwidth, frequency, offset;
	UINT npoints;
	bool ftflag, f1_lo_sideband;
	float phasea, phaseb, phasec;
	int dataclass;
	float yexpansion;
	float param[3][2];
	double basecoef[10];
	int start, stop;
	float ymax, ymin;
	float yvals[3];
	// methods
	void Bracket(float &ax, float &bx, float &cx, float&fa, float &fb, float &fc);
	float AmoTry(int ihi, int &nfunk, float fac);
	float EvalPhase(void);
	void Initialize();
	void SHFT(float &a, float &b, float &c, float d);
	void GetFixedHeaderItems(HD &hd);
};

#pragma once

class CTerm
{
public:
	double a, r;
	double la, lb, lc;
	CTerm(void) {
		a = r = la = lb = lc = 0.0;
	}

	virtual ~CTerm(void) {}

	virtual double Evaluate(const double x) {return 0.0;}

	virtual double PartDerA(const double x) {return 0.0;}

	virtual double PartDerR(const double x) {return 0.0;}

	void Insert(CTypedPtrList<CPtrList, CTerm *> &list)
	{
		if (list.IsEmpty())
			list.AddHead(this);
		else {
			POSITION pos = list.GetHeadPosition();
			while (pos) {
				CTerm *term = list.GetAt(pos);
				if (r > term->r) {
					list.InsertBefore(pos, this);
					return;
				}
				list.GetNext(pos);
			}
		}
	}

	void Serialize(CArchive &ar)
	{
		if (ar.IsStoring()) {
			ar << a << r << la << lb << lc;
		} else {
			ar >> a >> r >> la >> lb >> lc;
		}
	}
};

class CDTerm : public CTerm
{
public:
	CDTerm(void) {}

	~CDTerm(void){}

	double Evaluate(const double x)
	{
		return a * exp(-x * r);
	}

	double PartDerA(const double x)
	{
		return exp(-x * r);
	}

	double PartDerR(const double x)
	{
		return -x * a * exp(-x * r);
	}
};

class CRTerm : public CTerm
{
public:
	CRTerm(void) {}

	~CRTerm(void) {}

	double Evaluate(const double x)
	{
		return a * (1.0 - exp(-x * r));
	}

	double PartDerA(const double x)
	{
		return 1.0 - exp(-x * r);
	}

	double PartDerR(const double x)
	{
		return x * a * exp(-x * r);
	}
};

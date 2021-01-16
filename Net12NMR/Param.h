#pragma once

class CNet12NMRDoc;

class CParam {
private:
	int x;
	int y;
	CDC *aDC;
	CNet12NMRDoc *aDoc;
	CFont nameFont, headFont, unitFont;
	void Heading(LPCTSTR str);
	void Parameter(LPCTSTR name, LPCTSTR value, LPCTSTR units);
	void Decoupler(int decoupler, DECOUPLER &sdec, const CString &nucleus, const CString decmode);
public:
	CParam(int height);
	virtual ~CParam();
	void Draw(CDC *pDC, CNet12NMRDoc *pDoc);
};

#pragma once

class CProcessPage;
class CPhasePage;
class CSubtractPage;
class CRegionsPage;
class CYExpandPage;

// CProcSheet

class CProcSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CProcSheet)

public:
	CProcSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CProcSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CProcSheet();
	void UpdateContent();

// Attributes
private:
	CProcessPage *processPage;
	CPhasePage *phasePage;
	CRegionsPage *regionsPage;
	CYExpandPage *yexpandPage;
	CSubtractPage *subtractPage;

protected:
	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
};



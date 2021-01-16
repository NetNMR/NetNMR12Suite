#pragma once

// CTabbedDlg

class CLineTab;
class CSpectraTab;
class CFitTab;
class CNetSpanDoc;

class CTabbedDlg : public CPropertySheet
{
	DECLARE_DYNAMIC(CTabbedDlg)

public:
	CTabbedDlg(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CTabbedDlg(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CTabbedDlg();

protected:
	DECLARE_MESSAGE_MAP()
public:
	CLineTab *lineTab;
	CSpectraTab *specTab;
	CFitTab *fitTab;
	afx_msg void OnClose();
private:
	CNetSpanDoc *pDoc;
public:
	void ShowAllPages(bool bShow);
	void RefillSpecList(void);
};



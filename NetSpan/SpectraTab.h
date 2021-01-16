#pragma once
#include "afxwin.h"


// CSpectraTab dialog
class CNetSpanView;
class CNetSpanDoc;
class CMainFrame;

class CSpectraTab : public CPropertyPage
{
	DECLARE_DYNAMIC(CSpectraTab)

public:
	CSpectraTab();
	virtual ~CSpectraTab();

// Dialog Data
	enum { IDD = IDD_SPECTRADIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_spectraList;
	CButton m_changeButton;
	CButton m_removeButton;
	CButton m_moveupButton;
	CButton m_movedownButton;
	afx_msg void OnBnClickedChangebutton();
	afx_msg void OnBnClickedRemovebutton();
	afx_msg void OnBnClickedMoveupbutton();
	afx_msg void OnBnClickedMovedownbutton();
	afx_msg void OnLbnSelchangeSpectralist();
	afx_msg void OnBnClickedViewbutton();
	virtual BOOL OnInitDialog();
private:
	CMainFrame *pMain;
	CNetSpanView *pView;
	CNetSpanDoc *pDoc;
	CArray <int, int> arraySelection;
public:
	void RefillSpecList(void);
};

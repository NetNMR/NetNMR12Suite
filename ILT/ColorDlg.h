#pragma once
#include "afxwin.h"

// CStaticRect

class CStaticRect : public CStatic
{
	DECLARE_DYNAMIC(CStaticRect)

public:
	CStaticRect();
	virtual ~CStaticRect();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	COLORREF color;
};


// CColorDlg dialog

class CColorDlg : public CDialog
{
	DECLARE_DYNAMIC(CColorDlg)

public:
	CColorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CColorDlg();

// Dialog Data
	enum { IDD = IDD_COLORDIALOG };
	COLORREF colors[NCOLORS];

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	CStaticRect m_staticRect;
	CComboBox m_cboxName;
	afx_msg void OnCbnSelchangeColorcombo();
	afx_msg void OnBnClickedChangebutton();
	afx_msg void OnBnClickedBonwbutton();
	afx_msg void OnBnClickedWonbbutton();
	afx_msg void OnBnClickedDefaultbutton();
};

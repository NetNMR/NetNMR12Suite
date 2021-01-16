#pragma once
#include "afxwin.h"


// CParamDlg dialog

class CParamDlg : public CDialog
{
	DECLARE_DYNAMIC(CParamDlg)

public:
	CParamDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CParamDlg();

// Dialog Data
	enum { IDD = IDD_PARAMETERDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg void OnHorizRange(UINT uID);
	afx_msg void OnVertRange(UINT uID);
	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
public:
	int m_nHRange;
	int m_nVRange;
	CEdit m_editHFrom;
	CEdit m_editHTo;
	CEdit m_editVFrom;
	CEdit m_editVTo;
	CEdit m_editWidth;
	CEdit m_editHeight;
	CEdit m_edit1DHeight;
	CEdit m_edit1DOffset;
	CComboBox m_cboxPageColor;
	CComboBox m_cboxAxisColor;
	CComboBox m_cboxInfoColor;
	CComboBox m_cbox2DFile;
	CComboBox m_cboxNumContours;
	CEdit m_editFloor;
	CEdit m_editCeiling;
	CComboBox m_cboxSpacing;
	CComboBox m_cboxMode;
	CComboBox m_cboxPosColor;
	CComboBox m_cboxNegColor;
	CComboBox m_cboxHFile;
	CEdit m_editHFloor;
	CEdit m_editHCeiling;
	CComboBox m_cboxHColor;
	CComboBox m_cboxVFile;
	CEdit m_editVFloor;
	CEdit m_editVCeiling;
	CComboBox m_cboxVColor;
	// pDoc is set by the calling function, right after creation
	CNetComPlotDoc *pDoc;
	virtual BOOL OnInitDialog();
	TWODPARAM *params2d;
	ONEDPARAM *paramsh, *paramsv;

	afx_msg void OnCbnSelchange2dfilecombo();
	afx_msg void OnCbnSelchangeModecombo();
	afx_msg void OnCbnSelchangeNumbercombo();
	afx_msg void OnCbnSelchangeSpacingcombo();
	afx_msg void OnEnChangeFlooredit();
	afx_msg void OnEnChangeCeilingedit();
	afx_msg void OnCbnSelchangeNegcolorcombo();
	afx_msg void OnCbnSelchangePoscolorcombo();
	afx_msg void OnCbnSelchangeHfilecombo();
	afx_msg void OnEnChangeHceilingedit();
	afx_msg void OnEnChangeHflooredit();
	afx_msg void OnCbnSelchangeHcolorcombo();
	afx_msg void OnCbnSelchangeVfilecombo();
	afx_msg void OnEnChangeVceilingedit();
	afx_msg void OnEnChangeVflooredit();
	afx_msg void OnCbnSelchangeVcolorcombo();
	afx_msg void OnBnClickedFullhbutton();
	afx_msg void OnBnClickedFullvbutton();
private:
	int ncurrent2d, ncurrenth, ncurrentv;
};

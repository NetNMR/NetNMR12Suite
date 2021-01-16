#pragma once
#include "afxwin.h"
#include "afxcmn.h"

// CLineTab dialog

class CMainFrame;
class CNetSpanView;
class CNetSpanDoc;

class CLineTab : public CPropertyPage
{
	DECLARE_DYNAMIC(CLineTab)

public:
	CLineTab();
	virtual ~CLineTab();
private:
	CMainFrame *pMain;
	CNetSpanView *pView;
	CNetSpanDoc *pDoc;

// Dialog Data
public:
	enum { IDD = IDD_LINEDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CComboBox m_nameCombo;
	afx_msg void OnCbnSelchangeNameCombo();
	afx_msg void OnBnClickedRenameButton();
	afx_msg void OnBnClickedDeleteButton();
	afx_msg void OnCbnDropDownNameCombo();
	afx_msg void OnBnClickedShapecheck();
	afx_msg void OnBnClickedWidthcheck();
	afx_msg void OnBnClickedShiftcheck();
	afx_msg void OnEnKillfocusShapeedit();
	afx_msg void OnEnKillfocusWidthedit();
	afx_msg void OnEnKillfocusShiftedit();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	CSliderCtrl m_shapeSlider;
	CSliderCtrl m_widthSlider;
	CSliderCtrl m_shiftSlider;
	CEdit m_shapeEdit;
	CEdit m_widthEdit;
	CEdit m_shiftEdit;
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};

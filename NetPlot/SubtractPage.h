#pragma once
#include "afxwin.h"
#include "afxcmn.h"

// CSubtractPage dialog

class NetPlotDoc;

class CSubtractPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CSubtractPage)

public:
	CSubtractPage();
	virtual ~CSubtractPage();

// Dialog Data
	enum { IDD = IDD_SUBTRACT_PAGE };

private:
	CNetPlotDoc *GetDocument();
	void ShowSubtract(int);

public:
	vector<complex<float>> sum;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	float m_leftFloat;
	float m_rightFloat;
	float m_subtractFloat;
	CListBox m_subtractListBox;
	CButton m_subtractCheck;
	CButton m_subtractcancelButton;
	CSliderCtrl m_subtractSlider;
	CStatic m_subtractgroupStatic;
	CEdit m_subtractfromEdit;
	CEdit m_subtracttoEdit;
	CStatic m_subtractStatic;
	CStatic m_subtractfromStatic;
	CStatic m_subtracttoStatic;
	afx_msg void OnBnClickedSubtractcheck();
	afx_msg void OnBnClickedSubtractcancelbutton();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnEnChangeLeftedit();
	afx_msg void OnEnChangeRightedit();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
};

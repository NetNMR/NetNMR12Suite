#pragma once

/////////////////////////////////////////////////////////////////////////////
// CParamDlg dialog

class CNetPlotDoc;

class CParamDlg : public CDialog
{
// Construction
public:
	CParamDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	enum { IDD = IDD_PARAMETERS };
	BOOL	m_GlobalScaling;
	//CString	m_Label;
	CString	m_Title;
	int		m_Range;
	float	m_LabelX;
	float	m_LabelY;
	float	m_OffsetX;
	float	m_OffsetY;
	float	m_RangeFrom;
	float	m_RangeTo;
	float	m_Width;
	float	m_Height;
	float	m_YExpansion;
	CComboBox m_labelCombo;
	CComboBox m_titlefontCombo, m_labelfontCombo, m_axisfontCombo;


// Overrides
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnRangefull();
	afx_msg void OnRangehertz();
	afx_msg void OnRangeppm();
	afx_msg void OnChangeLabelname();
	afx_msg void OnAutoarrange();
	DECLARE_MESSAGE_MAP()
private:
	bool IsDifferent( float a, float b );
//	void ShowLabelControls();
	void OnRange(int rangeid);
private:
	CNetPlotDoc *pDoc;
};

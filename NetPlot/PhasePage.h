#pragma once
#include "afxwin.h"

class CNetPlotDoc;
class CNetPlotView;
class CMainFrame;

// CPhasePage dialog

class CPhasePage : public CPropertyPage
{
	DECLARE_DYNAMIC(CPhasePage)

public:
	CPhasePage();
	virtual ~CPhasePage();

private:
	BOOL allcheck;
	int zleft, zright;
	float savedA, savedB;
	void AllCheckClicked(void);
	void OnBnClickedSpecbutton(bool go_up);

// Dialog Data
	enum { IDD = IDD_PHASE_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

private:
	CNetPlotDoc *GetDocument();
	CNetPlotView *GetView();

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_phasepromptStatic;
	CButton m_allButton;
	CButton m_cloneButton;
	CButton m_cloneupButton;
	CButton m_clonedownButton;
	CButton m_atcursorCheck;
	CComboBox m_phaseselectCombo;
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnBnClickedPhaseautoabutton();
	afx_msg void OnBnClickedPhaseautoabbutton();
	afx_msg void OnBnClickedMagnitudebutton();
	afx_msg void OnCbnSelchangePhaseselectcombo();
	afx_msg void OnBnClickedAllcheck();
	afx_msg void OnBnClickedClonebutton();
	afx_msg void OnBnClickedAtcursorcheck();
	afx_msg void OnBnClickedUpbutton();
	afx_msg void OnBnClickedDownbutton();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedLeftphasebutton();
	afx_msg void OnBnClickedRightphasebutton();
	afx_msg void OnBnClickedCloneupbutton();
	afx_msg void OnBnClickedClonedownbutton();
	afx_msg void OnBnClickedUndophasebutton();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnCbnSetfocusPhaseselectcombo();
	afx_msg void OnBnClickedSpecupbutton();
	afx_msg void OnBnClickedSpecdownbutton();
};

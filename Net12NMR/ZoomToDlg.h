#pragma once


// CZoomToDlg dialog

class CZoomToDlg : public CDialog
{
	DECLARE_DYNAMIC(CZoomToDlg)

public:
	CZoomToDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CZoomToDlg();

// Dialog Data
	enum { IDD = IDD_ZOOMTO };
	CStatic	m_MsgCtrl;
	CButton	m_Time;
	CButton	m_Pts;
	CButton	m_PPM;
	CButton	m_Hertz;
	CString	m_ValueA;
	CString	m_ValueB;
	BOOL	m_bHertz;
	BOOL	m_bPPM;
	BOOL	m_bPTS;
	BOOL	m_bTime;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnZoomtoPts();
	afx_msg void OnZoomtoTime();
	afx_msg void OnZoomtoHertz();
	afx_msg void OnZoomtoPpm();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
private:
	TCHAR *ptsmsg, *timemsg, *hertzmsg, *ppmmsg;
	CString To, From;
	int Check;
};

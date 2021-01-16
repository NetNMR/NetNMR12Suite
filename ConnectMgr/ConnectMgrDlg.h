// ConnectMgrDlg.h : header file
//

#pragma once

#include "ServerList.h"

// CConnectMgrDlg dialog
class CConnectMgrDlg : public CDialog
{
// Construction
public:
	CConnectMgrDlg(CWnd* pParent = NULL);	// standard constructor
	CServerList Server;

// Dialog Data
	enum { IDD = IDD_CONNECTMGR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedEditbutton();
public:
	CComboBox m_serverCBox;
	virtual BOOL DestroyWindow();
};

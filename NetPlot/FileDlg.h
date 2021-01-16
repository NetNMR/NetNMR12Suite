#pragma once
#include "afxwin.h"
/////////////////////////////////////////////////////////////////////////////
// CFileDlg dialog

class CNetPlotDoc;

class CFileDlg : public CDialog
{
// Construction
public:
	CFileDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	enum { IDD = IDD_FILEDIALOG };
	CComboBox	m_DriveList;
	CButton	m_RemoveAll;
	CButton	m_Remove;
	CButton m_Reverse;
	CButton	m_MoveUp;
	CButton	m_MoveDown;
	CButton m_addButton;
	CStatic	m_CurrentPath;
	CListBox m_FileList;
	CListBox m_DirList;
	CListBox m_ServerList;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	CNMRServer NMRPort;
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeFilelist();
	afx_msg void OnRemove();
	afx_msg void OnRemoveall();
	afx_msg void OnBnClickedReverse();
	afx_msg void OnMovedown();
	afx_msg void OnMoveup();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSelchangeServerlist();
	afx_msg void OnSelchangeDriveCombo();
	afx_msg void OnBnClickedAddbutton();
	afx_msg void OnLbnSelchangeDirlist();
	DECLARE_MESSAGE_MAP()
private:
	//void MoveString( int direction );
	void FillDirList();
	CString currentname;
	void SetHExtFileList(void);
	CNetPlotDoc *pDoc;
	CArray <int, int> arraySelection;
};

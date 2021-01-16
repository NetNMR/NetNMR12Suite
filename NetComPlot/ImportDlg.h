#pragma once

// CImportDlg dialog

class CImportDlg : public CDialog
{
	DECLARE_DYNAMIC(CImportDlg)

public:
	CImportDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CImportDlg();

	enum FileType { IsDirectory, IsOneDNMR, IsTwoDNMR };

// Dialog Data
	enum { IDD = IDD_IMPORTDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_listServer;
	CListBox m_listFiles;
	CListBox m_listTwoD;
	CListBox m_listHOneD;
	CListBox m_listVOneD;
	CButton m_butVerttoHoriz;
	CButton m_butHoriztoVert;
	CButton m_butMoveUp;
	CButton m_butMoveDown;
	CButton m_butRemove;
	CButton m_butRemoveAll;
	CButton m_butEditInfo;
	CButton m_butProjection;
	CStatic m_staticCurrentDirectory;
	virtual BOOL OnInitDialog();
	void FillDirList(void);
	CNetComPlotDoc *pDoc;
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg void OnLbnSelchangeServerlist();
	afx_msg void OnLbnSelchangeTwodlist();
	afx_msg void OnLbnSelchangeHonedlist();
	afx_msg void OnLbnSelchangeVonedlist();
	afx_msg void OnBnClickedRemoveall();
	afx_msg void OnBnClickedRemove();
	afx_msg void OnBnClickedHoriztovert();
	afx_msg void OnBnClickedVerttohoriz();
	afx_msg void OnBnClickedMovedown();
	afx_msg void OnBnClickedMoveup();
	afx_msg void OnBnClickedEditinfo();
	afx_msg void OnBnClickedProjection();
	afx_msg void OnLbnDblclkFilelist();
private:
	BOOL bIsModified;
	CTypedPtrList<CPtrList, CTwoDSpec *> Remove2DList;
	CTypedPtrList<CPtrList, COneDSpec *> Remove1DList;
	void MoveString(int direction);
	void MoveBetweenHandV(CListBox & listFrom, CListBox & listTo);
	CTwoDSpec *Get2DSpectrum(LPCTSTR str);
	COneDSpec * Get1DSpectrum(LPCTSTR str);
	CNetNMRClient NetNMRClient;
	void OnLbnSelchange(CListBox &pList);
	CString *pstrInfo;
};

#pragma once
#include "afxwin.h"
#include "NetNMRClient.h"
#include "Spectrum.h"

// CAddSpectraDlg dialog

class CAddSpectraDlg : public CDialog
{
	DECLARE_DYNAMIC(CAddSpectraDlg)

public:
	CAddSpectraDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAddSpectraDlg();
	CTypedPtrList<CObList, CSpectrum *> Spectra;

// Dialog Data
	enum { IDD = IDD_ADDSPECTRADIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int np;
	double sw, offset, sf;
	CStatic m_pathStatic;
	CStatic m_driveStatic;
	CListBox m_serverList;
	CListBox m_driveList;
	CListBox m_directoryList;
	CListBox m_fileList;
	CButton m_gotoButton;
	CButton m_okButton;
	virtual BOOL OnInitDialog();
	afx_msg void OnLbnSelchangeServerlist();
	afx_msg void OnLbnSelchangeDrivelist();
	afx_msg void OnLbnDblclkFilelist();
	afx_msg void OnLbnDblclkDirectorylist();
	afx_msg void OnLbnSelchangeDirectorylist();
	afx_msg void OnLbnSelchangeFilelist();
	afx_msg void OnBnClickedGotobutton();
protected:
	virtual void OnOK();
private:
	CNetNMRClient nmrClient;
	void FillDirList(void);
protected:
	virtual void OnCancel();
};

// NetComPlotDoc.h : interface of the CNetComPlotDoc class
//


#pragma once

class CTwoDSpec;
class COneDSpec;

typedef enum viewtag {
	fullRange = 0,
	ppmRange = 1,
	hertzRange = 2
} VIEWRANGE;

typedef struct docparamtag {
	VIEWRANGE hRange, vRange;
	float f2DSize[2];
	short iShowInfo, nInfoColor, dmorient, pagesize;
	float f1DHeight, f1DOffset;
	float fFrom[2], fTo[2];
	int nPageColor, nAxisColor;
} DOCPARAM;

class CNetComPlotDoc : public CDocument
{
protected: // create from serialization only
	CNetComPlotDoc();
	DECLARE_DYNCREATE(CNetComPlotDoc)

// Attributes
public:
	CTypedPtrList<CPtrList, CTwoDSpec *> TwoDSpec;
	CTypedPtrList<CPtrList, COneDSpec *> HOneDSpec;
	CTypedPtrList<CPtrList, COneDSpec *> VOneDSpec;
	CTypedPtrList<CPtrList, DOCPARAM *> History;
	DOCPARAM param;

// Operations
public:
	void SetFullHRange(DOCPARAM &Param);
	void SetFullVRange(DOCPARAM &Param);

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CNetComPlotDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnUpdateEditParameters(CCmdUI *pCmdUI);
	afx_msg void OnEditParameters();
	void InitAllTransforms(void);
	afx_msg void OnFileData();
	virtual void DeleteContents();
	afx_msg void OnUpdateFilePrintPreview(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFilePrint(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFileSave(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFileSaveAs(CCmdUI *pCmdUI);
	afx_msg void OnProcessSymmetrize(UINT nID);
	afx_msg void OnUpdateProcessSymmetrize(CCmdUI *pCmdUI);
};



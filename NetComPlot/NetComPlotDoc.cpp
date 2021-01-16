// NetComPlotDoc.cpp : implementation of the CNetComPlotDoc class
//

#include "stdafx.h"
#include "NetComPlot.h"
#include "SpecDim.h"
#include "OneDSpec.h"
#include "TwoDSpec.h"
#include "NetComPlotDoc.h"
#include "NetNMRClient.h"
#include "ImportDlg.h"
#include "ParamDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CNetComPlotDoc

IMPLEMENT_DYNCREATE(CNetComPlotDoc, CDocument)

BEGIN_MESSAGE_MAP(CNetComPlotDoc, CDocument)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PARAMETERS, &CNetComPlotDoc::OnUpdateEditParameters)
	ON_COMMAND(ID_EDIT_PARAMETERS, &CNetComPlotDoc::OnEditParameters)
	ON_COMMAND(ID_FILE_DATA, &CNetComPlotDoc::OnFileData)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, &CNetComPlotDoc::OnUpdateFilePrintPreview)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT, &CNetComPlotDoc::OnUpdateFilePrint)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, &CNetComPlotDoc::OnUpdateFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, &CNetComPlotDoc::OnUpdateFileSaveAs)
	ON_COMMAND_RANGE(ID_PROCESS_SYMAVE, ID_PROCESS_SYMMIN, &CNetComPlotDoc::OnProcessSymmetrize)
	ON_UPDATE_COMMAND_UI_RANGE(ID_PROCESS_SYMAVE, ID_PROCESS_SYMMIN, &CNetComPlotDoc::OnUpdateProcessSymmetrize)
END_MESSAGE_MAP()


// CNetComPlotDoc construction/destruction

CNetComPlotDoc::CNetComPlotDoc()
{
	param.iShowInfo = 1;
	param.dmorient = DMORIENT_PORTRAIT;
	param.pagesize = DMPAPER_LETTER;
	param.f2DSize[0] = 16.0f;
	param.f2DSize[1] = 16.0f;
	param.f1DHeight = 2.0f;
	param.f1DOffset = 2.1f;
	param.hRange = param.vRange = ppmRange;
	param.fFrom[0] = param.fFrom[1] = param.fTo[0] = param.fTo[1] = 0.0f;
	param.nAxisColor = 0;
	param.nInfoColor = 0;
	param.nPageColor = 8;
}

CNetComPlotDoc::~CNetComPlotDoc()
{
	while (! TwoDSpec.IsEmpty())
		delete TwoDSpec.RemoveHead();
	while (! HOneDSpec.IsEmpty())
		delete HOneDSpec.RemoveHead();
	while (! VOneDSpec.IsEmpty())
		delete VOneDSpec.RemoveHead();
	while (! History.IsEmpty())
		delete History.RemoveHead();
}

BOOL CNetComPlotDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CNetComPlotDoc serialization

void CNetComPlotDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
		ar.Write(&param, sizeof(DOCPARAM));
		int i = (int)TwoDSpec.GetCount();
		ar << i;
		POSITION pos = TwoDSpec.GetHeadPosition();
		while (pos) {
			CTwoDSpec *p = TwoDSpec.GetNext(pos);
			p->Archive(ar);
		}
		i = (int)HOneDSpec.GetCount();
		ar << i;
		pos = HOneDSpec.GetHeadPosition();
		while (pos) {
			COneDSpec *p = HOneDSpec.GetNext(pos);
			p->Archive(ar);
		}
		i = (int)VOneDSpec.GetCount();
		ar << i;
		pos = VOneDSpec.GetHeadPosition();
		while (pos) {
			COneDSpec *p = VOneDSpec.GetNext(pos);
			p->Archive(ar);
		}
		SetModifiedFlag(FALSE);
	} else {
		ar.Read(&param, sizeof(DOCPARAM));
		// Convert older parameters, if needed.
		if (! (param.iShowInfo==BST_CHECKED || param.iShowInfo==BST_UNCHECKED)) {
			param.iShowInfo = BST_CHECKED;
			param.dmorient = DMORIENT_PORTRAIT;
			param.pagesize = DMPAPER_LETTER;
			param.nInfoColor = 0;
		}
		int count;
		ar >> count;
		for(int i=0; i<count; i++) {
			CTwoDSpec *p = new CTwoDSpec;
			p->Archive(ar);
			TwoDSpec.AddTail(p);
		}
		ar >> count;
		for(int i=0; i<count; i++) {
			COneDSpec *p = new COneDSpec;
			p->Archive(ar);
			HOneDSpec.AddTail(p);
		}
		ar >> count;
		for(int i=0; i<count; i++) {
			COneDSpec *p = new COneDSpec;
			p->Archive(ar);
			VOneDSpec.AddTail(p);
		}
		InitAllTransforms();
	}
}


// CNetComPlotDoc diagnostics

#ifdef _DEBUG
void CNetComPlotDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CNetComPlotDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CNetComPlotDoc commands

void CNetComPlotDoc::OnFileData()
{
	CImportDlg dlg;
	dlg.pDoc = this;
	if (IDOK == dlg.DoModal()) {
		CWaitCursor wait;
		if (TwoDSpec.GetCount()) {
			if (param.fFrom[0] <= param.fTo[0])
				SetFullHRange(param);
			if (param.fFrom[1] <= param.fTo[1])
				SetFullVRange(param);
		}
		InitAllTransforms();
		POSITION pos = GetFirstViewPosition();
		while (pos)
			GetNextView(pos)->UpdateWindow();
	}
}

void CNetComPlotDoc::OnUpdateEditParameters(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TwoDSpec.GetCount() > 0);
}

void CNetComPlotDoc::OnEditParameters()
{
	DOCPARAM tmp;
	memcpy(&tmp, &param, sizeof(DOCPARAM));
	CParamDlg dlg;
	dlg.pDoc = this;
	if (IDOK == dlg.DoModal()) {
		CString err = "";
		if (param.fFrom[0]<=param.fTo[0]) {
			err += "\nHorizontal range out of order, it will be swapped.";
			float x = param.fFrom[0];
			param.fFrom[0] = param.fTo[0];
			param.fTo[0] = x;
		}
		if (param.fFrom[1]<=param.fTo[1]) {
			err += "\nVertical range out of order, it will be swapped.";
			float x = param.fFrom[1];
			param.fFrom[1] = param.fTo[1];
			param.fTo[1] = x;
		}
		if (param.f2DSize[0] < 2.0f) {
			err += "\n2D Width is too small, it will be reset to 2.0 cm.";
			param.f2DSize[0] = 2.0f;
		}
		if (param.f2DSize[1] < 2.0f) {
			err += "\n2D Height is too small, it will be reset to 2.0 cm.";
			param.f2DSize[1] = 2.0f;
		}
		if (param.f1DHeight < 0.4f) {
			err += "\n1D Height is too small, it will be reset to 0.4 cm.";
			param.f1DHeight = 0.4f;
		}
		if (! err.IsEmpty()) {
			CString errstr = "The follow problems were noted:";
			errstr += err;
			AfxMessageBox(errstr, MB_OK|MB_ICONEXCLAMATION);
		}
		InitAllTransforms();
		AfxGetMainWnd()->GetActiveWindow()->Invalidate();
		if (tmp.fFrom[0]!=param.fFrom[0] || tmp.fFrom[1]!=param.fFrom[1] ||
			tmp.fTo[0]!=param.fTo[0] || tmp.fTo[1]!=param.fTo[1] ||
			tmp.hRange!=param.hRange || tmp.vRange!=param.vRange) {
				DOCPARAM *ppar = new DOCPARAM;
				memcpy(ppar, &tmp, sizeof(DOCPARAM));
				History.AddHead(ppar);
		}
	}
}

void CNetComPlotDoc::InitAllTransforms(void)
{
	POSITION pos = TwoDSpec.GetHeadPosition();
	while (pos) {
		CTwoDSpec *p = TwoDSpec.GetNext(pos);
		p->InitTransform(param.f2DSize, param.fFrom, param.fTo, param.hRange, param.vRange);
	}
	pos = HOneDSpec.GetHeadPosition();
	while (pos) {
		COneDSpec *p = HOneDSpec.GetNext(pos);
		p->InitTransform(param.f2DSize[0], param.f1DHeight, param.fFrom[0], param.fTo[0], (int)param.hRange);
	}
	pos = VOneDSpec.GetHeadPosition();
	while (pos) {
		COneDSpec *p = VOneDSpec.GetNext(pos);
		p->InitTransform(param.f2DSize[1], param.f1DHeight, param.fFrom[1], param.fTo[1], (int)param.vRange);
	}
}

void CNetComPlotDoc::DeleteContents()
{
	// TODO: Add your specialized code here and/or call the base class
	while (! TwoDSpec.IsEmpty())
		delete TwoDSpec.RemoveHead();
	while (! HOneDSpec.IsEmpty())
		delete HOneDSpec.RemoveHead();
	while (! VOneDSpec.IsEmpty())
		delete VOneDSpec.RemoveHead();
	while (! History.IsEmpty())
		delete History.RemoveHead();
	CDocument::DeleteContents();
}

void CNetComPlotDoc::OnUpdateFilePrintPreview(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TwoDSpec.GetCount() > 0);
}

void CNetComPlotDoc::OnUpdateFilePrint(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TwoDSpec.GetCount() > 0);
}

void CNetComPlotDoc::OnUpdateFileSave(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TwoDSpec.GetCount() > 0);
}

void CNetComPlotDoc::OnUpdateFileSaveAs(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TwoDSpec.GetCount() > 0);
}


void CNetComPlotDoc::OnProcessSymmetrize(UINT nID)
{
	CTwoDSpec *spec = TwoDSpec.GetHead();
	spec->Symmetrize(nID);
	SetModifiedFlag();
	spec->InitTransform(param.f2DSize, param.fFrom, param.fTo, param.hRange, param.vRange);
	AfxGetMainWnd()->GetActiveWindow()->Invalidate();
}

void CNetComPlotDoc::OnUpdateProcessSymmetrize(CCmdUI *pCmdUI)
{
	if (TwoDSpec.IsEmpty())
		pCmdUI->Enable(FALSE);
	else {
		CTwoDSpec *spec = TwoDSpec.GetHead();
		pCmdUI->Enable(spec->HSpecDim.Size()==spec->VSpecDim.Size()
			&& spec->HSpecDim.Offset()==spec->VSpecDim.Offset()
			&& spec->HSpecDim.SpecWidth()==spec->VSpecDim.SpecWidth());
	}
}

void CNetComPlotDoc::SetFullHRange(DOCPARAM &Param)
{
	bool first = true;
	// LOOP THROUGH ALL 2D SPECTRA AND LOOK FOR EXTREME SHIFTS
	POSITION pos = TwoDSpec.GetHeadPosition();
	while (pos) {
		CTwoDSpec *pspec = TwoDSpec.GetNext(pos);
		pspec->HSpecDim.from = 0;
		pspec->HSpecDim.to = pspec->HSpecDim.Size() - 1;
		float fval = (float)pspec->HSpecDim.from;
		float tval = (float)pspec->HSpecDim.to;
		fval = pspec->HSpecDim.N2Hertz(fval);
		tval = pspec->HSpecDim.N2Hertz(tval);
		if (ppmRange == Param.hRange) {
			fval /= pspec->HSpecDim.Frequency();
			tval /= pspec->HSpecDim.Frequency();
		}
		if (first) {
			Param.fFrom[0] = fval;
			Param.fTo[0] = tval;
			first = false;
		} else {
			if (Param.fFrom[0] < fval)
				Param.fFrom[0] = fval;
			if (Param.fTo[0] > tval)
				Param.fTo[0] = tval;
		}
	}
}

void CNetComPlotDoc::SetFullVRange(DOCPARAM &Param)
{
	bool first = true;
	// LOOP THROUGH ALL 2D SPECTRA AND LOOK FOR EXTREME SHIFTS
	POSITION pos = TwoDSpec.GetHeadPosition();
	while (pos) {
		CTwoDSpec *pspec = TwoDSpec.GetNext(pos);
		pspec->VSpecDim.from = 0;
		pspec->VSpecDim.to = pspec->VSpecDim.Size() - 1;
		float fval = (float)pspec->VSpecDim.from;
		float tval = (float)pspec->VSpecDim.to;
		tval = pspec->VSpecDim.N2Hertz(tval);
		fval = pspec->VSpecDim.N2Hertz(fval);
		if (ppmRange == Param.vRange) {
			tval /= pspec->VSpecDim.Frequency();
			fval /= pspec->VSpecDim.Frequency();
		}
		if (first) {
			Param.fFrom[1] = fval;
			Param.fTo[1] = tval;
			first = false;
		} else {
			if (Param.fFrom[1] < fval)
				Param.fFrom[1] = fval;
			if (Param.fTo[1] > tval)
				Param.fTo[1] = tval;
		}
	}
}
// ILTDoc.cpp : implementation of the CILTDoc class
//

#include "stdafx.h"
#include "ILT.h"

#include "ILTDoc.h"
#include "Decay.h"
#include "MainFrm.h"
#include "ILTView.h"
#include "ProcessDlg.h"
#include "KernelDlg.h"
#include "MonteCarloDlg.h"
#include "normdist.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifndef ILT_SCHEMA
#define ILT_SCHEMA 1
#endif


// CILTDoc

//IMPLEMENT_DYNCREATE(CILTDoc, CDocument)

IMPLEMENT_SERIAL(CILTDoc, CDocument, VERSIONABLE_SCHEMA | ILT_SCHEMA)

BEGIN_MESSAGE_MAP(CILTDoc, CDocument)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, &CILTDoc::OnUpdateFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, &CILTDoc::OnUpdateFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT, &CILTDoc::OnUpdateFilePrint)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, &CILTDoc::OnUpdateFilePrintPreview)
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW, &CILTDoc::OnUpdateFileNew)
	ON_COMMAND(ID_PROCESS_ILT, &CILTDoc::OnProcessIlt)
	ON_UPDATE_COMMAND_UI(ID_PROCESS_ILT, &CILTDoc::OnUpdateProcessIlt)
	ON_COMMAND(ID_PROCESS_KERNELTYPE, &CILTDoc::OnProcessKerneltype)
	ON_COMMAND(ID_PROCESS_MONTECARLO, &CILTDoc::OnProcessMontecarlo)
	ON_UPDATE_COMMAND_UI(ID_PROCESS_MONTECARLO, &CILTDoc::OnUpdateProcessMontecarlo)
END_MESSAGE_MAP()


// CILTDoc construction/destruction

CILTDoc::CILTDoc()
: bOptimize(FALSE)
, bSimplify(FALSE)
, dblAlpha(0.0)
, dblDACtoG(0.00221)
, dblGamma(2.674e8)
, dblFrom(0.01)
, dblObsFrom(0.01)
, dblObsTo(10000.0)
, dblTemperature(25.0)
, dblTo(10000.0)
, dblSIRTTolerance(1.0e-7)
, dblStdErr(0.1)
, dblTimeCubed(4.5e-7)
, dblViscosity(0.01)
, iConfidenceInterval(0)
, iCount(100)
, iIterations(1000)
, iMethod(IDC_ILTMETHODRADIO)
, iPoints(32)
, iSIRTCount(100000)
, iSpacing(1)
, kernelType(typeT2)
{
	strSolvent = "none";
}

CILTDoc::~CILTDoc()
{
	while (Model.GetCount())
		delete Model.RemoveHead();
}

BOOL CILTDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	DeleteContents();

	return TRUE;
}




// CILTDoc serialization

void CILTDoc::Serialize(CArchive& ar)
{
	ar.SerializeClass(RUNTIME_CLASS(CILTDoc));
	//CDocument::Serialize(ar);
	if (ar.IsStoring())
	{
		ar.SetObjectSchema(ILT_SCHEMA);
		ar << strTime;
		ar << iMethod-IDC_EGLMETHODRADIO << bOptimize << bSimplify;
		ar << iSIRTCount << dblSIRTTolerance;
		ar << dblAlpha << dblDACtoG << dblFrom << dblGamma << dblTimeCubed << dblTo;
		ar << strSolvent;
		ar << dblViscosity << dblTemperature;
		ar << iCount << kernelType;
		ar << Xorig.dim();
		for (int i=0; i<Xorig.dim(); i++)
			ar << Xorig[i] << Xvalue[i];
		int n = int(Decays.GetCount());
		ar << n;
		POSITION pos = Decays.GetHeadPosition();
		while (pos) {
			CDecay *decay = Decays.GetNext(pos);
			ar << decay->strTitle;
			decay->Serialize(ar);
		}
	}
	else
	{
		int iVersion = ar.GetObjectSchema();
		int m, n;
		ar >> strTime;
		ar >> iMethod >> bOptimize >> bSimplify;
		if (iMethod > 2)
			iMethod = 1;
		iMethod += IDC_EGLMETHODRADIO;
		ar >> iSIRTCount >> dblSIRTTolerance;
		ar >> dblAlpha >> dblDACtoG >> dblFrom >> dblGamma >> dblTimeCubed >> dblTo;
		if (iVersion > 0) {
			ar >> strSolvent;
			ar >> dblViscosity >> dblTemperature;
		}
		ar >> iCount >> m;
		if (m > 2)
			m = 2;
		kernelType = (EKernelType)m;
		ar >> m;
		Xorig = Array1D<double>(m);
		Xvalue = Array1D<double>(m);
		for (int i=0; i<m; i++)
			ar >> Xorig[i] >> Xvalue[i];
		ar >> n;
		for (int i=0; i<n; i++) {
			CString title;
			ar >> title;
			CDecay *decay = new CDecay(this, m, title);
			decay->Serialize(ar);
			Decays.AddTail(decay);
		}
		theApp.pDecay = NULL;
		UpdateAllViews(NULL);
	}
	SetModifiedFlag(FALSE);
}


// CILTDoc diagnostics

#ifdef _DEBUG
void CILTDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CILTDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

double CILTDoc::text2double(LPCTSTR text)
{
	char *stopstring;
	double result = strtod(text, &stopstring);
	switch (*stopstring) {
	case 'p':
		result *= 1.0e-9;
		break;
	case 'u':
		result *= 1.0e-6;
		break;
	case 'm':
		result *= 1.0e-3;
		break;
	}
	return result;
}

// CILTDoc commands

void CILTDoc::OpenTextILTFile(LPCTSTR filename)
{
	// open the file
	CStdioFile txtfile;
	if (! txtfile.Open(filename, CFile::modeRead|CFile::typeText|CFile::shareDenyWrite)) {
		AfxMessageBox("Can't open input file!", MB_ICONEXCLAMATION);
		return;
	}

	// read the file and sort it
	CStringList strlist;
	CString line, hdrline;
	while (true) {
		txtfile.ReadString(line);
		line = line.Trim();
		if (0 == line.GetLength())
			break;
		if (0 == line.FindOneOf("#")) {
			int i = 0;
			CString varname = line.Tokenize(" #=\t", i);
			CString varval = line.Tokenize("=\t", i);
			if (0==varname.CompareNoCase("dosygamma") || 0==varname.CompareNoCase("gamma"))
				dblGamma = atof(varval);
			else if (0==varname.CompareNoCase("timecubed") || 0==varname.CompareNoCase("tcubed"))
				dblTimeCubed = atof(varval);
			else if (0==varname.CompareNoCase("dactog"))
				dblDACtoG = atof(varval);
			else if (0==varname.CompareNoCase("temp") || 0==varname.CompareNoCase("temperature"))
				dblTemperature = atof(varval);
			else if (0==varname.CompareNoCase("viscosity") || 0==varname.CompareNoCase("visc"))
				dblViscosity = atof(varval);
			else if (0==varname.CompareNoCase("solvent"))
				strSolvent = varval;
		} else {
			if (hdrline.IsEmpty())
				hdrline = line;
			else {
				double xnew = text2double(line);
				POSITION pos = strlist.GetTailPosition();
				while (pos) {
					if (xnew >= text2double(strlist.GetAt(pos)))
						break;
					strlist.GetPrev(pos);
				}
				if (pos)
					strlist.InsertAfter(pos, line);
				else
					strlist.AddHead(line);
			}
		}
	}
	txtfile.Close();

	if (strlist.IsEmpty()) {
		AfxMessageBox("Bad input data!", MB_ICONERROR);
		return;
	}

	// how many columns and rows?
	line = strlist.GetTail();
	int nobs = -1;
	int ptr = 0;
	while (line.Tokenize(" \t", ptr).GetLength())
		nobs++;
	int Arows = int(strlist.GetCount());
	if (nobs<1 || Arows<3) {
		AfxMessageBox("Not enough data was found!", MB_ICONERROR);
		return;
	}

	// read the data
	Xvalue = Array1D<double>(Arows);
	if (1 == nobs) {
		ptr = 0;
		int i = 0;
		strTime = hdrline.Tokenize("\t", ptr).Trim();
		CDecay *decay = new CDecay(this, Arows, hdrline.Tokenize("\t", ptr).Trim());
		while (! strlist.IsEmpty()) {
			line = strlist.RemoveHead();
			ptr = 0;
			Xvalue[i] = text2double(line.Tokenize(" \t", ptr));
			double val = atof(line.Tokenize(" \t", ptr));
			decay->SetAt(i++, val);
		}
		decay->SSQ();
		Decays.AddTail(decay);
	} else {
		CTypedPtrList<CPtrList, CDecay *> tmplist;
		ptr = 0;
		strTime = hdrline.Tokenize("\t", ptr);
		for (int i=0; i<nobs; i++) {
			CString strTitle = hdrline.Tokenize("\t", ptr);
			CDecay *decay = new CDecay(this, Arows, strTitle);
			tmplist.AddTail(decay);
		}
		for (int i=0; i<Arows; i++) {
			line = strlist.RemoveHead();
			ptr = 0;
			Xvalue[i] = text2double(line.Tokenize(" \t", ptr));
			POSITION dpos = tmplist.GetHeadPosition();
			while (dpos) {
				CDecay *decay = tmplist.GetNext(dpos);
				double val = atof(line.Tokenize(" \t", ptr));
				decay->SetAt(i, val);
			}
		}

		// sort the decays by the number in strTitle
		// if there is no number in the first chars of strTitle
		// the order won't change
		while (! tmplist.IsEmpty()) {
			CDecay *decay = tmplist.RemoveHead();
			// take care of some CDecay internals
			decay->SSQ();
			double x = atof(decay->strTitle);
			POSITION pos = Decays.GetTailPosition();
			while (pos) {
				if (x >= atof(Decays.GetAt(pos)->strTitle))
					break;
				Decays.GetPrev(pos);
			}
			if (pos)
				Decays.InsertAfter(pos, decay);
			else
				Decays.AddHead(decay);
		}
	}
	// make a file name based on the text file name
	CString name = filename;
	if (0 == name.Right(4).CompareNoCase(".txt"))
		name = name.Left(name.GetLength()-4);
	int i = name.ReverseFind('\\');
	if (i >= 0)
		name = name.Mid(++i);
	SetTitle(name);

	// the View will default this to the first in the list.
	theApp.pDecay = NULL;

	Xorig = Array1D<double>(Xvalue.dim());
	Xorig = Xvalue.copy();
	OnProcessKerneltype();
	SetModifiedFlag(FALSE);
	UpdateAllViews(NULL);
}

void CILTDoc::DeleteContents()
{
	while (! Decays.IsEmpty())
		delete Decays.RemoveTail();
	theApp.pDecay = NULL;
	CDocument::DeleteContents();
}

void CILTDoc::OnUpdateFileSave(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(Decays.IsEmpty()?FALSE:TRUE);
}

void CILTDoc::OnUpdateFileSaveAs(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(Decays.IsEmpty()?FALSE:TRUE);
}

void CILTDoc::OnUpdateFilePrint(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(Decays.IsEmpty()?FALSE:TRUE);
}

void CILTDoc::OnUpdateFilePrintPreview(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(Decays.IsEmpty()?FALSE:TRUE);
}

void CILTDoc::OnUpdateFileNew(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(Decays.IsEmpty()?FALSE:TRUE);
}

void CILTDoc::OnProcessIlt()
{
	CWaitCursor wait;
	CProcessDlg dlg;

	// initialize items
	dlg.m_dblILTalpha = dblAlpha;
	dlg.m_dblILTFrom = dblFrom;
	dlg.m_dblILTTo = dblTo;
	dlg.m_intILTCount = iCount;
	if (bSimplify || bOptimize)
		dlg.m_dblILTalpha = 0.0;
	dlg.m_boolSimplifyCheck = bSimplify;
	dlg.m_boolOptimizeCheck = bOptimize;
	dlg.m_intMethod = iMethod;
	dlg.m_intConfidence = iConfidenceInterval;
	dlg.m_intSIRTCount = iSIRTCount;
	dlg.m_dblSIRTTolerance = dblSIRTTolerance;

	if (IDOK == dlg.DoModal()) {

		// recover variables
		dblAlpha = dlg.m_dblILTalpha;
		dblFrom = dlg.m_dblILTFrom;
		dblTo = dlg.m_dblILTTo;
		iCount = dlg.m_intILTCount;
		bSimplify = dlg.m_boolSimplifyCheck;
		bOptimize = dlg.m_boolOptimizeCheck;
		iMethod = dlg.m_intMethod;
		iConfidenceInterval = dlg.m_intConfidence;
		iSIRTCount = dlg.m_intSIRTCount;
		dblSIRTTolerance = dlg.m_dblSIRTTolerance;

		if (dlg.m_boolGlobalCheck) {
			POSITION pos = Decays.GetHeadPosition();
			while (pos) {
				CDecay *decay = Decays.GetAt(pos);
				theApp.pDecay = pos;
				for (int i=0; i<10; i++)
					while (! decay->mcterms[i].IsEmpty())
						delete decay->mcterms[i].RemoveHead();
				decay->DoProcess(iMethod, bSimplify, bOptimize, dblAlpha, iSIRTCount, dblSIRTTolerance, dblFrom, dblTo, iCount);
				UpdateAllViews(NULL);
				Decays.GetNext(pos);
			}
		} else {
			CDecay *p = Decays.GetAt(theApp.pDecay);
			for (int i=0; i<10; i++)
				while (! p->mcterms[i].IsEmpty())
					delete p->mcterms[i].RemoveHead();
			p->DoProcess(iMethod, bSimplify, bOptimize, dblAlpha, iSIRTCount, dblSIRTTolerance, dblFrom, dblTo, iCount);
			UpdateAllViews(NULL);
		}
		SetModifiedFlag(true);
		theApp.SetPaneText(1, " ");
	}
}

void CILTDoc::OnUpdateProcessIlt(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(Decays.IsEmpty()?FALSE:TRUE);
}

void CILTDoc::OnProcessKerneltype()
{
	CKernelDlg dlg;

	// initalize dialog data
	dlg.kernelType = kernelType;
	dlg.m_dblDACtoG = dblDACtoG;
	dlg.m_dblGamma = dblGamma;
	dlg.m_dblTimeCubed = dblTimeCubed;
	dlg.m_strSolvent = strSolvent;
	dlg.m_dblViscosity = dblViscosity;
	dlg.m_dblTemperature = dblTemperature;

	if (IDOK == dlg.DoModal()) {
		SetModifiedFlag(TRUE);

		// delete all the terms
		POSITION pos = Decays.GetHeadPosition();
		while (pos) {
			CDecay *decay = Decays.GetNext(pos);
			while (! decay->Terms.IsEmpty())
				delete decay->Terms.RemoveHead();
		}

		// recalculate the Xvalue[] array
		if (typeDosy == dlg.kernelType) {
			for (int i=0; i<Xorig.dim(); i++) {
				// the 0.01 is for converting g/cm to T/m
				double x = 0.01 * dblDACtoG * Xorig[i] * dblGamma;
				Xvalue[i] = 1.0e-10 * dblTimeCubed * x * x;
			}
		} else {
			Xvalue = Xorig.copy();
		}

		// update document variables;
		kernelType = dlg.kernelType;
		dblDACtoG = dlg.m_dblDACtoG;
		dblGamma = dlg.m_dblGamma;
		dblTimeCubed = dlg.m_dblTimeCubed;
		dblViscosity = dlg.m_dblViscosity;
		dblTemperature = dlg.m_dblTemperature;
		strSolvent = dlg.m_strSolvent;

		// update fitting default range
		dblAlpha = 0.0;
		dblFrom = (typeDosy==kernelType) ?   0.1 :    0.01;
		dblTo   = (typeDosy==kernelType) ? 100.0 : 10000.0;
		iCount  = (typeDosy==kernelType) ?    60 :     100;

		// update view
		UpdateAllViews(NULL);
	}
}


void CILTDoc::OnProcessMontecarlo()
{
	// NNLS is the only method allowed!
	iMethod = IDC_ILTMETHODRADIO;
	// delete all current contents
	DeleteContents();

	// get the kernel type from the user
	CKernelDlg kdlg;
	kdlg.isrealdata = false;
	kdlg.kernelType = kernelType;
	kdlg.m_dblDACtoG = dblDACtoG;
	kdlg.m_dblGamma = dblGamma;
	kdlg.m_dblTimeCubed = dblTimeCubed;
	if (IDOK != kdlg.DoModal())
		return;
	// update document variables;
	kernelType = kdlg.kernelType;
	dblDACtoG = kdlg.m_dblDACtoG;
	dblGamma = kdlg.m_dblGamma;
	dblTimeCubed = kdlg.m_dblTimeCubed;
	// update fitting default range
	//dblAlpha = 0.0;
	//dblFrom = (typeDosy==kernelType) ?  0.01 :    0.01;
	//dblTo   = (typeDosy==kernelType) ? 100.0 : 10000.0;
	//iCount  = (typeDosy==kernelType) ?    66 :     100;

	CMonteCarloDlg dlg;
	dlg.m_boolOptimize = bOptimize;
	dlg.m_boolSimplify = bSimplify;
	dlg.m_boolOptimize = bOptimize;
	dlg.m_dblAlpha = dblAlpha;
	dlg.m_dBasisFrom = dblFrom;
	dlg.m_dObsFrom = dblObsFrom;
	dlg.m_dObsTo = dblObsTo;
	dlg.m_dblStdErr = dblStdErr;
	dlg.m_dBasisTo = dblTo;
	dlg.m_iConfidenceInterval = iConfidenceInterval;
	dlg.m_iBasisCount = iCount;
	dlg.m_intIterations = iIterations;
	dlg.m_iObsCount = iPoints;
	dlg.m_iSpacing = iSpacing;
	if (IDOK != dlg.DoModal())
		return;

	CWaitCursor wait;
	bSimplify = dlg.m_boolSimplify;
	bOptimize = dlg.m_boolOptimize;
	dblAlpha = dlg.m_dblAlpha;
	dblFrom = dlg.m_dBasisFrom;
	dblObsFrom = dlg.m_dObsFrom;
	dblObsTo = dlg.m_dObsTo;
	dblStdErr = dlg.m_dblStdErr;
	dblTo = dlg.m_dBasisTo;
	iConfidenceInterval = dlg.m_iConfidenceInterval;
	iCount = dlg.m_iBasisCount;
	iIterations = dlg.m_intIterations;
	iPoints = dlg.m_iObsCount;
	iSpacing = dlg.m_iSpacing;

	Xvalue = Array1D<double>(iPoints);
	Xorig = Array1D<double>(iPoints);
	bool islog = iSpacing ? true : false;
	double fr = dblObsFrom;
	double to = dblObsTo;
	if (islog) {
		fr = log(fr);
		to = log(to);
	}
	double inc = (to - fr) / double(iPoints - 1);

	// build the noise-free decay from the users input terms and X-values
	CDecay *decay = new CDecay(this, iPoints, "MonteCarlo Sim");
	Array1D<double> base(iPoints);
	for (int i=0; i<iPoints; i++) {
		Xorig[i] = fr + inc * double(i);
		if (islog)
			Xorig[i] = exp(Xorig[i]);
		if (typeDosy==kernelType) {
			double x = 0.01 * dblDACtoG * Xorig[i] *dblGamma;
			Xvalue[i] = 1.0e-10 * dblTimeCubed * x * x;
		} else
			Xvalue[i] = Xorig[i];

		// build the base decay for the simulation
		// also put it in the current document
		base[i] = 0.0;
		POSITION pos = Model.GetHeadPosition();
		while (pos) {
			SMCModel *p = Model.GetNext(pos);
			switch (kernelType) {
				case typeT1:
					base[i] += p->a * (1.0 - exp(-Xvalue[i] / p->r));
					break;
				case typeT2:
					base[i] += p->a * exp(-Xvalue[i] / p->r);
					break;
				case typeDosy:
					base[i] += p->a * exp(-Xvalue[i] * p->r);
					break;
			}
		}
		decay->SetAt(i, base[i]);
	}
	// calculate sums of squares and show the data.
	decay->SSQ();
	Decays.AddHead(decay);
	UpdateAllViews(NULL);

	// init the random generator
	CNormalRandom nr(-666);

	//CStdioFile mcfile("MonteCarlo.txt", CFile::typeText|CFile::modeWrite|CFile::modeCreate);
	//mcfile.WriteString("Terms\tPDI\n");
	CStringList mcText;
	CString line = "kernelType\t";
	switch (kernelType) {
		case typeDosy:
			line += "DOSY";
			break;
		case typeT1:
			line += "T1";
			break;
		case typeT2:
			line += "T2";
			break;
		default:
			line += "UNKNOWN!";
			break;
	}
	mcText.AddTail(line);
	if (typeDosy == kernelType) {
		line.Format("DACtoG\t%g", dblDACtoG);
		mcText.AddTail(line);
		line.Format("Gamma\t%g", dblGamma);
		mcText.AddTail(line);
		line.Format("TimeCubed\t%g", dblTimeCubed);
		mcText.AddTail(line);
	}
	line.Format("alpha\t%g\tFrom\t%g\tTo\t%g\tCount\t%d", dblAlpha, dblFrom, dblTo, iCount);
	mcText.AddTail(line);
	if (0.0==dblAlpha && bSimplify) {
		line = "Simplified";
		if (bOptimize) {
			line += "\tOptimized\tConfidence\t";
			switch (iConfidenceInterval) {
				case 0:
					line += "70%";
					break;
				case 1:
					line += "90%";
					break;
				case 2:
					line += "95%";
					break;
				case 3:
					line += "99%";
					break;
				case 4:
					line += "99.9%";
					break;
				case 5:
					line += "99.99%";
					break;
			}
		}
		mcText.AddTail(line);
	}
	line.Format("Noise\t%g\nIterations\t%d", dblStdErr, iIterations);
	mcText.AddTail(line);
	mcText.AddTail("Term->a\tTerm->r");
	POSITION pos = Model.GetHeadPosition();
	while (pos) {
		SMCModel *p = Model.GetNext(pos);
		line.Format("%g\t%g", p->a, p->r);
		mcText.AddTail(line);
	}
	mcText.AddTail("X-value\tY-value");
	for (int i=0; i<iPoints; i++) {
		line.Format("%g\t%g", Xorig[i], decay->GetAt(i));
		mcText.AddTail(line);
	}
	mcText.AddTail("Iteration\t# Terms\tWgt Average\tPDI");

	bool firstterm = true;
	for (int count=0; count<iIterations; count++) {
		CString pmsg;
		pmsg.Format("Simulation %d of %d", count+1, dlg.m_intIterations);
		theApp.SetPaneText(1, pmsg);
		// generate new "observations" with desired noise
		for (int i=0; i<iPoints; i++)
			decay->SetAt(i, base[i] + dblStdErr * nr.get());

		// fit the new data
		decay->DoProcess(iMethod, bSimplify, bOptimize, dblAlpha, iSIRTCount, dblSIRTTolerance, dblFrom, dblTo, iCount);
		int nterms = int(decay->Terms.GetCount() - 1);
		if (nterms > 0) {
			if (nterms > 10)
				nterms = 10;

			double sumni=0.0, sumniri=0.0, sumnimi=0.0, sumnimi2=0.0;
			POSITION pos = decay->Terms.GetHeadPosition();
			while (pos) {
				CTerm *term = decay->Terms.GetNext(pos);
				if (pos) {
					sumni += term->a;
					sumniri += term->a * term->r;
					double mi = pow(term->r, -2);
					sumnimi += term->a * mi;
					sumnimi2 += term->a * mi * mi;
					SMCTerm *newterm = new SMCTerm;
					newterm->y.f = float(term->a);
					if (kernelType == typeDosy)
						newterm->x.f = float(term->r);
					else
						newterm->x.f = float(1.0 / term->r);
					
					// for each term see if it is the new x or y extreme
					if (firstterm) {
						mcymax = newterm->y.f;
						firstterm = false;
					} else {
						if (newterm->y.f > mcymax)
							mcymax = newterm->y.f;
					}
					decay->mcterms[nterms-1].AddTail(newterm);
				}
			}
			line.Format("%d\t%d\t%g\t%g", count, int(decay->Terms.GetCount())-1, sumniri/sumni, sumni*sumnimi2/pow(sumnimi,2));
			mcText.AddTail(line);
		}
	}
	// Copy to the Clipboard and start MonteCarlo.xlsm
	if (OpenClipboard(NULL)) {
		if (EmptyClipboard()) {
			CString cliptext = mcText.RemoveHead();
			while (mcText.GetCount())
				cliptext += '\n' + mcText.RemoveHead();
			int length = cliptext.GetLength();
			GLOBALHANDLE hgText = GlobalAlloc(GHND, length+1);
			if (hgText) {
				char *pgmem = (char *)GlobalLock(hgText);
				if (pgmem) {
					strncpy_s(pgmem, length+1, cliptext, length);
					GlobalUnlock(hgText);
					SetClipboardData(CF_TEXT, hgText);
					line = "start excel \"";
					line += theApp.m_pszHelpFilePath;
					int rfind = line.ReverseFind('\\') + 1;
					line = line.Mid(0, rfind) + "MonteCarlo.xlsm\"";
					system(line);
				} else
					AfxMessageBox("Can't lock clipboard memory!", MB_ICONEXCLAMATION);
			} else
				AfxMessageBox("Can't alloc clipboard memory!", MB_ICONEXCLAMATION);
		} else
			AfxMessageBox("Can't empty the clipboard!", MB_ICONEXCLAMATION);
		CloseClipboard();
	} else
		AfxMessageBox("Can't open the clipboard!", MB_ICONEXCLAMATION);
	mcText.RemoveAll();

	// now convert the Monte Carlo terms from floats to ints
	float xfac = float(15000.0 / log(dblTo / dblFrom));
	float yfac = 10000.0f / mcymax;
	for (int i=0; i<10; i++) {
		POSITION pos = decay->mcterms[i].GetHeadPosition();
		while (pos) {
			SMCTerm *term = decay->mcterms[i].GetNext(pos);
			term->y.i = int(yfac * term->y.f);
			term->x.i = int(xfac * log(term->x.f / double(dblFrom)));
		}
	}

	// All done, show the last fit!
	UpdateAllViews(NULL);
}

void CILTDoc::OnUpdateProcessMontecarlo(CCmdUI *pCmdUI)
{
	//BOOL show = FALSE;
	//if (Decays.GetCount() == 1) {
	//	CDecay *p = Decays.GetHead();
	//	if (p->Terms.GetCount()>1) {
	//		show = TRUE;
	//	}
	//}
	//pCmdUI->Enable(show);
	pCmdUI->Enable(TRUE);
}

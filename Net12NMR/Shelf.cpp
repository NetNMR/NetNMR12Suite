#include "StdAfx.h"
#include "Shelf.h"
#include "Net12NMR.h"
#include "Net12NMRDoc.h"
#include "MainFrame.h"
#include "MyRibbonBar.h"
#include "MainFrame.h"
#include "OutputWnd.h"

extern CNet12NMRApp theApp;

CShelf::CShelf(void)
{
}


CShelf::~CShelf(void)
{
	RemoveAll();
}

void CShelf::RemoveBaseline(void)
{
	while (bases.GetCount())
		delete bases.RemoveHead();
}

void CShelf::RemovePeaks(void)
{
	while (peaks.GetCount())
		delete peaks.RemoveHead();
}

void CShelf::RemoveIntegrals(void)
{
	while (integrals.GetCount())
		delete integrals.RemoveHead();
}

void CShelf::RemoveAll(void)
{
	RemoveBaseline();
	RemovePeaks();
	RemoveIntegrals();
}

// the {Base,Peak,Integral}RegionToShelf routines reuse allocated shelf items, if possible
void CShelf::BaseRegionToShelf(CNet12NMRDoc *pDoc)
{
	if (baselineMode != theApp.disMode)
		return;
	TWODSHELF *p = NULL;
	POSITION base = bases.GetHeadPosition();
	POSITION pos = pDoc->baseregions.GetHeadPosition();
	while (pos) {
		BREGION *pR = pDoc->baseregions.GetNext(pos);
		if (base)
			p = bases.GetAt(base);
		else
			p = new TWODSHELF;
		p->left = pDoc->ntoppm(pR->start);
		p->right = pDoc->ntoppm(pR->stop);
		if (base)
			bases.GetNext(base);
		else
			bases.AddTail(p);
	}
	while (pos) {
		POSITION deadbase = base;
		delete bases.GetNext(base);
		bases.RemoveAt(deadbase);
	}
}

void CShelf::PeakRegionToShelf(CNet12NMRDoc *pDoc)
{
	if (peakpickMode != theApp.disMode)
		return;
	float min, max;
	pDoc->GetMinMax(min, max);
	POSITION peak = peaks.GetHeadPosition();
	THREEDSHELF *p = NULL;
	POSITION pos = pDoc->peakregions.GetHeadPosition();
	while (pos) {
		PREGION *pR = pDoc->peakregions.GetNext(pos);
		if (peak)
			p = peaks.GetAt(peak);
		else
			p = new THREEDSHELF;
		p->left = pDoc->ntoppm(pR->start);
		p->right = pDoc->ntoppm(pR->stop);
		p->yvalue = double((pR->threshold-min) / (max-min));
		if (peak)
			peaks.GetNext(peak);
		else
			peaks.AddTail(p);
	}
	while (peak) {
		POSITION deadpeak = peak;
		delete peaks.GetNext(peak);
		peaks.RemoveAt(deadpeak);
	}
}

void CShelf::IntegralRegionToShelf(CNet12NMRDoc *pDoc)
{
	if (integralMode != theApp.disMode)
		return;
	POSITION integral = integrals.GetHeadPosition();
	THREEDSHELF *p = NULL;
	POSITION pos = pDoc->intregions.GetHeadPosition();
	while (pos) {
		IREGION *pR = pDoc->intregions.GetNext(pos);
		if (integral)
			p = integrals.GetAt(integral);
		else
			p = new THREEDSHELF;
		p->left = pDoc->ntoppm(pR->start); // pR->from;
		p->right = pDoc->ntoppm(pR->stop); // pR->to;
		p->yvalue = (int(pR->stop)==pDoc->setintegralindex)? pDoc->setintegralvalue : 0.0;
		if (integral)
			integrals.GetNext(integral);
		else
			integrals.AddTail(p);
	}
	while (integral) {
		POSITION deadintegral = integral;
		delete integrals.GetNext(integral);
		integrals.RemoveAt(deadintegral);
	}
	pDoc->RecalcIntegrals();
}

// the ShelfTo{Base,Peak,Integral}Region routines reuse allocated pDoc->{base,peak,int}regions, if possible
// they also load the proper vector arrays with pDoc->LoadState() and mark the document as modified.
void CShelf::ShelfToBaseRegion(CNet12NMRDoc *pDoc)
{
	if (baselineMode != theApp.disMode)
		return;
	POSITION pos = pDoc->baseregions.GetHeadPosition();
	BREGION *p = NULL;
	POSITION base = bases.GetHeadPosition();
	while (base) {
		TWODSHELF *pItem = bases.GetNext(base);
		if (pos)
			p = pDoc->baseregions.GetAt(pos);
		else
			p = new BREGION;
		p->start = pDoc->ppmton(pItem->left);
		p->stop = pDoc->ppmton(pItem->right);
		if (pos)
			pDoc->baseregions.GetNext(pos);
		else
			pDoc->baseregions.AddTail(p);
	}
	while (pos) {
		POSITION deadpos = pos;
		delete pDoc->baseregions.GetNext(pos);
		pDoc->baseregions.RemoveAt(deadpos);
	}
	pDoc->LoadState();
	pDoc->SetModifiedFlag();
}

void CShelf::ShelfToPeakRegion(CNet12NMRDoc *pDoc)
{
	if (peakpickMode != theApp.disMode)
		return;
	float min, max;
	pDoc->GetMinMax(min, max);
	PREGION *p = NULL;
	POSITION pos = pDoc->peakregions.GetHeadPosition();
	POSITION peak = peaks.GetHeadPosition();
	while (peak) {
		THREEDSHELF *pItem = peaks.GetNext(peak);
		if (pos)
			p = pDoc->peakregions.GetAt(pos);
		else
			p = new PREGION;
		p->start = pDoc->ppmton(pItem->left);
		p->stop = pDoc->ppmton(pItem->right);
		p->threshold = min + (max - min) * float(pItem->yvalue);
		if (pos)
			pDoc->peakregions.GetNext(pos);
		else
			pDoc->peakregions.AddTail(p);
	}
	while (pos) {
		POSITION deadpos = pos;
		delete pDoc->peakregions.GetNext(pos);
		pDoc->peakregions.RemoveAt(deadpos);
	}
	pDoc->LoadState();
	pDoc->SetModifiedFlag();
}

void CShelf::ShelfToIntegralRegion(CNet12NMRDoc *pDoc)
{
	if (integralMode != theApp.disMode)
		return;
	IREGION *p = NULL;
	POSITION pos = pDoc->intregions.GetHeadPosition();
	POSITION ipos = integrals.GetHeadPosition();
	while (ipos) {
		THREEDSHELF *pItem = integrals.GetNext(ipos);
		if (pos)
			p = pDoc->intregions.GetAt(pos);
		else
			p = new IREGION;
		p->selected = false;
		p->start = pDoc->ppmton(pItem->left);
		p->stop = pDoc->ppmton(pItem->right);
		//p->from = pItem->left;
		//p->to = pItem->right;
		p->total = 0.0;
		UINT size = p->stop - p->start;
		p->integral.resize(size);
		for (UINT i=0; i<size; i++) {
			p->total += pDoc->dt[i+p->start].real();
			p->integral[i] = p->total;
		}
		if (pItem->yvalue > 0.0) {
			pDoc->setintegralindex = p->stop;
			pDoc->setintegralvalue = pItem->yvalue;
			pDoc->sp.integralconst = float(pItem->yvalue / p->total);
			pDoc->sp.bintegralconst = true;
		}
		if (pos)
			pDoc->intregions.GetNext(pos);
		else
			pDoc->intregions.AddTail(p);
	}
	while (pos) {
		POSITION deadpos = pos;
		p = pDoc->intregions.GetNext(pos);
		p->integral.resize(0);
		delete p;
		pDoc->intregions.RemoveAt(deadpos);
	}
	pDoc->LoadState();
	pDoc->SetModifiedFlag();
}

bool CShelf::HasBaseline()
{
	return bases.GetCount() ? true : false;
}

bool CShelf::HasIntegrals()
{
	return integrals.GetCount() ? true : false;
}

bool CShelf::HasPeaks()
{
	return peaks.GetCount() ? true : false;
}

void CShelf::Read(LPCTSTR filename)
{
	LPCTSTR ws = " \t";
	RemoveAll();
	CStdioFile file;
	if (! file.Open(filename, CFile::typeText|CFile::shareDenyWrite|CFile::modeRead)) {
		AfxMessageBox("Can't open file!", MB_OK|MB_ICONERROR);
		return;
	}
	CString line;
	while (file.ReadString(line)) {
		line = line.Trim();
		if (line.IsEmpty())
			continue;
		int index = 0;
		CString typestr, leftstr, rightstr, valuestr;
		typestr = line.Tokenize(ws, index);
		if (typestr.IsEmpty())
			continue;
		leftstr = line.Tokenize(ws, index);
		if (leftstr.IsEmpty())
			continue;
		rightstr = line.Tokenize(ws, index);
		if (rightstr.IsEmpty())
			continue;
		double left = atof(leftstr);
		double right = atof(rightstr);
		if (left <= right)
			continue;
		if (typestr[0] == 'B') {
			TWODSHELF *pNew = new TWODSHELF;
			pNew->left = left;
			pNew->right = right;
			POSITION pos = bases.GetHeadPosition();
			while (pos) {
				TWODSHELF *pItem = bases.GetAt(pos);
				if (pNew->left > pItem->left) {
					bases.InsertBefore(pos, pNew);
					break;
				}
				bases.GetNext(pos);
			}
			if (NULL == pos)
				bases.AddTail(pNew);
		} else if (typestr[0] == 'I') {
			THREEDSHELF *pNew = new THREEDSHELF;
			pNew->left = left;
			pNew->right = right;
			valuestr = line.Tokenize(ws, index);
			pNew->yvalue = valuestr.IsEmpty() ? 0.0 : atof(valuestr);
			POSITION pos = integrals.GetHeadPosition();
			while (pos) {
				THREEDSHELF *pItem = integrals.GetAt(pos);
				if (pNew->left > pItem->left) {
					integrals.InsertBefore(pos, pNew);
					break;
				}
				integrals.GetNext(pos);
			}
			if (NULL == pos)
				integrals.AddTail(pNew);
		} else if (typestr[0] == 'P') {
			valuestr = line.Tokenize(ws, index);
			double value = valuestr.IsEmpty() ? 0.0 : atof(valuestr);
			if (value > 0.0) {
				THREEDSHELF *pNew = new THREEDSHELF;
				pNew->left = left;
				pNew->right = right;
				pNew->yvalue = value;
				POSITION pos = peaks.GetHeadPosition();
				while (pos) {
					THREEDSHELF *pItem = peaks.GetAt(pos);
					if (pNew->left > pItem->left) {
						peaks.InsertBefore(pos, pNew);
						break;
					}
					peaks.GetNext(pos);
				}
				if (NULL == pos)
					peaks.AddTail(pNew);
			}
		}
	}
	file.Close();
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	RibbonUpdate(pMain->m_wndRibbonBar);
}

void CShelf::Write(LPCTSTR filename)
{
	CStdioFile file;
	if (NULL == file.Open(filename, CFile::modeWrite|CFile::modeCreate|CFile::shareExclusive)) {
		AfxMessageBox("Can't open requested file!", MB_ICONEXCLAMATION);
		return;
	}
	POSITION pos = bases.GetHeadPosition();
	while (pos) {
		TWODSHELF *pItem = bases.GetNext(pos);
		CString line;
		line.Format("B\t%g\t%g\n", pItem->left, pItem->right);
		file.WriteString(line);
	}
	pos = peaks.GetHeadPosition();
	while (pos) {
		THREEDSHELF *pItem = peaks.GetNext(pos);
		CString line;
		line.Format("P\t%g\t%g\t%g\n", pItem->left, pItem->right, pItem->yvalue);
		file.WriteString(line);
	}
	pos = integrals.GetHeadPosition();
	while (pos) {
		THREEDSHELF *pItem = integrals.GetNext(pos);
		CString line;
		if (pItem->yvalue)
			line.Format("I\t%g\t%g\t%g\n", pItem->left, pItem->right, pItem->yvalue);
		else
			line.Format("I\t%g\t%g\n", pItem->left, pItem->right);
		file.WriteString(line);
	}
	file.Close();
}

int CShelf::BaseCount()
{
	return int(bases.GetCount());
}

int CShelf::PeakCount()
{
	return int(peaks.GetCount());
}

int CShelf::IntCount()
{
	return int(integrals.GetCount());
}

void CShelf::RibbonUpdate(CMyRibbonBar &pRibbon)
{
	CMainFrame *pMain = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	pMain->m_wndOutput.ClearShelfItems();
	CString str;
	if (BaseCount()+PeakCount()+IntCount() > 0) {
		str.Format("%4.4s\t%12.12s\t%12.12s\t%12.12s", "Type", "From(PPM)", "To(PPM)", "Y-value");
		pMain->m_wndOutput.AddShelfItem(str, NULL);
	}
	POSITION pos = bases.GetHeadPosition();
	while (pos) {
		TWODSHELF *pItem = bases.GetAt(pos);
		str.Format("%4.4s\t%12.6f\t%12.6f", "B", pItem->left, pItem->right);
		pMain->m_wndOutput.AddShelfItem(str, pItem);
		bases.GetNext(pos);
	}
	pos = peaks.GetHeadPosition();
	while (pos) {
		THREEDSHELF *pItem = peaks.GetAt(pos);
		str.Format("%4.4s\t%12.6f\t%12.6f\t%12.8f", "P", pItem->left, pItem->right, pItem->yvalue);
		pMain->m_wndOutput.AddShelfItem(str, pItem);
		peaks.GetNext(pos);
	}
	pos = integrals.GetHeadPosition();
	while (pos) {
		THREEDSHELF *pItem = integrals.GetAt(pos);
		if (pItem->yvalue)
			str.Format("%4.4s\t%12.6f\t%12.6f\t%12.4f", "I", pItem->left, pItem->right, pItem->yvalue);
		else
			str.Format("%4.4s\t%12.6f\t%12.6f", "I", pItem->left, pItem->right);
		pMain->m_wndOutput.AddShelfItem(str, pItem);
		integrals.GetNext(pos);
	}
	CString cursel;
	CMFCRibbonEdit *pEdit = STATIC_DOWNCAST(CMFCRibbonEdit, pRibbon.FindByID(ID_BASESHELFCOUNTEDIT));
	cursel.Format("%d", BaseCount());
	pEdit->SetEditText(cursel);
	pEdit = STATIC_DOWNCAST(CMFCRibbonEdit, pRibbon.FindByID(ID_PEAKCOUNTSHELFEDIT));
	cursel.Format("%d", PeakCount());
	pEdit->SetEditText(cursel);
	pEdit = STATIC_DOWNCAST(CMFCRibbonEdit, pRibbon.FindByID(ID_INTEGRALSHELFCOUNTEDIT));
	cursel.Format("%d", IntCount());
	pEdit->SetEditText(cursel);
}

void CShelf::DeleteByPointer(void *pDelete)
{
	POSITION pos = integrals.GetHeadPosition();
	while (pos) {
		THREEDSHELF *p = integrals.GetAt(pos);
		if (p == pDelete) {
			integrals.RemoveAt(pos);
			delete p;
			return;
		} else
			integrals.GetNext(pos);
	}
	pos = peaks.GetHeadPosition();
	while (pos) {
		THREEDSHELF *p = peaks.GetAt(pos);
		if (p == pDelete) {
			peaks.RemoveAt(pos);
			delete p;
			return;
		} else
			peaks.GetNext(pos);
	}
	pos = bases.GetHeadPosition();
	while (pos) {
		TWODSHELF *p = bases.GetAt(pos);
		if (p == pDelete) {
			bases.RemoveAt(pos);
			delete p;
			return;
		} else
			bases.GetNext(pos);
	}
}
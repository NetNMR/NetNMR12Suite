#include "StdAfx.h"
#include "resource.h"
#include "..\NMRDataTypes.h"
#include "MainFrame.h"
#include "Net12NMRDoc.h"
#include "Net12NMR.h"

#include "Param.h"

CParam::CParam(int height)
{
	headFont.CreateFont(400, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Arial");
	nameFont.CreateFont(400, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Times New Roman");
	unitFont.CreateFont(330, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Times New Roman");
	x = 20400;
	y = height - 2000;
}

CParam::~CParam()
{
	headFont.DeleteObject();
	nameFont.DeleteObject();
	unitFont.DeleteObject();
}

void CParam::Draw( CDC *pDC, CNet12NMRDoc *pDoc)
{
	CString vstr, nstr;
	CString pstr;
	aDC = pDC;
	aDoc = pDoc;
	Heading("F1");
	if (pDoc->sp.f1freq) {
		Parameter("Nucleus", pDoc->f1nucleus, "");
		if (pDoc->sp.f1highpower || pDoc->sp.f1power) {
			nstr.Format("%s Power", pDoc->sp.f1highpower?"High":"Low");
			vstr.Format("%d", pDoc->sp.f1power);
			Parameter(nstr, vstr, "dB");
		}
	}
	vstr.Format("%10.6f", pDoc->sp.f1freq);
	Parameter("Freq", vstr, "MHz");
	vstr.Format("%.2f", pDoc->sp.of/pDoc->sp.sf);
	Parameter("Offset", vstr, "PPM");
	CString decmode;
	pDoc->GetFlexHeaderItem("DecouplerMode", decmode);
	Decoupler(2, pDoc->sp.f2, pDoc->f2nucleus, decmode);
	decmode = "";
	Decoupler(3, pDoc->sp.f3, pDoc->f3nucleus, decmode);
	if (aDoc->sp.tempset) {
		Heading("Temperature");
		vstr.Format("%.1f", aDoc->sp.temp);
		Parameter("T", vstr, "deg C");
	}
	Heading( "Receiver");
	vstr.Format((aDoc->sp.gain<100.0)?"%.1f":"%.0f", aDoc->sp.gain);
	Parameter("Gain", vstr, "");
	if (aDoc->sp.filtermode || aDoc->sp.filterpreamp || aDoc->sp.filterlocktrap) {
		switch ( aDoc->sp.filtermode ) {
		case FILT_BES_MODE:
			nstr = "Bes";
			break;
		case FILT_BUT_MODE:
			nstr = "But";
			break;
		default:
			nstr = "Dir";
			break;
		}
		nstr += " Mode";
		vstr.Format("%.1f", aDoc->sp.filterfreq/1000.0f);
		Parameter(nstr, vstr, "kHz");
		vstr = aDoc->sp.filterpreamp ? "ON" : "OFF";
		Parameter("Preamp", vstr, "");
		vstr = aDoc->sp.filterlocktrap ? "ON" : "OFF";
		Parameter("Lock Trap", vstr, "");
	}
	Heading("Digitizer");
	switch (aDoc->sp.acqmode) {
		case 10:
			pstr = "QF";
			break;
		case 11:
			pstr = "QSIM";
			break;
		case 12:
			pstr = "QSEQ";
			break;
		case 13:
			pstr = "DQD";
			break;
		default:
			pstr.Empty();
			break;
	}
	if (! pstr.IsEmpty())
		Parameter("Mode", pstr, "");
	if (aDoc->sp.sw > 10000.0) {
		vstr.Format("%.1f", aDoc->sp.sw*1.0e-3);
		Parameter("SW", vstr, "kHz");
	} else {
		vstr.Format("%.1f", aDoc->sp.sw);
		Parameter("SW", vstr, "Hz");
	}
	if (aDoc->sp.blocksize > 1024) {
		vstr.Format("%g", double(aDoc->sp.blocksize)/1024.0);
		Parameter("CB", vstr, "k pts");
	} else {
		vstr.Format("%d", aDoc->sp.blocksize);
		Parameter("CB", vstr, "pts");
	}
	vstr.Format("%g", 1.0e6*aDoc->sp.dw);
	Parameter("DW", vstr, "us");
	if (aDoc->sp.at < 1.0) {
		vstr.Format("%g", aDoc->sp.at * 1000.0);
		Parameter( "AT", vstr, "ms");
	} else {
		vstr.Format("%g", aDoc->sp.at);
		Parameter("AT", vstr, "s");
	}
	if (0.0 < aDoc->sp.de) {
		vstr.Format("%g", aDoc->sp.de);
		Parameter("DE", vstr, "us");
	}
	if (0.0 < aDoc->sp.rg) {
		vstr.Format("%g", aDoc->sp.rg);
		Parameter("RG", vstr, "us");
	}
	Heading("Counters");
	vstr.Format("%d", aDoc->sp.ns);
	Parameter("NS", vstr, "");
	vstr.Format("%d", aDoc->sp.dg);
	Parameter("DG", vstr, "");

	if (aDoc->GetFlexHeaderItem("LockSolvent", pstr)) {	// A Bruker or Varian File
		if (strcmp(aDoc->locknucleus, "off")) {
			Heading( "Lock" );
			Parameter("Nucleus", aDoc->locknucleus, "");
			Parameter("Solvent", pstr, "");
			vstr.Format("%d", aDoc->sp.lock_power);
			Parameter("Power", vstr, "");
			vstr.Format("%d", aDoc->sp.lock_gain);
			Parameter("Gain", vstr, "");
		}
	}
	if (aDoc->fh.Lookup("DOSY", aDoc->fhmap)) {
		Heading("DOSY");
		POSITION pos = aDoc->fhmap->GetStartPosition();
		while (pos) {
			CString key, value;
			aDoc->fhmap->GetNextAssoc(pos, key, value);
			Parameter(key, value, "");
		}
	}
	Heading( "Experiment" );
	if (aDoc->GetFlexHeaderItem("seq_source", nstr)) {
		int index = nstr.ReverseFind('/');
		pstr = nstr.Mid(index+1);
		Parameter("Experiment", pstr, "");
	}
	if (aDoc->fh.Lookup("sequel", aDoc->fhmap)) {
		POSITION pos = aDoc->fhmap->GetStartPosition();
		while ( pos ) {
			CString key, value;
			aDoc->fhmap->GetNextAssoc(pos, key, value);
			Parameter( key, value, "" );
		}
	}
	if (aDoc->sp.ftflag) {
		Heading("Processing");
		if (pDoc->dt.size() > 1024) {
			vstr.Format("%g", double(pDoc->dt.size())/1024.0);
			Parameter("Size", vstr, "k pts");
		} else {
			vstr.Format("%d", pDoc->dt.size());
			Parameter("Size", vstr, "pts");
		}
		if (aDoc->sp.dclevel) 
			Parameter("DC Level", "true", "");
		if (aDoc->sp.qca || aDoc->sp.qcg) {
			vstr.Format("a:%.4f g:%.f", aDoc->sp.qca, aDoc->sp.qcg);
			Parameter( "Quad Corr", vstr, "");
		}
		if (aDoc->sp.lrshift) {
			pstr.Format("%d", aDoc->sp.lrshift);
			Parameter((aDoc->sp.lrshift>0)?"Left Shift":"Right Shift", pstr, (1==abs(aDoc->sp.lrshift)) ?"pt":"pts");
		}
		if (aDoc->sp.apoValue) {
			vstr.Format("%g", aDoc->sp.apoValue);
			Parameter(aDoc->sp.isLoren?"Lorentzian":"Gaussian", vstr, aDoc->sp.isPPM?"PPM":"Hertz");
		}
		if (aDoc->sp.magnitude) 
			Parameter( "Magnitude", "true", "" );
		else {
			pstr.Format("A:%.2f B:%.2f", pDoc->sp.pha, pDoc->sp.phb);
			Parameter( "Phase", pstr, "deg");
		}
		if (aDoc->sp.level)
			Parameter( "Level", "true", "");
		if (aDoc->sp.baseline)
			Parameter( "Baseline", aDoc->sp.polybaseline?"polynomial":"moving average", "");
		if (aDoc->sp.filtercomp)
			Parameter( "Filter Comp", "true", "");
	}
}

void CParam::Decoupler(int ndec, DECOUPLER &sdec, const CString &nucleus, const CString decmode)
{
	CString vstr, nstr;
	if (sdec.freq == 0.0 || 0==nucleus.CompareNoCase("off"))
			return;
	vstr.Format("F%d", ndec);
	Heading(vstr);
	Parameter("Nucleus", nucleus, "");
	vstr.Format("%10.6f", sdec.freq);
	Parameter("Freq", vstr, "MHz");
	vstr.Format("%.2f", sdec.offset);
	Parameter("Offset", vstr, "PPM");
	if (decmode.GetLength())
		Parameter("Mode", decmode, "");
	if (sdec.program || sdec.power || sdec.rate) {
		if (sdec.cw) {
			nstr = "CW";
		} else {
			switch (sdec.program) {
			case 0:
				nstr = "0 - 180";
				break;
			case 1:
				nstr = "MLEV-64";
				break;
			case 2:
				nstr = "90 - 180";
				break;
			case 3:
				nstr = "WALTZ-16";
				break;
			case 8:
				nstr = "CW";
				break;
			case 9:
				nstr = "GARP";
				break;
			case 10:
				nstr = "FM";
				break;
			case 11:
				nstr = "noise";
				break;
			case 12:
				nstr = "TPPM";
				break;
			default:
				nstr = "UNKNOWN";
				break;
			}
		}
		vstr.Format("%d", sdec.power);
		Parameter(nstr, vstr, "dB");
		if (!sdec.cw && !sdec.rate) {
			vstr.Format("%.0f", sdec.rate);
			Parameter("gamma", vstr, "Hz");
		}
	}
}

void CParam::Parameter(LPCTSTR name, LPCTSTR value, LPCTSTR units)
{
	CFont *oldfont = aDC->SelectObject(&nameFont);
	UINT oldalign = aDC->SetTextAlign(TA_LEFT|TA_BASELINE|TA_UPDATECP);
	COLORREF oldColor = aDC->SetTextColor(theApp.Color[paramColor]);
	aDC->MoveTo(x, y);
	if (strlen(name)) {
		aDC->TextOut(0, 0, name);
		aDC->TextOutA(0, 0, " = ");
	}
	if (strlen(value)) {
		aDC->SelectObject(&headFont);
		aDC->TextOut(0, 0, value);
		aDC->TextOutA(0, 0, " ");
		if (strlen(units)) {
			aDC->SelectObject(&unitFont);
			aDC->TextOut(0, 0, units);
		}
	}
	y -= 400;
	aDC->SetTextAlign( oldalign );
	aDC->SelectObject( oldfont );
	aDC->SetTextColor( oldColor );
}

void CParam::Heading(LPCTSTR str)
{
	y -= 100;
	CFont *oldFont = aDC->SelectObject(&headFont);
	COLORREF oldTextColor = aDC->SetTextColor(theApp.Color[parheColor]);
	UINT oldAlign = aDC->SetTextAlign(TA_CENTER|TA_BASELINE|TA_NOUPDATECP);
	aDC->FillSolidRect(x, y-100, 2200, 400, theApp.Color[parboColor]);
	aDC->MoveTo(x+1100, y);
	aDC->TextOut(x+1100, y, str);
	aDC->SetTextAlign(oldAlign);
	aDC->SetTextColor(oldTextColor);
	aDC->SelectObject(oldFont);
	y -= 460;
}

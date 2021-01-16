// ParamDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NetComPlot.h"
#include "SpecDim.h"
#include "OneDSpec.h"
#include "TwoDSpec.h"
#include "NetComPlotDoc.h"
#include "ParamDlg.h"

static char *g = "%g";
static char *d = "%d";

// CParamDlg dialog

IMPLEMENT_DYNAMIC(CParamDlg, CDialog)
CParamDlg::CParamDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CParamDlg::IDD, pParent)
	, m_nHRange(0)
	, m_nVRange(0)
	, params2d(NULL)
	, paramsh(NULL)
	, paramsv(NULL)
	, ncurrent2d(0)
	, ncurrenth(0)
	, ncurrentv(0)
{
}

CParamDlg::~CParamDlg()
{
	if (params2d)
		delete[] params2d;
	if (paramsh)
		delete[] paramsh;
	if (paramsv)
		delete[] paramsv;
}

void CParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_HPPMRADIO, m_nHRange);
	DDX_Control(pDX, IDC_HFROMEDIT, m_editHFrom);
	DDX_Control(pDX, IDC_HTOEDIT, m_editHTo);
	DDX_Radio(pDX, IDC_VPPMRADIO, m_nVRange);
	DDX_Control(pDX, IDC_VFROMEDIT, m_editVFrom);
	DDX_Control(pDX, IDC_VTOEDIT, m_editVTo);
	DDX_Control(pDX, IDC_2DFILECOMBO, m_cbox2DFile);
	DDX_Control(pDX, IDC_NUMBERCOMBO, m_cboxNumContours);
	DDX_Control(pDX, IDC_FLOOREDIT, m_editFloor);
	DDX_Control(pDX, IDC_CEILINGEDIT, m_editCeiling);
	DDX_Control(pDX, IDC_SPACINGCOMBO, m_cboxSpacing);
	DDX_Control(pDX, IDC_MODECOMBO, m_cboxMode);
	DDX_Control(pDX, IDC_POSCOLORCOMBO, m_cboxPosColor);
	DDX_Control(pDX, IDC_NEGCOLORCOMBO, m_cboxNegColor);
	DDX_Control(pDX, IDC_HFILECOMBO, m_cboxHFile);
	DDX_Control(pDX, IDC_HFLOOREDIT, m_editHFloor);
	DDX_Control(pDX, IDC_HCEILINGEDIT, m_editHCeiling);
	DDX_Control(pDX, IDC_HCOLORCOMBO, m_cboxHColor);
	DDX_Control(pDX, IDC_VFILECOMBO, m_cboxVFile);
	DDX_Control(pDX, IDC_VFLOOREDIT, m_editVFloor);
	DDX_Control(pDX, IDC_VCEILINGEDIT, m_editVCeiling);
	DDX_Control(pDX, IDC_VCOLORCOMBO, m_cboxVColor);
	DDX_Control(pDX, IDC_HFROMEDIT, m_editHFrom);
	DDX_Control(pDX, IDC_HTOEDIT, m_editHTo);
	DDX_Control(pDX, IDC_VFROMEDIT, m_editVFrom);
	DDX_Control(pDX, IDC_VTOEDIT, m_editVTo);
	DDX_Control(pDX, IDC_WIDTHEDIT, m_editWidth);
	DDX_Control(pDX, IDC_HEIGHTEDIT, m_editHeight);
	DDX_Control(pDX, IDC_1DHEIGHTEDIT, m_edit1DHeight);
	DDX_Control(pDX, IDC_1DOFFSETEDIT, m_edit1DOffset);
	DDX_Control(pDX, IDC_PAGECOLORCOMBO, m_cboxPageColor);
	DDX_Control(pDX, IDC_AXISCOLORCOMBO, m_cboxAxisColor);
	DDX_Control(pDX, IDC_INFOCOLORCOMBO, m_cboxInfoColor);
}


BEGIN_MESSAGE_MAP(CParamDlg, CDialog)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_HPPMRADIO, IDC_HHERTZRADIO, OnHorizRange)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_VPPMRADIO, IDC_VHERTZRADIO, OnVertRange)
	ON_CBN_SELCHANGE(IDC_2DFILECOMBO, OnCbnSelchange2dfilecombo)
	ON_CBN_SELCHANGE(IDC_MODECOMBO, OnCbnSelchangeModecombo)
	ON_CBN_SELCHANGE(IDC_NUMBERCOMBO, OnCbnSelchangeNumbercombo)
	ON_CBN_SELCHANGE(IDC_SPACINGCOMBO, OnCbnSelchangeSpacingcombo)
	ON_EN_CHANGE(IDC_FLOOREDIT, OnEnChangeFlooredit)
	ON_EN_CHANGE(IDC_CEILINGEDIT, OnEnChangeCeilingedit)
	ON_CBN_SELCHANGE(IDC_NEGCOLORCOMBO, OnCbnSelchangeNegcolorcombo)
	ON_CBN_SELCHANGE(IDC_POSCOLORCOMBO, OnCbnSelchangePoscolorcombo)
	ON_CBN_SELCHANGE(IDC_HFILECOMBO, OnCbnSelchangeHfilecombo)
	ON_EN_CHANGE(IDC_HCEILINGEDIT, OnEnChangeHceilingedit)
	ON_EN_CHANGE(IDC_HFLOOREDIT, OnEnChangeHflooredit)
	ON_CBN_SELCHANGE(IDC_HCOLORCOMBO, OnCbnSelchangeHcolorcombo)
	ON_CBN_SELCHANGE(IDC_VFILECOMBO, OnCbnSelchangeVfilecombo)
	ON_EN_CHANGE(IDC_VCEILINGEDIT, OnEnChangeVceilingedit)
	ON_EN_CHANGE(IDC_VFLOOREDIT, OnEnChangeVflooredit)
	ON_CBN_SELCHANGE(IDC_VCOLORCOMBO, OnCbnSelchangeVcolorcombo)
	ON_BN_CLICKED(IDC_FULLHBUTTON, &CParamDlg::OnBnClickedFullhbutton)
	ON_BN_CLICKED(IDC_FULLVBUTTON, &CParamDlg::OnBnClickedFullvbutton)
END_MESSAGE_MAP()


// CParamDlg message handlers

BOOL CParamDlg::OnInitDialog()
{

	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	for (int i=0; i<theApp.nColors; i++) {
		CString color = theApp.arrColors[i].name;
		m_cboxPosColor.AddString(color);
		m_cboxNegColor.AddString(color);
		m_cboxHColor.AddString(color);
		m_cboxVColor.AddString(color);
		m_cboxAxisColor.AddString(color);
		m_cboxPageColor.AddString(color);
		m_cboxInfoColor.AddString(color);
	}
	for (int i=2; i<10; i++) {
		CString str;
		str.Format(d, i);
		m_cboxNumContours.AddString(str);
	}

	// INITIALIZE DIALOG CONTROLS
	CString str;
	m_nHRange = pDoc->param.hRange - 1;
	m_nVRange = pDoc->param.vRange - 1;
	str.Format(g, pDoc->param.fFrom[0]);
	m_editHFrom.SetWindowText(str);
	m_editHFrom.EnableWindow();
	str.Format(g, pDoc->param.fTo[0]);
	m_editHTo.SetWindowText(str);
	m_editHTo.EnableWindow();
	str.Format(g, pDoc->param.fFrom[1]);
	m_editVFrom.SetWindowText(str);
	m_editVFrom.EnableWindow();
	str.Format(g, pDoc->param.fTo[1]);
	m_editVTo.SetWindowText(str);
	m_editVTo.EnableWindow();
	str.Format(g, pDoc->param.f2DSize[0]);
	m_editWidth.SetWindowText(str);
	str.Format(g, pDoc->param.f2DSize[1]);
	m_editHeight.SetWindowText(str);
	str.Format(g, pDoc->param.f1DHeight);
	m_edit1DHeight.SetWindowText(str);
	str.Format(g, pDoc->param.f1DOffset);
	m_edit1DOffset.SetWindowText(str);
	m_cboxPageColor.SetCurSel(pDoc->param.nPageColor);
	m_cboxAxisColor.SetCurSel(pDoc->param.nAxisColor);
	m_cboxInfoColor.SetCurSel(pDoc->param.nInfoColor);
	INT_PTR count = pDoc->TwoDSpec.GetCount();
	if (count > 0) {	// # of 2D specs should never be zero
		// ALLOCATE ARRAY MEMORY FOR PARAMETER
		params2d = new TWODPARAM[count];
		// THEN INITIALIZE THE ARRAY FROM SPECTRA DATA
		count = 0;
		POSITION pos = pDoc->TwoDSpec.GetHeadPosition();
		while (pos) {
			int dummy = 0;
			CTwoDSpec *p = pDoc->TwoDSpec.GetNext(pos);
			CString strName = p->strInfo.Tokenize("\r\n", dummy);
			m_cbox2DFile.SetItemDataPtr(m_cbox2DFile.AddString(strName), p);
			memcpy(&params2d[count++], &p->param, sizeof(twodparamtag));
		}
		// FINALLY INITIALIZE THE DIALOG WITH POSITION ZERO
		m_cbox2DFile.SetCurSel(0);
		OnCbnSelchange2dfilecombo();
	}
	count = pDoc->HOneDSpec.GetCount();
	if (count > 0) {
		paramsh = new ONEDPARAM[count];
		count = 0;
		POSITION pos = pDoc->HOneDSpec.GetHeadPosition();
		while (pos) {
			int dummy = 0;
			COneDSpec *p = pDoc->HOneDSpec.GetNext(pos);
			CString strName = p->strInfo.Tokenize("\r\n", dummy);
			m_cboxHFile.SetItemDataPtr(m_cboxHFile.AddString(strName), p);
			memcpy(&paramsh[count++], &p->param, sizeof(onedparamtag));
		}
		m_cboxHFile.SetCurSel(0);
		OnCbnSelchangeHfilecombo();
	} else {
		m_cboxHFile.EnableWindow(FALSE);
		m_editHFloor.EnableWindow(FALSE);
		m_editHCeiling.EnableWindow(FALSE);
		m_cboxHColor.EnableWindow(FALSE);
	}
	count = pDoc->VOneDSpec.GetCount();
	if (count > 0) {
		paramsv = new ONEDPARAM[count];
		count = 0;
		POSITION pos = pDoc->VOneDSpec.GetHeadPosition();
		while (pos) {
			int dummy = 0;
			COneDSpec *p = pDoc->VOneDSpec.GetNext(pos);
			CString strName = p->strInfo.Tokenize("\r\n", dummy);
			m_cboxVFile.SetItemDataPtr(m_cboxVFile.AddString(strName), p);
			memcpy(&paramsv[count++], &p->param, sizeof(onedparamtag));
		}
		m_cboxVFile.SetCurSel(0);
		OnCbnSelchangeVfilecombo();
	} else {
		m_cboxVFile.EnableWindow(FALSE);
		m_editVFloor.EnableWindow(FALSE);
		m_editVCeiling.EnableWindow(FALSE);
		m_cboxVColor.EnableWindow(FALSE);
	}
		
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CParamDlg::OnHorizRange(UINT uID)
{
	float gfval, gtval;
	bool first = true;
	m_editHFrom.EnableWindow();
	m_editHTo.EnableWindow();
	// LOOP THROUGH ALL 2D SPECTRA AND LOOK FOR EXTREME SHIFTS
	POSITION pos = pDoc->TwoDSpec.GetHeadPosition();
	while (pos) {
		CTwoDSpec *pspec = pDoc->TwoDSpec.GetNext(pos);
		//if (uID == IDC_HFULLRADIO) {
		//	pspec->HSpecDim.from = 0;
		//	pspec->HSpecDim.to = pspec->HSpecDim.Size() - 1;
		//}
		float fval = (float)pspec->HSpecDim.from;
		float tval = (float)pspec->HSpecDim.to;
		fval = pspec->HSpecDim.N2Hertz(fval);
		tval = pspec->HSpecDim.N2Hertz(tval);
		if (uID == IDC_HPPMRADIO) {
			fval /= pspec->HSpecDim.Frequency();
			tval /= pspec->HSpecDim.Frequency();
		}
		if (first) {
			gfval = fval;
			gtval = tval;
			first = false;
		} else {
			if (gfval < fval)
				gfval = fval;
			if (gtval > tval)
				gtval = tval;
		}
	}
	CString str;
	str.Format(g, gfval);
	m_editHFrom.SetWindowText(str);
	str.Format(g, gtval);
	m_editHTo.SetWindowText(str);
}

void CParamDlg::OnVertRange(UINT uID)
{
	float gfval, gtval;
	bool first = true;
	m_editVFrom.EnableWindow();
	m_editVTo.EnableWindow();
	// LOOP THROUGH ALL 2D SPECTRA AND LOOK FOR EXTREME SHIFTS
	POSITION pos = pDoc->TwoDSpec.GetHeadPosition();
	while (pos) {
		CTwoDSpec *pspec = pDoc->TwoDSpec.GetNext(pos);
		//if (uID == IDC_VFULLRADIO) {
		//	pspec->VSpecDim.from = 0;
		//	pspec->VSpecDim.to = pspec->VSpecDim.Size() - 1;
		//}
		float fval = (float)pspec->VSpecDim.from;
		float tval = (float)pspec->VSpecDim.to;
		tval = pspec->VSpecDim.N2Hertz(tval);
		fval = pspec->VSpecDim.N2Hertz(fval);
		if (uID == IDC_VPPMRADIO) {
			tval /= pspec->VSpecDim.Frequency();
			fval /= pspec->VSpecDim.Frequency();
		}
		if (first) {
			gfval = fval;
			gtval = tval;
			first = false;
		} else {
			if (gfval < fval)
				gfval = fval;
			if (gtval > tval)
				gtval = tval;
		}
	}
	CString str;
	str.Format(g, gfval);
	m_editVFrom.SetWindowText(str);
	str.Format(g, gtval);
	m_editVTo.SetWindowText(str);
}

void CParamDlg::OnCbnSelchange2dfilecombo()
{
	ncurrent2d = m_cbox2DFile.GetCurSel();
	m_cboxMode.SetCurSel(params2d[ncurrent2d].mode);
	m_cboxNumContours.SetCurSel(params2d[ncurrent2d].contour-2);
	m_cboxSpacing.SetCurSel(params2d[ncurrent2d].spacing);
	m_cboxPosColor.SetCurSel(params2d[ncurrent2d].poscolor);
	m_cboxNegColor.SetCurSel(params2d[ncurrent2d].negcolor);
	CString str;
	str.Format(g, params2d[ncurrent2d].floor);
	m_editFloor.SetWindowText(str);
	str.Format(g, params2d[ncurrent2d].ceiling);
	m_editCeiling.SetWindowText(str);
}

void CParamDlg::OnCbnSelchangeModecombo()
{
	params2d[ncurrent2d].mode = (EMODE)m_cboxMode.GetCurSel();
}

void CParamDlg::OnCbnSelchangeNumbercombo()
{
	params2d[ncurrent2d].contour = m_cboxNumContours.GetCurSel() + 2;
}

void CParamDlg::OnCbnSelchangeSpacingcombo()
{
	params2d[ncurrent2d].spacing = (ESPACING)m_cboxSpacing.GetCurSel();
}

void CParamDlg::OnEnChangeFlooredit()
{
	CString str;
	m_editFloor.GetWindowText(str);
	double floor = atof(str);
	//if (floor >= params2d[ncurrent2d].ceiling) {
	//	AfxMessageBox("The floor cannot be greater or equal to the ceiling!", MB_OK|MB_ICONERROR);
	//	floor = params2d[ncurrent2d].ceiling - 1.0f;
	//	str.Format(g, floor);
	//	m_editFloor.SetWindowText(str);
	//}
	params2d[ncurrent2d].floor = floor;
}

void CParamDlg::OnEnChangeCeilingedit()
{
	CString str;
	m_editCeiling.GetWindowText(str);
	double ceiling = atof(str);
	//if (ceiling <= params2d[ncurrent2d].floor) {
	//	AfxMessageBox("The ceiling cannot be less or equal to the floor!", MB_OK|MB_ICONERROR);
	//	ceiling = params2d[ncurrent2d].floor + 1.0f;
	//	str.Format(g, ceiling);
	//	m_editFloor.SetWindowText(str);
	//}
	params2d[ncurrent2d].ceiling = ceiling;
}

void CParamDlg::OnCbnSelchangeNegcolorcombo()
{
	params2d[ncurrent2d].negcolor = m_cboxNegColor.GetCurSel();
}

void CParamDlg::OnCbnSelchangePoscolorcombo()
{
	params2d[ncurrent2d].poscolor = m_cboxPosColor.GetCurSel();
}

void CParamDlg::OnCbnSelchangeHfilecombo()
{
	ncurrenth = m_cboxHFile.GetCurSel();
	m_cboxHColor.SetCurSel(paramsh[ncurrenth].color);
	CString str;
	str.Format(g, paramsh[ncurrenth].ceiling);
	m_editHCeiling.SetWindowText(str);
	str.Format(g, paramsh[ncurrenth].floor);
	m_editHFloor.SetWindowText(str);
}

void CParamDlg::OnEnChangeHceilingedit()
{
	CString str;
	m_editHCeiling.GetWindowText(str);
	double ceiling = atof(str);
	paramsh[ncurrenth].ceiling = ceiling;
}

void CParamDlg::OnEnChangeHflooredit()
{
	CString str;
	m_editHFloor.GetWindowText(str);
	double floor = atof(str);
	paramsh[ncurrenth].floor = floor;
}

void CParamDlg::OnCbnSelchangeHcolorcombo()
{
	paramsh[ncurrenth].color = m_cboxHColor.GetCurSel();
}

void CParamDlg::OnCbnSelchangeVfilecombo()
{
	ncurrentv = m_cboxVFile.GetCurSel();
	m_cboxVColor.SetCurSel(paramsv[ncurrentv].color);
	CString str;
	str.Format(g, paramsv[ncurrentv].ceiling);
	m_editVCeiling.SetWindowText(str);
	str.Format(g, paramsv[ncurrentv].floor);
	m_editVFloor.SetWindowText(str);
}

void CParamDlg::OnEnChangeVceilingedit()
{
	CString str;
	m_editVCeiling.GetWindowText(str);
	double ceiling = atof(str);
	paramsv[ncurrentv].ceiling = ceiling;
}

void CParamDlg::OnEnChangeVflooredit()
{
	CString str;
	m_editVFloor.GetWindowText(str);
	double floor = atof(str);
	paramsv[ncurrentv].floor = floor;
}

void CParamDlg::OnCbnSelchangeVcolorcombo()
{
	paramsv[ncurrentv].color = m_cboxVColor.GetCurSel();
}

void CParamDlg::OnOK()
{
	DOCPARAM param;
	BOOL bIsModified = FALSE;
	CDialog::OnOK();
	param.dmorient = pDoc->param.dmorient;
	param.pagesize = pDoc->param.pagesize;
	param.iShowInfo = pDoc->param.iShowInfo;
	param.hRange = (VIEWRANGE)(m_nHRange + 1);
	param.vRange = (VIEWRANGE)(m_nVRange + 1);
	param.nAxisColor = m_cboxAxisColor.GetCurSel();
	param.nPageColor = m_cboxPageColor.GetCurSel();
	param.nInfoColor = m_cboxInfoColor.GetCurSel();
	CString str;
	m_editHFrom.GetWindowText(str);
	param.fFrom[0] = (float)atof(str);
	m_editVFrom.GetWindowText(str);
	param.fFrom[1] = (float)atof(str);
	m_editHTo.GetWindowText(str);
	param.fTo[0] = (float)atof(str);
	m_editVTo.GetWindowText(str);
	param.fTo[1] = (float)atof(str);
	m_editWidth.GetWindowText(str);
	param.f2DSize[0] = (float)atof(str);
	m_editHeight.GetWindowText(str);
	param.f2DSize[1] = (float)atof(str);
	m_edit1DHeight.GetWindowText(str);
	param.f1DHeight = (float)atof(str);
	m_edit1DOffset.GetWindowText(str);
	param.f1DOffset = (float)atof(str);
	if (memcmp(&param, &pDoc->param, sizeof(docparamtag))) {
		bIsModified = TRUE;
		memcpy(&pDoc->param, &param, sizeof(docparamtag));
	}
	INT_PTR count = pDoc->TwoDSpec.GetCount();
	if (count) {
		POSITION pos = pDoc->TwoDSpec.GetTailPosition();
		while (pos) {
			CTwoDSpec *p = pDoc->TwoDSpec.GetPrev(pos);
			if (memcmp(&p->param, &params2d[--count], sizeof(twodparamtag))) {
				bIsModified = TRUE;
				memcpy(&p->param, &params2d[count], sizeof(twodparamtag));
			}
		}
	}
	count = pDoc->HOneDSpec.GetCount();
	if (count) {
		POSITION pos = pDoc->HOneDSpec.GetTailPosition();
		while (pos) {
			COneDSpec *p = pDoc->HOneDSpec.GetPrev(pos);
			if (memcmp(&p->param, &paramsh[--count], sizeof(onedparamtag))) {
				bIsModified = TRUE;
				memcpy(&p->param, &paramsh[count], sizeof(onedparamtag));
			}
		}
	}
	count = pDoc->VOneDSpec.GetCount();
	if (count) {
		POSITION pos = pDoc->VOneDSpec.GetTailPosition();
		while (pos) {
			COneDSpec *p = pDoc->VOneDSpec.GetPrev(pos);
			if (memcmp(&p->param, &paramsv[--count], sizeof(onedparamtag))) {
				bIsModified = TRUE;
				memcpy(&p->param, &paramsv[count], sizeof(onedparamtag));
			}
		}
	}
	if (TRUE==bIsModified)
		pDoc->SetModifiedFlag();
}

void CParamDlg::OnBnClickedFullhbutton()
{
	DOCPARAM param;
	param.hRange = (VIEWRANGE)(m_nHRange + 1);
	pDoc->SetFullHRange(param);
	CString str;
	str.Format("%g", param.fFrom[0]);
	m_editHFrom.SetWindowText(str);
	str.Format("%g", param.fTo[0]);
	m_editHTo.SetWindowText(str);
}

void CParamDlg::OnBnClickedFullvbutton()
{
	DOCPARAM param;
	param.vRange = (VIEWRANGE)(m_nVRange + 1);
	pDoc->SetFullVRange(param);
	CString str;
	str.Format("%g", param.fFrom[1]);
	m_editVFrom.SetWindowText(str);
	str.Format("%g", param.fTo[1]);
	m_editVTo.SetWindowText(str);
}

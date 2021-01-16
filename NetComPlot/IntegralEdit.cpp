// IntegralEdit.cpp : implementation file
//

#include "stdafx.h"
#include "NetComPlot.h"
#include "IntegralEdit.h"
#include ".\integraledit.h"


// CIntegralEdit dialog

IMPLEMENT_DYNAMIC(CIntegralEdit, CDialog)
CIntegralEdit::CIntegralEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CIntegralEdit::IDD, pParent)
	, m_integralEdit(_T(""))
{
}

CIntegralEdit::~CIntegralEdit()
{
}

void CIntegralEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_INTEGRALEDIT, m_integralEdit);
}


BEGIN_MESSAGE_MAP(CIntegralEdit, CDialog)
	ON_BN_CLICKED(IDC_INTCOPYBUTTON, OnBnClickedIntcopybutton)
END_MESSAGE_MAP()


// CIntegralEdit message handlers

void CIntegralEdit::OnBnClickedIntcopybutton()
{
	if (OpenClipboard()) {
		if (EmptyClipboard()) {
			int size = m_integralEdit.GetLength() + 1;
			GLOBALHANDLE hgText = GlobalAlloc(GHND, size);
			if (hgText) {
				char *pgmem = (char *)GlobalLock(hgText);
				if (pgmem) {
					strcat_s(pgmem, size, m_integralEdit);
					GlobalUnlock(hgText);
					SetClipboardData(CF_TEXT, hgText);
				}
			}
		}
		CloseClipboard();
	}
}

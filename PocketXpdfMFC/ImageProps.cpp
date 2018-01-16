// ImageProps.cpp : implementation file
//

#include "stdafx.h"
#include "PocketXpdfMFC.h"
#include "ImageProps.h"
#include <stdlib.h>


// CImageProps dialog

IMPLEMENT_DYNAMIC(CImageProps, CDialog)

CImageProps::CImageProps(CWnd* pParent /*=NULL*/)
	: CDialog(CImageProps::IDD, pParent)
	, m_ImageSize(_T(""))
	, m_RenderDPI(_T(""))
{
	m_bFullScreen = FALSE;
}

CImageProps::~CImageProps()
{
}

void CImageProps::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_IMAGE_SIZE, m_ImageSize);
	DDX_Control(pDX, IDC_RENDER_DPI, m_ctlRenderDPI);
	DDX_CBString(pDX, IDC_RENDER_DPI, m_RenderDPI);
}


BEGIN_MESSAGE_MAP(CImageProps, CDialog)
END_MESSAGE_MAP()


// CImageProps message handlers

BOOL CImageProps::OnInitDialog()
{
	CDialog::OnInitDialog();

	int j;
	CString str;
	m_ImageSize.Format(_T("%d x %d"), ImageSize.cx, ImageSize.cy);
	m_RenderDPI.Format(_T("%.2f"), RenderDPI);

	m_ctlRenderDPI.ResetContent();

	const int low = 4;
	const int high = 4;

	for(j = low; j > 0; j--) {
		str.Format(_T("%.2f"), RenderDPI / (j + 1));
		m_ctlRenderDPI.AddString(str);
	}

	str.Format(_T("%.2f"), RenderDPI);
	m_ctlRenderDPI.AddString(str);
	
	for(j = 0; j < high; j++) {
		str.Format(_T("%.2f"), RenderDPI * (j + 2));
		m_ctlRenderDPI.AddString(str);
	}

	SetCBSize(m_ctlRenderDPI, low + high + 1);
	UpdateData(FALSE);

	return TRUE;
}
/* parse string in the form [digit]+.[digit]+ */
double stof(const wchar_t * str)
{
	int n, m = 1;
	for(;*str && isspace(*str); str++)
		;

	n = 0;
	for(;*str && isdigit(*str); str++)
		n = n * 10 + *str - _T('0');

	if (*str == _T('.')) {
		for(str++; *str && isdigit(*str); str++) {
			n = n * 10 + *str - _T('0');
			m *= 10;
		}
	}

	if (*str != 0)
		return 0.0;

	return double(n) / double(m);
}

void CImageProps::OnOK()
{
	UpdateData(TRUE);
	double newdpi = stof(m_RenderDPI);

	if (newdpi > 0)
		RenderDPI = newdpi;

	CDialog::OnOK();
}

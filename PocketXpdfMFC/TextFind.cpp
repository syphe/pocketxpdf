// TextFind.cpp : implementation file
//

#include "stdafx.h"
#include "PocketXpdfMFC.h"
#include "TextFind.h"


// CTextFind dialog

IMPLEMENT_DYNAMIC(CTextFind, CDialog)

CTextFind::CTextFind(CWnd* pParent /*=NULL*/)
	: CDialog(CTextFind::IDD, pParent)
	, m_text(_T(""))
	, m_caseSensetive(FALSE)
	, m_forward(TRUE)
	, m_begin(TRUE)
	, m_markAll(TRUE)
{
	m_bFullScreen = FALSE;
}

CTextFind::~CTextFind()
{
}

void CTextFind::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TEXT, m_text);
	DDX_Check(pDX, IDC_CASE, m_caseSensetive);
	DDX_Check(pDX, IDC_FORWARD, m_forward);
	DDX_Check(pDX, IDC_BEGIN, m_begin);
	DDX_Check(pDX, IDC_MARK_ALL, m_markAll);
}


BEGIN_MESSAGE_MAP(CTextFind, CDialog)
END_MESSAGE_MAP()


// CTextFind message handlers
BOOL CTextFind::execute()
{
	return CDialog::DoModal() == IDOK;
}

BOOL CTextFind::OnInitDialog()
{
	CDialog::OnInitDialog();
	return TRUE;
}

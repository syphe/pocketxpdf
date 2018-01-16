// PasswordDialog.cpp : implementation file
//

#include "stdafx.h"
#include "PocketXpdfMFC.h"
#include "PasswordDialog.h"


// CPasswordDialog dialog

IMPLEMENT_DYNAMIC(CPasswordDialog, CDialog)

CPasswordDialog::CPasswordDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CPasswordDialog::IDD, pParent)
	, m_OwnerPassword(_T(""))
{
	m_bFullScreen=FALSE;
}

CPasswordDialog::~CPasswordDialog()
{
}

void CPasswordDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_OWNERPW, m_OwnerPassword);
}


BEGIN_MESSAGE_MAP(CPasswordDialog, CDialog)
END_MESSAGE_MAP()


// CPasswordDialog message handlers

// WaitDialog.cpp : implementation file
//

#include "stdafx.h"
#include "PocketXpdfMFC.h"
#include "WaitDialog.h"


// CWaitDialog dialog

IMPLEMENT_DYNAMIC(CWaitDialog, CDialog)

CWaitDialog::CWaitDialog(CWnd* pParent /*=NULL*/)
	//: CDialog(CWaitDialog::IDD, pParent)
	:CDialog()
{
	m_bFullScreen=FALSE;
	Create(CWaitDialog::IDD,pParent);
}

CWaitDialog::~CWaitDialog()
{
}

void CWaitDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_WAITMESSAGE, m_StaticWaitMessage);
}


BEGIN_MESSAGE_MAP(CWaitDialog, CDialog)
	ON_WM_ACTIVATE()
	ON_WM_SETTINGCHANGE()
END_MESSAGE_MAP()


// CWaitDialog message handlers

void CWaitDialog::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CWnd::OnActivate(nState, pWndOther, bMinimized);

}

void CWaitDialog::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CWnd::OnSettingChange(uFlags, lpszSection);

}

void CWaitDialog::setWaitMessage(CString waitmessage)
{
	m_StaticWaitMessage.SetWindowTextW(waitmessage);
	m_StaticWaitMessage.Invalidate();
}

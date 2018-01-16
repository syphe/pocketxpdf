// ButtonGrabberDialog.cpp : implementation file
//

#include "stdafx.h"
#include "PocketXpdfMFC.h"
#include "ButtonGrabberDialog.h"
#include "MainFrm.h"

// CButtonGrabberDialog dialog

IMPLEMENT_DYNAMIC(CButtonGrabberDialog, CDialog)

CButtonGrabberDialog::CButtonGrabberDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CButtonGrabberDialog::IDD, pParent)
	, m_key(0)
	, m_hotkey(0)
{
	m_bFullScreen=FALSE;
	//Create(CButtonGrabberDialog::IDD,pParent);
}

CButtonGrabberDialog::~CButtonGrabberDialog()
{
	if (AfxGetApp()->GetProfileInt(L"Xpdf",L"MapHWButtons",0)){
	//Enable hardware button handling
	typedef BOOL (__stdcall *UnregisterFunc1Proc)( UINT, UINT ); 
	HINSTANCE hCoreDll; 
	UnregisterFunc1Proc procUnregisterFunc; 
	hCoreDll = LoadLibrary(_T("coredll.dll")); 
	ASSERT(hCoreDll); 
	procUnregisterFunc = (UnregisterFunc1Proc)GetProcAddress( hCoreDll, _T("UnregisterFunc1")); 
	ASSERT(procUnregisterFunc); 
	for (int i=0xc1; i<=0xcf; i++) 
	{ 
		BOOL res=procUnregisterFunc(MOD_WIN, i); 
		if (!res){
			MessageBox(L"Cannot unregister HW-Buttons");
		}
		res= RegisterHotKey(((CMainFrame*)AfxGetMainWnd())->m_wndView.GetSafeHwnd(), i, MOD_WIN, i);
		if (!res){
			MessageBox(L"Cannot setup HW-Buttons");
		}
		
	} 
	FreeLibrary(hCoreDll);
	}
}

void CButtonGrabberDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_WAITMESSAGE, CString("Press button!"));
}


// CButtonGrabberDialog message handlers



BOOL CButtonGrabberDialog::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message==WM_KEYDOWN)
	{
		m_key=pMsg->wParam;
		this->OnOK();
	}
	if (pMsg->message==WM_HOTKEY)
	{
		m_hotkey=pMsg->wParam;
		this->OnOK();
	}

	return CDialog::PreTranslateMessage(pMsg);
}


BOOL CButtonGrabberDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (AfxGetApp()->GetProfileInt(L"Xpdf",L"MapHWButtons",0)){
	//Enable hardware button handling
	typedef BOOL (__stdcall *UnregisterFunc1Proc)( UINT, UINT ); 
	HINSTANCE hCoreDll; 
	UnregisterFunc1Proc procUnregisterFunc; 
	hCoreDll = LoadLibrary(_T("coredll.dll")); 
	ASSERT(hCoreDll); 
	procUnregisterFunc = (UnregisterFunc1Proc)GetProcAddress( hCoreDll, _T("UnregisterFunc1")); 
	ASSERT(procUnregisterFunc); 
	for (int i=0xc1; i<=0xcf; i++) 
	{ 
		BOOL res=procUnregisterFunc(MOD_WIN, i); 
		if (!res){
			MessageBox(L"Cannot unregister HW-Buttons");
		}
		res= RegisterHotKey(this->GetSafeHwnd(), i, MOD_WIN, i);
		if (!res){
			MessageBox(L"Cannot setup HW-Buttons");
		}
		
	} 
	FreeLibrary(hCoreDll);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

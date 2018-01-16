// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "PocketXpdfMFC.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TOOLBAR_HEIGHT 24

const DWORD dwAdornmentFlags = 0; // exit button

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_COPYDATA()
	ON_MESSAGE(WM_USER+1,PostCreateWindow)
	ON_COMMAND(ID_TOOLS_FULLSCREEN, &CMainFrame::OnToolsFullscreen)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_FULLSCREEN, &CMainFrame::OnUpdateToolsFullscreen)
	ON_WM_INITMENUPOPUP()
END_MESSAGE_MAP()


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
: m_Fullscreen(false)
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// create a view to occupy the client area of the frame
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}

	int toolbar_id = IDR_MAINFRAME;
#ifdef _DEVICE_RESOLUTION_AWARE
	if(DRA::LogPixelsX() == 192) toolbar_id = IDR_TOOLBAR_VGA;
#endif


	if (!m_wndCommandBar.Create(this) 
		|| !m_wndCommandBar.InsertMenuBar(IDR_MAINFRAME,0) 
		|| !m_wndCommandBar.AddAdornments(dwAdornmentFlags) 
		#ifndef WIN32_PLATFORM_WFSP
		|| AfxGetAygshellUIModel()!=Smartphone? !m_wndCommandBar.LoadToolBar(toolbar_id):false
		#endif
		)
	{
		TRACE0("Failed to create CommandBar\n");
		return -1;      // fail to create
	}
	m_wndCommandBar.SetBarStyle(m_wndCommandBar.GetBarStyle() | CBRS_SIZE_FIXED);
	CWnd* pWnd = CWnd::FromHandlePermanent(m_wndCommandBar.m_hWnd);

	RECT rect, rectDesktop;
	pWnd->GetWindowRect(&rect);
	pWnd->GetDesktopWindow()->GetWindowRect(&rectDesktop);

	int cx = rectDesktop.right - rectDesktop.left;
	int cy = (rectDesktop.bottom - rectDesktop.top) - (rect.bottom - rect.top);
	this->SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOMOVE | SWP_NOZORDER);
	this->SetForegroundWindow();

	this->PostMessageW(WM_USER+1,0,0);
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CFrameWnd::PreCreateWindow(cs))
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}



// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}
#endif //_DEBUG

// CMainFrame message handlers

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// forward focus to the view window
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}
LRESULT CMainFrame::PostCreateWindow(WPARAM wParam, LPARAM lParam){

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
		res= RegisterHotKey(m_wndView.GetSafeHwnd(), i, MOD_WIN, i);
		if (!res){
			MessageBox(L"Cannot setup HW-Buttons");
		}
		
	} 
	FreeLibrary(hCoreDll);
	}


	return (LRESULT)0;
}


void CMainFrame::setCaption(LPWSTR text)
{
	if (!m_Fullscreen){
		//SetWindowText undoes fullscreen
		SetWindowText(text);
	}
}

void CMainFrame::OnToolsFullscreen()
{
	static RECT origsize;
	m_Fullscreen=!m_Fullscreen;
	if (m_Fullscreen){
		RECT fullscreenSize;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &fullscreenSize, 0);
		
		//Save non-fullscreen size
		this->GetWindowRect(&origsize);
		
		BOOL chgScreen = SHFullScreen(m_hWnd,SHFS_HIDESTARTICON | SHFS_HIDESIPBUTTON | SHFS_HIDETASKBAR);
		if (chgScreen == FALSE)
		{
			MessageBox(_T("Could not modify the window."),MB_OK);
		}
		m_wndCommandBar.Show(FALSE);
		RECT fsRect;
		fsRect.top=0;
		fsRect.left=0;
		fsRect.right=fullscreenSize.right;
		fsRect.bottom=fullscreenSize.bottom;
		this->MoveWindow(&fsRect);

	}else {
		this->MoveWindow(&origsize);
		m_wndCommandBar.Show(TRUE);
		BOOL chgScreen = SHFullScreen(m_hWnd,SHFS_SHOWSTARTICON | SHFS_SHOWSIPBUTTON | SHFS_SHOWTASKBAR);

	}
}

void CMainFrame::OnUpdateToolsFullscreen(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_Fullscreen);
}

void CMainFrame::DoModal(CWnd * wnd, CWnd * parent)
{
	BOOL enabled = FALSE;
	ASSERT( ::IsWindow( wnd->m_hWnd) );

	if (parent && ::IsWindow(parent->m_hWnd))
		enabled = !parent->EnableWindow(FALSE);

	while(wnd->IsWindowVisible() && AfxGetApp()->PumpMessage())
		;

	if (parent && enabled)
		parent->EnableWindow(TRUE);

	m_wndCommandBar.Show(TRUE);
}

bool CMainFrame::UpdateMenu(CCmdTarget *cmdtarget, CMenu * pPopupMenu){
	bool has_enabled_items = false;
	int count =pPopupMenu->GetMenuItemCount();
	CCmdUI state;
	state.m_nIndexMax=count;
	for (int i=0;i<count;i++){
		state.m_nID=pPopupMenu->GetMenuItemID(i);
		state.m_nIndex=i;
		state.m_pMenu=pPopupMenu;
		state.DoUpdate(cmdtarget,FALSE);
		if (!(pPopupMenu->GetMenuState(i,MF_BYPOSITION)&MF_GRAYED)){
			has_enabled_items = true;
		}
		 
		CMenu * subMenu = pPopupMenu->GetSubMenu(i);
		if (subMenu!=NULL){
			if (!UpdateMenu(cmdtarget,subMenu)){
				//Everything disabled, so disable submenu
				pPopupMenu->EnableMenuItem(i,MF_BYPOSITION|MF_GRAYED);
			} else {
				pPopupMenu->EnableMenuItem(i,MF_BYPOSITION|MF_ENABLED);
			}
		}

	}
	return has_enabled_items;
}

void CMainFrame::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CFrameWnd::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

	UpdateMenu(this,pPopupMenu);
}

void CMainFrame::HideCommandBar(bool unhide)
{
		m_wndCommandBar.Show(unhide);
}

BOOL CMainFrame::OnCopyData(CWnd *wnd, COPYDATASTRUCT *cds)
{

	LPTSTR str = (LPTSTR)cds->lpData;
	//MessageBox(str);
	::SetForegroundWindow(m_hWnd);
	if (m_wndView.loadFile(str))
	{
		m_wndView.renderPage();
	}
	return true;

}
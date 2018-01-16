// PocketXpdfMFC.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "PocketXpdfMFC.h"
#include "MainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPocketXpdfMFCApp

BEGIN_MESSAGE_MAP(CPocketXpdfMFCApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CPocketXpdfMFCApp::OnAppAbout)
	ON_COMMAND(ID_MENU_QUIT, &CPocketXpdfMFCApp::OnMenuQuit)
END_MESSAGE_MAP()



// CPocketXpdfMFCApp construction
CPocketXpdfMFCApp::CPocketXpdfMFCApp()
	: CWinApp()
{
	CheckForRunningInstance();
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CPocketXpdfMFCApp object
CPocketXpdfMFCApp theApp;

// CPocketXpdfMFCApp initialization

BOOL CPocketXpdfMFCApp::InitInstance()
{
	// SHInitExtraControls should be called once during your application's initialization to initialize any
    // of the Windows Mobile specific controls such as CAPEDIT and SIPPREF.
    SHInitExtraControls();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	
	// Change the registry key under which our settings are stored
	SetRegistryKey(L"pocketxpdf.sf.net");

	// Store the current version number in the registry (might be useful
	// when updating to future versions)
	TCHAR szExe[MAX_PATH];
	::GetModuleFileName(NULL, szExe, MAX_PATH);
	DWORD dwHandle;
    DWORD dwDataSize = ::GetFileVersionInfoSize(szExe, &dwHandle);
	LPBYTE  lpVersionData=new BYTE[dwDataSize];
	if (!::GetFileVersionInfo(szExe, dwHandle, dwDataSize, (void**)lpVersionData) ){
		MessageBox(NULL,L"Cannot read file version info",L"Error",MB_OK);
        return FALSE;
    }
	
	LPVOID pVal;
	UINT nValLen;
	if (!::VerQueryValue(lpVersionData,L"\\StringFileInfo\\040904e4\\ProductVersion",&pVal,&nValLen)){
		MessageBox(NULL,L"Cannot read product version info",L"Error",MB_OK);
        return FALSE;
	} else {
		
		WriteProfileString(L"",L"Version",(TCHAR*)pVal);
	}
	delete[] lpVersionData;
	
	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// create and load the frame with its resources
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL, NULL);

	// The one and only window has been initialized, so show and update it
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();
	return TRUE;
}

// CPocketXpdfMFCApp message handlers


// CAboutDlg dialog used for App About
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
#ifdef _DEVICE_RESOLUTION_AWARE
	afx_msg void OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/);
#endif
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;	// return TRUE unless you set the focus to a control
			// EXCEPTION: OCX Property Pages should return FALSE
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
#ifdef _DEVICE_RESOLUTION_AWARE
	ON_WM_SIZE()
#endif
END_MESSAGE_MAP()

#ifdef _DEVICE_RESOLUTION_AWARE
void CAboutDlg::OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/)
{
	DRA::RelayoutDialog(
		AfxGetInstanceHandle(), 
		this->m_hWnd, 
		DRA::GetDisplayMode() != DRA::Portrait ? MAKEINTRESOURCE(IDD_ABOUTBOX_WIDE) : MAKEINTRESOURCE(IDD_ABOUTBOX));
}
#endif

// App command to run the dialog
void CPocketXpdfMFCApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void CPocketXpdfMFCApp::OnMenuQuit()
{
	//m_pMainWnd->PostMessageW(WM_CLOSE,0,0);
	OnAppExit();
}

int CPocketXpdfMFCApp::ExitInstance()
{
	return CWinApp::ExitInstance();
}

void SetCBSize(CComboBox & cb, int max_items)
{
	RECT r;
	cb.GetWindowRect(&r);

	int count = cb.GetCount();
	int item_height = cb.GetItemHeight(0);
	int width = r.right - r.left;
	int height = r.bottom - r.top;

	if (count > max_items) count = max_items;
	else if (count < 1) count = 1;

	cb.SetWindowPos(0, 0, 0, width, item_height * count + height + 4,
		SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW );
}


typedef struct ModuleInfo
{
	wchar_t *pszModuleFileName;
	HWND hwnd;
	COPYDATASTRUCT *cbs;
} 
MODULEINFO, *PMODULEINFO;

BOOL CALLBACK FindModuleInfoProc(HWND hwnd, LPARAM lParam)
{
	DWORD		dwProcessID;
	PMODULEINFO	pModuleInfo = (PMODULEINFO)lParam;

	::GetWindowThreadProcessId(hwnd, &dwProcessID);
	if (!dwProcessID) 
	{
		return TRUE;
	}

	wchar_t szModuleFileName[MAX_PATH];
	if (!::GetModuleFileName((HMODULE)dwProcessID, szModuleFileName, MAX_PATH)) 
	{
		return TRUE;
	}

	if (!::wcsicmp(szModuleFileName, pModuleInfo->pszModuleFileName))
	{	
		//wchar_t string[255];
		//wsprintf(string,L"%X",hwnd);
		//MessageBox(NULL,string,L".",MB_OK);

		SendMessage(hwnd,WM_COPYDATA,0,(LPARAM)pModuleInfo->cbs);

		pModuleInfo->hwnd = hwnd;
		return TRUE;
	}

	return TRUE;
}

void CPocketXpdfMFCApp::CheckForRunningInstance(void)
{
	// only one application instance can be run
		wchar_t szTempFileName[MAX_PATH], szModuleFileName[MAX_PATH];
	
		int nFileNameLen = ::GetModuleFileName(NULL, szModuleFileName, MAX_PATH);			
		int idxTempFileName = 0;
		for (int idxModuleFileName = 0; idxModuleFileName < nFileNameLen; idxModuleFileName++) 
		{
			if (szModuleFileName[idxModuleFileName] == L'\\') 
			{
				szTempFileName[idxTempFileName++] = L'/';
			}
			else
			{
				szTempFileName[idxTempFileName++] = szModuleFileName[idxModuleFileName];
			}
		}
		szTempFileName[idxTempFileName] = L'\0';

		HANDLE hMutex = ::CreateMutex(NULL, FALSE, szTempFileName);
		if (hMutex != NULL)
		{
			if (::GetLastError() == ERROR_ALREADY_EXISTS) 
			{
				LPWSTR *szArglist;
				int nArgs;
				int i;
				szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
				if( NULL == szArglist )
				{
					wprintf(L"CommandLineToArgvW failed\n");
					exit(1);
				}
				if (nArgs==1)
				{
					LPTSTR filename = szArglist[0];
					COPYDATASTRUCT cds;
					cds.dwData=0;
					cds.cbData=(wcslen((wchar_t *)filename)+1)*2;
					cds.lpData=filename;

					MODULEINFO moduleInfo;
					ZeroMemory(&moduleInfo, sizeof(MODULEINFO));
					moduleInfo.pszModuleFileName = szModuleFileName;
					moduleInfo.cbs=&cds;

					::EnumWindows(FindModuleInfoProc, (LPARAM)&moduleInfo);
					::ReleaseMutex(hMutex);
	   				LocalFree(szArglist);
					exit(0);
							
				}
				// Free memory allocated for CommandLineToArgvW arguments.
   				LocalFree(szArglist);
			}
			::ReleaseMutex(hMutex);
			CloseHandle(hMutex);
		}
}

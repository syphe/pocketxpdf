// ConfigDialog.cpp : implementation file
//

#include "stdafx.h"
#include "PocketXpdfMFC.h"
#include "ConfigDialog.h"
#include "ButtonGrabberDialog.h"


// CConfigDialog dialog

IMPLEMENT_DYNAMIC(CConfigDialog, CDialog)

CConfigDialog::CConfigDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigDialog::IDD, pParent)
	, m_AntiAliasing(FALSE)
	, m_MapHWButtons(FALSE)
	, m_FileAssoc(FALSE)
	, m_HideScrollbars(FALSE)
{

	CWinApp* pApp = AfxGetApp();
	m_AntiAliasing = pApp->GetProfileInt(L"Xpdf",L"AntiAliasing",1);
	m_MapHWButtons = pApp->GetProfileInt(L"Xpdf",L"MapHWButtons",0);
	m_HideScrollbars = pApp->GetProfileInt(L"Xpdf",L"HideScrollbars",0);
	
	const int NUMDEFAULTMAPPINGS=6;
	ButtonMapping defaultmappings[NUMDEFAULTMAPPINGS] = {
		{VK_UP,PRIORSCREEN},
		{VK_DOWN,NEXTSCREEN},
		{VK_LEFT,LEFTSCREEN},
		{VK_RIGHT,RIGHTSCREEN},
		{VK_RETURN,NEXTPAGE},
		{VK_F10,PREVPAGE},

	};
	ButtonMapping none = {-1,NONE};
	
	for (int i=0;i<MAXMAPPINGS;i++)
	{
		CString name;
		name.Format(L"Mapping%d",i);
		CString mapping = pApp->GetProfileString(L"Xpdf",name,L"");
		if (mapping==L"")
		{
			if (i<NUMDEFAULTMAPPINGS)
			{
				m_ButtonMappings[i]=defaultmappings[i];
			} else
			{
				m_ButtonMappings[i]=none;
			}
		} else
		{
			wchar_t *stopstring;
			int delim = mapping.Find(L"|");
			m_ButtonMappings[i].button=wcstol(mapping.Left(delim),&stopstring,10);
			m_ButtonMappings[i].function=(Functions)wcstol(mapping.Right(mapping.GetLength()-delim-1),&stopstring,10);
		}
	}
}

CConfigDialog::~CConfigDialog()
{
	CWinApp* pApp = AfxGetApp();
	
	pApp->WriteProfileInt(L"Xpdf",L"AntiAliasing",m_AntiAliasing);
	
	pApp->WriteProfileInt(L"Xpdf",L"MapHWButtons",m_MapHWButtons);

	pApp->WriteProfileInt(L"Xpdf",L"HideScrollbars",m_HideScrollbars);


	for (int i=0;i<MAXMAPPINGS;i++)
	{
		if (m_ButtonMappings[i].button==-1)
		{
			break;
		}
		
		CString name;
		name.Format(L"Mapping%d",i);
		CString mapping;
		mapping.Format(L"%d|%d",m_ButtonMappings[i].button,m_ButtonMappings[i].function);
		pApp->WriteProfileString(L"Xpdf",name,mapping);
	}
}

void CConfigDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_ANTIALIASING, m_AntiAliasing);
	DDX_Check(pDX, IDC_MAPHWBUTTONS, m_MapHWButtons);
	DDX_Control(pDX, IDC_MAPFUNCTION, m_MapFunctionBox);
	DDX_Control(pDX, IDC_MAPBUTTONS, m_MapButtonBox);
	DDX_Check(pDX, IDC_FILEASSOC, m_FileAssoc);
	DDX_Check(pDX, IDC_HIDESCROLLBARS, m_HideScrollbars);
}


BEGIN_MESSAGE_MAP(CConfigDialog, CDialog)
	ON_CBN_SELCHANGE(IDC_MAPBUTTONS, &CConfigDialog::OnCbnSelchangeMapbuttons)
	ON_CBN_SELCHANGE(IDC_MAPFUNCTION, &CConfigDialog::OnCbnSelchangeMapfunction)
	ON_BN_CLICKED(IDOK, &CConfigDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDC_MAPHWBUTTONS, &CConfigDialog::OnToggleMapHWButtons)
	ON_BN_CLICKED(IDC_SELECTBUTTON, &CConfigDialog::OnBnClickedSelectbutton)
END_MESSAGE_MAP()


// CConfigDialog message handlers

BOOL CConfigDialog::OnInitDialog()
{
	HKEY key;
	/*if (RegOpenKeyEx(HKEY_CLASSES_ROOT,L"xpdffile\\Shell\\Open\\Command",0,KEY_QUERY_VALUE,&key)==ERROR_SUCCESS){
		wchar_t str[256];
		DWORD size=512;
		DWORD type=REG_SZ;
		ZeroMemory(str,512);
		RegQueryValueEx(key,0,0,&type,(BYTE*)str,&size);
		RegCloseKey(key);
		CString val(str);

		TCHAR szExe[MAX_PATH];
		::GetModuleFileName(NULL, szExe, MAX_PATH); 
		TCHAR value[MAX_PATH+10];
		wsprintf(value,L"\"%s\" \"%%1\"",szExe);

		m_FileAssoc=(val.Compare(value)==0);
	}*/
	if (RegOpenKeyEx(HKEY_CLASSES_ROOT,L".pdf",0,KEY_QUERY_VALUE,&key)==ERROR_SUCCESS){
		wchar_t str[256];
		DWORD size=512;
		DWORD type=REG_SZ;
		ZeroMemory(str,512);
		RegQueryValueEx(key,0,0,&type,(BYTE*)str,&size);
		RegCloseKey(key);

		CString val(str);
		TCHAR szExe[MAX_PATH];
		::GetModuleFileName(NULL, szExe, MAX_PATH); 
		TCHAR value[MAX_PATH+10];
		wsprintf(value,L"xpdffile",szExe);

		m_FileAssoc=(val.Compare(value)==0);
	}	CDialog::OnInitDialog();

	for (int i = 0; i<MAXMAPPINGS; i++){
		m_LocalButtonMappings[i] = m_ButtonMappings[i];
	}

	setupButtons();
	m_MapFunctionBox.AddString(L"None");
	m_MapFunctionBox.AddString(L"Next Page");
	m_MapFunctionBox.AddString(L"Prev Page");
	m_MapFunctionBox.AddString(L"Scroll Up");
	m_MapFunctionBox.AddString(L"Smartscroll Up");
	m_MapFunctionBox.AddString(L"Scroll Down");
	m_MapFunctionBox.AddString(L"Smartscroll Down");
	m_MapFunctionBox.AddString(L"Scroll Left");
	m_MapFunctionBox.AddString(L"Smartscroll Left");
	m_MapFunctionBox.AddString(L"Scroll Right");
	m_MapFunctionBox.AddString(L"Smartscroll Right");
	m_MapFunctionBox.AddString(L"Zoom in");
	m_MapFunctionBox.AddString(L"Zoom out");
	m_MapFunctionBox.AddString(L"Fit to Width");
	m_MapFunctionBox.AddString(L"Fit to Height");
	m_MapFunctionBox.AddString(L"Toggle Fullscreen");
	m_MapFunctionBox.AddString(L"Find Text");
	m_MapFunctionBox.AddString(L"Find Next");
	m_MapFunctionBox.AddString(L"Find Prior");
	m_MapFunctionBox.AddString(L"Show Outline");

	//Done-Button for Smartphone 
	SHINITDLGINFO shidi;
	ZeroMemory(&shidi, sizeof(SHINITDLGINFO));
	shidi.dwMask = SHIDIM_FLAGS;
	shidi.dwFlags = SHIDIF_DONEBUTTON | SHIDIF_SIPDOWN | SHIDIF_SIZEDLGFULLSCREEN | SHIDIF_EMPTYMENU;
	shidi.hDlg = m_hWnd;
	::SetForegroundWindow(m_hWnd);
	AYGSHELL::SHInitDialog(&shidi);
	

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CConfigDialog::OnCbnSelchangeMapbuttons()
{
	int button = m_MapButtonBox.GetItemData(m_MapButtonBox.GetCurSel());
	int i;
	for (i = 0; i<MAXMAPPINGS; i++)
	{
		
		if (m_LocalButtonMappings[i].button==button)
		{
			m_MapFunctionBox.SetCurSel(m_LocalButtonMappings[i].function);
			return;
		} 
		if (m_LocalButtonMappings[i].button==-1)
		{
			//Add new mapping entry
			m_LocalButtonMappings[i].button=button;
			m_MapFunctionBox.SetCurSel(m_LocalButtonMappings[i].function);
			return;
		}

	}
	
}

void CConfigDialog::OnCbnSelchangeMapfunction()
{
	int button = m_MapButtonBox.GetItemData(m_MapButtonBox.GetCurSel());
	int function = m_MapFunctionBox.GetCurSel();

	for (int i = 0; i<MAXMAPPINGS; i++)
	{
		if (m_LocalButtonMappings[i].button==button)
		{
			m_LocalButtonMappings[i].function = (Functions) function;
		}
	}
	
}

void CConfigDialog::OnOK()
{
	for (int i = 0; i<MAXMAPPINGS; i++){
		m_ButtonMappings[i] = m_LocalButtonMappings[i];
	}
	UpdateData();
	if (m_FileAssoc){
		// Set file association
		DWORD dwDisp;
		HKEY key1;
		RegCreateKeyEx(HKEY_CLASSES_ROOT,L".pdf",0, NULL, REG_OPTION_NON_VOLATILE,KEY_SET_VALUE, NULL, &key1, &dwDisp);
		RegSetValueEx(key1,0,0,REG_SZ,(BYTE*)L"xpdffile",18);
		RegCloseKey(key1);
		RegCreateKeyEx(HKEY_CLASSES_ROOT,L"xpdffile\\Shell\\Open\\Command",0, NULL, REG_OPTION_NON_VOLATILE,KEY_SET_VALUE, NULL, &key1, &dwDisp);
		TCHAR szExe[MAX_PATH];
		int size = ::GetModuleFileName(NULL, szExe, MAX_PATH); 
		TCHAR *value=new TCHAR[size+7+1];
		wsprintf(value,L"\"%s\" \"%%1\"",szExe);
		if (RegSetValueEx(key1,0,0,REG_SZ,(BYTE*)value,(size+7+1)*2)!=ERROR_SUCCESS){
			MessageBox(L"Cannot write path");
		}
		RegCloseKey(key1);
		RegCreateKeyEx(HKEY_CLASSES_ROOT,L"xpdffile\\Shell\\OpenDoc\\Command",0, NULL, REG_OPTION_NON_VOLATILE,KEY_SET_VALUE, NULL, &key1, &dwDisp);
		if (RegSetValueEx(key1,0,0,REG_SZ,(BYTE*)value,(size+7+1)*2)!=ERROR_SUCCESS){
			MessageBox(L"Cannot write path");
		}
		RegCloseKey(key1);

		//Set file icon
		RegCreateKeyEx(HKEY_CLASSES_ROOT,L"xpdffile\\DefaultIcon",0, NULL, REG_OPTION_NON_VOLATILE,KEY_SET_VALUE, NULL, &key1, &dwDisp);
		
//		TCHAR *pLastSlash = _tcsrchr(szExe, _T('\\'));
//		_tcscpy(pLastSlash, _T("\\XpdfFile.ico")); 

		wsprintf(value,L"%s,-142",szExe);
		if (RegSetValueEx(key1,0,0,REG_SZ,(BYTE*)value,(_tcsclen(value))*2)!=ERROR_SUCCESS){
			MessageBox(L"Cannot write path");
		}
		RegCloseKey(key1);

		delete value;

	} else {
		DWORD dwDisp;
		HKEY key1;
		RegCreateKeyEx(HKEY_CLASSES_ROOT,L".pdf",0, NULL, REG_OPTION_NON_VOLATILE,KEY_SET_VALUE, NULL, &key1, &dwDisp);
		RegSetValueEx(key1,0,0,REG_SZ,(BYTE*)L"pdffile",16);
		RegCloseKey(key1);
	}

	/*HKEY key1;
	if (RegCreateKeyEx(HKEY_LOCAL_MACHINE,L"System\\GWE\\Commctrl",0, NULL, REG_OPTION_NON_VOLATILE,KEY_SET_VALUE, NULL, &key1, NULL)!=ERROR_SUCCESS){
			MessageBox(L"Cannot open delay key");
	}
	if (m_ContextMenuDelay){
		DWORD value = 1;
		if (RegSetValueEx(key1,L"UseLongDelayForGestures",0,REG_DWORD,(BYTE*)&value,4)!=ERROR_SUCCESS){
			MessageBox(L"Cannot set delay value");
		}
	}
	else{
		DWORD value = 0;
		if (RegSetValueEx(key1,L"UseLongDelayForGestures",0,REG_DWORD,(BYTE*)&value,4)!=ERROR_SUCCESS){
			MessageBox(L"Cannot set delay value");
		}
	}*/

	CDialog::OnOK();
}

void CConfigDialog::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CConfigDialog::setupButtons()
{
	m_MapButtonBox.ResetContent();
	m_MapButtonBox.SetItemData(m_MapButtonBox.AddString(L"Up"),VK_UP);
	m_MapButtonBox.SetItemData(m_MapButtonBox.AddString(L"Down"),VK_DOWN);
	m_MapButtonBox.SetItemData(m_MapButtonBox.AddString(L"Left"),VK_LEFT);
	m_MapButtonBox.SetItemData(m_MapButtonBox.AddString(L"Right"),VK_RIGHT);
	m_MapButtonBox.SetItemData(m_MapButtonBox.AddString(L"Center"),VK_RETURN);
	m_MapButtonBox.SetItemData(m_MapButtonBox.AddString(L"Record"),VK_F10);
	for (int i=6; m_ButtonMappings[i].button!=-1; i++)
	{
		CString button;
		button.Format(L"Button #%d",m_ButtonMappings[i].button);
		m_MapButtonBox.SetItemData(m_MapButtonBox.AddString(button),m_ButtonMappings[i].button);
	}

	/*if (m_MapHWButtons){
		m_MapButtonBox.SetItemData(m_MapButtonBox.AddString(L"HW-Button 1"),0xc1);
		m_MapButtonBox.SetItemData(m_MapButtonBox.AddString(L"HW-Button 2"),0xc2);
		m_MapButtonBox.SetItemData(m_MapButtonBox.AddString(L"HW-Button 3"),0xc3);
		m_MapButtonBox.SetItemData(m_MapButtonBox.AddString(L"HW-Button 4"),0xc4);
		m_MapButtonBox.SetItemData(m_MapButtonBox.AddString(L"HW-Button 5"),0xc5);
	}*/
}

void CConfigDialog::OnToggleMapHWButtons()
{
	UpdateData();
	setupButtons();
}

void CConfigDialog::OnBnClickedSelectbutton()
{
	CButtonGrabberDialog bgd;
	bgd.DoModal();
	
	CString str;
	if (bgd.m_key)
	{
		switch (bgd.m_key)
		{
		case VK_UP:
			m_MapButtonBox.SelectString(-1,L"Up");
			OnCbnSelchangeMapbuttons();
			return;
		case VK_DOWN:
			m_MapButtonBox.SelectString(-1,L"Down");
			OnCbnSelchangeMapbuttons();
			return;
		case VK_LEFT:
			m_MapButtonBox.SelectString(-1,L"Left");
			OnCbnSelchangeMapbuttons();
			return;
		case VK_RIGHT:
			m_MapButtonBox.SelectString(-1,L"Right");
			OnCbnSelchangeMapbuttons();
			return;
		case VK_RETURN:
			m_MapButtonBox.SelectString(-1,L"Center");
			OnCbnSelchangeMapbuttons();
			return;
		case VK_F10:
			m_MapButtonBox.SelectString(-1,L"Record");
			OnCbnSelchangeMapbuttons();
			return;
		}
		CString button;
		button.Format(L"Button #%d",bgd.m_key);
		if (m_MapButtonBox.SelectString(-1,button)==CB_ERR)
		{
			int index=m_MapButtonBox.AddString(button);
			m_MapButtonBox.SetItemData(index,bgd.m_key);
			m_MapButtonBox.SetCurSel(index);
			OnCbnSelchangeMapbuttons();
		}
	}
	if (bgd.m_hotkey)
	{
		CString button;
		button.Format(L"Button #%d",bgd.m_hotkey);
		if (m_MapButtonBox.SelectString(-1,button)==CB_ERR)
		{
			int index=m_MapButtonBox.AddString(button);
			m_MapButtonBox.SetItemData(index,bgd.m_hotkey);
			m_MapButtonBox.SetCurSel(index);
			OnCbnSelchangeMapbuttons();
		}
	}
}

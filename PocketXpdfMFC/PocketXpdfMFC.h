// PocketXpdfMFC.h : main header file for the PocketXpdfMFC application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resourceppc.h"

// CPocketXpdfMFCApp:
// See PocketXpdfMFC.cpp for the implementation of this class
//

class CPocketXpdfMFCApp : public CWinApp
{
public:
	CPocketXpdfMFCApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
public:
	afx_msg void OnAppAbout();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMenuQuit();
public:
	virtual int ExitInstance();
public:
	void CheckForRunningInstance(void);
};

void SetCBSize(CComboBox & cb, int max_items);
extern CPocketXpdfMFCApp theApp;

#pragma once
#include "afxwin.h"


// CWaitDialog dialog

class CWaitDialog : public CDialog
{
	DECLARE_DYNAMIC(CWaitDialog)

public:
	CWaitDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CWaitDialog();

// Dialog Data
	enum { IDD = IDD_WAITDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
public:
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
public:
	CStatic m_StaticWaitMessage;
public:
	void setWaitMessage(CString waitmessage);
};

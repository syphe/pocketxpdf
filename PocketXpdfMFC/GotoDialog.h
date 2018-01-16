#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CGotoDialog dialog

class CGotoDialog : public CDialog
{
	DECLARE_DYNAMIC(CGotoDialog)

public:
	CGotoDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGotoDialog();

// Dialog Data
	enum { IDD = IDD_GOTO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
public:
	CListBox m_PagesListBox;
public:
	afx_msg void OnLbnSelchangeList1();
public:
	virtual BOOL OnInitDialog();
public:
	int m_lastPage;
public:
	int m_currentPage;
};

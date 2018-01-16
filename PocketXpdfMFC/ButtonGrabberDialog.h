#pragma once


// CButtonGrabberDialog dialog

class CButtonGrabberDialog : public CDialog
{
	DECLARE_DYNAMIC(CButtonGrabberDialog)

public:
	CButtonGrabberDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CButtonGrabberDialog();

// Dialog Data
	enum { IDD = IDD_WAITDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	virtual BOOL OnInitDialog();
public:
	int m_key;
public:
	int m_hotkey;
};

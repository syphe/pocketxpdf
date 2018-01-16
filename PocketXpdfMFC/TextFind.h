#pragma once

// CTextFind dialog

class CTextFind : public CDialog
{
	DECLARE_DYNAMIC(CTextFind)

public:
	CTextFind(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTextFind();

// Dialog Data
	enum { IDD = IDD_FINDTEXT };

	BOOL execute();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_text;
	BOOL m_caseSensetive;
	BOOL m_forward;
	BOOL m_begin;
public:
	BOOL m_markAll;
public:
	virtual BOOL OnInitDialog();
};

#pragma once
#include "afxcmn.h"

class Outline;
class GList;
class LinkAction;

// COutlineView dialog

class COutlineView : public CDialog
{
	DECLARE_DYNAMIC(COutlineView)

public:
	COutlineView(CWnd* pParent = NULL);   // standard constructor
	virtual ~COutlineView();

// Dialog Data
	enum { IDD = IDD_OUTLINE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	void SetOutline(Outline*);
	LinkAction * GetLinkAction();
	bool HasOutline();

private:
	CTreeCtrl m_OutlineCtrl;
	Outline * m_Outline;
	LinkAction * m_result;
	CCommandBar m_CommandBar;

	void LoadLevel(HTREEITEM tree_root, GList * doc_root);
	afx_msg void OnItemExpanding(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNMDblclkOutline(NMHDR *pNMHDR, LRESULT *pResult);
};

// MainFrm.h : interface of the CMainFrame class
//


#pragma once

#include "ChildView.h"



class CMainFrame : public CFrameWnd
{
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
#endif

public:  // control bar embedded members
	CCommandBar m_wndCommandBar;
	CChildView    m_wndView;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg LRESULT PostCreateWindow(WPARAM wParam, LPARAM lParam);
	
	DECLARE_MESSAGE_MAP()
public:
	void setCaption(LPWSTR text);
public:
	bool m_Fullscreen;
public:
	afx_msg void OnToolsFullscreen();
public:
	afx_msg void OnUpdateToolsFullscreen(CCmdUI *pCmdUI);

	void DoModal(CWnd * wnd, CWnd * parent);
private:
	bool UpdateMenu(CCmdTarget *cmdtarget, CMenu * pPopupMenu);
public:
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg BOOL OnCopyData(CWnd *, COPYDATASTRUCT*);
public:
	void HideCommandBar(bool unhide=false);
};



#pragma once
#include "afxwin.h"
// CTextOutDialog dialog
#define NO_CMAP
#include "PDFDoc.h"
#include "TextOutputDev.h"
#undef NO_CMAP

#include "ColorCtrl.h" 
#include "FontCtrl.h"
#include "TextOutEditCtrl.h"


class CTextOutDialog : public CDialog
{
	DECLARE_DYNAMIC(CTextOutDialog)

public:
	CTextOutDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTextOutDialog();

// Dialog Data
	enum { IDD = IDD_TEXTONLYDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CColorCtrl<CFontCtrl<CTextOutEditCtrl>> m_TextEdit;
public:
	virtual BOOL OnInitDialog();
public:
	PDFDoc* m_pPDFDoc;
public:
	int m_CurrentPage;
public:
	CCommandBar m_CommandBar;
public:
	afx_msg void OnNextPage();
public:
	afx_msg void OnPrevPage();
public:
	void renderPage(void);
public:
	afx_msg void OnToolsFullscreen();
public:
	afx_msg void OnToolsGotopage();
public:
	afx_msg void OnFontsize(UINT nID);
public:
	afx_msg void OnUpdateFontsize(CCmdUI *pCmdUI);
public:
	afx_msg void OnFontsizeDoublesize();
public:
	afx_msg void OnUpdateFontsizeDoublesize(CCmdUI *pCmdUI);
public:
	bool m_DoubleFontSize;
public:
	int m_FontSize;
public:
	void updateFontSize(void);
public:
	void extractTextFromPdf(TextOutputDev *dev,CString * output);

public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
public:
	bool m_FullScreen;
public:
	afx_msg void OnUpdateToolsFullscreen(CCmdUI *pCmdUI);
public:
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
public:
	afx_msg void OnPopupCopy();
};

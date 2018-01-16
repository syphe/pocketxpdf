// ChildView.h : interface of the CChildView class
//


#pragma once

#define NO_CMAP
#include <PDFDoc.h>
#include "afxwin.h"
#include "atltypes.h"

#include "SplashOutputDev.h"
#include "configdialog.h"
#include "gotodialog.h"
#include "waitdialog.h"
#include "TextOutDialog.h"
#include "TextFind.h"
#include "ImageProps.h"
#include "OutlineView.h"

#undef NO_CMAP

enum
{
	LEFT_SCREEN, RIGHT_SCREEN,
	PRIOR_SCREEN, NEXT_SCREEN,
	NEXT_ROW, NEXT_CELL
};

/* tap zones */

enum
{
	UL, UC, UR,
	ML, MC, MR,
	BL, BC, BR
};

// CChildView window

class CChildView : public CWnd
{
// Construction
public:
	CChildView();

// Attributes
public:

// Operations
public:

// Overrides
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CChildView();

	// Generated message map functions
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	PDFDoc *m_PDFDoc;
public:
	SplashOutputDev *m_splashOut;
public:
	afx_msg void OnMenuOpen();
public:
	unsigned char*m_bitmapBytes;
public:
	int m_CurrentPage;
public:
	CBitmap *m_Bitmap;
public:
	int m_ViewOffsetX;
public:
	int m_ViewOffsetY;
public:
	CWaitDialog m_WaitDialog;
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
public:
	CPoint m_LastMousePos;
public:
	CRect m_LastDragRect;
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
	afx_msg void OnNextPage();
public:
	afx_msg void OnZoomOut();
public:
	afx_msg void OnZoomIn();
public:
	afx_msg void OnPrevPage();
	void renderPage(BOOL redraw = TRUE);
	static const int VERT_SCROLL_STEPS_PER_PAGE =16;
	static const int HORZ_SCROLL_STEPS_PER_PAGE =16;
	double m_renderDPI;
public:
	afx_msg void OnUpdateNextPage(CCmdUI *pCmdUI);
public:
	afx_msg void OnUpdatePrevPage(CCmdUI *pCmdUI);
public:
	afx_msg void OnFileOptions();
public:
	CConfigDialog m_ConfigDialog;
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
public:
	CComboBox *m_ZoomComboBox;
public:
	afx_msg void OnToolsFitscreenwidth();
	afx_msg void FitscreenwidthInternal();
public:
	afx_msg void OnToolsFitscreenheight();
public:
	afx_msg void OnToolsGotopage();
public:
	CGotoDialog m_GotoDialog;
public:
	afx_msg void OnRotate90();
public:
	afx_msg void OnRotate90CCW();
public:
	afx_msg void OnRotate180();
public:
	int m_Rotation;
public:
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
public:
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
public:
	afx_msg LRESULT OnHotkey(WPARAM,LPARAM);
public:
	void execFunction(CConfigDialog::Functions function);

public:
	bool loadFile(CString fileName);
public:
	afx_msg void OnRotation0();
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
public:
	afx_msg void OnToolsViewtextonly();
public:
	CTextOutDialog m_TextOutDialog;
public:
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
public:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
public:
	afx_msg LRESULT PostCreateWindow(WPARAM wParam, LPARAM lParam);

public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
public:
	bool m_IsInDragZoomMode;
public:
	afx_msg void OnDragZoom();
public:
	afx_msg void OnUpdateDragZoom(CCmdUI *pCmdUI);

private:
	int m_PageWidth;
	int m_PageHeight;
	int m_Columns;
	double m_LastRenderDPI;
	BOOL m_MouseDown;
	BOOL m_DragMode;
	BOOL m_SearchStarted;
	BOOL m_HideMarks;
	DWORD m_DragTime;
	CRect m_bbox; /* image bounding box */
	CTextFind m_FindDlg;
	CImageProps m_ImagePropsDlg;
	COutlineView m_OutlineView;

	int m_SearchPage;
	CArray<CRect> m_Selection; /* all the found areas on page */
	TextOutputDev m_FindPage;  /* used for iterative search */

	static Unicode * GetUnicodeString(LPCTSTR, int length);
	CSize GetPageExtent();
	CSize GetScreenSize();
	void UpdateScrollbars();
	void MoveView(int dx, int dy, BOOL redraw = TRUE, BOOL smooth = FALSE);
	void GotoView(int x, int y, BOOL redraw = TRUE, BOOL smooth = FALSE);
	void GotoPage(int pageno, int x, int y, BOOL redraw = TRUE);
	void RefreshPageView(int x, int y);
	CPoint FindCoord(CPoint);

	/* page search */
	BOOL FindText(int searchPage, BOOL reverse);
	/* step by step search */
	void FindFirst();
	void FindNext(BOOL reverse);
	void AdvanceCell(int mode);
	void SearchCompleted(int searchPage);

	void RedrawWindow();
	void ScrollScreen(int dir);

	afx_msg void OnNextScreen()  { ScrollScreen(NEXT_SCREEN); }
	afx_msg void OnPriorScreen() { ScrollScreen(PRIOR_SCREEN); }
	afx_msg void OnLeftScreen()  { ScrollScreen(LEFT_SCREEN); }
	afx_msg void OnRightScreen() { ScrollScreen(RIGHT_SCREEN); }
	afx_msg void OnNextRow()     { ScrollScreen(NEXT_ROW); }
	afx_msg void OnNextCell()    { ScrollScreen(NEXT_CELL); }

	afx_msg void OnUpdateNextScreen(CCmdUI *pCmdUI);
	
	afx_msg void OnUpdatePriorScreen(CCmdUI *pCmdUI);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnUpdateFindCommand(CCmdUI *pCmdUI);
	afx_msg void OnToolsFindtext();
	afx_msg void OnFindNext();
	afx_msg void OnFindPrior();
	afx_msg void OnFindClearmarks();
	afx_msg void RequireDocument(CCmdUI *pCmdUI);
	afx_msg void OnSetColumnBrowse1();
	afx_msg void OnSetColumnBrowse2();
	afx_msg void OnUpdateColumnBrowse1(CCmdUI *pCmdUI);
	afx_msg void OnUpdateColumnBrowse2(CCmdUI *pCmdUI);
	afx_msg void OnToolsImageInfo();
	afx_msg void OnToolsOutline();
	afx_msg void OnUpdateToolsOutline(CCmdUI *pCmdUI);
	CWinThread *m_renderingThread;
	static DWORD WINAPI RenderingThread (LPVOID param);
	static int m_PageToRenderByThread;
	static int m_LastPageRenderedByThread;

	CString GetBookmarkString(void);
	void RestoreBookmark(CString bookmark);
	afx_msg void OnBookmarkSave(UINT nID);
	afx_msg void OnBookmarkLoad(UINT nID);
	afx_msg void OnUpdateBookmarkSave(CCmdUI *pCmdUI);
	afx_msg void OnUpdateBookmarkLoad(CCmdUI *pCmdUI);
private:
	void OnUpdateBookmarkInternal(CCmdUI * pCmdUI, UINT lastButtonID);
	afx_msg void OnRecentFiles(UINT nID);
	afx_msg void OnUpdateRecentFiles(CCmdUI *pCmdUI);
};

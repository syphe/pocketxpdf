// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "PocketXpdfMFC.h"
#include "ChildView.h"
#include "MainFrm.h"
#include <exception>

#include "WaitDialog.h"
#include "PasswordDialog.h"
#include "errorcodes.h"
#include "MiscPropertyPage.h"
#include "ButtonMappingPropertyPage.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "TextOutputDev.h"
#include "SplashBitmap.h"
#include "Splash.h"
#include "Outline.h"
#include "GList.h"
#include "Link.h"

#define NO_CMAP
#include "globalParams.h"
#undef NO_CMAP
// CChildView

#define FIND_DPI 300
#define SPACE_X 16
#define SPACE_Y 16
int CChildView::m_PageToRenderByThread=-1;
int CChildView::m_LastPageRenderedByThread=-1;
CChildView::CChildView()
: m_PDFDoc(NULL)
, m_splashOut(NULL)
, m_bitmapBytes(NULL)
, m_CurrentPage(1)
, m_Bitmap(NULL)
, m_ViewOffsetX(0)
, m_ViewOffsetY(0)
, m_LastMousePos(0)
, m_LastDragRect(0)
, m_renderDPI(72)
, m_LastRenderDPI(72)
, m_ZoomComboBox(NULL)
, m_Rotation(0)
, m_IsInDragZoomMode(false)
, m_Columns(1)
, m_SearchStarted(FALSE)
, m_FindPage(NULL, gTrue, gFalse, gFalse)
, m_renderingThread(NULL)
, m_MouseDown(FALSE)
, m_HideMarks(FALSE)
{
	TCHAR szExe[MAX_PATH];
	int size = ::GetModuleFileName(NULL, szExe, MAX_PATH); 
	TCHAR *pLastSlash = _tcsrchr(szExe, _T('\\'));
	
	if (pLastSlash){
		// Truncate at slash to get app dir
		*pLastSlash=L'\0';
	}

	globalParams = new GlobalParams("notexistent");
    globalParams->setEnableT1lib("no");
    globalParams->setEnableFreeType("yes");
    globalParams->setErrQuiet(gTrue);
	if (m_ConfigDialog.m_AntiAliasing)
		globalParams->setAntialias("yes");
	else
		globalParams->setAntialias("no");

	char *baseDir = new char[wcslen(szExe)+1];
	sprintf(baseDir,"%S",szExe);
	globalParams->setupBaseFonts(baseDir);
	delete baseDir;
	

}

CChildView::~CChildView()
{
	
	if (m_PDFDoc!=NULL)
	{
		// Update recent file entry (we know it is on the top of the list)
		CWinApp* pApp = AfxGetApp();
		pApp->WriteProfileString(L"Xpdf",L"RecentFile0",GetBookmarkString());
	}
	
	delete globalParams;
	
	if (m_splashOut!=NULL)
	{
		delete m_splashOut;
	}
	
	if (m_PDFDoc!=NULL)
	{
		delete m_PDFDoc;
	}

	if (m_Bitmap!=NULL)
	{
		delete m_Bitmap;
	}
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_CONTEXTMENU()
	ON_WM_KEYUP()
	ON_WM_CREATE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONUP()
	ON_WM_SIZE()
	ON_MESSAGE(WM_HOTKEY,&CChildView::OnHotkey) 
	ON_MESSAGE(WM_USER+1,&CChildView::PostCreateWindow)
	ON_COMMAND(ID_MENU_OPEN, &CChildView::OnMenuOpen)
	ON_COMMAND(ID_NEXT_PAGE, &CChildView::OnNextPage)
	ON_COMMAND(ID_PREV_PAGE, &CChildView::OnPrevPage)
	ON_COMMAND(ID_ZOOM_OUT, &CChildView::OnZoomOut)
	ON_COMMAND(ID_ZOOM_IN, &CChildView::OnZoomIn)
	ON_COMMAND(ID_FILE_OPTIONS, &CChildView::OnFileOptions)
	ON_COMMAND(ID_TOOLS_FITSCREENWIDTH, &CChildView::OnToolsFitscreenwidth)
	ON_COMMAND(ID_TOOLS_FITSCREENHEIGHT, &CChildView::OnToolsFitscreenheight)
	ON_COMMAND(ID_TOOLS_GOTOPAGE, &CChildView::OnToolsGotopage)
	ON_COMMAND(ID_ROTATE_90, &CChildView::OnRotate90)
	ON_COMMAND(ID_ROTATE_91, &CChildView::OnRotate90CCW)
	ON_COMMAND(ID_ROTATE_180, &CChildView::OnRotate180)
	ON_COMMAND(ID_ROTATION_0, &CChildView::OnRotation0)
	ON_COMMAND(ID_TOOLS_VIEWTEXTONLY, &CChildView::OnToolsViewtextonly)
	ON_COMMAND(ID_DRAG_ZOOM, &CChildView::OnDragZoom)
	ON_COMMAND(ID_NEXT_SCREEN, &CChildView::OnNextScreen)
	ON_COMMAND(ID_PRIOR_SCREEN, &CChildView::OnPriorScreen)
	ON_COMMAND(ID_TOOLS_FIND, &CChildView::OnToolsFindtext)
	ON_COMMAND(ID_TOOLS_FIND_NEXT, &CChildView::OnFindNext)
	ON_COMMAND(ID_TOOLS_FIND_PRIOR, &CChildView::OnFindPrior)
	ON_COMMAND(ID_TOOLS_FIND_CLEARMARKS, &CChildView::OnFindClearmarks)
	ON_COMMAND(ID_COLUMNBROWSE_1COLUMN, &CChildView::OnSetColumnBrowse1)
	ON_COMMAND(ID_COLUMNBROWSE_2COLUMN, &CChildView::OnSetColumnBrowse2)
	ON_COMMAND(ID_TOOLS_IMAGE_INFO, &CChildView::OnToolsImageInfo)
	ON_COMMAND(ID_TOOLS_OUTLINE, &CChildView::OnToolsOutline)
	ON_UPDATE_COMMAND_UI(ID_NEXT_PAGE, &CChildView::OnUpdateNextPage)
	ON_UPDATE_COMMAND_UI(ID_PREV_PAGE, &CChildView::OnUpdatePrevPage)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_IN, &CChildView::RequireDocument)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_OUT, &CChildView::RequireDocument)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_FITSCREENWIDTH, &CChildView::RequireDocument)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_FITSCREENHEIGHT, &CChildView::RequireDocument)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_GOTOPAGE, &CChildView::RequireDocument)
	ON_UPDATE_COMMAND_UI(ID_ROTATE_90, &CChildView::RequireDocument)
	ON_UPDATE_COMMAND_UI(ID_ROTATE_91, &CChildView::RequireDocument)
	ON_UPDATE_COMMAND_UI(ID_ROTATE_180, &CChildView::RequireDocument)
	ON_UPDATE_COMMAND_UI(ID_ROTATION_0, &CChildView::RequireDocument)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_VIEWTEXTONLY, &CChildView::RequireDocument)
	ON_UPDATE_COMMAND_UI(ID_DRAG_ZOOM, &CChildView::OnUpdateDragZoom)
	ON_UPDATE_COMMAND_UI(ID_NEXT_SCREEN, &CChildView::OnUpdateNextScreen)
	ON_UPDATE_COMMAND_UI(ID_PRIOR_SCREEN, &CChildView::OnUpdatePriorScreen)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_FIND, &CChildView::OnUpdateFindCommand)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_FIND_NEXT, &CChildView::OnUpdateFindCommand)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_FIND_PRIOR, &CChildView::OnUpdateFindCommand)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_FIND_CLEARMARKS, &CChildView::OnUpdateFindCommand)
	ON_UPDATE_COMMAND_UI(ID_COLUMNBROWSE_1COLUMN, &CChildView::OnUpdateColumnBrowse1)
	ON_UPDATE_COMMAND_UI(ID_COLUMNBROWSE_2COLUMN, &CChildView::OnUpdateColumnBrowse2)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_IMAGE_INFO, &CChildView::RequireDocument)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_OUTLINE, &CChildView::OnUpdateToolsOutline)
	ON_COMMAND_RANGE(ID_BOOKMARK_SAVE_1, ID_BOOKMARK_SAVE_5, &CChildView::OnBookmarkSave)
	ON_COMMAND_RANGE(ID_BOOKMARK_LOAD_1, ID_BOOKMARK_LOAD_5, &CChildView::OnBookmarkLoad)
	ON_UPDATE_COMMAND_UI_RANGE(ID_BOOKMARK_SAVE_1, ID_BOOKMARK_SAVE_5, &CChildView::OnUpdateBookmarkSave)
	ON_UPDATE_COMMAND_UI_RANGE(ID_BOOKMARK_LOAD_1, ID_BOOKMARK_LOAD_5, &CChildView::OnUpdateBookmarkLoad)
	ON_COMMAND_RANGE(ID_RECENTFILES_1, ID_RECENTFILES_5, &CChildView::OnRecentFiles)
	ON_UPDATE_COMMAND_UI_RANGE(ID_RECENTFILES_1, ID_RECENTFILES_5, &CChildView::OnUpdateRecentFiles)
END_MESSAGE_MAP()



// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.style |= WS_VSCROLL | WS_HSCROLL ;

	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), 
		(HBRUSH) ::GetStockObject(GRAY_BRUSH), NULL);

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	if (m_Bitmap != NULL) {
		
		// Draw the rendered document

		CDC mdc;
		mdc.CreateCompatibleDC(&dc);
		CBitmap* pOld = mdc.SelectObject(m_Bitmap);

		RECT clientArea;
		this->GetClientRect(&clientArea);
		
		int bmWidth = m_Bitmap->GetBitmapDimension().cx;
		int bmHeight = m_Bitmap->GetBitmapDimension().cy;
		int clientWidth = clientArea.right-clientArea.left;
		int clientHeight = clientArea.bottom-clientArea.top;
		
		int targetOffsetX=0;
		int targetOffsetY=0;

		int viewOffsetX=m_ViewOffsetX;
		int viewOffsetY=m_ViewOffsetY;


		BOOL bRet = dc.BitBlt(0,0,clientWidth,clientHeight,&mdc,viewOffsetX,viewOffsetY,SRCCOPY);
		mdc.SelectObject(pOld);


		//Draw background if neccessary, update scrollstate

		CBrush background;
		background.CreateStockObject(GRAY_BRUSH);
		if (viewOffsetX<0)
			dc.FillRect(CRect(clientArea.left,clientArea.top,-viewOffsetX,clientArea.bottom),&background);
		if (viewOffsetY<0)
			dc.FillRect(CRect(clientArea.left,clientArea.top,clientArea.right,-viewOffsetY),&background);
		if (bmWidth-m_ViewOffsetX < clientWidth )
			dc.FillRect(CRect(bmWidth-m_ViewOffsetX,clientArea.top,clientArea.right,clientArea.bottom),&background);
		if (bmHeight-m_ViewOffsetY < clientHeight)
			dc.FillRect(CRect(clientArea.left,bmHeight-m_ViewOffsetY,clientArea.right,clientArea.bottom),&background);
		
		
		//Update Scrollbars
		
		SetScrollPos(SB_HORZ,viewOffsetX);
		SetScrollPos(SB_VERT,viewOffsetY);

		/* draw selection */
		if (!m_HideMarks && m_CurrentPage == m_SearchPage) {
			const double mul = m_renderDPI / FIND_DPI;

			CPen draw_pen, * old_pen;
			CGdiObject * old_brush;

			draw_pen.CreatePen(PS_SOLID, 0, RGB(127, 127, 255));
			old_pen = dc.SelectObject(&draw_pen);
			old_brush = dc.SelectStockObject(NULL_BRUSH);

			for(int j = 0; j < m_Selection.GetCount(); j++) {
				/* transform selection into current DPI and
				 * offset it by current upper/left corner.
				 * Provided m_renderDPI was integer we could
				 * use CRect::MulDiv.
				 */
				CRect nsel = m_Selection[j];
				CRect r;

				nsel.left   = int(nsel.left   * m_renderDPI / FIND_DPI);
				nsel.top    = int(nsel.top    * m_renderDPI / FIND_DPI);
				nsel.right  = int(nsel.right  * m_renderDPI / FIND_DPI);
				nsel.bottom = int(nsel.bottom * m_renderDPI / FIND_DPI);

				/* enlarge right/bottom */
				nsel.right++;
				nsel.bottom++;

				/* rotate using predefined angles only */
				switch(m_Rotation) {
				case 0:
					r = nsel;
					break;

				case 90:
					r = CRect(
						m_PageWidth - nsel.bottom,
						nsel.left,
						m_PageWidth - nsel.top,
						nsel.right);
					break;

				case 180:
					r = CRect(
						m_PageWidth - nsel.right,
						m_PageHeight - nsel.bottom,
						m_PageWidth - nsel.left,
						m_PageHeight - nsel.top);
					break;

				case 270:
					r = CRect(
						nsel.top,
						m_PageHeight - nsel.right,
						nsel.bottom,
						m_PageHeight - nsel.left);
					break;
				}

				r.OffsetRect(-m_ViewOffsetX, -m_ViewOffsetY);

				/* TODO: this needs better indication */
				dc.Rectangle(r);
				r.DeflateRect(1,1);
				dc.InvertRect(r);
			}

			dc.SelectObject(old_pen);
			dc.SelectObject(old_brush);
		}

		if (m_IsInDragZoomMode) {
			dc.SetTextColor(RGB(255,0,0));
			dc.DrawText(L"Drag a Rectangle around the \narea to zoom!",CRect(20,20,200,200),DT_NOCLIP);
		}
	}
	
	/*CRect rect,rect2;
	GetTopLevelFrame()->GetClientRect(&rect);
	GetClientRect(&rect2);
	MEMORYSTATUS gmi;
	GlobalMemoryStatus (&gmi);
	CString meminfo;
	meminfo.Format(L"%d / %d, %d %d %d %d\n%d %d %d %d",gmi.dwAvailPhys,gmi.dwTotalPhys,
		rect.top,rect.bottom,rect.left,rect.right,
		rect2.top,rect2.bottom,rect2.left,rect2.right);
	dc.DrawText(meminfo,meminfo.GetLength(),CRect(20,20,200,200),DT_NOCLIP);
	*/
}
bool CChildView::loadFile(CString fileName)
{

	if (m_PDFDoc!=NULL)
	{
		//if a file is currently loaded, it is on top of the recent file list,
		//so we update that entry before loading a new file and reorganizing the list
		CWinApp* pApp = AfxGetApp();
		pApp->WriteProfileString(L"Xpdf",L"RecentFile0",GetBookmarkString());
	}
		
	char ascii[256];
	for (int i=0; i<fileName.GetLength()&&i<255;i++){
		ascii[i]=(char)fileName.GetAt(i);
		
	}
	ascii[fileName.GetLength()]='\0';

	if (m_splashOut!=NULL)
	{
		delete m_splashOut;
	}
	SplashColor paperColor;
	paperColor[0] = 0xff;
	paperColor[1] = 0xff;
	paperColor[2] = 0xff;
	
	//Note: the alignment is given by GDI requirements: bitmaps have to be 16-bit aligned.
	m_splashOut = new SplashOutputDev(splashModeBGR8, 4, gFalse, paperColor);

	if (m_PDFDoc!=0){
		delete m_PDFDoc;
	}

	m_WaitDialog.setWaitMessage(L"Loading PDF...");
	m_WaitDialog.ShowWindow(SW_SHOWNA);
	m_WaitDialog.RedrawWindow(0,0,RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
	m_PDFDoc = new PDFDoc(new GString(ascii), NULL,NULL);
	m_WaitDialog.ShowWindow(SW_HIDE);
	
	while (!m_PDFDoc->isOk()) 
	{
		
		if (m_PDFDoc->getErrorCode() == errEncrypted)
		{
			CPasswordDialog pwd;
			if (pwd.DoModal() == IDCANCEL)
			{

				delete m_PDFDoc;
				m_PDFDoc=NULL;
				return false;
			}
			wchar_t *w_opw = pwd.m_OwnerPassword.GetBuffer();
			char *opw = new char[wcslen(w_opw)+1];
			sprintf(opw,"%S",w_opw);
			GString gpw(opw);
			delete opw;
							
			m_PDFDoc = new PDFDoc(new GString(ascii), &gpw,&gpw);
		
		} else 
		{
			delete m_PDFDoc;
			m_PDFDoc=NULL;
			return false;
		}
		
		
	}
	
	//File has been properly loaded

	m_splashOut->startDoc(m_PDFDoc->getXRef());
	m_OutlineView.SetOutline(m_PDFDoc->getOutline());
	m_LastPageRenderedByThread=-1;

	//Restore last state, if applicable
	CWinApp* pApp = AfxGetApp();
	int i;
	for (i=0;i<5;i++)
	{
		CString name;
		name.Format(L"RecentFile%d",i);
		CString bookmark = pApp->GetProfileString(L"Xpdf",name,L"");
		if (bookmark.Find(fileName)!=-1)
		{
			RestoreBookmark(bookmark);
			break;
		}
	}
	
	if (i == 5)
	{
		//File is not in the recent file list, set defaults
		m_CurrentPage = 1;
		m_ViewOffsetX = 0;
		m_ViewOffsetY = 0;
		FitscreenwidthInternal();
	}

	//Reorganize recent file list (new file on top, last entry may drop out)
	if (i == 5)
	{
		//last entry will not be copied
		i=4;
	}
	for (;i>0;i--)
	{
		CString name,name2;
		name.Format(L"RecentFile%d",i);
		name2.Format(L"RecentFile%d",i-1);
		CString bookmark = pApp->GetProfileString(L"Xpdf",name2,L"");
		pApp->WriteProfileString(L"Xpdf",name,bookmark);
	}
	// Write new file as top entry
	pApp->WriteProfileString(L"Xpdf",L"RecentFile0",GetBookmarkString());

	
	return true;
}

void CChildView::OnMenuOpen()
{
//#ifndef WIN32_PLATFORM_WFSP
	CFileDialog fileDialog(TRUE, L"pdf", L"*.pdf",
      OFN_FILEMUSTEXIST, L"PDF Documents (*.pdf)|*.pdf|All Files (*.*)|*.*||", this);
	if (fileDialog.DoModal()==IDOK){
		CString filename = fileDialog.GetPathName();
		
		
		if (loadFile(filename))
		{
			renderPage();
		} else
		{
			MessageBox(L"Failed to load file.",L"Error");
			dynamic_cast<CMainFrame*>(GetTopLevelFrame())->setCaption(L"PocketXpdf");

		}
  

	}
//#endif
}

void CChildView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_IsInDragZoomMode)
	{
		CClientDC dc(this);
		CRect newRect(m_LastMousePos,point);
		newRect.NormalizeRect();
		dc.DrawDragRect(newRect,CSize(),m_LastDragRect == CRect()?0:m_LastDragRect,CSize());
		m_LastDragRect=newRect;
	} else {
		//regular document dragging
		if (!m_DragMode) {
			int dx = abs(point.x - m_LastMousePos.x);
			int dy = abs(point.y - m_LastMousePos.y);
			if (dx > 3 || dy > 3)
				m_DragMode = TRUE;
		}

		if (m_DragMode) {
			MoveView(m_LastMousePos.x-point.x, m_LastMousePos.y-point.y);
			m_LastMousePos.x = point.x;
			m_LastMousePos.y = point.y;
		}
	}
}

void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_LastMousePos.SetPoint(point.x,point.y);
	m_LastDragRect.SetRectEmpty();
	m_DragMode = FALSE;
	m_DragTime = GetTickCount();
	m_MouseDown = TRUE;
	CWnd::OnLButtonDown(nFlags, point);
}

void CChildView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_PDFDoc != NULL) {
		if (m_IsInDragZoomMode) {
			RECT clientArea;
		
			GetClientRect(&clientArea);
			double height = clientArea.bottom-clientArea.top;
			double width = clientArea.right-clientArea.left;
			double factor=0.0;

			m_IsInDragZoomMode=false;
			if (m_LastDragRect.Width()!=0){
				factor= width/m_LastDragRect.Width();
			}

			if (m_LastDragRect.Height()!=0){
				double newfactor= height/m_LastDragRect.Height();
				if (newfactor<factor){
					factor=newfactor;
				}
			}

			if (factor>0.0){
				m_renderDPI*= factor;
				renderPage();
				GotoView((m_ViewOffsetX+m_LastDragRect.left)*factor,(m_ViewOffsetY+m_LastDragRect.top)*factor);
				//m_ViewOffsetX=(int) ((m_ViewOffsetX+m_LastDragRect.left)*factor);
				//m_ViewOffsetY=(int) ((m_ViewOffsetY+m_LastDragRect.top)*factor);
			}

		}  else if (false) //TODO Tapzones disabled until release of version 0.4.0
				//m_MouseDown && !m_DragMode && GetTickCount() - m_DragTime < 1500) 
		{
			CRect r;
			int dx, dy;
			/* determine tap zone */
			GetClientRect(r);

			dx = r.Width() / 3;
			dy = r.Height() / 3;

			int zone = point.x / dx + 3 * (point.y / dy);

			/* TODO: zone map should be configured */

			switch(zone) {
			case UL: break;
			case UC: OnPriorScreen(); break;
			case UR: break;
			case ML: OnLeftScreen(); break;
			case MC: break;
			case MR: OnRightScreen(); break;
			case BL: OnNextRow(); break;
			case BC: OnNextScreen(); break;
			case BR: OnNextCell(); break;
			}
		}
	}
	m_MouseDown = FALSE;
	CWnd::OnLButtonUp(nFlags, point);
}

void CChildView::OnNextPage()
{
	GotoPage(m_CurrentPage + 1, m_ViewOffsetX, m_ViewOffsetY);
}

void CChildView::OnUpdateNextPage(CCmdUI *pCmdUI)
{
	if (m_PDFDoc !=NULL && m_CurrentPage<m_PDFDoc->getNumPages()){
		pCmdUI->Enable();
	} else {
		pCmdUI->Enable(false);
	}
}

void CChildView::OnPrevPage()
{
	GotoPage(m_CurrentPage - 1, m_ViewOffsetX, m_ViewOffsetY);
}

void CChildView::OnUpdatePrevPage(CCmdUI *pCmdUI)
{
	if (m_CurrentPage>1 && m_PDFDoc !=NULL){
		pCmdUI->Enable();
	} else {
		pCmdUI->Enable(false);
	}
}

void CChildView::OnZoomOut()
{
	m_renderDPI = m_renderDPI * 3 / 4;
	RefreshPageView(m_ViewOffsetX / 2 , m_ViewOffsetY / 2);
}

void CChildView::RequireDocument(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_PDFDoc!=NULL);
}

void CChildView::OnZoomIn()
{
	m_renderDPI = m_renderDPI * 4 / 3;
	RefreshPageView(m_ViewOffsetX * 2 , m_ViewOffsetY * 2);
}

DWORD WINAPI CChildView::RenderingThread (LPVOID param)
{
	CChildView * cv= (CChildView*) param;
	try
	{
		cv->m_PDFDoc->displayPage(cv->m_splashOut, m_PageToRenderByThread, 
								cv->m_renderDPI, cv->m_renderDPI, cv->m_Rotation, 
								gFalse, gTrue, gFalse);
		m_LastPageRenderedByThread=m_PageToRenderByThread;
	}catch(const std::exception)
	{
				cv->MessageBox(L"Ouf of memory. Quitting.");
				AfxGetMainWnd()->PostMessageW(WM_CLOSE,0,0);
	}
	return 0;
}

void CChildView::renderPage(BOOL redraw)
{
	if (m_PDFDoc != NULL) {
	
		//Show the wait window
		m_WaitDialog.setWaitMessage(L"Rendering page...");
		m_WaitDialog.ShowWindow(SW_SHOW);
		m_WaitDialog.RedrawWindow(0,0,RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);

		if (m_TextOutDialog.GetSafeHwnd()!=NULL && m_TextOutDialog.IsWindowVisible()) {
			m_TextOutDialog.m_CurrentPage=m_CurrentPage;
			m_TextOutDialog.renderPage();
		} else {

			int bmWidth, bmHeight;

			/* since page rendering could take a long time,
			 * ensure window content is updated if painting
			 * is pending.
			 */
			UpdateWindow();

			m_splashOut->clearModRegion();

			//Delete the old bitmap, we always create a fresh one after rendering
			if (m_Bitmap){
				delete m_Bitmap;
			}	
			m_Bitmap = new CBitmap();

			//Heuristically check if we have enough memory ;-)
			double newbytes = (m_renderDPI/72.0)*(m_renderDPI/72.0) * m_PDFDoc->getPageCropWidth(m_CurrentPage)* m_PDFDoc->getPageCropHeight(m_CurrentPage); //new approx. number of pixels
			newbytes = newbytes * 3 + newbytes*2; //24-bit splashbitmap, 16-bit gdi-bitmap
			newbytes=(newbytes*1.2); //Safety area;
			newbytes -= m_splashOut->getBitmap()->getWidth()*m_splashOut->getBitmap()->getHeight()*3; //substract old 24-bit splash bitmap size

			void* testAllocation = malloc((int)newbytes);
			if (testAllocation==0){
				//We better dont zoom this far in!
				MessageBox(L"Not enough free memory to zoom in that close. Resetting to standard zoom now.");
				
//				m_ViewOffsetX=0;
//				m_ViewOffsetY=0;
				if (m_renderDPI <= m_LastRenderDPI)
					m_renderDPI /= 2;
				else
					m_renderDPI = m_LastRenderDPI;

				m_WaitDialog.ShowWindow(SW_SHOWNA);
				m_WaitDialog.RedrawWindow(0,0,RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);

				/*static bool reentranceCheck=false;
				//This "should" set m_renderDPI to a reasonable value, unless the page has a very strange paper size
				if (!reentranceCheck){
					reentranceCheck=true;
					this->SendMessageW(WM_COMMAND,ID_TOOLS_FITSCREENWIDTH);
					reentranceCheck=false;
				} else {
					MessageBox(L"Ouch, should never happen. Please send me an email mentioning \"renderPage reentrance\". Quitting now.");
					AfxGetMainWnd()->PostMessageW(WM_CLOSE,0,0);
				}
				return;*/
			} else {
				free (testAllocation);
			}

			//CWaitDialog waitDialog(this);

			try{
				//Wait for previous threads and delete them
				if (m_renderingThread!=0)
					{
						DWORD exitcode=0;
						GetExitCodeThread(m_renderingThread->m_hThread,&exitcode);
						if (exitcode==STILL_ACTIVE){
							//MessageBox(L"Waiting for Thread");
						}
						while (exitcode==STILL_ACTIVE){
							GetExitCodeThread(m_renderingThread->m_hThread,&exitcode);
							Sleep(50);
						}
						delete m_renderingThread;
						m_renderingThread=NULL;
					}

				if (m_renderDPI!=m_LastRenderDPI)
				{
					//Invalidate prerendered page
					m_LastPageRenderedByThread=-1;
				}
				if (m_LastPageRenderedByThread != m_CurrentPage)
				{
					m_PageToRenderByThread = m_CurrentPage;
					
					m_renderingThread = AfxBeginThread((AFX_THREADPROC)RenderingThread,(LPVOID) this,3,0,CREATE_SUSPENDED);
					m_renderingThread->m_bAutoDelete=FALSE;
					m_renderingThread->ResumeThread();
					
					DWORD exitcode;
					GetExitCodeThread(m_renderingThread->m_hThread,&exitcode);
					if (exitcode==STILL_ACTIVE){
						//MessageBox(L"Waiting for Directrender Thread");
					}
					while (exitcode==STILL_ACTIVE){
						GetExitCodeThread(m_renderingThread->m_hThread,&exitcode);
						Sleep(50);
					}

					delete m_renderingThread;
					m_renderingThread=NULL;
				}
			//m_Bitmap->CreateBitmap(m_splashOut->getBitmap()->getWidth(),m_splashOut->getBitmap()->getHeight(),1,24,(void *)m_splashOut->getBitmap()->getDataPtr());
			
			//********START DIB
			CClientDC clientDC(this);
			m_Bitmap->CreateCompatibleBitmap(&clientDC,m_splashOut->getBitmap()->getWidth(),m_splashOut->getBitmap()->getHeight());
			CDC mdc;
			mdc.CreateCompatibleDC(&clientDC);
			mdc.SelectObject(m_Bitmap);
			CBitmap* pOld = mdc.SelectObject(m_Bitmap);
			
			BITMAPINFO bmi;
			ZeroMemory(&bmi,sizeof(bmi));
			bmi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth=m_splashOut->getBitmap()->getWidth();//+(m_splashOut->getBitmap()->getWidth()%2);
			bmi.bmiHeader.biHeight=m_splashOut->getBitmap()->getHeight();
			bmi.bmiHeader.biPlanes=1;
			bmi.bmiHeader.biBitCount=24;
			bmi.bmiHeader.biCompression=BI_RGB;

			bmi.bmiColors[0].rgbBlue = 0;
			bmi.bmiColors[0].rgbGreen = 0;
			bmi.bmiColors[0].rgbRed = 0;
			bmi.bmiColors[0].rgbReserved = 0;

			::StretchDIBits(mdc.m_hDC,0,0,bmi.bmiHeader.biWidth,bmi.bmiHeader.biHeight,0,bmi.bmiHeader.biHeight,bmi.bmiHeader.biWidth,-bmi.bmiHeader.biHeight,
				(void *)m_splashOut->getBitmap()->getDataPtr(),&bmi,DIB_RGB_COLORS,SRCCOPY);
			
			mdc.SelectObject(pOld);
			//********END DIB


			SplashBitmap * bitmap = m_splashOut->getBitmap();
			bmWidth = bitmap->getWidth();
			bmHeight = bitmap->getHeight();
			m_PageHeight = bmHeight;
			m_PageWidth  = bmWidth;

			m_Bitmap->SetBitmapDimension(m_splashOut->getBitmap()->getWidth(),m_splashOut->getBitmap()->getHeight());
			} catch(const std::exception){
				MessageBox(L"Ouf of memory. Quitting.");
				AfxGetMainWnd()->PostMessageW(WM_CLOSE,0,0);
			}

			int box_left, box_top, box_right, box_bottom;

			m_splashOut->getModRegion(&box_left, &box_top, &box_right, &box_bottom);
			m_bbox = CRect(box_left, box_top, box_right, box_bottom);

			m_LastRenderDPI = m_renderDPI;


			//prerender next page
			if (m_renderingThread!=0){
				MessageBox(L"Warning: Old thread still existing while starting background thread!");
				DWORD exitcode=0;
						GetExitCodeThread(m_renderingThread->m_hThread,&exitcode);
						if (exitcode==STILL_ACTIVE){
							MessageBox(L"Warning: Old thread still running!");
						}
			}

			if (m_CurrentPage+1 <= m_PDFDoc->getNumPages())
			{
				m_PageToRenderByThread = m_CurrentPage+1;
				m_renderingThread = AfxBeginThread((AFX_THREADPROC)RenderingThread,(LPVOID) this,THREAD_PRIORITY_BELOW_NORMAL,0,CREATE_SUSPENDED);
				m_renderingThread->m_bAutoDelete=FALSE;
				m_renderingThread->ResumeThread();
			}

			if (!m_ConfigDialog.m_HideScrollbars)
			{
				RECT clientArea;
				//Get clientArea with invisible scrollbars
				this->GetClientRect(&clientArea);
				int clientWidth = clientArea.right-clientArea.left;
				int clientHeight = clientArea.bottom-clientArea.top;

				if (clientWidth < bmWidth){
					SetScrollRange(SB_HORZ,0,bmWidth - 1);
				} else {
					SetScrollRange(SB_HORZ,0,0);
				}
				if (clientHeight < bmHeight){
					SetScrollRange(SB_VERT,0,bmHeight-1);
				} else {
					SetScrollRange(SB_VERT,0,0);
				}
				//Now get client rect with potentially visible scrollbars
				this->GetClientRect(&clientArea);
				clientWidth = clientArea.right-clientArea.left;
				clientHeight = clientArea.bottom-clientArea.top;
				SCROLLINFO info;
				info.cbSize = sizeof(SCROLLINFO);
				info.fMask = SIF_PAGE;
				info.nPage = clientHeight;
				SetScrollInfo(SB_VERT,&info);
				info.nPage = clientWidth;
				SetScrollInfo(SB_HORZ,&info);
			} else {
				SetScrollRange(SB_HORZ,0,0);
				SetScrollRange(SB_VERT,0,0);
			}
		}
		wchar_t caption[128];
		wsprintf(caption,L"(%d/%d) PocketXpdf",m_CurrentPage,m_PDFDoc->getNumPages());
		dynamic_cast<CMainFrame*>(GetTopLevelFrame())->setCaption(caption);
	/*	wchar_t msg[512];
		wsprintf(msg,L"%lf/%lf -> %d/%d ",m_PDFDoc->getPageMediaWidth(m_CurrentPage),m_PDFDoc->getPageMediaHeight(m_CurrentPage),m_splashOut->getBitmap()->getWidth(),m_splashOut->getBitmap()->getHeight());
		MessageBox(msg);
		wsprintf(msg,L"Crop %lf/%lf -> %d/%d ",m_PDFDoc->getPageCropWidth(m_CurrentPage),m_PDFDoc->getPageCropHeight(m_CurrentPage),m_splashOut->getBitmap()->getWidth(),m_splashOut->getBitmap()->getHeight());
		MessageBox(msg);
		*/
		m_WaitDialog.ShowWindow(SW_HIDE);
		if (redraw)
			RedrawWindow();
	} else {
		//No document
		SetScrollRange(SB_HORZ,0,0);
		SetScrollRange(SB_VERT,0,0);
	}
}

void CChildView::OnFileOptions()
{
	/*	
	CPropertySheet sheet;
	CMiscPropertyPage misc;
	CButtonMappingPropertyPage buttonmapping;
	sheet.AddPage(&misc);
	sheet.AddPage(&buttonmapping);
	sheet.DoModal();
	*/
	/* save state */
	BOOL HideScrollbars = m_ConfigDialog.m_HideScrollbars;
	BOOL AntiAliasing = m_ConfigDialog.m_AntiAliasing;
	BOOL render = FALSE;

	int rc = m_ConfigDialog.DoModal();
	if (rc != IDOK)
		return;

	render = HideScrollbars != m_ConfigDialog.m_HideScrollbars;
	if (AntiAliasing != m_ConfigDialog.m_AntiAliasing) {
		if (m_ConfigDialog.m_AntiAliasing)
			globalParams->setAntialias("yes");
		else
			globalParams->setAntialias("no");

		render = TRUE;
	}

	if (render)
		RefreshPageView(m_ViewOffsetX, m_ViewOffsetY);
}

void CChildView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	for (int i=0;i<CConfigDialog::MAXMAPPINGS;i++)
	{
		if (m_ConfigDialog.m_ButtonMappings[i].button==-1)
		{
			break;
		}
		if (m_ConfigDialog.m_ButtonMappings[i].button==nChar)
		{
			execFunction(m_ConfigDialog.m_ButtonMappings[i].function);
		}
	}
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CChildView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}

LRESULT CChildView::OnHotkey(WPARAM wParam,LPARAM lParam)
{
	for (int i=0;i<CConfigDialog::MAXMAPPINGS;i++)
	{
		if (m_ConfigDialog.m_ButtonMappings[i].button==-1)
		{
			break;
		}
		if (m_ConfigDialog.m_ButtonMappings[i].button==wParam)
		{
			execFunction(m_ConfigDialog.m_ButtonMappings[i].function);
		}
	}

	return (LRESULT) TRUE;
}

void CChildView::OnToolsFitscreenwidth()
{
	if (m_PDFDoc!=NULL){
		FitscreenwidthInternal();
		RefreshPageView(0, 0);
	}
}

void CChildView::FitscreenwidthInternal()
{
	if (m_PDFDoc!=NULL){
		RECT clientArea;

		//We start with invisible scrollbars. If needed, we enable SB_VERT later.
		SetScrollRange(SB_VERT,0,0);
		SetScrollRange(SB_HORZ,0,0);

		GetClientRect(&clientArea);
		double height = clientArea.bottom-clientArea.top;
		double width = clientArea.right-clientArea.left;
		double contentHeight = 0;
		if (m_Rotation==0||m_Rotation==180){
			contentHeight = m_PDFDoc->getPageCropHeight(m_CurrentPage);
		} else {
			contentHeight = m_PDFDoc->getPageCropWidth(m_CurrentPage);
		}
		double contentWidth = 0;
		if (m_Rotation==0||m_Rotation==180){
			contentWidth = m_PDFDoc->getPageCropWidth(m_CurrentPage);
		} else {
			contentWidth = m_PDFDoc->getPageCropHeight(m_CurrentPage);
		}

		m_renderDPI = 72.0/contentWidth*width;
		//Now we know the required DPI if there weren't any scrollbars

		if (!m_ConfigDialog.m_HideScrollbars)
		{
			double targetHeight = (m_renderDPI*contentHeight)/72.0;
			if (targetHeight>height)
			{
				width -= GetSystemMetrics(SM_CXVSCROLL);
/*
				SetScrollRange(SB_VERT,0,100);
				GetClientRect(&clientArea);
				height = clientArea.bottom-clientArea.top;
				width = clientArea.right-clientArea.left;
*/
			} 
			m_renderDPI = 72.0/contentWidth*width;
			//Now the result takes the potential scrollbar into account
		}
	}
}

void CChildView::OnToolsFitscreenheight()
{
	if (m_PDFDoc!=NULL){
		RECT clientArea;

		//We start with invisible scrollbars. If needed, we enable SB_HORZ later.
		SetScrollRange(SB_VERT,0,0);
		SetScrollRange(SB_HORZ,0,0);

		GetClientRect(&clientArea);
		double height = clientArea.bottom-clientArea.top;
		double width = clientArea.right-clientArea.left;
		double contentHeight = 0;
		if (m_Rotation==0||m_Rotation==180){
			contentHeight = m_PDFDoc->getPageCropHeight(m_CurrentPage);
		} else {
			contentHeight = m_PDFDoc->getPageCropWidth(m_CurrentPage);
		}
		double contentWidth = 0;
		if (m_Rotation==0||m_Rotation==180){
			contentWidth = m_PDFDoc->getPageCropWidth(m_CurrentPage);
		} else {
			contentWidth = m_PDFDoc->getPageCropHeight(m_CurrentPage);
		}

		m_renderDPI = 72.0/contentHeight*height;
		//Now we know the required DPI if there weren't any scrollbars
		if (!m_ConfigDialog.m_HideScrollbars)
		{
			double targetWidth = (m_renderDPI*contentWidth)/72.0;
			if (targetWidth > width)
			{
				height -= GetSystemMetrics(SM_CYHSCROLL);
/*				SetScrollRange(SB_VERT,0,targetWidth-1);
				GetClientRect(&clientArea);
				height = clientArea.bottom-clientArea.top;
				width = clientArea.right-clientArea.left;
*/
			} 
			m_renderDPI = 72.0/contentHeight*height;
			//Now the result takes the potential scrollbar into account
		}
		RefreshPageView(0, 0);
	}
}


void CChildView::OnToolsGotopage()
{
	m_GotoDialog.m_lastPage=m_PDFDoc->getNumPages();
	m_GotoDialog.m_currentPage=m_CurrentPage;
	if (m_GotoDialog.DoModal() == IDOK) {
		GotoPage(m_GotoDialog.m_lastPage+1, m_ViewOffsetX, m_ViewOffsetY);
		//m_GotoDialog.m_PagesListBox.SetCurSel(-1);
	}
}

void CChildView::OnRotation0()
{
	m_Rotation = 0;
	RefreshPageView(m_ViewOffsetX, m_ViewOffsetY);
}

void CChildView::OnRotate90()
{
	m_Rotation = 90;
	RefreshPageView(m_ViewOffsetX, m_ViewOffsetY);
}

void CChildView::OnRotate90CCW()
{
	m_Rotation = 270;
	RefreshPageView(m_ViewOffsetX, m_ViewOffsetY);
}

void CChildView::OnRotate180()
{
	m_Rotation = 180;
	RefreshPageView(m_ViewOffsetX, m_ViewOffsetY);
}

void CChildView::OnContextMenu(CWnd* pWnd, CPoint point)
{
   CMenu Menu;
   Menu.LoadMenu(IDR_POPUP);
   Menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN,point.x,point.y,AfxGetMainWnd());
}


void CChildView::execFunction(CConfigDialog::Functions function)
{

	if (this->GetUpdateRect(0)){
		//Dont execute functions received while the window isnt drawn (usually keystrokes while rendering)
		return;
	}

	RECT clientArea;
	GetClientRect(&clientArea);
	int deltaY = 10;//(clientArea.bottom-clientArea.top)/VERT_SCROLL_STEPS_PER_PAGE;
	int deltaX = 10;//(clientArea.right-clientArea.left)/HORZ_SCROLL_STEPS_PER_PAGE;

	switch (function){
		case CConfigDialog::NEXTPAGE:
			OnNextPage();
			break;
		case CConfigDialog::PREVPAGE:
			OnPrevPage();
			break;
		case CConfigDialog::SCROLLUP:
			MoveView(0, -deltaY);
			break;
		case CConfigDialog::SCROLLDOWN:
			MoveView(0, deltaY);
			break;
		case CConfigDialog::SCROLLLEFT:
			MoveView(-deltaX, 0);
			break;
		case CConfigDialog::SCROLLRIGHT:
			MoveView(deltaX, 0);
			break;
		case CConfigDialog::ZOOMIN:
			OnZoomIn();
			break;
		case CConfigDialog::ZOOMOUT:
			OnZoomOut();
			break;
		case CConfigDialog::FITTOHEIGHT:
			OnToolsFitscreenheight();
			break;
		case CConfigDialog::FITTOWIDTH:
			OnToolsFitscreenwidth();
			break;
		case CConfigDialog::FULLSCREEN:
			GetTopLevelFrame()->SendMessage(WM_COMMAND,ID_TOOLS_FULLSCREEN,0);
			break;
		case CConfigDialog::FINDTEXT:
			OnToolsFindtext();
			break;
		case CConfigDialog::FINDNEXT:
			OnFindNext();
			break;
		case CConfigDialog::FINDPRIOR:
			OnFindPrior();
			break;
		case CConfigDialog::NEXTSCREEN:
			OnNextScreen();
			break;
		case CConfigDialog::PRIORSCREEN:
			OnPriorScreen();
			break;
		case CConfigDialog::LEFTSCREEN:
			OnLeftScreen();
			break;
		case CConfigDialog::RIGHTSCREEN:
			OnRightScreen();
			break;
		case CConfigDialog::OUTLINE:
			OnToolsOutline();
			break;
		default:
			MessageBox(L"Invalid button Mapping");
	}
}

int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	this->PostMessageW(WM_USER+1,0,0);
	//renderPage();
	return 0;
}

void CChildView::OnToolsViewtextonly()
{
	if (m_PDFDoc!=NULL){
	m_TextOutDialog.m_pPDFDoc = m_PDFDoc;
	m_TextOutDialog.m_CurrentPage = m_CurrentPage;
	m_TextOutDialog.DoModal();
	renderPage();
	}
}

void CChildView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	RECT clientArea;
	GetClientRect(&clientArea);
	int height = (clientArea.bottom-clientArea.top);

	int linesize = height/VERT_SCROLL_STEPS_PER_PAGE;
	int pagesize = height;

	int oldPos = GetScrollPos(SB_VERT);
	int newPos = 0;
	switch (nSBCode)
    {
	case SB_THUMBTRACK:
        newPos = nPos;
		break;
	case SB_THUMBPOSITION:
        newPos = nPos;
		break;
    case SB_LINEUP:
        newPos = oldPos - linesize;
        break;
	case SB_LINEDOWN:
        newPos = oldPos + linesize;
        break;
    case SB_PAGEUP:
        newPos = oldPos - pagesize;
        break;
    case SB_PAGEDOWN:
        newPos = oldPos + pagesize;
        break;
	default:
		CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
		return;
    }
	GotoView(m_ViewOffsetX, newPos);
//	m_ViewOffsetY = newPos;
//    SetScrollPos(SB_VERT, newPos, TRUE);
	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CChildView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	RECT clientArea;
	GetClientRect(&clientArea);
	int width = (clientArea.right-clientArea.left);

	int linesize = width/HORZ_SCROLL_STEPS_PER_PAGE;
	int pagesize = width;

	int oldPos = GetScrollPos(SB_HORZ);
	int newPos = 0;
	switch (nSBCode)
    {
	case SB_THUMBTRACK:
        newPos = nPos;
		break;
	case SB_THUMBPOSITION:
        newPos = nPos;
		break;
    case SB_LINELEFT:
        newPos = oldPos - linesize;
        break;
	case SB_LINERIGHT:
        newPos = oldPos + linesize;
        break;
    case SB_PAGELEFT:
        newPos = oldPos - pagesize;
        break;
    case SB_PAGERIGHT:
        newPos = oldPos +pagesize;
        break;
	default:
		CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
		return;
    }
	GotoView(newPos, m_ViewOffsetY);
//	m_ViewOffsetX = newPos;
//    SetScrollPos(SB_HORZ, newPos, TRUE);
	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}
LRESULT CChildView::PostCreateWindow(WPARAM wParam, LPARAM lParam)
{
	CCommandLineInfo info;
	AfxGetApp()->ParseCommandLine(info);
	if (info.m_strFileName.Compare(L"")!=0){
		if (loadFile(info.m_strFileName))
		{
			renderPage();
		}
	}
	return (LRESULT)0;
}
BOOL CChildView::OnEraseBkgnd(CDC* pDC)
{
	RECT clientArea;
	GetClientRect(&clientArea);
	int width = (clientArea.right-clientArea.left);
	int height = (clientArea.bottom-clientArea.top);

	if (m_PDFDoc==NULL || m_Bitmap == NULL){
		return CWnd::OnEraseBkgnd(pDC);
	}

	/*if (m_ViewOffsetX < 0 || m_ViewOffsetY < 0){
		return CWnd::OnEraseBkgnd(pDC);
	}

	if (m_Bitmap->GetBitmapDimension().cx-m_ViewOffsetX < width || m_Bitmap->GetBitmapDimension().cy-m_ViewOffsetY < height){
		return CWnd::OnEraseBkgnd(pDC);
	}*/

	return TRUE;


	
	//return CWnd::OnEraseBkgnd(pDC);
}
void CChildView::OnDragZoom()
{
	m_IsInDragZoomMode=!m_IsInDragZoomMode;
	Invalidate(FALSE);
//	if (m_IsInDragZoomMode) {
//		CClientDC dc(this);
//		dc.SetTextColor(RGB(255,0,0));
//		dc.DrawText(L"Drag a Rectangle around the \narea to zoom!",CRect(20,20,200,200),DT_NOCLIP);
//	}
}

void CChildView::OnUpdateDragZoom(CCmdUI *pCmdUI)
{
	if (m_PDFDoc != NULL){
		pCmdUI->Enable();
		pCmdUI->SetCheck(m_IsInDragZoomMode);
	} else {
		pCmdUI->Enable(false);
	}
}

void CChildView::RedrawWindow()
{
	Invalidate(FALSE);
//	UpdateWindow();
}

void CChildView::RefreshPageView(int x, int y)
{
	renderPage(FALSE);
	GotoView(x , y, FALSE);
	RedrawWindow();
}

CSize CChildView::GetScreenSize()
{
	CRect r;
	GetClientRect(r);
	return CSize(r.Width(), r.Height());
}

CSize CChildView::GetPageExtent()
{
	CRect r;
	GetClientRect(r);
	return CSize(m_PageWidth - r.Width(), m_PageHeight - r.Height());
}

void CChildView::GotoPage(int pageno, int x, int y, BOOL redraw)
{
	if (m_PDFDoc != NULL
		&& pageno >= 1
		&& pageno <= m_PDFDoc->getNumPages())
	{
		if (m_CurrentPage != pageno) {
			m_CurrentPage = pageno;
			renderPage(FALSE);
		}
		GotoView(x, y, FALSE);
		if (redraw) RedrawWindow();

		SetScrollPos(SB_HORZ, m_ViewOffsetX);
		SetScrollPos(SB_VERT, m_ViewOffsetY);
	}
}

void CChildView::GotoView(int x, int y, BOOL redraw, BOOL smooth)
{
	CSize pageSize = GetPageExtent();
	CSize screenSize = GetScreenSize();

	if (m_PageWidth <= screenSize.cx)
		x = -(screenSize.cx - m_PageWidth) / 2;
	else if (x < 0)
		x = 0;
	else if (x > pageSize.cx + screenSize.cx)
		x = pageSize.cx + screenSize.cx;

	if (m_PageHeight <= screenSize.cy)
		y = -(screenSize.cy - m_PageHeight) / 2;
	else if (y < 0)
		y = 0;
	else if (y > pageSize.cy + screenSize.cy)
		y = pageSize.cy + screenSize.cy;

	if (x != m_ViewOffsetX || y != m_ViewOffsetY) {

		if (smooth && redraw) {
			int dx = m_ViewOffsetX - x;
			int dy = m_ViewOffsetY - y;
			int steps = max(abs(dx) / 10, abs(dy) / 10);
			int j;

			for(j = steps - 1; j >= 0 ; j--) {
				m_ViewOffsetX = x + j * dx / steps;
				m_ViewOffsetY = y + j * dy / steps;
				Invalidate(FALSE);
				UpdateWindow();
				Sleep(0);
			}

		} else {
			m_ViewOffsetX = x;
			m_ViewOffsetY = y;
			if (redraw)
				RedrawWindow();
		}
		SetScrollPos(SB_HORZ, x);
		SetScrollPos(SB_VERT, y);
	}
}

void CChildView::MoveView(int dx, int dy, BOOL redraw, BOOL smooth)
{
	GotoView(m_ViewOffsetX + dx, m_ViewOffsetY + dy, redraw, smooth);
}

void CChildView::AdvanceCell(int mode)
{
	ASSERT(mode == NEXT_ROW || mode == NEXT_CELL);

	CSize screenSize = GetScreenSize();

	int x1 = m_ViewOffsetX < m_bbox.left ? m_bbox.left - SPACE_X : m_ViewOffsetX;
	int y1 = m_ViewOffsetY < m_bbox.top ? m_bbox.top - SPACE_Y: m_ViewOffsetY;
	int x2 = x1 + screenSize.cx;
	int y2 = y1 + screenSize.cy;
	int xc = (x1 + x2) / 2;

	int columnWidth = m_bbox.Width() / m_Columns;
	int column      = xc / columnWidth;
	int columnBegin = (xc - m_bbox.left) / columnWidth * columnWidth + m_bbox.left;
	int columnEnd   = columnBegin + columnWidth;
	int curPage     = m_CurrentPage;

	if (mode == NEXT_CELL) {
		if (x2 < columnEnd) {
			x1 += screenSize.cx - SPACE_X;
			/* make sure stripe center is properly aligned */
			if (x1 + screenSize.cx / 2 >= columnEnd) /* shift back */
				x1 = columnEnd - screenSize.cx + SPACE_X;
		} else
			x1 = columnBegin - SPACE_X;
	} else {
		ASSERT( mode == NEXT_ROW );

		if (y2 < m_bbox.bottom) {
			x1 = columnBegin - SPACE_X;
			y1 += screenSize.cy - SPACE_Y;
		} else if (column < m_Columns - 1) {
			x1 = columnEnd - SPACE_X;
			y1 = m_bbox.top - SPACE_Y;
		} else {
			curPage++;
		}
	}

	if (curPage != m_CurrentPage) {
		ASSERT (mode == NEXT_ROW);
		GotoPage(curPage, 0, 0, FALSE);
		MoveView(m_bbox.left - SPACE_X, m_bbox.top - SPACE_Y, FALSE);
		RedrawWindow();
	} else {
		GotoView(x1, y1, TRUE, TRUE);
	}
}

void CChildView::ScrollScreen(int dir)
{
	CSize pageSize = GetPageExtent();
	CSize screenSize = GetScreenSize();

	switch(dir) {
	case LEFT_SCREEN:
		if (m_ViewOffsetX > 0)
			MoveView(-screenSize.cx + SPACE_X, 0);
		else
			switch (m_Rotation){
				case 90:
					GotoPage(m_CurrentPage + 1, pageSize.cx, m_ViewOffsetY);
					break;
				case 270:
					GotoPage(m_CurrentPage - 1, pageSize.cx, m_ViewOffsetY);
					break;
				default:
					// No page change in portrait modes
					break;
			}
		break;

	case RIGHT_SCREEN:
		if (m_ViewOffsetX < pageSize.cx)
			MoveView(screenSize.cx - SPACE_Y, 0);
		else
			switch (m_Rotation){
				case 90:
					GotoPage(m_CurrentPage - 1, 0, m_ViewOffsetY);
					break;
				case 270:
					GotoPage(m_CurrentPage + 1, 0, m_ViewOffsetY);
					break;
				default:
					// No page change in portrait modes
					break;
			}
		break;

	case PRIOR_SCREEN:
		if (m_ViewOffsetY > 0)
			MoveView(0, -screenSize.cy + SPACE_Y);
		else
			switch (m_Rotation){
				case 0:
					GotoPage(m_CurrentPage - 1, m_ViewOffsetX, pageSize.cy);
					break;
				case 180:
					GotoPage(m_CurrentPage + 1, m_ViewOffsetX, pageSize.cy);
					break;
				default:
					// No page change in landscape modes
					break;
			}
		break;

	case NEXT_SCREEN:
		if (m_ViewOffsetY < pageSize.cy)
			MoveView(0, screenSize.cy - SPACE_Y);
		else
			switch (m_Rotation){
				case 0:
					GotoPage(m_CurrentPage + 1, m_ViewOffsetX, 0);
					break;
				case 180:
					GotoPage(m_CurrentPage - 1, m_ViewOffsetX, 0);
					break;
				default:
					// No page change in landscape modes
					break;
			}
		break;

	case NEXT_ROW:  AdvanceCell(dir); break;
	case NEXT_CELL: AdvanceCell(dir); break;
	}
}

void CChildView::OnUpdateNextScreen(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_PDFDoc != NULL /*
		&& m_CurrentPage < m_PDFDoc->getNumPages()*/);
}

void CChildView::OnUpdatePriorScreen(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_PDFDoc != NULL /*&& m_CurrentPage > 1*/);
}

void CChildView::UpdateScrollbars()
{
	if (m_PDFDoc == NULL) {
		//No document
		SetScrollRange(SB_HORZ,0,0);
		SetScrollRange(SB_VERT,0,0);
		return;
	}

	if (m_TextOutDialog.GetSafeHwnd() != NULL && m_TextOutDialog.IsWindowVisible()) {
		return;
	}

	if (m_ConfigDialog.m_HideScrollbars) {
		SetScrollRange(SB_HORZ,0,0);
		SetScrollRange(SB_VERT,0,0);
		return;
	}

	RECT clientArea;
	int bmWidth, bmHeight;
	int clientWidth, clientHeight;
	SplashBitmap * bitmap;

	//Get clientArea with invisible scrollbars
	GetClientRect(&clientArea);

	clientWidth = clientArea.right-clientArea.left;
	clientHeight = clientArea.bottom-clientArea.top;
	bitmap = m_splashOut->getBitmap();
	bmWidth = bitmap->getWidth();
	bmHeight = bitmap->getHeight();

	if (clientWidth < bmWidth){
		SetScrollRange(SB_HORZ,0,bmWidth - 1);
	} else {
		SetScrollRange(SB_HORZ,0,0);
	}
	if (clientHeight < bmHeight){
		SetScrollRange(SB_VERT,0,bmHeight - 1);
	} else {
		SetScrollRange(SB_VERT,0,0);
	}

	//Now get client rect with potentially visible scrollbars
	GetClientRect(&clientArea);
	clientWidth = clientArea.right-clientArea.left;
	clientHeight = clientArea.bottom-clientArea.top;

	SCROLLINFO info;
	info.cbSize = sizeof(SCROLLINFO);
	info.fMask = SIF_PAGE;
	info.nPage = clientHeight;
	SetScrollInfo(SB_VERT,&info);
	info.nPage = clientWidth;
	SetScrollInfo(SB_HORZ,&info);
}

void CChildView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	/* window (screen) size has been changed, update scrollbars */
	UpdateScrollbars();
	GotoView(m_ViewOffsetX, m_ViewOffsetY);
}

Unicode * CChildView::GetUnicodeString(LPCTSTR str, int length)
{
	Unicode * ucstring = new Unicode[length + 1];
	int j;

	if (ucstring == NULL)
		return NULL;

	/* it is almost safe to transform from UCS2 to UCS4 this way */
	for( j = 0; j < length; j++)
		ucstring[j] = str[j];

	ucstring[j] = 0;
	return ucstring;
}

void CChildView::SearchCompleted(int searchPage)
{
	m_SearchStarted = m_Selection.GetCount() > 0;
	if (!m_SearchStarted)
		return;
	m_SearchPage    = searchPage;
	m_HideMarks     = FALSE;

	CPoint p;
	CRect r(FindCoord(m_Selection[0].TopLeft()),
		    FindCoord(m_Selection[0].BottomRight()));

	GotoPage(searchPage, 0, 0, FALSE);
	/* center found string */
	CSize screenSize = GetScreenSize();

	if (r.Width() + SPACE_X * 2 > screenSize.cx)
		p.x = r.left - SPACE_X;
	else if (r.left < screenSize.cx / 2)
		p.x = 0;
	else
		p.x = r.left - screenSize.cx / 2;
	
	if (r.Height() + SPACE_Y * 2 > screenSize.cy)
		p.y = r.top - SPACE_Y;
	else if (r.top < screenSize.cy / 2)
		p.y = 0;
	else
		p.y = r.top - screenSize.cy / 2;

	GotoView(p.x, p.y, FALSE);
	RedrawWindow();
}

/* find and mark all the occurencies of text on a page */
BOOL CChildView::FindText(int searchPage, BOOL reverse)
{
	double x0, y0, x1, y1;
	GBool rc, startAtTop, startAtLast, backward;
	Unicode * ucstring;
	int length = m_FindDlg.m_text.GetLength();
	TextOutputDev FindPage(NULL, gTrue, gFalse, gFalse);

	ucstring = GetUnicodeString(m_FindDlg.m_text, length);
	if (ucstring == NULL) {
		AfxMessageBox(L"Out of memory");
		return FALSE;
	}

	startAtTop = gTrue;
	startAtLast = gFalse;
	backward = m_FindDlg.m_forward == reverse;
	m_SearchStarted = FALSE;
	m_Selection.RemoveAll();

	/* use fixed DPI and rotation, this would lead to somehwere
	 * wrong positioning displaying more than 300 DPI document
	 * don't think it's a problem thought
	 */

	m_PDFDoc->displayPage(&FindPage, searchPage,
		FIND_DPI, FIND_DPI, 0, gFalse, gTrue, gFalse);

	if (!FindPage.isOk()) {
		AfxMessageBox(L"Error while rendering page");
		delete [] ucstring;
		return FALSE;
	}

	while(true) {

		rc = FindPage.findText(ucstring, length,
			startAtTop, gTrue, startAtLast, gFalse,
			m_FindDlg.m_caseSensetive, backward,
			&x0, &y0, &x1, &y1);

		if (rc) {
			m_Selection.Add(CRect((int)x0, (int)y0, (int)x1, (int)y1));
			startAtTop = gFalse;
			startAtLast = gTrue;
			continue;
		}

		if (m_Selection.GetCount() > 0)
			break;

		/* go to next page */
		startAtTop = gTrue;
		startAtLast = gFalse;

		if (backward)
			searchPage--;
		else
			searchPage++;

		if (searchPage < 1 || searchPage > m_PDFDoc->getNumPages()) {
			AfxMessageBox(L"Not found");
			delete [] ucstring;
			return FALSE;
		}

		m_PDFDoc->displayPage(&FindPage, searchPage,
			FIND_DPI, FIND_DPI, 0, gFalse, gTrue, gFalse);

		if (!FindPage.isOk()) {
			AfxMessageBox(L"Error while rendering page");
			delete [] ucstring;
			return FALSE;
		}
	}

	delete [] ucstring;
	SearchCompleted(searchPage);
	return TRUE;
}

void CChildView::FindNext(BOOL reverse)
{
	double x0, y0, x1, y1;
	GBool startAtTop, startAtLast, backward;
	GBool rc;
	int searchPage = m_CurrentPage;
	Unicode * ucstring;
	int length = m_FindDlg.m_text.GetLength();

	if (!m_SearchStarted)
		return;

	m_Selection.RemoveAll();

	ucstring = GetUnicodeString(m_FindDlg.m_text, length);
	if (ucstring == NULL) {
		AfxMessageBox(L"Out of memory");
		return;
	}

	startAtTop  = gFalse;
	startAtLast = gTrue;
	backward    = m_FindDlg.m_forward == reverse;
	m_SearchStarted = FALSE;

	while(true) {
		rc = m_FindPage.findText(ucstring, length,
			startAtTop, gTrue, startAtLast, gFalse,
			m_FindDlg.m_caseSensetive, backward,
			&x0, &y0, &x1, &y1);

		if (rc) {
			/* found */
			m_Selection.Add(CRect((int)x0, (int)y0, (int)x1, (int)y1));
			break;
		}

		startAtTop = gTrue;
		startAtLast = gFalse;

		/* go to next page */
		if (backward)
			searchPage--;
		else
			searchPage++;

		if (searchPage < 1 || searchPage > m_PDFDoc->getNumPages()) {
			AfxMessageBox(L"Not found");
			delete [] ucstring;
			return;
		}

		m_PDFDoc->displayPage(&m_FindPage, searchPage,
			FIND_DPI, FIND_DPI, 0, gFalse, gTrue, gFalse);

		if (!m_FindPage.isOk()) {
			AfxMessageBox(L"Error while rendering page");
			delete [] ucstring;
			return;
		}
	}

	/* x0, y0, x1, y1 contain found rectange */
	delete [] ucstring;
	SearchCompleted(searchPage);
}

void CChildView::FindFirst()
{
	int searchPage = m_CurrentPage;
	Unicode * ucstring;
	int length = m_FindDlg.m_text.GetLength();
	double x0, y0, x1, y1;
	GBool rc;

	m_SearchStarted = FALSE;
	m_Selection.RemoveAll();

	if (m_FindDlg.m_begin) {
		if (m_FindDlg.m_forward)
			searchPage = 1;
		else
			searchPage = m_PDFDoc->getNumPages();
	}

	ucstring = GetUnicodeString(m_FindDlg.m_text, length);
	if (ucstring == NULL) {
		AfxMessageBox(L"Out of memory");
		return;
	}

	while(true) {
		m_PDFDoc->displayPage(&m_FindPage, searchPage,
			FIND_DPI, FIND_DPI, 0, gFalse, gTrue, gFalse);

		if (!m_FindPage.isOk()) {
			AfxMessageBox(L"Error while rendering page");
			delete [] ucstring;
			return;
		}

		rc = m_FindPage.findText(ucstring, length,
			gTrue, gTrue, gFalse, gFalse,
			m_FindDlg.m_caseSensetive,
			!m_FindDlg.m_forward,
			&x0, &y0, &x1, &y1);

		if (rc) {
			/* found */
			m_Selection.Add(CRect((int)x0, (int)y0, (int)x1, (int)y1));
			break;
		}

		if (m_FindDlg.m_forward)
			searchPage++;
		else
			searchPage--;

		if (searchPage < 1 || searchPage > m_PDFDoc->getNumPages()) {
			AfxMessageBox(L"Not found");
			delete [] ucstring;
			return;
		}
	}

	delete [] ucstring;
	SearchCompleted(searchPage);
}

void CChildView::OnToolsFindtext()
{
	if (m_FindDlg.execute())  {
		m_SearchStarted = FALSE;
		if (m_FindDlg.m_markAll) {
			int searchPage = m_CurrentPage;
			if (m_FindDlg.m_begin)
				searchPage = 1;
			FindText(searchPage, FALSE);
		} else
			FindFirst();
	}
}

void CChildView::OnUpdateFindCommand(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_PDFDoc != NULL);
}

void CChildView::OnFindNext()
{
	if (m_SearchStarted) {
		if (m_FindDlg.m_markAll)
			FindText(m_CurrentPage + 1, FALSE);
		else
			FindNext(FALSE);
	}
}

void CChildView::OnFindPrior()
{
	if (m_SearchStarted) {
		if (m_FindDlg.m_markAll)
			FindText(m_CurrentPage - 1, TRUE);
		else
			FindNext(TRUE);
	}
}

void CChildView::OnFindClearmarks()
{
	if (m_SearchStarted) {
		m_HideMarks = !m_HideMarks;
		RedrawWindow();
	}
}

CPoint CChildView::FindCoord(CPoint p)
{
	CPoint np(LPARAM(p.x * m_renderDPI / FIND_DPI), LPARAM(p.y * m_renderDPI / FIND_DPI));

	/* rotate using predefined angles only */
	switch(m_Rotation) {
	case 90: return CPoint(m_PageWidth - np.y, np.x);
	case 180: return CPoint(m_PageWidth - np.x, m_PageHeight - np.y);
	case 270: return CPoint(np.y, m_PageHeight - np.x);
	default:
		ASSERT(m_Rotation == 0);
		return np;
	}
}

void CChildView::OnSetColumnBrowse1()
{
	m_Columns = 1;
}

void CChildView::OnSetColumnBrowse2()
{
	m_Columns = 2;
}

void CChildView::OnUpdateColumnBrowse1(CCmdUI *pCmdUI)
{
	if (m_PDFDoc != NULL)
	{
		pCmdUI->Enable();
		pCmdUI->SetRadio(m_Columns == 1);
	} else 
	{
		pCmdUI->Enable(false);
	}
}

void CChildView::OnUpdateColumnBrowse2(CCmdUI *pCmdUI)
{
	if (m_PDFDoc != NULL)
	{
		pCmdUI->Enable();
		pCmdUI->SetRadio(m_Columns == 2);
	} else 
	{
		pCmdUI->Enable(false);
	}
}

void CChildView::OnToolsImageInfo()
{
	if (m_PDFDoc != NULL) {
		m_ImagePropsDlg.ImageSize = CSize(m_PageWidth, m_PageHeight);
		m_ImagePropsDlg.RenderDPI = m_renderDPI;
		if (m_ImagePropsDlg.DoModal() == IDOK) {
			if (m_ImagePropsDlg.RenderDPI != m_renderDPI) {
				m_renderDPI = m_ImagePropsDlg.RenderDPI;
				RefreshPageView(m_ViewOffsetX, m_ViewOffsetY);
			}
		}
	}
}

void CChildView::OnToolsOutline()
{
	//(Sebastian) Doing the outline dialog non-modal somehow messed up
	// the hotkeys, but seems to work OK with a modal dialog.
	m_OutlineView.DoModal();
	LinkAction * link = m_OutlineView.GetLinkAction();
	if (link == NULL)
		return;

	if (link->getKind() == actionGoTo) {
		LinkGoTo * p = (LinkGoTo *)link;
		LinkDest * dest;
		GString  * namedDest;
		int pageno;
		int x = m_ViewOffsetX, y = m_ViewOffsetY;
		int dx, dy;
		dest = p->getDest();

		if (dest == NULL) {
			if ((namedDest = p->getNamedDest()) == NULL)
				return;

			dest = m_PDFDoc->findDest(namedDest);
		}

		if (dest == NULL || !dest->isOk())
			return;

		if (dest->isPageRef()) {
			Ref ref = dest->getPageRef();
			pageno = m_PDFDoc->getCatalog()->findPage(ref.num, ref.gen);
		} else
			pageno = dest->getPageNum();

		if (pageno <= 0 || pageno >= m_PDFDoc->getNumPages())
			return;

		/* go to the page */
		GotoPage(pageno, 0, 0, FALSE);

		switch (dest->getKind()) {
		case destXYZ:
			{
				m_splashOut->cvtUserToDev(dest->getLeft(),
					dest->getTop(), &dx, &dy);

				if (dest->getChangeLeft()) x = dx;
				if (dest->getChangeTop())  y = dy;
				/* ignore zoom */
			}
			break;
			/* preserve user defined zoom factor, so do not fit */
		case destFit:
		case destFitB:
			x = y = 0;
			break;
		case destFitH:
		case destFitBH:
			//~ do fit
			m_splashOut->cvtUserToDev(0, dest->getTop(), &dx, &dy);
			x = 0;
			y = dy;
			break;
		case destFitV:
		case destFitBV:
			//~ do fit
			m_splashOut->cvtUserToDev(dest->getLeft(), 0, &dx, &dy);
			x = dx;
			y = 0;
			break;
		case destFitR:
			//~ do fit
			m_splashOut->cvtUserToDev(dest->getLeft(), dest->getTop(), &dx, &dy);
			x = dx;
			y = dy;
			break;
		}
		GotoView(x, y, FALSE);
		RedrawWindow();
	}
}

void CChildView::OnUpdateToolsOutline(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_PDFDoc != NULL
		&& m_OutlineView.HasOutline());
}



CString CChildView::GetBookmarkString(void)
{
	CString bookmark;
	bookmark.Format(L"%s\n%d\n%f\n%d\n%d\n%d\n",
		m_PDFDoc->getFileName()->getCStringW(),
		m_CurrentPage,
		m_renderDPI,
		m_Rotation,
		m_ViewOffsetX,
		m_ViewOffsetY);
	return bookmark;
}

void CChildView::RestoreBookmark(CString bookmark)
{
	
	CString resToken;
	int curPos= 0;
	resToken= bookmark.Tokenize(L"\n",curPos);
	if (m_PDFDoc == NULL || resToken != m_PDFDoc->getFileName()->getCStringW())
	{
		if	(!loadFile(resToken))
		{
			return;
		}
	} 
	
	wchar_t *stopstring;
	resToken= bookmark.Tokenize(L"\n",curPos);
	m_CurrentPage = wcstol(resToken,&stopstring,10);
	if (m_CurrentPage > m_PDFDoc->getNumPages()){
		//This must be a different file with the same name, so quit restoring the bookmark
		m_CurrentPage = 1;
		renderPage();
		return;
	}
	resToken= bookmark.Tokenize(L"\n",curPos);
	m_renderDPI = wcstod(resToken,&stopstring);
	resToken= bookmark.Tokenize(L"\n",curPos);
	m_Rotation = wcstol(resToken,&stopstring,10);
	resToken= bookmark.Tokenize(L"\n",curPos);
	m_ViewOffsetX = wcstol(resToken,&stopstring,10);
	resToken= bookmark.Tokenize(L"\n",curPos);
	m_ViewOffsetY = wcstol(resToken,&stopstring,10);
	 
}

void CChildView::OnBookmarkSave(UINT nID)
{
	CString entryname = L"Bookmark";
	entryname.AppendFormat(L"%d",nID - ID_BOOKMARK_SAVE_1);
	CWinApp *app=AfxGetApp();
	app->WriteProfileString(L"Xpdf",entryname,GetBookmarkString());
}

void CChildView::OnBookmarkLoad(UINT nID)
{
	CString entryname = L"Bookmark";
	entryname.AppendFormat(L"%d",nID - ID_BOOKMARK_LOAD_1);
	CWinApp *app=AfxGetApp();
	CString bookmark = app->GetProfileString(L"Xpdf",entryname,L"");
	RestoreBookmark(bookmark);
	renderPage();
}

void CChildView::OnUpdateBookmarkSave(CCmdUI *pCmdUI)
{
	if (m_PDFDoc == NULL)
	{
		pCmdUI->Enable(FALSE);
	} else 
	{
		pCmdUI->Enable(TRUE);
	}
	
	CString entryname = L"Bookmark";
	entryname.AppendFormat(L"%d",pCmdUI->m_nID - ID_BOOKMARK_SAVE_1);
	CWinApp *app=AfxGetApp();
	CString bookmark = app->GetProfileString(L"Xpdf",entryname,L"");
	if (bookmark == L"")
	{
		pCmdUI->SetText(L"No Entry");
		return;
	}

	CString menuItemName;

	CString resToken;
	int curPos= 0;
	resToken= bookmark.Tokenize(L"\n",curPos);
	menuItemName = resToken.Right(resToken.GetLength() - resToken.ReverseFind(L'\\') - 1) + L", Page ";
	resToken= bookmark.Tokenize(L"\n",curPos);
	menuItemName += resToken;
	pCmdUI->SetText(menuItemName);
}

void CChildView::OnUpdateBookmarkLoad(CCmdUI *pCmdUI)
{
	CString entryname = L"Bookmark";
	entryname.AppendFormat(L"%d",pCmdUI->m_nID - ID_BOOKMARK_LOAD_1);
	CWinApp *app=AfxGetApp();
	CString bookmark = app->GetProfileString(L"Xpdf",entryname,L"");
	if (bookmark == L"")
	{
		pCmdUI->SetText(L"No Entry");
		pCmdUI->Enable(FALSE);
		return;
	} else 
	{
		pCmdUI->Enable(TRUE);
	}
	
	CString menuItemName;

	CString resToken;
	int curPos= 0;
	resToken= bookmark.Tokenize(L"\n",curPos);
	menuItemName = resToken.Right(resToken.GetLength() - resToken.ReverseFind(L'\\') - 1) + L", Page ";
	resToken= bookmark.Tokenize(L"\n",curPos);
	menuItemName += resToken;
	pCmdUI->SetText(menuItemName);
}

void CChildView::OnUpdateBookmarkInternal(CCmdUI * pCmdUI, UINT lastButtonID)
{
}

void CChildView::OnRecentFiles(UINT nID)
{
	CString entryname = L"RecentFile";
	entryname.AppendFormat(L"%d",nID - ID_RECENTFILES_1);
	CWinApp *app=AfxGetApp();
	CString bookmark = app->GetProfileString(L"Xpdf",entryname,L"");
	RestoreBookmark(bookmark);
	renderPage();
}

void CChildView::OnUpdateRecentFiles(CCmdUI *pCmdUI)
{
	CString entryname = L"RecentFile";
	entryname.AppendFormat(L"%d",pCmdUI->m_nID - ID_RECENTFILES_1);
	CWinApp *app=AfxGetApp();
	CString bookmark = app->GetProfileString(L"Xpdf",entryname,L"");
	if (bookmark == L"")
	{
		pCmdUI->SetText(L"No Entry");
		pCmdUI->Enable(FALSE);
		return;
	} else 
	{
		pCmdUI->Enable(TRUE);
	}
	
	CString menuItemName;

	CString resToken;
	int curPos= 0;
	resToken= bookmark.Tokenize(L"\n",curPos);
	menuItemName = resToken.Right(resToken.GetLength() - resToken.ReverseFind(L'\\') - 1) + L", Page ";
	resToken= bookmark.Tokenize(L"\n",curPos);
	menuItemName += resToken;
	pCmdUI->SetText(menuItemName);
}


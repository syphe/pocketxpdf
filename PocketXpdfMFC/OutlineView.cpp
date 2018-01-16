// Outline.cpp : implementation file
//

#include "stdafx.h"
#include "PocketXpdfMFC.h"
#include "OutlineView.h"
#include "Outline.h"
#include "GList.h"
#include "Link.h"
#include "MainFrm.h"

// COutlineView dialog

IMPLEMENT_DYNAMIC(COutlineView, CDialog)

COutlineView::COutlineView(CWnd* pParent /*=NULL*/)
	: CDialog(COutlineView::IDD, pParent)
{

}

COutlineView::~COutlineView()
{
}

void COutlineView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OUTLINE, m_OutlineCtrl);
}


BEGIN_MESSAGE_MAP(COutlineView, CDialog)
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_OUTLINE, &COutlineView::OnItemExpanding)
	ON_WM_SIZE()
	ON_NOTIFY(NM_DBLCLK, IDC_OUTLINE, &COutlineView::OnNMDblclkOutline)
END_MESSAGE_MAP()


// COutlineView message handlers
bool COutlineView::HasOutline()
{
	return m_Outline != NULL;
}

BOOL COutlineView::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_OutlineCtrl.DeleteAllItems();
	if (m_Outline->getItems() != NULL) {
		LoadLevel(TVI_ROOT, m_Outline->getItems());
	}

	m_CommandBar.Create(this);
	m_CommandBar.InsertMenuBar(IDR_CANCEL);
	return TRUE;
}

void COutlineView::SetOutline(Outline* outline)
{
	m_Outline = outline->getItems() ? outline : NULL;
}

void COutlineView::LoadLevel(HTREEITEM tree_root, GList * doc_root)
{
	OutlineItem * item;
	int j = 0;
	int list_len = doc_root->getLength();

	for(j = 0; j < list_len; j++) {
		item = (OutlineItem *)doc_root->get(j);
		Unicode * title = item->getTitle();
		wchar_t * sztitle;
		int len = item->getTitleLength();
		int k;
		LinkAction * action = item->getAction();

		/* FIXME: should we procedd UCS4 better than just stripping? */
		sztitle = new wchar_t[len + 1];
		for(k = 0; k < len; k++)
			sztitle[k] = title[k];

		sztitle[k] = 0;

		TVINSERTSTRUCT tree_item;
		tree_item.hParent = tree_root;
		tree_item.hInsertAfter = TVI_LAST;

		tree_item.item.mask =
			TVIF_STATE
			| TVIF_PARAM
			| TVIF_TEXT
			| TVIF_CHILDREN;

		tree_item.item.state = 0;

		item->open();

		if (item->getKids()->getLength() != NULL)
			tree_item.item.state |= TVIS_BOLD;

		if (action->getKind() != actionGoTo)
			tree_item.item.state |= TVIS_CUT;

		tree_item.item.stateMask = TVIS_CUT | TVIS_BOLD;
		tree_item.item.pszText   = sztitle;
		tree_item.item.cChildren = item->getKids()->getLength() != 0;
		tree_item.item.lParam    = (LPARAM)item;

		m_OutlineCtrl.InsertItem(&tree_item);

		delete sztitle;
		item->close();
	}
}

void COutlineView::OnItemExpanding(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	OutlineItem * item = (OutlineItem *)pNMTreeView->itemNew.lParam;
	
	switch(pNMTreeView->action) {
	case TVE_EXPAND:
		if (item->getKids() == NULL) {
			item->open();
			if (item->getKids()->getLength() != NULL)
				LoadLevel(pNMTreeView->itemNew.hItem, item->getKids());
		}
		break;

	case TVE_COLLAPSE:
//		item->close();
		break;
	}

	*pResult = 0;
}

void COutlineView::OnOK()
{
	HTREEITEM hitem = m_OutlineCtrl.GetSelectedItem();
	m_result = NULL;

	if (hitem != NULL) {
		OutlineItem * item = (OutlineItem *)m_OutlineCtrl.GetItemData(hitem);
		if (item != NULL)
			m_result = item->getAction();
	}
	
	CDialog::OnOK();
}

void COutlineView::OnCancel()
{
	m_result = NULL;
	CDialog::OnCancel();
}

LinkAction * COutlineView::GetLinkAction()
{
	if (m_Outline == NULL)
		return NULL;

	return m_result;
}

void COutlineView::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	m_OutlineCtrl.SetWindowPos(NULL, 0, 0, cx, cy,
		SWP_NOZORDER | SWP_NOMOVE);
}

void COutlineView::OnNMDblclkOutline(NMHDR *pNMHDR, LRESULT *pResult)
{
	OnOK();
	*pResult = 0;
}

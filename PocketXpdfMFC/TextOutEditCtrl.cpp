#include "StdAfx.h"
#include "TextOutEditCtrl.h"

CTextOutEditCtrl::CTextOutEditCtrl(void)
{
}

CTextOutEditCtrl::~CTextOutEditCtrl(void)
{
}
BEGIN_MESSAGE_MAP(CTextOutEditCtrl, CEdit)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

void CTextOutEditCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar){
		case VK_UP:
			LineScroll(-1);
			break;
		case VK_DOWN:
			LineScroll(1);
			break;
		default:
	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
	}
}

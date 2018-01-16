#pragma once
#include "afxwin.h"

class CTextOutEditCtrl :
	public CEdit
{
public:
	CTextOutEditCtrl(void);
public:
	~CTextOutEditCtrl(void);
public:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

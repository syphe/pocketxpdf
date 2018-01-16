// MiscPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "PocketXpdfMFC.h"
#include "MiscPropertyPage.h"


// CMiscPropertyPage dialog

#ifndef WIN32_PLATFORM_WFSP
IMPLEMENT_DYNAMIC(CMiscPropertyPage, CPropertyPage)

CMiscPropertyPage::CMiscPropertyPage()
	: CPropertyPage(CMiscPropertyPage::IDD)
{

}

CMiscPropertyPage::~CMiscPropertyPage()
{
}

void CMiscPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMiscPropertyPage, CPropertyPage)
END_MESSAGE_MAP()


// CMiscPropertyPage message handlers
#endif
// ButtonMappingPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "PocketXpdfMFC.h"
#include "ButtonMappingPropertyPage.h"


// CButtonMappingPropertyPage dialog

#ifndef WIN32_PLATFORM_WFSP
IMPLEMENT_DYNAMIC(CButtonMappingPropertyPage, CPropertyPage)

CButtonMappingPropertyPage::CButtonMappingPropertyPage()
	: CPropertyPage(CButtonMappingPropertyPage::IDD)
{

}

CButtonMappingPropertyPage::~CButtonMappingPropertyPage()
{
}

void CButtonMappingPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CButtonMappingPropertyPage, CPropertyPage)
END_MESSAGE_MAP()


// CButtonMappingPropertyPage message handlers
#endif
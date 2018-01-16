#pragma once


// CButtonMappingPropertyPage dialog
#ifndef WIN32_PLATFORM_WFSP
class CButtonMappingPropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CButtonMappingPropertyPage)

public:
	CButtonMappingPropertyPage();
	virtual ~CButtonMappingPropertyPage();

// Dialog Data
	enum { IDD = IDD_BUTTONMAPPINGPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
#endif
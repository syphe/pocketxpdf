#pragma once


// CMiscPropertyPage dialog
#ifndef WIN32_PLATFORM_WFSP
class CMiscPropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CMiscPropertyPage)

public:
	CMiscPropertyPage();
	virtual ~CMiscPropertyPage();

// Dialog Data
	enum { IDD = IDD_MISCPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
#endif
#pragma once
#include "afxwin.h"


// CConfigDialog dialog

class CConfigDialog : public CDialog
{
	DECLARE_DYNAMIC(CConfigDialog)

	
public:
	CConfigDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CConfigDialog();

// Dialog Data
	enum { IDD = IDD_CONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
public:
	BOOL m_AntiAliasing;
public:
	BOOL m_MapHWButtons;
public:
	CComboBox m_MapFunctionBox;
public:
	virtual BOOL OnInitDialog();
public:
	CComboBox m_MapButtonBox;
public:

public :
	/*enum Buttons{ 
		   UP = 0,
		   DOWN,
		   LEFT,
		   RIGHT,
		   CENTER,
		   RECORD,
		   HWBUTTON1,
		   HWBUTTON2,
		   HWBUTTON3,
		   HWBUTTON4,
		   HWBUTTON5,
			};*/

	enum Functions{ 
		   NONE = 0,
		   NEXTPAGE,
		   PREVPAGE,
		   SCROLLUP,
		   PRIORSCREEN,
		   SCROLLDOWN,
		   NEXTSCREEN,
		   SCROLLLEFT,
		   LEFTSCREEN,
		   SCROLLRIGHT,
		   RIGHTSCREEN,
		   ZOOMIN,
		   ZOOMOUT,
		   FITTOWIDTH,
		   FITTOHEIGHT,
		   FULLSCREEN,
		   FINDTEXT,
		   FINDNEXT,
		   FINDPRIOR,
		   OUTLINE
			};
	const wchar_t **buttonNames;
	const static wchar_t **functionNames;

	struct ButtonMapping {
		int button;
		Functions function;
	};

	static const int MAXMAPPINGS = 32;

	ButtonMapping m_ButtonMappings[MAXMAPPINGS];
private:
	ButtonMapping m_LocalButtonMappings[MAXMAPPINGS];
public:
	
public:
	afx_msg void OnCbnSelchangeMapbuttons();
public:
	afx_msg void OnCbnSelchangeMapfunction();
protected:
	virtual void OnOK();
public:
	BOOL m_FileAssoc;
public:
	BOOL m_HideScrollbars;

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnToggleMapHWButtons();

private:
	void setupButtons();

	afx_msg void OnBnClickedSelectbutton();
};

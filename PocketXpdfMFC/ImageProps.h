#pragma once
#include "afxwin.h"


// CImageProps dialog

class CImageProps : public CDialog
{
	DECLARE_DYNAMIC(CImageProps)

public:
	CImageProps(CWnd* pParent = NULL);   // standard constructor
	virtual ~CImageProps();

// Dialog Data
	enum { IDD = IDD_IMAGE_PROPS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CSize  ImageSize;
	double RenderDPI;

private:
	CString m_ImageSize;
	CString m_RenderDPI;
	CComboBox m_ctlRenderDPI;

	virtual BOOL OnInitDialog();
	virtual void OnOK();

};

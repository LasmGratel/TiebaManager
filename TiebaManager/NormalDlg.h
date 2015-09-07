#pragma once
#include "ResizeControl.h"


// CNormalDlg �Ի���

class CNormalDlg : public CDialog
{
	DECLARE_DYNAMIC(CNormalDlg)

public:
	CNormalDlg(UINT nIDTemplate, CWnd* pParentWnd = NULL);
	virtual ~CNormalDlg();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);


protected:
	CResizeControl m_resize;
};

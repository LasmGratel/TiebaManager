#pragma once
#include "afxwin.h"


// CExplorerPage �Ի���

class CExplorerPage : public CDialog
{
	DECLARE_DYNAMIC(CExplorerPage)

public:
	CExplorerPage(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CExplorerPage();

// �Ի�������
	enum { IDD = IDD_EXPLORER_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	virtual void OnCancel();
	virtual void OnOK();
public:
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog() = 0;


public:
	CStatic m_static;
	CEdit m_edit;
	CButton m_gotoButton;
	CListCtrl m_list;
};

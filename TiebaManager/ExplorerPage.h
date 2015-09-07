#pragma once
#include "afxwin.h"
#include "NormalDlg.h"


// CExplorerPage �Ի���

class CExplorerPage : public CNormalDlg
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
public:
	virtual BOOL OnInitDialog();


public:
	CStatic m_static;
	CEdit m_edit;
	CButton m_gotoButton;
	CListCtrl m_list;
};

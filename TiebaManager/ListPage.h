#pragma once
#include "NormalDlg.h"


// CListPage �Ի���

class CListPage : public CNormalDlg
{
	DECLARE_DYNAMIC(CListPage)

public:
	CListPage(CWnd* pParent = NULL);   // ��׼���캯��
	CListPage(UINT nIDTemplate, CWnd* pParentWnd = NULL);
	virtual ~CListPage();

// �Ի�������
	enum { IDD = IDD_LIST_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDblclkList1();
	afx_msg void OnClickedButton1();
	afx_msg void OnClickedButton2();
	afx_msg void OnClickedButton3();


public:
	CListBox m_list;
	CStatic m_contentStatic;
	CEdit m_edit;
	CButton m_addButton;
	CButton m_deleteButton;
	CButton m_changeButton;
	CStatic m_static;
protected:
	COLORREF m_staticColor;
};

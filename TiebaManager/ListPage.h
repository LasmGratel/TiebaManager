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
	afx_msg virtual void OnClickedButton1();
	afx_msg virtual void OnClickedButton2();
	afx_msg virtual void OnClickedButton3();
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg virtual void OnClickedButton4() = 0;
	afx_msg virtual void OnClickedButton5() = 0;
	afx_msg virtual void OnClickedButton6();

	virtual BOOL SetItem(int index) = 0;
	virtual void OnAdd(int index) {}
	virtual void OnDelete(int index) {}


public:
	CListCtrl m_list;
	CButton m_addButton;
	CButton m_deleteButton;
	CButton m_changeButton;
	CButton m_exportButton;
	CButton m_importButton;
	CButton m_clearButton;
	CStatic m_static;
protected:
	COLORREF m_staticColor;
};

#pragma once
#include "afxwin.h"


// CTrustedThreadPage �Ի���

class CTrustedThreadPage : public CDialog
{
	DECLARE_DYNAMIC(CTrustedThreadPage)

public:
	CTrustedThreadPage(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CTrustedThreadPage();

// �Ի�������
	enum { IDD = IDD_TRUSTED_THREAD_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	virtual void OnCancel();
	virtual void OnOK();
public:
	afx_msg void OnClose();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLbnDblclkList1();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();


public:
	CListBox m_list;
	CEdit m_edit;
	CButton m_addButton;
	CButton m_deleteButton;
	CButton m_changeButton;
	CStatic m_static;
};

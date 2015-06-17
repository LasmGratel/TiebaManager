#pragma once
#include "afxwin.h"


// CUsersPage �Ի���

class CUsersPage : public CDialogEx
{
	DECLARE_DYNAMIC(CUsersPage)

public:
	CUsersPage(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CUsersPage();

// �Ի�������
	enum { IDD = IDD_USERS_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	virtual void OnCancel();
	virtual void OnOK();
public:
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();


public:
	CStatic m_currentUserStatic;
	CListBox m_list;
	CButton m_loginButton;
	CButton m_deleteButton;
	CButton m_switchButton;
};

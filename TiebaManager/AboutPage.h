#pragma once
#include "afxwin.h"
#include "TextButton.h"
#include "NormalDlg.h"


// CAboutPage �Ի���

class CAboutPage : public CNormalDlg
{
	DECLARE_DYNAMIC(CAboutPage)

public:
	CAboutPage(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAboutPage();

// �Ի�������
	enum { IDD = IDD_ABOUT_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnStnClickedStatic1();
	afx_msg void OnStnClickedStatic2();


public:
	CEdit m_edit;
	CTextButton m_checkUpdateStatic;
	CButton m_autoCheckUpdateCheck;
	CTextButton m_authorStatic;
};

#pragma once
#include "ListPage.h"


// CBlackListPage �Ի���

class CBlackListPage : public CListPage
{
	DECLARE_DYNAMIC(CBlackListPage)

public:
	CBlackListPage(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CBlackListPage();

// �Ի�������
	enum { IDD = IDD_LIST_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusEdit1();

protected:
	virtual BOOL CheckMatchTooMuch(const CString& text, BOOL isRegex);
	virtual void PostChangeList();
	virtual BOOL TestMatch(const CString& test, const CString& text, BOOL isRegex);
};

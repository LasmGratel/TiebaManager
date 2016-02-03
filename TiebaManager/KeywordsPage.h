#pragma once
#include "RegListPage.h"


// CKeywordsPage �Ի���

class CKeywordsPage : public CRegListPage
{
	DECLARE_DYNAMIC(CKeywordsPage)

public:
	CKeywordsPage(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CKeywordsPage();

// �Ի�������
	enum { IDD = IDD_LIST_TEST_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

protected:
	virtual BOOL CheckMatchTooMuch(const CString& text, BOOL isRegex);
	virtual void PostChangeList();
	virtual BOOL TestMatch(const CString& test, const CString& text, BOOL isRegex);
};

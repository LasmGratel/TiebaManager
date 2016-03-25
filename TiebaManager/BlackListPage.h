#pragma once
#include "RegListPage.h"


// CBlackListTestPage �Ի���

class CBlackListPage : public CRegListPage
{
	DECLARE_DYNAMIC(CBlackListPage)

public:
	CBlackListPage(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CBlackListPage();

// �Ի�������
	enum { IDD = IDD_LIST_TEST_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

protected:
	virtual BOOL CheckMatchTooMuch(const CString& text, BOOL isRegex);
	virtual void OnAdd(int index);
	virtual BOOL TestMatch(const CString& test, const CString& text, BOOL isRegex);
};

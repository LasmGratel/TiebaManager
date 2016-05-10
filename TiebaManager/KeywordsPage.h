#pragma once
#include "RegListPage.h"
#include <TBMConfig.h>


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

	virtual BOOL SetItem(int index);
	virtual BOOL Export(const CString& path);
	virtual BOOL Import(const CString& path);
	virtual void ShowList(const vector<CPlan::Keyword>& list);
	virtual void ApplyList(vector<CPlan::Keyword>& list);
protected:
	virtual void OnAdd(int index);
	virtual BOOL CheckMatchTooMuch(const CString& text, BOOL isRegex);
	virtual BOOL TestMatch(const CString& test, const CString& text, BOOL isRegex);
};

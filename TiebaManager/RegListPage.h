#pragma once
#include "ListTestPage.h"
#include "TypeHelper.h"


// CRegListPage �Ի���

class CRegListPage : public CListTestPage
{
	DECLARE_DYNAMIC(CRegListPage)

public:
	CRegListPage(const CString& inputTitle, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CRegListPage();

	// �Ի�������
	enum { IDD = IDD_LIST_TEST_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg virtual void OnClickedButton4();
	afx_msg virtual void OnClickedButton5();

	virtual BOOL SetItem(int index);
	virtual void ShowList(const vector<RegexText>& list);
	virtual void ApplyList(vector<RegexText>& list);
protected:
	virtual BOOL CheckMatchTooMuch(const CString& text, BOOL isRegex){ return FALSE; };
	virtual void PostChangeList(){}
	virtual BOOL TestMatch(int index);
	virtual BOOL TestMatch(const CString& test, const CString& text, BOOL isRegex) = 0;


protected:
	const CString m_inputTitle;
};

static const TCHAR IS_REGEX_TEXT[] = _T("��");

// KeywordsPage.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "KeywordsPage.h"
#include "Global.h"
#include "SettingDlg.h"


// CKeywordsPage �Ի���

IMPLEMENT_DYNAMIC(CKeywordsPage, CRegListPage)

CKeywordsPage::CKeywordsPage(CWnd* pParent /*=NULL*/)
	: CRegListPage(pParent)
{

}

CKeywordsPage::~CKeywordsPage()
{
}

void CKeywordsPage::DoDataExchange(CDataExchange* pDX)
{
	CRegListPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CKeywordsPage, CRegListPage)
END_MESSAGE_MAP()


// CKeywordsPage ��Ϣ�������


BOOL CKeywordsPage::OnInitDialog()
{
	CRegListPage::OnInitDialog();

	m_static.SetWindowText(_T("֧��ͼƬ��ַ\\��Ƶ��ַ���������ı������ı�ʱƥ��\r\n\
ע��������ʽ .* ƥ�������ı����벻Ҫ��������ʹ�ã���"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}

BOOL CKeywordsPage::CheckMatchTooMuch(const CString& text, BOOL isRegex)
{
	return StringIncludes(MATCH_TOO_MUCH_CONTENT_TEST1, text, isRegex) 
		&& StringIncludes(MATCH_TOO_MUCH_CONTENT_TEST2, text, isRegex);
}

void CKeywordsPage::PostChangeList()
{
	((CSettingDlg*)GetParent()->GetParent())->m_clearScanCache = TRUE;
}

BOOL CKeywordsPage::TestMatch(const CString& test, const CString& text, BOOL isRegex)
{
	return StringIncludes(test, text, isRegex);
}

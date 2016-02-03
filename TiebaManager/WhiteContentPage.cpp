// WhiteContentPage.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "WhiteContentPage.h"
#include "StringHelper.h"


// CWhiteContentPage �Ի���

IMPLEMENT_DYNAMIC(CWhiteContentPage, CRegListPage)

CWhiteContentPage::CWhiteContentPage(CWnd* pParent /*=NULL*/)
	: CRegListPage(_T("�������ݣ�"), pParent)
{

}

CWhiteContentPage::~CWhiteContentPage()
{
}

void CWhiteContentPage::DoDataExchange(CDataExchange* pDX)
{
	CRegListPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWhiteContentPage, CRegListPage)
END_MESSAGE_MAP()


// CWhiteContentPage ��Ϣ�������


BOOL CWhiteContentPage::OnInitDialog()
{
	CRegListPage::OnInitDialog();

	m_static.SetWindowText(_T("֧��ͼƬ��ַ\\��Ƶ��ַ���������ı������ı�ʱƥ��"));
	m_static.ModifyStyle(0, SS_CENTERIMAGE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}

BOOL CWhiteContentPage::CheckMatchTooMuch(const CString& text, BOOL isRegex)
{
	return StringIncludes(MATCH_TOO_MUCH_CONTENT_TEST1, text, isRegex) 
		&& StringIncludes(MATCH_TOO_MUCH_CONTENT_TEST2, text, isRegex);
}

BOOL CWhiteContentPage::TestMatch(const CString& test, const CString& text, BOOL isRegex)
{
	return StringIncludes(test, text, isRegex);
}

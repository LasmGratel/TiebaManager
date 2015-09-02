// WhiteListPage.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "WhiteListPage.h"
#include "Global.h"


// CWhiteListPage �Ի���

IMPLEMENT_DYNAMIC(CWhiteListPage, CListPage)

CWhiteListPage::CWhiteListPage(CWnd* pParent /*=NULL*/)
	: CListPage(pParent)
{

}

CWhiteListPage::~CWhiteListPage()
{
}

void CWhiteListPage::DoDataExchange(CDataExchange* pDX)
{
	CListPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWhiteListPage, CListPage)
END_MESSAGE_MAP()


// CWhiteListPage ��Ϣ�������


BOOL CWhiteListPage::OnInitDialog()
{
	CListPage::OnInitDialog();

	m_static.SetWindowText(_T("�������ı������ı�ʱƥ��(������)"));
	m_static.ModifyStyle(0, SS_CENTERIMAGE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}

BOOL CWhiteListPage::TestMatch(const CString& test, const CString& text, BOOL isRegex)
{
	return test == text;
}

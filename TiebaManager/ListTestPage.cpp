// ListTestPage.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ListTestPage.h"


// CListTestPage �Ի���

IMPLEMENT_DYNAMIC(CListTestPage, CListPage)

CListTestPage::CListTestPage(CWnd* pParent /*=NULL*/)
	: CListPage(CListTestPage::IDD, pParent)
{
}

#pragma region MFC
CListTestPage::~CListTestPage()
{
}

void CListTestPage::DoDataExchange(CDataExchange* pDX)
{
	CListPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_testEdit);
	DDX_Control(pDX, IDC_BUTTON7, m_testButton);
}


BEGIN_MESSAGE_MAP(CListTestPage, CListPage)
	ON_BN_CLICKED(IDC_BUTTON7, &CListTestPage::OnClickedButton7)
END_MESSAGE_MAP()
#pragma endregion

// CListTestPage ��Ϣ�������

// ��ʼ��
BOOL CListTestPage::OnInitDialog()
{
	CListPage::OnInitDialog();

	m_resize.AddControl(&m_testEdit, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, this, RT_KEEP_DIST_TO_RIGHT, this);
	m_resize.AddControl(&m_testButton, RT_KEEP_DIST_TO_RIGHT, &m_static, RT_KEEP_DIST_TO_BOTTOM, &m_testEdit);

	m_testEdit.SetWindowText(_T("�������ı�"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}

// ����
void CListTestPage::OnClickedButton7()
{
	int index = m_list.GetSelectionMark();
	if (index == LB_ERR)
		return;
	if (TestMatch(index))
	{
		m_staticColor = RGB(0, 255, 0);
		m_static.SetWindowText(_T("ƥ��ɹ���"));
	}
	else
	{
		m_staticColor = RGB(255, 0, 0);
		m_static.SetWindowText(_T("ƥ��ʧ�ܣ�����ƥ���ı���"));
	}
}

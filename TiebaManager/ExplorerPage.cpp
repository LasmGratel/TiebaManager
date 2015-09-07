// ExplorerPage.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ExplorerPage.h"


// CExplorerPage �Ի���

IMPLEMENT_DYNAMIC(CExplorerPage, CNormalDlg)

CExplorerPage::CExplorerPage(CWnd* pParent /*=NULL*/)
	: CNormalDlg(CExplorerPage::IDD, pParent)
{

}

#pragma region MFC
CExplorerPage::~CExplorerPage()
{
}

void CExplorerPage::DoDataExchange(CDataExchange* pDX)
{
	CNormalDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC1, m_static);
	DDX_Control(pDX, IDC_EDIT2, m_edit);
	DDX_Control(pDX, IDC_BUTTON1, m_gotoButton);
	DDX_Control(pDX, IDC_LIST1, m_list);
}


BEGIN_MESSAGE_MAP(CExplorerPage, CNormalDlg)
END_MESSAGE_MAP()
#pragma endregion

// CExplorerPage ��Ϣ�������

// ��ʼ��
BOOL CExplorerPage::OnInitDialog()
{
	CNormalDlg::OnInitDialog();

	m_resize.AddControl(&m_list, RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, this);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}

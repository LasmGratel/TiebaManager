// ListPage.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ListPage.h"


// CListPage �Ի���

IMPLEMENT_DYNAMIC(CListPage, CNormalDlg)

CListPage::CListPage(CWnd* pParent /*=NULL*/)
	: CNormalDlg(CListPage::IDD, pParent)
{
	m_staticColor = RGB(255, 0, 0);
}

CListPage::CListPage(UINT nIDTemplate, CWnd* pParentWnd)
	: CNormalDlg(nIDTemplate, pParentWnd)
{
	m_staticColor = RGB(255, 0, 0);
}

#pragma region MFC
CListPage::~CListPage()
{
}

void CListPage::DoDataExchange(CDataExchange* pDX)
{
	CNormalDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDC_BUTTON1, m_addButton);
	DDX_Control(pDX, IDC_BUTTON2, m_deleteButton);
	DDX_Control(pDX, IDC_BUTTON3, m_changeButton);
	DDX_Control(pDX, IDC_BUTTON4, m_exportButton);
	DDX_Control(pDX, IDC_BUTTON5, m_importButton);
	DDX_Control(pDX, IDC_STATIC1, m_static);
}


BEGIN_MESSAGE_MAP(CListPage, CNormalDlg)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON1, &CListPage::OnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CListPage::OnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CListPage::OnClickedButton3)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CListPage::OnNMDblclkList1)
	ON_BN_CLICKED(IDC_BUTTON4, &CListPage::OnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CListPage::OnClickedButton5)
END_MESSAGE_MAP()
#pragma endregion

// CListPage ��Ϣ�������

#pragma region UI
// �ؼ���ɫ
HBRUSH CListPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CNormalDlg::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->m_hWnd == m_static.m_hWnd)
		pDC->SetTextColor(m_staticColor);

	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}
#pragma endregion

// ��ʼ��
BOOL CListPage::OnInitDialog()
{
	CNormalDlg::OnInitDialog();

	m_resize.AddControl(&m_list, RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, this);
	m_resize.AddControl(&m_addButton, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, this);
	m_resize.AddControl(&m_deleteButton, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, this);
	m_resize.AddControl(&m_changeButton, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, this);
	m_resize.AddControl(&m_exportButton, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, this);
	m_resize.AddControl(&m_importButton, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, this);
	m_resize.AddControl(&m_static, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, this, RT_KEEP_DIST_TO_RIGHT, this);

	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}

// ���
void CListPage::OnClickedButton1()
{
	int index = m_list.GetSelectionMark() + 1;
	index = m_list.InsertItem(index, NULL);
	if (SetItem(index))
		m_list.SetSelectionMark(index);
	else
		m_list.DeleteItem(index);
}

// ɾ��
void CListPage::OnClickedButton2()
{
	int index = m_list.GetSelectionMark();
	if (index == LB_ERR)
		return;
	m_list.DeleteItem(index);
	m_list.SetSelectionMark(index > 0 ? index - 1 : index);
}

// �޸�
void CListPage::OnClickedButton3()
{
	int index = m_list.GetSelectionMark();
	if (index == LB_ERR)
		return;
	SetItem(index);
}

// ˫��
void CListPage::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	OnClickedButton3();
	*pResult = 0;
}

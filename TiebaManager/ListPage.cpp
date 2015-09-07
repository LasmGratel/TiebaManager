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
	DDX_Control(pDX, IDC_STATIC2, m_contentStatic);
	DDX_Control(pDX, IDC_EDIT1, m_edit);
	DDX_Control(pDX, IDC_BUTTON1, m_addButton);
	DDX_Control(pDX, IDC_BUTTON2, m_deleteButton);
	DDX_Control(pDX, IDC_BUTTON3, m_changeButton);
	DDX_Control(pDX, IDC_STATIC1, m_static);
}


BEGIN_MESSAGE_MAP(CListPage, CNormalDlg)
	ON_WM_CTLCOLOR()
	ON_LBN_DBLCLK(IDC_LIST1, &CListPage::OnDblclkList1)
	ON_BN_CLICKED(IDC_BUTTON1, &CListPage::OnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CListPage::OnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CListPage::OnClickedButton3)
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
	m_resize.AddControl(&m_contentStatic, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list);
	m_resize.AddControl(&m_edit, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list, RT_KEEP_DIST_TO_RIGHT, this);
	m_resize.AddControl(&m_addButton, RT_KEEP_DIST_TO_RIGHT, &m_edit, RT_KEEP_DIST_TO_BOTTOM, &m_list);
	m_resize.AddControl(&m_deleteButton, RT_KEEP_DIST_TO_RIGHT, &m_edit, RT_KEEP_DIST_TO_BOTTOM, &m_list);
	m_resize.AddControl(&m_changeButton, RT_KEEP_DIST_TO_RIGHT, &m_edit, RT_KEEP_DIST_TO_BOTTOM, &m_list);
	m_resize.AddControl(&m_static, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list, RT_KEEP_DIST_TO_RIGHT, this);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}

// ˫���б�
void CListPage::OnDblclkList1()
{
	CString tmp;
	m_list.GetText(m_list.GetCurSel(), tmp);
	m_edit.SetWindowText(tmp);
}

// ���
void CListPage::OnClickedButton1()
{
	CString text;
	m_edit.GetWindowText(text);
	if (text == _T(""))
	{
		AfxMessageBox(_T("���ݲ���Ϊ�գ�"), MB_ICONERROR);
		return;
	}
	int index = m_list.GetCurSel();
	index = m_list.InsertString(index + 1, text);
	m_list.SetCurSel(index);
}

// ɾ��
void CListPage::OnClickedButton2()
{
	int index = m_list.GetCurSel();
	if (index == LB_ERR)
		return;
	m_list.DeleteString(index);
	m_list.SetCurSel(index == 0 ? 0 : index - 1);
}

// �޸�
void CListPage::OnClickedButton3()
{
	CString text;
	m_edit.GetWindowText(text);
	if (text == _T(""))
	{
		AfxMessageBox(_T("���ݲ���Ϊ�գ�"), MB_ICONERROR);
		return;
	}
	int index = m_list.GetCurSel();
	if (index == LB_ERR)
		return;
	m_list.DeleteString(index);
	index = m_list.InsertString(index, text);
	m_list.SetCurSel(index);
}

// ListTestPage.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "RegListPage.h"


// CRegListPage �Ի���

IMPLEMENT_DYNAMIC(CRegListPage, CNormalDlg)

CRegListPage::CRegListPage(CWnd* pParent /*=NULL*/)
	: CNormalDlg(CRegListPage::IDD, pParent)
{
	m_staticColor = RGB(255, 0, 0);
}

#pragma region MFC
CRegListPage::~CRegListPage()
{
}

void CRegListPage::DoDataExchange(CDataExchange* pDX)
{
	CNormalDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDC_EDIT1, m_edit);
	DDX_Control(pDX, IDC_CHECK1, m_regexCheck);
	DDX_Control(pDX, IDC_BUTTON1, m_addButton);
	DDX_Control(pDX, IDC_BUTTON2, m_deleteButton);
	DDX_Control(pDX, IDC_BUTTON3, m_changeButton);
	DDX_Control(pDX, IDC_EDIT6, m_testEdit);
	DDX_Control(pDX, IDC_STATIC1, m_static);
	DDX_Control(pDX, IDC_BUTTON6, m_testButton);
}


BEGIN_MESSAGE_MAP(CRegListPage, CNormalDlg)
	ON_WM_CTLCOLOR()
	ON_LBN_DBLCLK(IDC_LIST1, &CRegListPage::OnDblclkList1)
	ON_BN_CLICKED(IDC_BUTTON1, &CRegListPage::OnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CRegListPage::OnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CRegListPage::OnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON6, &CRegListPage::OnClickedButton6)
END_MESSAGE_MAP()
#pragma endregion

// CRegListPage ��Ϣ�������

#pragma region UI
// �ؼ���ɫ
HBRUSH CRegListPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CNormalDlg::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->m_hWnd == m_static.m_hWnd)
		pDC->SetTextColor(m_staticColor);

	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}
#pragma endregion

// ��ʼ��
BOOL CRegListPage::OnInitDialog()
{
	CNormalDlg::OnInitDialog();

	m_resize.AddControl(&m_list, RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, this);
	m_resize.AddControl(&m_edit, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list, RT_KEEP_DIST_TO_RIGHT, this);
	m_resize.AddControl(&m_regexCheck, RT_KEEP_DIST_TO_RIGHT, &m_edit, RT_KEEP_DIST_TO_BOTTOM, &m_list);
	m_resize.AddControl(&m_addButton, RT_KEEP_DIST_TO_RIGHT, &m_edit, RT_KEEP_DIST_TO_BOTTOM, &m_list);
	m_resize.AddControl(&m_deleteButton, RT_KEEP_DIST_TO_RIGHT, &m_edit, RT_KEEP_DIST_TO_BOTTOM, &m_list);
	m_resize.AddControl(&m_changeButton, RT_KEEP_DIST_TO_RIGHT, &m_edit, RT_KEEP_DIST_TO_BOTTOM, &m_list);
	m_resize.AddControl(&m_testEdit, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list, RT_KEEP_DIST_TO_RIGHT, this);
	m_resize.AddControl(&m_static, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list, RT_KEEP_DIST_TO_RIGHT, this);
	m_resize.AddControl(&m_testButton, RT_KEEP_DIST_TO_RIGHT, &m_static, RT_KEEP_DIST_TO_BOTTOM, &m_testEdit);

	m_testEdit.SetWindowText(_T("�������ı�"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}

// ˫���б�
void CRegListPage::OnDblclkList1()
{
	CString tmp;
	m_list.GetText(m_list.GetCurSel(), tmp);
	m_regexCheck.SetCheck(tmp.Left(REGEX_PREFIX_LENGTH) == IS_REGEX_PREFIX);
	m_edit.SetWindowText(tmp.Right(tmp.GetLength() - REGEX_PREFIX_LENGTH));
}

// ���
void CRegListPage::OnClickedButton1()
{
	CString text;
	m_edit.GetWindowText(text);
	if (text == _T(""))
	{
		AfxMessageBox(_T("���ݲ���Ϊ�գ�"), MB_ICONERROR);
		return;
	}
	BOOL isRegex = m_regexCheck.GetCheck();
	if (CheckMatchTooMuch(text, isRegex))
	{
		AfxMessageBox(_T("ƥ��̫������"), MB_ICONERROR);
		return;
	}
	int index = m_list.GetCurSel();
	index = m_list.InsertString(index + 1, (isRegex ? IS_REGEX_PREFIX : NOT_REGEX_PREFIX) + text);
	m_list.SetCurSel(index);
	PostChangeList();
}

// ɾ��
void CRegListPage::OnClickedButton2()
{
	int index = m_list.GetCurSel();
	if (index == LB_ERR)
		return;
	m_list.DeleteString(index);
	m_list.SetCurSel(index == 0 ? 0 : index - 1);
}

// �޸�
void CRegListPage::OnClickedButton3()
{
	CString text;
	m_edit.GetWindowText(text);
	if (text == _T(""))
	{
		AfxMessageBox(_T("���ݲ���Ϊ�գ�"), MB_ICONERROR);
		return;
	}
	BOOL isRegex = m_regexCheck.GetCheck();
	if (CheckMatchTooMuch(text, isRegex))
	{
		AfxMessageBox(_T("ƥ��̫������"), MB_ICONERROR);
		return;
	}
	int index = m_list.GetCurSel();
	if (index == LB_ERR)
		return;
	m_list.DeleteString(index);
	index = m_list.InsertString(index, (isRegex ? IS_REGEX_PREFIX : NOT_REGEX_PREFIX) + text);
	m_list.SetCurSel(index);
	PostChangeList();
}

// ����
void CRegListPage::OnClickedButton6()
{
	CString text;
	m_edit.GetWindowText(text);
	BOOL isRegex = m_regexCheck.GetCheck();
	CString test;
	m_testEdit.GetWindowText(test);
	if (TestMatch(test, text, isRegex))
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

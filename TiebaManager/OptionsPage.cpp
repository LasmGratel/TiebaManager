// OptionsPage.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "OptionsPage.h"
#include "Setting.h"
#include "SettingDlg.h"


// COptionsPage �Ի���

IMPLEMENT_DYNAMIC(COptionsPage, CNormalDlg)

COptionsPage::COptionsPage(CWnd* pParent /*=NULL*/)
	: CNormalDlg(COptionsPage::IDD, pParent)
{

}

#pragma region MFC
COptionsPage::~COptionsPage()
{
}

void COptionsPage::DoDataExchange(CDataExchange* pDX)
{
	CNormalDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDC_EDIT1, m_edit);
	DDX_Control(pDX, IDC_BUTTON1, m_newOptionsButton);
	DDX_Control(pDX, IDC_BUTTON2, m_deleteOptionsButton);
	DDX_Control(pDX, IDC_BUTTON3, m_renameOptionsButton);
	DDX_Control(pDX, IDC_BUTTON6, m_loadOptionsButton);
	DDX_Control(pDX, IDC_BUTTON4, m_saveOptionsButton);
	DDX_Control(pDX, IDC_STATIC1, m_currentOptionStatic);
}


BEGIN_MESSAGE_MAP(COptionsPage, CNormalDlg)
	ON_LBN_DBLCLK(IDC_LIST1, &COptionsPage::OnLbnDblclkList1)
	ON_BN_CLICKED(IDC_BUTTON1, &COptionsPage::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &COptionsPage::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &COptionsPage::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON6, &COptionsPage::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON4, &COptionsPage::OnBnClickedButton4)
END_MESSAGE_MAP()
#pragma endregion

// COptionsPage ��Ϣ�������

// ��ʼ��
BOOL COptionsPage::OnInitDialog()
{
	CNormalDlg::OnInitDialog();

	m_resize.AddControl(&m_currentOptionStatic, RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_RIGHT, this);
	m_resize.AddControl(&m_list, RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, this);
	m_resize.AddControl(&m_edit, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list);
	m_resize.AddControl(&m_newOptionsButton, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list);
	m_resize.AddControl(&m_deleteOptionsButton, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list);
	m_resize.AddControl(&m_renameOptionsButton, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list);
	m_resize.AddControl(&m_loadOptionsButton, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list);
	m_resize.AddControl(&m_saveOptionsButton, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}

// ˫���б�
void COptionsPage::OnLbnDblclkList1()
{
	CString tmp;
	m_list.GetText(m_list.GetCurSel(), tmp);
	m_edit.SetWindowText(tmp);
}

// �½�
void COptionsPage::OnBnClickedButton1()
{
	CString name;
	m_edit.GetWindowText(name);
	if (name == _T(""))
	{
		AfxMessageBox(_T("����������Ϊ�գ�"), MB_ICONERROR);
		return;
	}
	if (!PathFileExists(OPTIONS_PATH))
		CreateDirectory(OPTIONS_PATH, NULL);
	if (PathFileExists(OPTIONS_PATH + name + _T(".tb")))
	{
		AfxMessageBox(_T("�����Ѵ��ڣ�"), MB_ICONERROR);
		return;
	}
	m_list.SetCurSel(m_list.AddString(name));
	OnBnClickedButton4();
	OnBnClickedButton6();
}

// ɾ��
void COptionsPage::OnBnClickedButton2()
{
	int index = m_list.GetCurSel();
	if (index == LB_ERR)
		return;
	CString name;
	m_list.GetText(index, name);
	if (!DeleteFile(OPTIONS_PATH + name + _T(".tb")))
	{
		AfxMessageBox(_T("ɾ������ʧ�ܣ�"), MB_ICONERROR);
		return;
	}
	m_list.DeleteString(index);
	m_list.SetCurSel(index == 0 ? 0 : index - 1);
	if (m_list.GetCurSel() == LB_ERR)
	{
		ReadOptions(_T(""));
		((CSettingDlg*)GetParent()->GetParent())->ShowCurrentOptions();
	}
	else
		OnBnClickedButton6();
}

// ������
void COptionsPage::OnBnClickedButton3()
{
	CString newName;
	m_edit.GetWindowText(newName);
	if (newName == _T(""))
	{
		AfxMessageBox(_T("����������Ϊ�գ�"), MB_ICONERROR);
		return;
	}
	int index = m_list.GetCurSel();
	if (index == LB_ERR)
		return;
	CString name;
	m_list.GetText(index, name);
	if (!MoveFile(OPTIONS_PATH + name + _T(".tb"), OPTIONS_PATH + newName + _T(".tb")))
	{
		AfxMessageBox(_T("����������ʧ�ܣ�"), MB_ICONERROR);
		return;
	}
	if (g_currentOption == name)
	{
		g_currentOption = newName;
		m_currentOptionStatic.SetWindowText(_T("��ǰ������") + newName);
	}
	m_list.DeleteString(index);
	m_list.InsertString(index, newName);
	m_list.SetCurSel(index);
}

// ����
void COptionsPage::OnBnClickedButton6()
{
	int index = m_list.GetCurSel();
	if (index == LB_ERR)
	{
		AfxMessageBox(_T("��ѡ��һ��������"), MB_ICONERROR);
		return;
	}
	CString name;
	m_list.GetText(index, name);
	((CSettingDlg*)GetParent()->GetParent())->ShowOptionsInFile(OPTIONS_PATH + name + _T(".tb"));
	m_currentOptionStatic.SetWindowText(_T("��ǰ������") + name);
}

// ����
void COptionsPage::OnBnClickedButton4()
{
	int index = m_list.GetCurSel();
	if (index == LB_ERR)
	{
		AfxMessageBox(_T("��ѡ��һ��������"), MB_ICONERROR);
		return;
	}
	if (!PathFileExists(OPTIONS_PATH))
		CreateDirectory(OPTIONS_PATH, NULL);
	CString name;
	m_list.GetText(index, name);
	((CSettingDlg*)GetParent()->GetParent())->SaveOptionsInDlg(OPTIONS_PATH + name + _T(".tb"));
}

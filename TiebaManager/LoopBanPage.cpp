// LoopBanPage.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LoopBanPage.h"

#include "StringHelper.h"
#include "NetworkHelper.h"

#include "TiebaCollect.h"

#include "SuperFunctionDlg.h"


// CLoopBanPage �Ի���

IMPLEMENT_DYNAMIC(CLoopBanPage, CListPage)

CLoopBanPage::CLoopBanPage(CWnd* pParent /*=NULL*/)
	: CListPage(CLoopBanPage::IDD, pParent)
{

}

#pragma region MFC
CLoopBanPage::~CLoopBanPage()
{
}

void CLoopBanPage::DoDataExchange(CDataExchange* pDX)
{
	CListPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK6, m_logCheck);
	DDX_Control(pDX, IDC_CHECK7, m_enableCheck);
}


BEGIN_MESSAGE_MAP(CLoopBanPage, CListPage)
END_MESSAGE_MAP()
#pragma endregion

// CLoopBanPage ��Ϣ�������

// ��ʼ��
BOOL CLoopBanPage::OnInitDialog()
{
	CListPage::OnInitDialog();

	m_resize.AddControl(&m_enableCheck, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list);
	m_resize.AddControl(&m_logCheck, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}

// ���
void CLoopBanPage::OnClickedButton1()
{
	CString text;
	m_edit.GetWindowText(text);
	if (text == _T(""))
	{
		AfxMessageBox(_T("���ݲ���Ϊ�գ�"), MB_ICONERROR);
		return;
	}

	CString pid = GetPIDFromUser(text);
	if (pid == NET_TIMEOUT_TEXT)
		pid = _T("");

	int index = m_list.GetCurSel();
	index = m_list.InsertString(index + 1, text);
	m_pid.insert(m_pid.begin() + index, pid);
	m_list.SetCurSel(index);

	((CSuperFunctionDlg*)GetParent()->GetParent())->m_clearScanCache = TRUE;
}

// ɾ��
void CLoopBanPage::OnClickedButton2()
{
	int index = m_list.GetCurSel();
	if (index == LB_ERR)
		return;
	m_list.DeleteString(index);
	m_pid.erase(m_pid.begin() + index);
	m_list.SetCurSel(index == 0 ? 0 : index - 1);
}

// �޸�
void CLoopBanPage::OnClickedButton3()
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

	CString pid = GetPIDFromUser(text);
	if (pid == NET_TIMEOUT_TEXT)
		pid = _T("");

	m_list.DeleteString(index);
	index = m_list.InsertString(index, text);
	m_pid.insert(m_pid.begin() + index, pid);
	m_list.SetCurSel(index);

	((CSuperFunctionDlg*)GetParent()->GetParent())->m_clearScanCache = TRUE;
}

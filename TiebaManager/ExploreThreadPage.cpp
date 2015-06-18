// ExploreThreadPage.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TiebaManager.h"
#include "ExploreThreadPage.h"
#include "afxdialogex.h"


// CExploreThreadPage �Ի���

IMPLEMENT_DYNAMIC(CExploreThreadPage, CExplorerPage)

CExploreThreadPage::CExploreThreadPage(CWnd* pParent /*=NULL*/)
	: CExplorerPage(pParent)
{

}

CExploreThreadPage::~CExploreThreadPage()
{
}

void CExploreThreadPage::DoDataExchange(CDataExchange* pDX)
{
	CExplorerPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CExploreThreadPage, CExplorerPage)
	ON_BN_CLICKED(IDC_BUTTON1, &CExploreThreadPage::OnBnClickedButton1)
END_MESSAGE_MAP()


// CExploreThreadPage ��Ϣ�������

BOOL CExploreThreadPage::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_edit.SetWindowText(_T("1"));

	int i = 0;
	m_list.InsertColumn(i++, _T("�ظ�"), LVCFMT_CENTER, 30);
	m_list.InsertColumn(i++, _T("����"), LVCFMT_CENTER, 300);
	m_list.InsertColumn(i++, _T("����"), LVCFMT_CENTER, 50);

	OnBnClickedButton1();

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}

// ת��
void CExploreThreadPage::OnBnClickedButton1()
{
	CString sPage;
	m_edit.GetWindowText(sPage);
	int iPage = _ttoi(sPage);
	if (iPage < 1)
	{
		m_edit.SetWindowText(_T("1"));
		iPage = 1;
	}
	CString ignoreThread; // ����ǰ��������
	ignoreThread.Format(_T("%d"), (iPage - 1) * 50);

	GetThreads(g_forumName, ignoreThread, m_threads);
	m_list.DeleteAllItems();
	for (const ThreadInfo& i : m_threads)
	{
		int index = m_list.GetItemCount() - 1;
		m_list.InsertItem(index, i.reply);
		m_list.SetItemText(index, 1, i.title);
		m_list.SetItemText(index, 2, i.author);
	}
}

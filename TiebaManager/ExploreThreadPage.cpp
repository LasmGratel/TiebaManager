// ExploreThreadPage.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ExploreThreadPage.h"
#include "TiebaCollect.h"
#include "ExplorerDlg.h"
#include "ScanImage.h"


// CExploreThreadPage �Ի���

IMPLEMENT_DYNAMIC(CExploreThreadPage, CExplorerPage)

CExploreThreadPage::CExploreThreadPage(CWnd* pParent /*=NULL*/)
	: CExplorerPage(pParent)
{

}

#pragma region MFC
CExploreThreadPage::~CExploreThreadPage()
{
}

void CExploreThreadPage::DoDataExchange(CDataExchange* pDX)
{
	CExplorerPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CExploreThreadPage, CExplorerPage)
	ON_BN_CLICKED(IDC_BUTTON1, &CExploreThreadPage::OnBnClickedButton1)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CExploreThreadPage::OnItemchangedList1)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CExploreThreadPage::OnDblclkList1)
END_MESSAGE_MAP()
#pragma endregion

// CExploreThreadPage ��Ϣ�������

// ��ʼ��
BOOL CExploreThreadPage::OnInitDialog()
{
	CExplorerPage::OnInitDialog();

	m_edit.SetWindowText(_T("1"));

	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	int i = 0;
	m_list.InsertColumn(i++, _T(""), LVCFMT_LEFT, 0);
	m_list.InsertColumn(i++, _T("�ظ�"), LVCFMT_RIGHT, 50);
	m_list.InsertColumn(i++, _T("����"), LVCFMT_CENTER, 540);
	m_list.InsertColumn(i++, _T("����"), LVCFMT_CENTER, 130);
	m_list.DeleteColumn(0); // �����һ�����ֲ����Ҷ��������

	OnBnClickedButton1();

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}

// ת��
void CExploreThreadPage::OnBnClickedButton1()
{
	m_gotoButton.EnableWindow(FALSE);

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
	((CExplorerDlg*)GetParent()->GetParent())->m_edit.SetWindowText(_T(""));
	for (const ThreadInfo& i : m_threads)
	{
		int index = m_list.GetItemCount();
		m_list.InsertItem(index, i.reply);
		m_list.SetItemText(index, 1, i.title);
		m_list.SetItemText(index, 2, i.author);
	}

	m_gotoButton.EnableWindow(TRUE);
}

// ѡ����ı�
void CExploreThreadPage::OnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	if (pNMLV->iItem != LB_ERR)
	{
		CExplorerDlg* explorerDlg = (CExplorerDlg*)GetParent()->GetParent();
		explorerDlg->m_edit.SetWindowText(m_threads[pNMLV->iItem].title + _T("\r\n") 
			+ m_threads[pNMLV->iItem].preview + _T("\r\n\r\n") + m_threads[pNMLV->iItem].author);
		vector<CString>* img = new vector<CString>();
		GetThreadImage(m_threads[pNMLV->iItem].preview).GetImage(*img);
		explorerDlg->ViewImages(img);
	}

	*pResult = 0;
}

// ˫���б�
void CExploreThreadPage::OnDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	if (pNMItemActivate->iItem != LB_ERR)
	{
		CExplorerDlg* parentDlg = (CExplorerDlg*)GetParent()->GetParent();
		CExplorePostPage& explorePostPage = parentDlg->m_explorePostPage;
		explorePostPage.m_tid = m_threads[pNMItemActivate->iItem].tid;
		explorePostPage.m_edit.SetWindowText(_T("1"));
		explorePostPage.OnBnClickedButton1();
		parentDlg->m_tab.SetCurSel(1);
		LRESULT tmp;
		parentDlg->OnTcnSelchangeTab1(NULL, &tmp);
	}

	*pResult = 0;
}

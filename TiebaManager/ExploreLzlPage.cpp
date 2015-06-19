// ExploreThreadPage.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TiebaManager.h"
#include "ExploreLzlPage.h"
#include "afxdialogex.h"
#include "Tieba.h"
#include "ExplorerDlg.h"


// CExploreLzlPage �Ի���

IMPLEMENT_DYNAMIC(CExploreLzlPage, CExplorerPage)

CExploreLzlPage::CExploreLzlPage(CWnd* pParent /*=NULL*/)
	: CExplorerPage(pParent)
{

}

#pragma region MFC
CExploreLzlPage::~CExploreLzlPage()
{
}

void CExploreLzlPage::DoDataExchange(CDataExchange* pDX)
{
	CExplorerPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CExploreLzlPage, CExplorerPage)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CExploreLzlPage::OnItemchangedList1)
END_MESSAGE_MAP()
#pragma endregion

// CExploreLzlPage ��Ϣ�������

// ��ʼ��
BOOL CExploreLzlPage::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_static.ShowWindow(SW_HIDE);
	m_edit.ShowWindow(SW_HIDE);
	m_gotoButton.ShowWindow(SW_HIDE);

	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	int i = 0;
	m_list.InsertColumn(i++, _T(""), LVCFMT_LEFT, 0);
	m_list.InsertColumn(i++, _T("¥��"), LVCFMT_RIGHT, 50);
	m_list.InsertColumn(i++, _T("����"), LVCFMT_LEFT, 540);
	m_list.InsertColumn(i++, _T("����"), LVCFMT_CENTER, 130);
	m_list.DeleteColumn(0); // �����һ�����ֲ����Ҷ��������

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}

// ѡ����ı�
void CExploreLzlPage::OnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	if (pNMLV->iItem != LB_ERR)
		((CExplorerDlg*)GetParent()->GetParent())->m_edit.SetWindowText(m_lzls[pNMLV->iItem].content +
			_T("\r\n\r\n") + m_lzls[pNMLV->iItem].author);

	*pResult = 0;
}

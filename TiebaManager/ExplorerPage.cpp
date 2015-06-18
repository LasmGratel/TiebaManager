// ExplorerPage.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TiebaManager.h"
#include "ExplorerPage.h"
#include "afxdialogex.h"


// CExplorerPage �Ի���

IMPLEMENT_DYNAMIC(CExplorerPage, CDialog)

CExplorerPage::CExplorerPage(CWnd* pParent /*=NULL*/)
	: CDialog(CExplorerPage::IDD, pParent)
{

}

#pragma region MFC
CExplorerPage::~CExplorerPage()
{
}

void CExplorerPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC1, m_static);
	DDX_Control(pDX, IDC_EDIT2, m_edit);
	DDX_Control(pDX, IDC_BUTTON1, m_gotoButton);
	DDX_Control(pDX, IDC_LIST1, m_list);
}


BEGIN_MESSAGE_MAP(CExplorerPage, CDialog)
	ON_WM_CLOSE()
	ON_WM_SIZE()
END_MESSAGE_MAP()
#pragma endregion

// CExplorerPage ��Ϣ�������

#pragma region UI
// ����Esc�رմ���
void CExplorerPage::OnCancel()
{
}

// ���λس��رմ���
void CExplorerPage::OnOK()
{
}

// ���ٴ���
void CExplorerPage::OnClose()
{
	DestroyWindow();

	CDialog::OnClose();
}

// �ı�ߴ�
void CExplorerPage::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (m_list.m_hWnd == NULL)
		return;

	CRect rect;
	GetClientRect(&rect); // Ĭ��684 * 315
	m_list.SetWindowPos(NULL, 0, 0, rect.Width() - 22, rect.Height() - 56, SWP_NOMOVE | SWP_NOREDRAW);

	Invalidate();
}
#pragma endregion

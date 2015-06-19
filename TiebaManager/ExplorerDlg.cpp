// ExplorerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TiebaManager.h"
#include "ExplorerDlg.h"
#include "afxdialogex.h"
#include "TiebaManagerDlg.h"


// CExplorerDlg �Ի���

IMPLEMENT_DYNAMIC(CExplorerDlg, CDialog)

// ���캯��
CExplorerDlg::CExplorerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CExplorerDlg::IDD, pParent)
{
	// ��ʼ��m_pages
	int i = 0;
	m_pages[i++] = &m_exploreThreadPage;
	//m_pages[i++] = &m_keywordsPage;
	//m_pages[i++] = &m_blackListPage;
}

#pragma region MFC
CExplorerDlg::~CExplorerDlg()
{
}

void CExplorerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tab);
	DDX_Control(pDX, IDC_EDIT1, m_edit);
	DDX_Control(pDX, IDC_BUTTON1, m_deleteButton);
	DDX_Control(pDX, IDC_BUTTON2, m_banButton);
	DDX_Control(pDX, IDC_BUTTON3, m_explorerButton);
}


BEGIN_MESSAGE_MAP(CExplorerDlg, CDialog)
	ON_WM_CLOSE()
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CExplorerDlg::OnTcnSelchangeTab1)
END_MESSAGE_MAP()
#pragma endregion

// CExplorerDlg ��Ϣ�������

#pragma region UI
// ���� /////////////////////////////////////////////////////////////////////////////////

// ����Esc�رմ���
void CExplorerDlg::OnCancel()
{
}

// ���λس��رմ���
void CExplorerDlg::OnOK()
{
}

// ���ٴ���
void CExplorerDlg::OnClose()
{
	DestroyWindow();
}

// �ͷ�this
void CExplorerDlg::PostNcDestroy()
{
	CDialog::PostNcDestroy();

	((CTiebaManagerDlg*)AfxGetApp()->m_pMainWnd)->m_explorerDlg = NULL;
	delete this;
}

// ������С�ߴ�
void CExplorerDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = 586;
	lpMMI->ptMinTrackSize.y = 482;

	CDialog::OnGetMinMaxInfo(lpMMI);
}

// �ı�ߴ�
void CExplorerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (m_tab.m_hWnd == NULL)
		return;

	CRect rect;
	GetClientRect(&rect); // Ĭ��798 * 564
	m_tab.SetWindowPos(NULL, 0, 0, rect.Width() - 21, 315, SWP_NOMOVE | SWP_NOREDRAW);
	m_edit.SetWindowPos(NULL, 0, 0, rect.Width() - 94, rect.Height() - 350, SWP_NOMOVE | SWP_NOREDRAW);
	int x = rect.Width() - 72;
	m_deleteButton.SetWindowPos(NULL, x, 338, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);
	m_banButton.SetWindowPos(NULL, x, 371, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);
	m_explorerButton.SetWindowPos(NULL, x, 405, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);

	m_tab.GetClientRect(&rect);
	rect.left += 1; rect.right -= 3; rect.top += 23; rect.bottom -= 2;
	for (int i = 0; i < _countof(m_pages); i++)
		m_pages[i]->SetWindowPos(NULL, 0, 0, rect.Width(), rect.Height(), SWP_NOMOVE | SWP_NOREDRAW);

	Invalidate();
}

// �л���ǩ
void CExplorerDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	int index = m_tab.GetCurSel();
	for (int i = 0; i < _countof(m_pages); i++)
		m_pages[i]->ShowWindow(i == index ? SW_SHOW : SW_HIDE);
}
#pragma endregion

// ��ʼ��
BOOL CExplorerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(hIcon, FALSE);			// ����Сͼ��

	// ��ʼ��m_tab
	int i = 0;
	m_tab.InsertItem(i++, _T("����"));
	//m_tab.InsertItem(i++, _T("����"));
	//m_tab.InsertItem(i++, _T("¥��¥"));

	// ��ʼ����ҳ
	m_exploreThreadPage.Create(IDD_EXPLORER_PAGE, &m_tab);

	CRect rect;
	m_tab.GetClientRect(&rect);
	rect.left += 1; rect.right -= 3; rect.top += 23; rect.bottom -= 2;
	m_pages[0]->SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_SHOWWINDOW);
	for (i = 1; i < _countof(m_pages); i++)
		m_pages[i]->SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_HIDEWINDOW);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}

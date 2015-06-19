// ExplorerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TiebaManager.h"
#include "ExplorerDlg.h"
#include "afxdialogex.h"
#include "TiebaManagerDlg.h"
#include "Tieba.h"
#include "Global.h"
#include <Mmsystem.h>


// CExplorerDlg �Ի���

IMPLEMENT_DYNAMIC(CExplorerDlg, CDialog)

// ���캯��
CExplorerDlg::CExplorerDlg(CWnd* pParent /*=NULL*/)

	: CDialog(CExplorerDlg::IDD, pParent)
{
	// ��ʼ��m_pages
	int i = 0;
	m_pages[i++] = &m_exploreThreadPage;
	m_pages[i++] = &m_explorePostPage;
	m_pages[i++] = &m_exploreLzlPage;
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
	ON_BN_CLICKED(IDC_BUTTON1, &CExplorerDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CExplorerDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CExplorerDlg::OnBnClickedButton3)
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
	m_tab.InsertItem(i++, _T("����"));
	m_tab.InsertItem(i++, _T("¥��¥"));

	// ��ʼ����ҳ
	m_exploreThreadPage.Create(IDD_EXPLORER_PAGE, &m_tab);
	m_explorePostPage.Create(IDD_EXPLORER_PAGE, &m_tab);
	m_exploreLzlPage.Create(IDD_EXPLORER_PAGE, &m_tab);

	CRect rect;
	m_tab.GetClientRect(&rect);
	rect.left += 1; rect.right -= 3; rect.top += 23; rect.bottom -= 2;
	m_pages[0]->SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_SHOWWINDOW);
	for (i = 1; i < _countof(m_pages); i++)
		m_pages[i]->SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_HIDEWINDOW);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}

// ɾ��
void CExplorerDlg::OnBnClickedButton1()
{
	int tabIndex = m_tab.GetCurSel(); 
	POSITION pos = m_pages[tabIndex]->m_list.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return;
	int index = m_pages[tabIndex]->m_list.GetNextSelectedItem(pos);


	CString code;
	if (tabIndex == 0) // ����
	{
		ThreadInfo& thread = m_exploreThreadPage.m_threads[index];
		code = DeleteThread(thread.tid);
		if (code == _T("0"))
			g_deletedTID.insert(_ttoi64(thread.tid));
	}
	else if (tabIndex == 1) // ����
		code = DeletePost(m_explorePostPage.m_tid, m_explorePostPage.m_posts[index].pid);
	else // ¥��¥
		code = DeleteLZL(m_explorePostPage.m_tid, m_exploreLzlPage.m_lzls[index].pid);


	if (code != _T("0"))
		AfxMessageBox(_T("ɾ��ʧ�ܣ��������" + code + _T("(") + GetTiebaErrorText(code) + _T(")")), MB_ICONERROR);
	else
		sndPlaySound(_T("ɾ��.wav"), SND_ASYNC | SND_NODEFAULT);
}

// ���
void CExplorerDlg::OnBnClickedButton2()
{
	int tabIndex = m_tab.GetCurSel();
	POSITION pos = m_pages[tabIndex]->m_list.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return;
	int index = m_pages[tabIndex]->m_list.GetNextSelectedItem(pos);


	CString author;
	get_ip_tbs banTBS;
	BOOL result;
	if (tabIndex == 0) // ����
	{
		author = m_exploreThreadPage.m_threads[index].author;
		result = GetBanTBS(m_exploreThreadPage.m_threads[index].tid, author, banTBS);
	}
	else if (tabIndex == 1) // ����
	{
		author = m_explorePostPage.m_posts[index].author;
		result = GetBanTBS(m_explorePostPage.m_tid, author, banTBS);
	}
	else // ¥��¥
	{
		author = m_exploreLzlPage.m_lzls[index].author;
		result = GetBanTBS(m_explorePostPage.m_tid, author, banTBS);
	}
	if (!result)
	{
		AfxMessageBox(_T("��ȡ��������ʧ��"), MB_ICONERROR);
		return;
	}


	CString code = BanID(author, banTBS.tbs_ban_user);
	if (code != _T("0"))
		AfxMessageBox(_T("���ʧ�ܣ��������" + code + _T("(") + GetTiebaErrorText(code) + _T(")")), MB_ICONERROR);
	else
		sndPlaySound(_T("���.wav"), SND_ASYNC | SND_NODEFAULT);
}

// �����
void CExplorerDlg::OnBnClickedButton3()
{
	CString url;
	if (m_tab.GetCurSel() == 0)
	{
		POSITION pos = m_exploreThreadPage.m_list.GetFirstSelectedItemPosition();
		if (pos == NULL)
			return;
		int index = m_exploreThreadPage.m_list.GetNextSelectedItem(pos);
		url = _T("http://tieba.baidu.com/p/") + m_exploreThreadPage.m_threads[index].tid;
	}
	else
	{
		POSITION pos = m_exploreThreadPage.m_list.GetFirstSelectedItemPosition();
		if (pos == NULL)
			return;
		int index = m_exploreThreadPage.m_list.GetNextSelectedItem(pos);
		CString page;
		m_exploreThreadPage.m_edit.GetWindowText(page);
		url = _T("http://tieba.baidu.com/p/") + m_explorePostPage.m_tid + _T("?pn=") + page;
	}
	ShellExecute(NULL, _T("open"), url, NULL, NULL, SW_NORMAL);
}

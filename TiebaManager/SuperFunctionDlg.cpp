// SuperFunctionDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SuperFunctionDlg.h"
#include "TiebaManagerDlg.h"


// CSuperFunctionDlg �Ի���

IMPLEMENT_DYNAMIC(CSuperFunctionDlg, CNormalDlg)

// ���캯��
CSuperFunctionDlg::CSuperFunctionDlg(CWnd* pParent /*=NULL*/)
	: CNormalDlg(CSuperFunctionDlg::IDD, pParent),
	m_pagesResize(&m_tab)
{
	// ��ʼ��m_pages
	int i = 0;
	m_pages[i++] = &m_loopBanPage;
	m_pages[i++] = &m_defriendPage;
	m_pages[i++] = &m_lockThreadPage;
}

#pragma region MFC
CSuperFunctionDlg::~CSuperFunctionDlg()
{
}

void CSuperFunctionDlg::DoDataExchange(CDataExchange* pDX)
{
	CNormalDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tab);
	DDX_Control(pDX, IDOK, m_okButton);
	DDX_Control(pDX, IDCANCEL, m_cancelButton);
}


BEGIN_MESSAGE_MAP(CSuperFunctionDlg, CNormalDlg)
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CSuperFunctionDlg::OnTcnSelchangeTab1)
	ON_WM_CLOSE()
END_MESSAGE_MAP()
#pragma endregion

// CSuperFunctionDlg ��Ϣ�������

// ��ʼ��
BOOL CSuperFunctionDlg::OnInitDialog()
{
	CNormalDlg::OnInitDialog();

	HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(hIcon, FALSE);			// ����Сͼ��

	SetWindowText(_T("��������"));

	// ��ʼ��m_tab
	int i = 0;
	m_tab.InsertItem(i++, _T("ѭ����"));
	m_tab.InsertItem(i++, _T("��������"));
	m_tab.InsertItem(i++, _T("����"));

	// ��ʼ����ҳ
#define CREATE_PAGE(page) page.Create(page.IDD, &m_tab)
	CREATE_PAGE(m_loopBanPage);
	CREATE_PAGE(m_defriendPage);
	CREATE_PAGE(m_lockThreadPage);

	CRect rect;
	m_tab.GetClientRect(&rect);
	rect.left += 1; rect.right -= 3; rect.top += 23; rect.bottom -= 2;
	m_pages[0]->SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_SHOWWINDOW);
	for (i = 1; i < _countof(m_pages); i++)
		m_pages[i]->SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_HIDEWINDOW);

	m_resize.AddControl(&m_tab, RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, this);
	m_resize.AddControl(&m_okButton, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, &m_tab);
	m_resize.AddControl(&m_cancelButton, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, &m_tab);
	for (i = 0; i < _countof(m_pages); i++)
		m_pagesResize.AddControl(m_pages[i], RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_RIGHT, &m_tab, RT_KEEP_DIST_TO_BOTTOM, &m_tab);

	// ��ʾ����
	ShowCurrentOptions();
	m_clearCache = FALSE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}

#pragma region UI
// ���� /////////////////////////////////////////////////////////////////////////////////

// ȡ��
void CSuperFunctionDlg::OnCancel()
{
	DestroyWindow();
}

// ��ʾ�Ƿ񱣴�
void CSuperFunctionDlg::OnClose()
{
	int result = AfxMessageBox(_T("�������ã�"), MB_ICONQUESTION | MB_YESNOCANCEL);
	if (result == IDYES)
	{
		OnOK();
		return;
	}
	else if (result == IDCANCEL)
		return;

	DestroyWindow();
}

// �ͷ�this
void CSuperFunctionDlg::PostNcDestroy()
{
	CNormalDlg::PostNcDestroy();

	((CTiebaManagerDlg*)AfxGetApp()->m_pMainWnd)->m_superFunctionDlg = NULL;
	delete this;
}

// ������С�ߴ�
void CSuperFunctionDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	/*lpMMI->ptMinTrackSize.x = 666;
	lpMMI->ptMinTrackSize.y = 576;*/

	CNormalDlg::OnGetMinMaxInfo(lpMMI);
}

// �ı�ߴ�
void CSuperFunctionDlg::OnSize(UINT nType, int cx, int cy)
{
	CNormalDlg::OnSize(nType, cx, cy);
	m_pagesResize.Resize();
}

// �л���ǩ
void CSuperFunctionDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	int index = m_tab.GetCurSel();
	for (int i = 0; i < _countof(m_pages); i++)
		m_pages[i]->ShowWindow(i == index ? SW_SHOW : SW_HIDE);
}
#pragma endregion

// ��ʾ��ǰ����
void CSuperFunctionDlg::ShowCurrentOptions()
{
	CString tmp;
	// ѭ����
	CLoopBanConfig loopBanConfig;
	loopBanConfig.Load(CURRENT_USER_DIR_PATH + _T("options2.xml"));
	
	m_loopBanPage.ShowList(loopBanConfig.m_userList);				// �û���
	m_loopBanPage.m_pid = std::move(*loopBanConfig.m_pidList);		// PID
	m_loopBanPage.m_logCheck.SetCheck(loopBanConfig.m_log);			// �����־
	m_loopBanPage.m_enableCheck.SetCheck(loopBanConfig.m_enable);	// ����
	tmp.Format(_T("%g"), *loopBanConfig.m_banInterval);
	m_loopBanPage.m_banIntervalEdit.SetWindowText(tmp);				// ������
}

// Ӧ�öԻ����е�����
void CSuperFunctionDlg::ApplyOptionsInDlg()
{
	CString strBuf;
	// ѭ����
	CLoopBanConfig loopBanConfig;

	m_loopBanPage.ApplyList(loopBanConfig.m_userList);					// �û���
	*loopBanConfig.m_pidList = m_loopBanPage.m_pid;						// PID
	*loopBanConfig.m_log = m_loopBanPage.m_logCheck.GetCheck();			// �����־
	*loopBanConfig.m_enable = m_loopBanPage.m_enableCheck.GetCheck();	// ����
	m_loopBanPage.m_banIntervalEdit.GetWindowText(strBuf);
	*loopBanConfig.m_banInterval = (float)_ttof(strBuf);				// ������

	loopBanConfig.Save(CURRENT_USER_DIR_PATH + _T("options2.xml"));

	if (m_clearCache)
		DeleteFile(CURRENT_USER_DIR_PATH + _T("LoopBanDate.xml"));
}

// ȷ��
void CSuperFunctionDlg::OnOK()
{
	ApplyOptionsInDlg();

	DestroyWindow();
}

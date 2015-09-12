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

	// ��ʼ����ҳ
	m_loopBanPage.Create(IDD_LIST_PAGE, &m_tab);

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
	lpMMI->ptMinTrackSize.x = 666;
	lpMMI->ptMinTrackSize.y = 576;

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

// ȷ��
void CSuperFunctionDlg::OnOK()
{


	DestroyWindow();
}

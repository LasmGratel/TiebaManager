// SuperFunctionDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SuperFunctionDlg.h"
#include "TiebaManagerDlg.h"


// CSuperFunctionDlg �Ի���

IMPLEMENT_DYNAMIC(CSuperFunctionDlg, CDialog)

// ���캯��
CSuperFunctionDlg::CSuperFunctionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSuperFunctionDlg::IDD, pParent)
{
	// ��ʼ��m_pages
	int i = 0;
	////////////////////m_pages[i++] = &m_prefPage;
}

#pragma region MFC
CSuperFunctionDlg::~CSuperFunctionDlg()
{
}

void CSuperFunctionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tab);
	DDX_Control(pDX, IDOK, m_okButton);
	DDX_Control(pDX, IDCANCEL, m_cancelButton);
}


BEGIN_MESSAGE_MAP(CSuperFunctionDlg, CDialog)
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
	CDialog::OnInitDialog();

	HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(hIcon, FALSE);			// ����Сͼ��

	SetWindowText(_T("��������"));

	// ��ʼ��m_tab
	int i = 0;
	m_tab.InsertItem(i++, _T("��ѡ��"));

	// ��ʼ����ҳ
	/////////////m_prefPage.Create(IDD_PREF_PAGE, &m_tab);

	CRect rect;
	m_tab.GetClientRect(&rect);
	rect.left += 1; rect.right -= 3; rect.top += 23; rect.bottom -= 2;
	m_pages[0]->SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_SHOWWINDOW);
	for (i = 1; i < _countof(m_pages); i++)
		m_pages[i]->SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_HIDEWINDOW);

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
	CDialog::PostNcDestroy();

	((CTiebaManagerDlg*)AfxGetApp()->m_pMainWnd)->m_superFunctionDlg = NULL;
	delete this;
}

// ������С�ߴ�
void CSuperFunctionDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = 489;
	lpMMI->ptMinTrackSize.y = 411;

	CDialog::OnGetMinMaxInfo(lpMMI);
}

// �ı�ߴ�
void CSuperFunctionDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (m_tab.m_hWnd == NULL)
		return;

	CRect rect;
	GetClientRect(&rect); // Ĭ��473 * 373
	m_tab.SetWindowPos(NULL, 0, 0, rect.Width() - 21, rect.Height() - 58, SWP_NOMOVE | SWP_NOREDRAW);
	int y = rect.Height() - 35;
	m_okButton.SetWindowPos(NULL, rect.Width() - 200, y, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);
	m_cancelButton.SetWindowPos(NULL, rect.Width() - 105, y, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);

	m_tab.GetClientRect(&rect);
	rect.left += 1; rect.right -= 3; rect.top += 23; rect.bottom -= 2;
	for (int i = 0; i < _countof(m_pages); i++)
		m_pages[i]->SetWindowPos(NULL, 0, 0, rect.Width(), rect.Height(), SWP_NOMOVE | SWP_NOREDRAW);

	Invalidate();
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

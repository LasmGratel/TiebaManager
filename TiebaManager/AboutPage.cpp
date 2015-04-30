// AboutPage.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TiebaManager.h"
#include "AboutPage.h"
#include "afxdialogex.h"
#include "Global.h"


// CAboutPage �Ի���

IMPLEMENT_DYNAMIC(CAboutPage, CDialog)

CAboutPage::CAboutPage(CWnd* pParent /*=NULL*/)
	: CDialog(CAboutPage::IDD, pParent)
{

}

#pragma region MFC
CAboutPage::~CAboutPage()
{
}

void CAboutPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_edit);
	DDX_Control(pDX, IDC_STATIC1, m_checkUpdateStatic);
	DDX_Control(pDX, IDC_CHECK1, m_autoCheckUpdateCheck);
	DDX_Control(pDX, IDC_STATIC2, m_authorStatic);
}


BEGIN_MESSAGE_MAP(CAboutPage, CDialog)
	ON_WM_CLOSE()
	ON_STN_CLICKED(IDC_STATIC1, &CAboutPage::OnStnClickedStatic1)
	ON_STN_CLICKED(IDC_STATIC2, &CAboutPage::OnStnClickedStatic2)
	ON_WM_SIZE()
END_MESSAGE_MAP()
#pragma endregion

// CAboutPage ��Ϣ�������

// ��ʼ��
BOOL CAboutPage::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_edit.SetWindowText(_T("Ŀ�������ڰٶ�����Ψһ�İ��������\r\n\
\r\n\
����������ʽ���Լ�ȥ�ٶ�һ��~\r\n\
\r\n\
����ֻת����\"'&<>���пո���������ҳԴ��Ϊ׼���绻�з�Ϊ<br> (��ע��Ļ��᲻ƥ���ƥ��̫��)\r\n\
\r\n\
�˳���ͨ��GPLЭ�鿪Դ���������https://github.com/xfgryujk/TiebaManager�õ�Դ�룬��ӭ���Ƴ���\r\n\
\r\n\
�Ը�����־����Ȥ�Ļ�������https://github.com/xfgryujk/TiebaManager/commits/master\r\n\
\r\n\
��ֻ���ṩ�������������ɵĹ��ߣ�����������ɵȶ�����Ϊ��ɵ���ʧ���˲�����"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}

#pragma region UI
// ����Esc�رմ���
void CAboutPage::OnCancel()
{
}

// ���λس��رմ���
void CAboutPage::OnOK()
{
}

// ���ٴ���
void CAboutPage::OnClose()
{
	DestroyWindow();

	CDialog::OnClose();
}

// �ı�ߴ�
void CAboutPage::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (m_edit.m_hWnd == NULL)
		return;

	CRect rect;
	GetClientRect(&rect); // Ĭ��446 * 287
	m_edit.SetWindowPos(NULL, 0, 0, rect.Width() - 19, rect.Height() - 45, SWP_NOMOVE | SWP_NOREDRAW);
	int y = rect.Height() - 28;
	m_checkUpdateStatic.SetWindowPos(NULL, 11, y, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);
	m_autoCheckUpdateCheck.SetWindowPos(NULL, 116, y, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);
	m_authorStatic.SetWindowPos(NULL, rect.Width() - 124, y, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);

	Invalidate();
}
#pragma endregion


// ������
void CAboutPage::OnStnClickedStatic1()
{
	switch (CheckUpdate())
	{
	case UPDATE_NO_UPDATE:
		AfxMessageBox(_T("�Ѿ������°汾"));
		break;
	case UPDATE_FAILED_TO_GET_FILE_ID:
		if (AfxMessageBox(_T("��ȡ�ļ�IDʧ�ܣ��ֶ����£�"), MB_ICONQUESTION | MB_YESNO) == IDYES)
			ShellExecute(NULL, _T("open"), _T("http://pan.baidu.com/s/1hq86os8#dir/path=%2F%E6%88%91%E7%9A\
%84%E5%88%86%E4%BA%AB%2F%E7%99%BE%E5%BA%A6%E8%B4%B4%E5%90%A7%E7%9B%B8%E5%85%B3"), NULL, NULL, SW_NORMAL);
		break;
	case UPDATE_FAILED_TO_GET_LINK:
		if (AfxMessageBox(_T("��ȡ���ص�ַʧ�ܣ��ֶ����£�"), MB_ICONQUESTION | MB_YESNO) == IDYES)
			ShellExecute(NULL, _T("open"), _T("http://pan.baidu.com/s/1hq86os8#dir/path=%2F%E6%88%91%E7%9A\
%84%E5%88%86%E4%BA%AB%2F%E7%99%BE%E5%BA%A6%E8%B4%B4%E5%90%A7%E7%9B%B8%E5%85%B3"), NULL, NULL, SW_NORMAL);
		break;
	}
}

// ��������
void CAboutPage::OnStnClickedStatic2()
{
	ShellExecute(NULL, _T("open"), _T("http://tieba.baidu.com/i/37897560"), NULL, NULL, SW_NORMAL);
}

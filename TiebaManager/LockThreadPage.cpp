// CLockThreadPage.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LockThreadPage.h"
#include "TiebaManagerDlg.h"
#include <StringHelper.h>
#include <MiscHelper.h>
#include <TiebaClawer.h>
#include "TBMOperate.h"
#include <TBMConfig.h>
#include <Mmsystem.h>


// CLockThreadPage �Ի���

IMPLEMENT_DYNAMIC(CLockThreadPage, CNormalDlg)

CLockThreadPage::CLockThreadPage(CWnd* pParent /*=NULL*/)
	: CNormalDlg(CLockThreadPage::IDD, pParent)
{

}

CLockThreadPage::~CLockThreadPage()
{
}

void CLockThreadPage::DoDataExchange(CDataExchange* pDX)
{
	CNormalDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_tidEdit);
	DDX_Control(pDX, IDC_EDIT2, m_pageEdit);
	DDX_Control(pDX, IDC_EDIT7, m_floorEdit);
	DDX_Control(pDX, IDC_BUTTON1, m_startButton);
	DDX_Control(pDX, IDC_BUTTON2, m_stopButton);
}


BEGIN_MESSAGE_MAP(CLockThreadPage, CNormalDlg)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON1, &CLockThreadPage::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CLockThreadPage::OnBnClickedButton2)
END_MESSAGE_MAP()


// CLockThreadPage ��Ϣ�������

CLockThreadPage* CLockThreadPage::m_instance = NULL;
volatile BOOL CLockThreadPage::m_stopFlag = TRUE;

// ��ʼ��
BOOL CLockThreadPage::OnInitDialog()
{
	CNormalDlg::OnInitDialog();

	m_instance = this;

	if (!m_stopFlag)
	{
		m_tidEdit.EnableWindow(FALSE);
		m_pageEdit.EnableWindow(FALSE);
		m_floorEdit.EnableWindow(FALSE);
		m_startButton.EnableWindow(FALSE);
		m_stopButton.EnableWindow(TRUE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}

// ����
void CLockThreadPage::OnDestroy()
{
	m_instance = NULL;

	CNormalDlg::OnDestroy();
}

// ��ʼ
void CLockThreadPage::OnBnClickedButton1()
{
	m_tidEdit.EnableWindow(FALSE);
	m_pageEdit.EnableWindow(FALSE);
	m_floorEdit.EnableWindow(FALSE);
	m_startButton.EnableWindow(FALSE);
	m_stopButton.EnableWindow(TRUE);
	m_stopFlag = FALSE;
	AfxBeginThread(LockThreadThread, NULL);
}

// ֹͣ
void CLockThreadPage::OnBnClickedButton2()
{
	m_stopFlag = TRUE;
}

UINT AFX_CDECL CLockThreadPage::LockThreadThread(LPVOID)
{
	// ��ʼ��
	CTiebaManagerDlg* mainDlg = (CTiebaManagerDlg*)AfxGetApp()->m_pMainWnd;
	if (!CoInitializeHelper())
		return 0;

	CString tid, page, floor;
	if (m_instance != NULL)
	{
		m_instance->m_tidEdit.GetWindowText(tid);
		m_instance->m_pageEdit.GetWindowText(page);
		m_instance->m_floorEdit.GetWindowText(floor);
		if (_ttoi(page) < 1)
		{
			page = _T("1");
			m_instance->m_pageEdit.SetWindowText(page);
		}
		if (_ttoi(floor) < 1)
		{
			floor = _T("1");
			m_instance->m_floorEdit.SetWindowText(page);
		}
	}
	int iFloor = _ttoi(floor);

	// ����
	while (!m_stopFlag)
	{
		vector<PostInfo> posts;
		GetPosts(tid, _T(""), page, posts);

		for (const PostInfo& post : posts)
			if (_ttoi(post.floor) > iFloor)
			{
				CString code = g_tiebaOperate->DeletePost(tid, post.pid);
				if (code != _T("0"))
				{
					CString content;
					content.Format(_T("������ %s¥<font color=red> ɾ��ʧ�ܣ�������룺%s(%s)</font><a href=\"dl:%s,%s\">����</a>"), 
						(LPCTSTR)post.floor, (LPCTSTR)code, (LPCTSTR)GetTiebaErrorText(code), (LPCTSTR)tid, (LPCTSTR)post.pid);
					mainDlg->m_log.Log(content);
				}
				else
				{
					sndPlaySound(_T("ɾ��.wav"), SND_ASYNC | SND_NODEFAULT);
					mainDlg->m_log.Log(_T("<font color=red>������ɾ�� </font>") + post.floor + _T("¥"));
					for (int i = 0; i < (int)(g_plan.m_deleteInterval * 10); i++)
					{
						if (m_stopFlag)
							break;
						Sleep(100);
					}
				}
			}

		// ɨ����3s
		for (int i = 0; i < 30; i++)
		{
			if (m_stopFlag)
				break;
			Sleep(100);
		}
	}

	CoUninitialize();
	if (m_instance != NULL)
	{
		m_instance->m_tidEdit.EnableWindow(TRUE);
		m_instance->m_pageEdit.EnableWindow(TRUE);
		m_instance->m_floorEdit.EnableWindow(TRUE);
		m_instance->m_startButton.EnableWindow(TRUE);
		m_instance->m_stopButton.EnableWindow(FALSE);
	}
	return 0;
}

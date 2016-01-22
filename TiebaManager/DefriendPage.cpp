// DefriendPage.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DefriendPage.h"
#include "TiebaManagerDlg.h"
#include "StringHelper.h"
#include "NetworkHelper.h"
#include "TiebaVariable.h"
#include "TiebaOperate.h"


// CDefriendPage �Ի���

IMPLEMENT_DYNAMIC(CDefriendPage, CNormalDlg)

CDefriendPage::CDefriendPage(CWnd* pParent /*=NULL*/)
	: CNormalDlg(CDefriendPage::IDD, pParent)
{

}

CDefriendPage::~CDefriendPage()
{
}

void CDefriendPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_startPageEdit);
	DDX_Control(pDX, IDC_EDIT2, m_endPageEdit);
	DDX_Control(pDX, IDC_BUTTON1, m_startButton);
	DDX_Control(pDX, IDC_BUTTON2, m_stopButton);
	DDX_Control(pDX, IDC_STATIC2, m_stateStatic);
}


BEGIN_MESSAGE_MAP(CDefriendPage, CNormalDlg)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON1, &CDefriendPage::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CDefriendPage::OnBnClickedButton2)
END_MESSAGE_MAP()


// CDefriendPage ��Ϣ�������

CDefriendPage* CDefriendPage::m_instance = NULL;
volatile BOOL CDefriendPage::m_stopFlag = TRUE;

// ��ʼ��
BOOL CDefriendPage::OnInitDialog()
{
	CNormalDlg::OnInitDialog();

	m_instance = this;

	if (!m_stopFlag)
	{
		m_startPageEdit.EnableWindow(FALSE);
		m_endPageEdit.EnableWindow(FALSE);
		m_startButton.EnableWindow(FALSE);
		m_stopButton.EnableWindow(TRUE);
		m_stateStatic.SetWindowText(_T("������"));
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}

// ����
void CDefriendPage::OnDestroy()
{
	m_instance = NULL;

	CNormalDlg::OnDestroy();
}

// ��ʼ
void CDefriendPage::OnBnClickedButton1()
{
	m_startPageEdit.EnableWindow(FALSE);
	m_endPageEdit.EnableWindow(FALSE);
	m_startButton.EnableWindow(FALSE);
	m_stopButton.EnableWindow(TRUE);
	m_stopFlag = FALSE;
	AfxBeginThread(DefriendThread, NULL);
}

// ֹͣ
void CDefriendPage::OnBnClickedButton2()
{
	m_stopFlag = TRUE;
}

UINT AFX_CDECL CDefriendPage::DefriendThread(LPVOID)
{
	// ��ʼ��
	CTiebaManagerDlg* mainDlg = (CTiebaManagerDlg*)AfxGetApp()->m_pMainWnd;
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	CString sStartPage, sEndPage;
	if (m_instance != NULL)
	{
		m_instance->m_startPageEdit.GetWindowText(sStartPage);
		m_instance->m_endPageEdit.GetWindowText(sEndPage);
	}
	int startPage = _ttoi(sStartPage), endPage = _ttoi(sEndPage);
	if (startPage > endPage)
	{
		int t = startPage;
		startPage = endPage;
		endPage = t;
	}

	// ��ȡ�����б�
	int index = 0;
	std::vector<CString> userName, userID;
	for (int page = startPage; page <= endPage; page++)
	{
		if (m_stopFlag)
			break;
		CString state;
		state.Format(_T("�ɼ���%dҳ"), page);
		if (m_instance != NULL)
			m_instance->m_stateStatic.SetWindowText(state);
		CString url;
		url.Format(_T("http://tieba.baidu.com/bawu2/platform/listMember?ie=utf-8&word=%s&pn=%d"), g_encodedForumName, page);
		CString src = HTTPGet(url);
		if (m_stopFlag)
			break;

		CStringArray rawUsers;
		SplitString(rawUsers, src, _T("<tr><td class=\"left_cell\">"));
		if (rawUsers.GetSize() < 2)
		{
			if (src != NET_TIMEOUT_TEXT)
				WriteString(src, _T("defriend.txt"));
			else
				page--;
			continue;
		}

		userName.resize((int)userName.size() + (int)rawUsers.GetSize() - 1);
		userID.resize((int)userID.size() + (int)rawUsers.GetSize() - 1);
		CString tmp;
		for (int i = 1; i < rawUsers.GetSize(); i++, index++)
		{
			userName[index] = tmp = GetStringBetween(rawUsers[i], _T("un=\""), _T("\""));
			userID[index] = tmp = GetStringBetween(rawUsers[i], _T("id=\""), _T("\""));
		}
	}

	// ����
	for (UINT i = 0; i < userName.size(); i++)
	{
		if (m_stopFlag)
			break;
		/*if (g_defriendedUser.find(userName[i]) != g_defriendedUser.end())
			continue;*/
		CString state;
		state.Format(_T("�����У�ʣ��%u"), userName.size() - i - 1);
		if (m_instance != NULL)
			m_instance->m_stateStatic.SetWindowText(state);

		CString code = Defriend(userID[i]);
		if (code != _T("0"))
		{
			CString content;
			content.Format(_T("<font color=red>���� </font>%s<font color=red> ʧ�ܣ�������룺%s(%s)</font><a href=")
						   _T("\"df:%s\">����</a>"), userName[i], code, GetTiebaErrorText(code), userID[i]);
			mainDlg->m_log.Log(content);
		}
		else
		{
			g_userCache.m_defriendedUser->insert(userName[i]);
			mainDlg->m_log.Log(_T("<font color=red>���� </font>") + userName[i]);
		}

		if (i % 20 == 19 && i < userName.size() - 1)
		{
			if (m_instance != NULL)
				m_instance->m_stateStatic.SetWindowText(_T("�ӳ���"));
			for (int i = 0; i < 400; i++)
			{
				if (m_stopFlag)
					break;
				Sleep(100);
			}
		}
	}

	m_stopFlag = TRUE;
	CoUninitialize();
	if (m_instance != NULL)
	{
		m_instance->m_startPageEdit.EnableWindow(TRUE);
		m_instance->m_endPageEdit.EnableWindow(TRUE);
		m_instance->m_startButton.EnableWindow(TRUE);
		m_instance->m_stopButton.EnableWindow(FALSE);
		m_instance->m_stateStatic.SetWindowText(_T(""));
	}
	return 0;
}

// LoopBanPage.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LoopBanPage.h"

#include "StringHelper.h"
#include "NetworkHelper.h"
#include "Setting.h"

#include "TiebaCollect.h"
#include "TiebaOperate.h"

#include "SuperFunctionDlg.h"
#include "TiebaManagerDlg.h"


// CLoopBanPage �Ի���

IMPLEMENT_DYNAMIC(CLoopBanPage, CListPage)

CLoopBanPage::CLoopBanPage(CWnd* pParent /*=NULL*/)
	: CListPage(CLoopBanPage::IDD, pParent)
{

}

#pragma region MFC
CLoopBanPage::~CLoopBanPage()
{
}

void CLoopBanPage::DoDataExchange(CDataExchange* pDX)
{
	CListPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK6, m_logCheck);
	DDX_Control(pDX, IDC_CHECK7, m_enableCheck);
}


BEGIN_MESSAGE_MAP(CLoopBanPage, CListPage)
END_MESSAGE_MAP()
#pragma endregion

// CLoopBanPage ��Ϣ�������

// ��ʼ��
BOOL CLoopBanPage::OnInitDialog()
{
	CListPage::OnInitDialog();

	m_resize.AddControl(&m_enableCheck, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list);
	m_resize.AddControl(&m_logCheck, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}

// ���
void CLoopBanPage::OnClickedButton1()
{
	CString text;
	m_edit.GetWindowText(text);
	if (text == _T(""))
	{
		AfxMessageBox(_T("���ݲ���Ϊ�գ�"), MB_ICONERROR);
		return;
	}

	CString pid = GetPIDFromUser(text);
	if (pid == NET_TIMEOUT_TEXT)
		pid = _T("");

	int index = m_list.GetCurSel();
	index = m_list.InsertString(index + 1, text);
	m_pid.insert(m_pid.begin() + index, pid);
	m_list.SetCurSel(index);

	((CSuperFunctionDlg*)GetParent()->GetParent())->m_clearScanCache = TRUE;
}

// ɾ��
void CLoopBanPage::OnClickedButton2()
{
	int index = m_list.GetCurSel();
	if (index == LB_ERR)
		return;
	m_list.DeleteString(index);
	m_pid.erase(m_pid.begin() + index);
	m_list.SetCurSel(index == 0 ? 0 : index - 1);
}

// �޸�
void CLoopBanPage::OnClickedButton3()
{
	CString text;
	m_edit.GetWindowText(text);
	if (text == _T(""))
	{
		AfxMessageBox(_T("���ݲ���Ϊ�գ�"), MB_ICONERROR);
		return;
	}
	int index = m_list.GetCurSel();
	if (index == LB_ERR)
		return;

	CString pid = GetPIDFromUser(text);
	if (pid == NET_TIMEOUT_TEXT)
		pid = _T("");

	m_list.DeleteString(index);
	index = m_list.InsertString(index, text);
	m_pid.insert(m_pid.begin() + index, pid);
	m_list.SetCurSel(index);

	((CSuperFunctionDlg*)GetParent()->GetParent())->m_clearScanCache = TRUE;
}

// ѭ�����߳�
UINT AFX_CDECL LoopBanThread(LPVOID _dlg)
{
	CTiebaManagerDlg* dlg = (CTiebaManagerDlg*)_dlg;

	// һ����ѭ��������ٷ�
	SYSTEMTIME time;
	GetLocalTime(&time);
	CFile file;
	if (file.Open(CURRENT_USER_PATH + _T("\\LoopBanDate.tb"), CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite))
	{
		SYSTEMTIME lastTime;
		file.Read(&lastTime, sizeof(lastTime));
		if (time.wDay == lastTime.wDay && time.wMonth == lastTime.wMonth && time.wYear == lastTime.wYear)
			return 0;
	}

	gzFile f = gzopen_w(CURRENT_USER_PATH + _T("\\options2.tb"), "rb");
	if (f == NULL)
		return 0;

	// ͷ��
	char header[2];
	gzread(f, header, sizeof(header));
	if (header[0] != 'T' || header[1] != 'B')
	{
		gzclose(f);
		return 0;
	}

	// ��ȡ����������
	int size;
	gzread(f, &size, sizeof(int)); // ����
	vector<CString> name(size), pid(size);
	for (int i = 0; i < size; i++)
	{
		ReadText(f, name[i]);
		ReadText(f, pid[i]);
	}
	BOOL log = FALSE, enable;
	gzread(f, &log, sizeof(BOOL)); // �����־
	if (gzread(f, &enable, sizeof(BOOL)) != sizeof(BOOL)) // ����
		enable = TRUE;
	gzclose(f);
	if (!enable)
		return 0;

	// ����ʱ��
	if (file.m_hFile != NULL)
	{
		file.SeekToBegin();
		file.Write(&time, sizeof(time));
		file.Close();
	}

	BOOL updatePID = FALSE;
	// ѭ����
	dlg->m_stateStatic.SetWindowText(_T("ѭ�������"));
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	for (int i = 0; i < size; i++)
	{
		CString code;
		if (pid[i] != _T("")) // ������PID���
			code = BanID(name[i], pid[i]);
		if (pid[i] == _T("") || code != _T("0")) // ���Բ���PID������û�����Ϊ���ɻ�Ա��
		{
			code = BanID(name[i]);
			if (code != _T("0")) // ���Ի�ȡ�µ�PID����PID���
			{
				pid[i] = GetPIDFromUser(name[i]);
				updatePID = TRUE;
				code = BanID(name[i], pid[i]);
			}
		}

		if (log)
		{
			if (code != _T("0"))
			{
				CString content;
				content.Format(_T("<font color=red>��� </font>%s<font color=red> ʧ�ܣ�������룺%s(%s)</font><a href=")
					_T("\"bd:%s,%s\">����</a>"), name[i], code, GetTiebaErrorText(code), pid[i], name[i]);
				dlg->m_log.Log(content);
			}
			else
				dlg->m_log.Log(_T("<font color=red>��� </font>") + name[i]);
		}

		if (code == _T("0") && i < size - 1)
			Sleep(3000);
	}
	CoUninitialize();

	// ����PID
	if (updatePID)
	{
		f = gzopen_w(CURRENT_USER_PATH + _T("\\options2.tb"), "wb");
		if (f == NULL)
		{
			dlg->m_stateStatic.SetWindowText(_T("������"));
			return 0;
		}

		// ͷ��
		gzwrite(f, "TB", 2);

		// ѭ����
		int size;
		gzwrite(f, &(size = (int)name.size()), sizeof(int)); // ����
		for (int i = 0; i < size; i++)
		{
			WriteText(f, name[i]);
			WriteText(f, pid[i]);
		}

		gzwrite(f, &log, sizeof(BOOL));		// �����־
		gzwrite(f, &enable, sizeof(BOOL));	// ����

		gzclose(f);
	}

	dlg->m_stateStatic.SetWindowText(_T("������"));
	return 0;
}

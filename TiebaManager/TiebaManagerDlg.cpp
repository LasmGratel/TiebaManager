/*
Copyright (C) 2015  xfgryujk
http://tieba.baidu.com/f?kw=%D2%BB%B8%F6%BC%AB%C6%E4%D2%FE%C3%D8%D6%BB%D3%D0xfgryujk%D6%AA%B5%C0%B5%C4%B5%D8%B7%BD

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "stdafx.h"
#include "TiebaManagerDlg.h"

#include "SettingDlg.h"
#include "ExplorerDlg.h"
#include "SuperFunctionDlg.h"
#include "LoopBanPage.h"

#include <TBMConfig.h>
#include "ConfigHelper.h"
#include <StringHelper.h>
#include <NetworkHelper.h>
#include <MiscHelper.h>
#include "Update.h"

#include "TiebaVariable.h"
#include <TiebaClawer.h>
#include "TiebaScan.h"
#include "TBMOperate.h"

#include "ScanImage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����
static const UINT WM_TASKBARCREATED = RegisterWindowMessage(_T("TaskbarCreated"));
static const UINT WM_TRAY = WM_APP + 1;


// ���캯��
CTiebaManagerDlg::CTiebaManagerDlg(CWnd* pParent /*=NULL*/)
	: CNormalDlg(CTiebaManagerDlg::IDD, pParent), 
	m_log(m_logExplorer)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_clearLogStatic.m_normalColor = m_saveLogStatic.m_normalColor = RGB(128, 128, 128);
	m_clearLogStatic.m_hoverColor = m_saveLogStatic.m_hoverColor = RGB(192, 192, 192);

	m_explorerDlg = NULL;
	m_settingDlg = NULL;
	m_superFunctionDlg = NULL;

	// ��ʼ������ͼ������
	m_nfData.cbSize = sizeof(NOTIFYICONDATA);
	m_nfData.hIcon = AfxGetApp()->LoadIcon(IDI_TRAY);	
	_tcscpy_s(m_nfData.szTip, _T("���ɹ�����"));
	m_nfData.uCallbackMessage = WM_TRAY;
	m_nfData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
}

#pragma region MFC
void CTiebaManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CNormalDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EXPLORER1, m_logExplorer);
	DDX_Control(pDX, IDC_EDIT1, m_forumNameEdit);
	DDX_Control(pDX, IDC_BUTTON1, m_confirmButton);
	DDX_Control(pDX, IDC_BUTTON2, m_startButton);
	DDX_Control(pDX, IDC_BUTTON3, m_stopButton);
	DDX_Control(pDX, IDC_EDIT2, m_pageEdit);
	DDX_Control(pDX, IDC_BUTTON4, m_superFunctionButton);
	DDX_Control(pDX, IDC_BUTTON5, m_settingButton);
	DDX_Control(pDX, IDC_STATIC4, m_logStatic);
	DDX_Control(pDX, IDC_STATIC5, m_stateStatic);
	DDX_Control(pDX, IDC_STATIC6, m_clearLogStatic);
	DDX_Control(pDX, IDC_STATIC7, m_saveLogStatic);
	DDX_Control(pDX, IDC_BUTTON7, m_explorerButton);
	DDX_Control(pDX, IDC_LIST2, m_stateList);
}

BEGIN_MESSAGE_MAP(CTiebaManagerDlg, CNormalDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_GETMINMAXINFO()
	ON_WM_SYSCOMMAND()
	ON_MESSAGE(WM_TRAY, OnTray)
	ON_REGISTERED_MESSAGE(WM_TASKBARCREATED, OnTaskBarCreated)
	ON_BN_CLICKED(IDC_BUTTON1, &CTiebaManagerDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CTiebaManagerDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CTiebaManagerDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CTiebaManagerDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CTiebaManagerDlg::OnBnClickedButton5)
	ON_STN_CLICKED(IDC_STATIC6, &CTiebaManagerDlg::OnStnClickedStatic6)
	ON_STN_CLICKED(IDC_STATIC7, &CTiebaManagerDlg::OnStnClickedStatic7)
	ON_BN_CLICKED(IDC_BUTTON7, &CTiebaManagerDlg::OnBnClickedButton7)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CTiebaManagerDlg, CNormalDlg)
	ON_EVENT(CTiebaManagerDlg, IDC_EXPLORER1, 250, CTiebaManagerDlg::BeforeNavigate2Explorer1, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
END_EVENTSINK_MAP()


// CTiebaManagerDlg ��Ϣ�������


// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CTiebaManagerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CNormalDlg::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CTiebaManagerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
#pragma endregion

// ��ʼ��
BOOL CTiebaManagerDlg::OnInitDialog()
{
	CNormalDlg::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	m_resize.AddControl(&m_explorerButton, RT_KEEP_DIST_TO_RIGHT, this);
	m_resize.AddControl(&m_superFunctionButton, RT_KEEP_DIST_TO_RIGHT, this);
	m_resize.AddControl(&m_settingButton, RT_KEEP_DIST_TO_RIGHT, this);
	m_resize.AddControl(&m_logStatic, RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, this);
	m_resize.AddControl(&m_logExplorer, RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_RIGHT, &m_logStatic, RT_KEEP_DIST_TO_BOTTOM, &m_logStatic);
	m_resize.AddControl(&m_stateStatic, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_logStatic, RT_KEEP_DIST_TO_RIGHT, this, RT_NULL, NULL);
	m_resize.AddControl(&m_saveLogStatic, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, &m_logStatic, RT_NULL, this, RT_NULL, NULL);
	m_resize.AddControl(&m_clearLogStatic, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, &m_logStatic, RT_NULL, this, RT_NULL, NULL);
	m_resize.AddControl(&m_stateList, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, this, RT_KEEP_DIST_TO_RIGHT, this);

	m_pageEdit.SetWindowText(_T("1"));

	// ��ʼ�����̴��ھ��
	m_nfData.hWnd = m_hWnd;

	// ��ʼ����־
	m_log.Init();

	// ��ȡ����
	g_globalConfig.Load(GLOBAL_CONFIG_PATH);
	SetCurrentUser(g_globalConfig.m_currentUser, FALSE);
	
	// �Զ�����
	if (g_globalConfig.m_autoUpdate)
		AfxBeginThread(AutoUpdateThread, this);

	// ���������ȿ�����
	if (g_globalConfig.m_firstRun)
	{
		*g_globalConfig.m_firstRun = FALSE;
		*g_globalConfig.m_firstRunAfterUpdate = FALSE;
		g_globalConfig.Save(GLOBAL_CONFIG_PATH);
		m_settingDlg = new CSettingDlg();
		m_settingDlg->Create(m_settingDlg->IDD, this);
		m_settingDlg->m_tab.SetCurSel(SETTING_DLG_PAGE_COUNT - 1);
		LRESULT tmp;
		m_settingDlg->OnTcnSelchangeTab1(NULL, &tmp);
	}
	else if (g_globalConfig.m_firstRunAfterUpdate) // ����������־
	{
		*g_globalConfig.m_firstRunAfterUpdate = FALSE;
		g_globalConfig.Save(GLOBAL_CONFIG_PATH);
		AfxMessageBox(UPDATE_LOG, MB_ICONINFORMATION);
	}


	// ÿ24Сʱ����ѷ���������ʼѭ����
	SetTimer(0, 24 * 60 * 60 * 1000, [](HWND, UINT, UINT_PTR, DWORD)
		{
			g_userCache.m_bannedUser->clear();
			AfxBeginThread(LoopBanThread, (CTiebaManagerDlg*)AfxGetApp()->m_pMainWnd);
		});

	// ÿ30�������ͼƬ����
	SetTimer(1, 30 * 60 * 1000, [](HWND, UINT, UINT_PTR, DWORD)
		{
			CFileFind fileFind;
			BOOL flag = fileFind.FindFile(IMG_CACHE_PATH + _T("*"));
			while (flag)
			{
				flag = fileFind.FindNextFile();
				DeleteFile(fileFind.GetFilePath());
			}
		});


	// ����
	/*vector<ThreadInfo> threads;
	GetThreads(_T("һ����������ֻ��xfgryujk֪���ĵط�"), _T("0"), threads);
	g_userTiebaInfo.m_forumID = _T("309740");
	vector<PostInfo> posts, lzls;
	GetPosts(_T("3033489261"), _T(""), _T("1"), posts, lzls);*/


	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// ���洢���ڴ��ڵ�����
void CTiebaManagerDlg::OnClose()
{
	if (g_plan.m_autoSaveLog)
		m_log.Save(_T("Log"));
	m_log.Release();

	CNormalDlg::OnClose();
}

// �����������ݡ��ͷ�
void CTiebaManagerDlg::OnDestroy()
{
	CNormalDlg::OnDestroy();

	SaveCurrentUserConfig();
	g_globalConfig.Save(GLOBAL_CONFIG_PATH);
	g_plan.Save(OPTIONS_PATH + g_userConfig.m_plan + _T(".xml"));

	g_stopScanFlag = TRUE; // ʵ�����̲߳��᷵�أ�����ǰ�ͱ����ˣ���

	g_plan.m_images.clear(); // ��֪��Ϊʲô�������Release��رպ�����...

	// �������ڴ�й©�������Ҳ�����...
}

#pragma region UI
// ���� /////////////////////////////////////////////////////////////////////////////////

// ������С�ߴ�
void CTiebaManagerDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	/*lpMMI->ptMinTrackSize.x = 705;
	lpMMI->ptMinTrackSize.y = 293;*/

	CNormalDlg::OnGetMinMaxInfo(lpMMI);
}

// ���� /////////////////////////////////////////////////////////////////////////////////

// ��С��
void CTiebaManagerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (nID == SC_MINIMIZE) // ��С��ʱ���ش���
	{
		ShowWindow(SW_HIDE);
		Shell_NotifyIcon(NIM_ADD, &m_nfData);
		return;
	}

	CNormalDlg::OnSysCommand(nID, lParam);
}

// ������Ϣ
LRESULT CTiebaManagerDlg::OnTray(WPARAM wParam, LPARAM lParam)
{
	if (lParam == WM_LBUTTONDOWN) // ������ʾ����
	{
		Shell_NotifyIcon(NIM_DELETE, &m_nfData);
		ShowWindow(SW_SHOW);
	}

	return 0;
}

// �������ؽ�
LRESULT CTiebaManagerDlg::OnTaskBarCreated(WPARAM wParam, LPARAM lParam)
{
	if (!IsWindowVisible())
		Shell_NotifyIcon(NIM_ADD, &m_nfData);
	return 0;
}
#pragma endregion

#pragma region ��־
// ��־�����������
void CTiebaManagerDlg::BeforeNavigate2Explorer1(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers, BOOL* Cancel)
{
	CString url = _bstr_t(URL->bstrVal);
	if (url == _T("about:blank"))
		return;
	*Cancel = TRUE;

	CString prefix = url.Left(3);
	if (prefix == _T("dt:")) // ɾ����
	{
		CString code = g_tiebaOperate->DeleteThread(url.Right(url.GetLength() - 3));
		if (code == _T("0"))
			m_log.Log(_T("<font color=green>ɾ���ɹ���</font>"));
		else
			m_log.Log(_T("<font color=red>ɾ��ʧ�ܣ�</font>"));
	}
	else if (prefix == _T("dp:")) // ɾ����
	{
		CStringArray args;
		SplitString(args, url.Right(url.GetLength() - 3), _T(","));
		CString code = g_tiebaOperate->DeletePost(args[0], args[1]);
		if (code == _T("0"))
			m_log.Log(_T("<font color=green>ɾ���ɹ���</font>"));
		else
			m_log.Log(_T("<font color=red>ɾ��ʧ�ܣ�</font>"));
	}
	else if (prefix == _T("dl:")) // ɾ¥��¥
	{
		CStringArray args;
		SplitString(args, url.Right(url.GetLength() - 3), _T(","));
		CString code = g_tiebaOperate->DeleteLZL(args[0], args[1]);
		if (code == _T("0"))
			m_log.Log(_T("<font color=green>ɾ���ɹ���</font>"));
		else
			m_log.Log(_T("<font color=red>ɾ��ʧ�ܣ�</font>"));
	}
	else if (prefix == _T("bd:")) // ��ID
	{
		CStringArray args;
		SplitString(args, url.Right(url.GetLength() - 3), _T(","));
		CString code = (g_plan.m_wapBanInterface /*|| g_plan.m_banDuration == 1*/ || args[1] == _T("")) ? 
			g_tiebaOperate->BanIDClient(args[0]) : g_tiebaOperate->BanID(args[0], args[1]);
		if (code == _T("0"))
			m_log.Log(_T("<font color=green>����ɹ���</font>"));
		else
			m_log.Log(_T("<font color=red>���ʧ�ܣ�</font>"));
	}
	else if (prefix == _T("df:")) // ����
	{
		CString code = g_tiebaOperate->Defriend(url.Right(url.GetLength() - 3));
		if (code == _T("0"))
			m_log.Log(_T("<font color=green>���ڳɹ���</font>"));
		else
			m_log.Log(_T("<font color=red>����ʧ�ܣ�</font>"));
	}
	else
		ShellExecute(NULL, _T("open"), url, NULL, NULL, SW_NORMAL);
}

// �����־
void CTiebaManagerDlg::OnStnClickedStatic6()
{
	m_log.Clear();
}

// ������־
void CTiebaManagerDlg::OnStnClickedStatic7()
{
	m_saveLogStatic.EnableWindow(FALSE);
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(bi));
	bi.lpszTitle = _T("Ҫ�����Ŀ¼��");
	bi.ulFlags = BIF_STATUSTEXT;
	LPITEMIDLIST pidlSel = SHBrowseForFolder(&bi);

	if (pidlSel != NULL)
	{
		CString folder;
		SHGetPathFromIDList(pidlSel, folder.GetBuffer(MAX_PATH));
		folder.ReleaseBuffer();
		if (!CoInitializeHelper())
			return;

		m_log.Save(folder);

		CoUninitialize();
	}

	m_saveLogStatic.EnableWindow(TRUE);
}
#pragma endregion

#pragma region �������Ի���
// �鿴����
void CTiebaManagerDlg::OnBnClickedButton7()
{
	if (m_explorerDlg == NULL)
	{
		m_explorerDlg = new CExplorerDlg();
		m_explorerDlg->Create(m_explorerDlg->IDD, GetDesktopWindow());
	}
}

// ��������
void CTiebaManagerDlg::OnBnClickedButton4()
{
	if (m_superFunctionDlg == NULL)
	{
		m_superFunctionDlg = new CSuperFunctionDlg();
		m_superFunctionDlg->Create(m_superFunctionDlg->IDD, this);
	}
}

// ����
void CTiebaManagerDlg::OnBnClickedButton5()
{
	if (m_settingDlg == NULL)
	{
		m_settingDlg = new CSettingDlg();
		m_settingDlg->Create(m_settingDlg->IDD, this);
	}
}
#pragma endregion

// ɨ�� /////////////////////////////////////////////////////////////////////////////////

// ȷ��
void CTiebaManagerDlg::OnBnClickedButton1()
{
	CString forumName;
	m_forumNameEdit.GetWindowText(forumName);
	if (forumName == _T(""))
	{
		AfxMessageBox(_T("��������������"), MB_ICONERROR);
		return;
	}

	m_forumNameEdit.EnableWindow(FALSE);
	m_confirmButton.EnableWindow(FALSE);
	m_stateStatic.SetWindowText(_T("��֤������"));


	g_tiebaOperate.reset(new CTiebaOperate(g_cookieConfig.m_cookie, g_plan.m_banDuration, g_plan.m_banReason));
	switch (g_tiebaOperate->SetTieba(forumName))
	{
	case CTiebaOperate::SET_TIEBA_TIMEOUT:
		AfxMessageBox(_T("���ӳ�ʱ..."), MB_ICONERROR);
		goto Error;

	case CTiebaOperate::SET_TIEBA_NOT_FOUND:
		AfxMessageBox(_T("���ɲ����ڣ�(Ҳ�����Ƕ������...)"), MB_ICONERROR);
		goto Error;

	case CTiebaOperate::SET_TIEBA_NOT_LOGIN:
		AfxMessageBox(_T("��������-�˺Ź����¼�ٶ��˺�"), MB_ICONERROR);
		goto Error;

	case CTiebaOperate::SET_TIEBA_NO_POWER:
		AfxMessageBox(_T("�����ǰ�����С����������С�࣬�޷�ɾ����ţ�"), MB_ICONWARNING);
		break;

	case CTiebaOperate::SET_TIEBA_NO_TBS:
		AfxMessageBox(_T("��ȡ�����ʧ�ܣ�"), MB_ICONERROR);
		goto Error;
	}

	SetWindowText(_T("���ɹ����� - ") + g_tiebaOperate->GetUserName_());

	// ���������û�
	g_plan.m_whiteList->insert(g_tiebaOperate->GetUserName_());


	m_stateStatic.SetWindowText(_T("������"));
	m_startButton.EnableWindow(TRUE);
	m_pageEdit.EnableWindow(TRUE);
	m_explorerButton.EnableWindow(TRUE);
	m_superFunctionButton.EnableWindow(TRUE);
	*g_userConfig.m_forumName = g_tiebaOperate->GetForumName();
	g_userConfig.Save(USER_PROFILE_PATH);
	m_log.Log(_T("<font color=green>ȷ�ϼ�����ɣ�</font>") + g_tiebaOperate->GetForumName()
		+ _T("<font color=green> �ɣ�ʹ���˺ţ�</font>" + g_tiebaOperate->GetUserName_()));
	// ��ʼѭ����
	AfxBeginThread(LoopBanThread, this);

	return;

Error:
	m_stateStatic.SetWindowText(_T("������"));
	m_forumNameEdit.EnableWindow(TRUE);
	m_confirmButton.EnableWindow(TRUE);
}

// ��ʼ
void CTiebaManagerDlg::OnBnClickedButton2()
{
	if (g_plan.m_keywords->empty() && g_plan.m_images.empty() && g_plan.m_blackList->empty() && g_plan.m_illegalLevel <= 0)
	{
		AfxMessageBox(_T("��������һ��Υ�����"), MB_ICONERROR);
		OnBnClickedButton5();
		return;
	}
	CString tmp;
	m_pageEdit.GetWindowText(tmp);
	if (_ttoi(tmp) < 1)
		m_pageEdit.SetWindowText(_T("1"));
	g_scanThread = AfxBeginThread(ScanThread, this);
}

// ֹͣ
void CTiebaManagerDlg::OnBnClickedButton3()
{
	g_stopScanFlag = TRUE;
}

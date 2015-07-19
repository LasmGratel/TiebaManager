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
#include "TiebaManager.h"
#include "TiebaManagerDlg.h"
#include "afxdialogex.h"
#include "Setting.h"
#include <zlib.h>
#include "Global.h"
#include "Update.h"
#include "Tieba.h"
#include "LoginDlg.h"
#include "ScanImage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����
static const UINT WM_TASKBARCREATED = RegisterWindowMessage(_T("TaskbarCreated"));
static const UINT WM_TRAY = WM_APP + 1;

#pragma region
static const TCHAR LOG_FRAME[] = _T("<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; \
charset=gb2312\" /><title>��־</title><style type=\"text/css\"><!--body {border:1px solid #000000;\
overflow:auto;margin-left:3px;margin-top:3px;margin-right:3px;margin-bottom:3px;font-family:\"����\",Verdana;\
font-size:9pt;line-height:12px}body,td,th{color:#000000}a:link{text-decoration:none}a:hover{text-decoration:\
underline}a:visited{text-decoration:none}--></style></head><body>");
#pragma endregion

WNDPROC CTiebaManagerDlg::s_oldExplorerWndProc;


// ���캯��
CTiebaManagerDlg::CTiebaManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTiebaManagerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_clearLogStatic.m_normalColor = m_saveLogStatic.m_normalColor = RGB(128, 128, 128);
	m_clearLogStatic.m_hoverColor = m_saveLogStatic.m_hoverColor = RGB(192, 192, 192);

	m_explorerDlg = NULL;
	m_settingDlg = NULL;

	// ��ʼ������ͼ������
	m_nfData.cbSize = sizeof(NOTIFYICONDATA);
	m_nfData.hIcon = AfxGetApp()->LoadIcon(IDI_TRAY);	
	_tcscpy_s(m_nfData.szTip, _T("���ɹ�����"));
	m_nfData.uCallbackMessage = WM_TRAY;
	m_nfData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;

	// ��־��ʼʱ��
	GetLocalTime(&m_logStartTime);
}

#pragma region MFC
void CTiebaManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EXPLORER1, m_logExplorer);
	DDX_Control(pDX, IDC_EDIT1, m_forumNameEdit);
	DDX_Control(pDX, IDC_BUTTON1, m_confirmButton);
	DDX_Control(pDX, IDC_BUTTON2, m_startButton);
	DDX_Control(pDX, IDC_BUTTON3, m_stopButton);
	DDX_Control(pDX, IDC_EDIT2, m_pageEdit);
	DDX_Control(pDX, IDC_BUTTON4, m_backStageButton);
	DDX_Control(pDX, IDC_BUTTON5, m_settingButton);
	DDX_Control(pDX, IDC_STATIC4, m_logStatic);
	DDX_Control(pDX, IDC_STATIC5, m_stateStatic);
	DDX_Control(pDX, IDC_STATIC6, m_clearLogStatic);
	DDX_Control(pDX, IDC_STATIC7, m_saveLogStatic);
	DDX_Control(pDX, IDC_BUTTON7, m_explorerButton);
	DDX_Control(pDX, IDC_LIST2, m_stateList);
}

BEGIN_MESSAGE_MAP(CTiebaManagerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
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
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CTiebaManagerDlg, CDialog)
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
		CDialog::OnPaint();
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
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	m_pageEdit.SetWindowText(_T("1"));

	// ��ʼ�����̴��ھ��
	m_nfData.hWnd = m_hWnd;

	// ��ʼ����־
	m_logExplorer.Navigate(_T("about:blank"), NULL, NULL, NULL, NULL);
	do
	{
		Delay(1);
		GetLogDocument(m_logDocument);
	} while (m_logDocument.p == NULL);
	WriteDocument(LOG_FRAME, m_logDocument);
	HWND ExplorerHwnd = m_logExplorer.m_hWnd;
	EnumChildWindows(ExplorerHwnd, EnumChildProc, (LPARAM)&ExplorerHwnd);
	s_oldExplorerWndProc = (WNDPROC)SetWindowLong(ExplorerHwnd, GWL_WNDPROC, (LONG)ExplorerWndProc);

	// ��ȡ����
	TCHAR buffer[260];
	// �˺�
	GetPrivateProfileString(_T("Setting"), _T("UserName"), _T("[NULL]"), buffer, _countof(buffer), ALL_PROFILE_PATH);
	SetCurrentUser(buffer);
	
	// �Զ�����
	g_autoUpdate = GetPrivateProfileInt(_T("Setting"), _T("AutoUpdate"), 1, ALL_PROFILE_PATH) != 0;
	if (g_autoUpdate)
		AfxBeginThread(AutoUpdateThread, this);

	// ���������ȿ�����
	if (GetPrivateProfileInt(_T("Setting"), _T("FirstRun"), 1, ALL_PROFILE_PATH) != 0)
	{
		WritePrivateProfileString(_T("Setting"), _T("FirstRun"), _T("0"), ALL_PROFILE_PATH);
		m_settingDlg = new CSettingDlg();
		m_settingDlg->Create(IDD_SETTING_DIALOG, this);
		m_settingDlg->m_tab.SetCurSel(SETTING_DLG_PAGE_COUNT - 1);
		LRESULT tmp;
		m_settingDlg->OnTcnSelchangeTab1(NULL, &tmp);
	}


	// �Ը��ٶ���ʾ�ظ���Ϊ0��BUG��ʮ�������һ�λظ���¼
	SetTimer(0, 10 * 60 * 1000, [](HWND, UINT, UINT_PTR, DWORD)
		{
			if (!g_briefLog)
				((CTiebaManagerDlg*)AfxGetApp()->m_pMainWnd)->Log(_T("<font color=green>���ɨ���¼</font>"));
			g_reply.clear();
		});


	// ����
	//vector<ThreadInfo> threads;
	//GetThreads(_T("һ����������ֻ��xfgryujk֪���ĵط�"), _T("0"), threads);
	//g_forumID = _T("309740");
	//vector<PostInfo> posts, lzls;
	//GetPosts(_T("3033489261"), _T(""), _T("1"), posts, lzls);


	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �ͷ�
void CTiebaManagerDlg::OnDestroy()
{
	CDialog::OnDestroy();

	SaveCurrentUserProfile();
	WritePrivateProfileString(_T("Setting"), _T("UserName"), g_currentUser, ALL_PROFILE_PATH);

	g_stopScanFlag = TRUE; // ʵ�����̲߳��᷵�أ�

	g_images.clear(); // ��֪��Ϊʲô�������Release��رպ�����...
}

#pragma region UI
// ���� /////////////////////////////////////////////////////////////////////////////////

// ����Esc�رմ���
void CTiebaManagerDlg::OnCancel()
{
}

// ���λس��رմ���
void CTiebaManagerDlg::OnOK()
{
}

// ���ٴ���
void CTiebaManagerDlg::OnClose()
{
	DestroyWindow();

	CDialog::OnClose();
}

// ������־�Ҽ��˵�
LRESULT CALLBACK CTiebaManagerDlg::ExplorerWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONUP)
		return 0;
	return CallWindowProc(s_oldExplorerWndProc, hwnd, uMsg, wParam, lParam);
}

// ������С�ߴ�
void CTiebaManagerDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = 530;
	lpMMI->ptMinTrackSize.y = 238;

	CDialog::OnGetMinMaxInfo(lpMMI);
}

// �ı�ߴ�
void CTiebaManagerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (m_backStageButton.m_hWnd == NULL)
		return;

	CRect rect;
	GetClientRect(&rect); // Ĭ��557 * 486
	m_explorerButton.SetWindowPos(NULL, rect.Width() - 221, 45, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);
	m_backStageButton.SetWindowPos(NULL, rect.Width() - 147, 45, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);
	m_settingButton.SetWindowPos(NULL, rect.Width() - 74, 45, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);
	m_logStatic.SetWindowPos(NULL, 0, 0, rect.Width() - 21, rect.Height() - 207, SWP_NOMOVE | SWP_NOREDRAW);
	m_logExplorer.SetWindowPos(NULL, 0, 0, rect.Width() - 42, rect.Height() - 240, SWP_NOMOVE | SWP_NOREDRAW);
	int y = rect.Height() - 126;
	m_stateStatic.SetWindowPos(NULL, 11, y, rect.Width() - 158, 26, SWP_NOREDRAW);
	m_clearLogStatic.SetWindowPos(NULL, rect.Width() - 137, y, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);
	m_saveLogStatic.SetWindowPos(NULL, rect.Width() - 74, y, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);
	m_stateList.SetWindowPos(NULL, 13, rect.Height() - 90, rect.Width() - 21, 81, SWP_NOREDRAW);

	Invalidate();
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

	CDialog::OnSysCommand(nID, lParam);
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
// ȡ��־document��Ϊ���̰߳�ȫ��Ҫ��m_logExplorer.get_Document()
void CTiebaManagerDlg::GetLogDocument(CComPtr<IHTMLDocument2>& document)
{
	static HWND ExplorerHwnd = NULL;
	if (ExplorerHwnd == NULL)
		EnumChildWindows(m_logExplorer.m_hWnd, EnumChildProc, (LPARAM)&ExplorerHwnd);
	LRESULT lRes;
	UINT nMsg = RegisterWindowMessage(_T("WM_HTML_GETOBJECT"));
	SendMessageTimeout(ExplorerHwnd, nMsg, 0L, 0L, SMTO_ABORTIFHUNG, 1000, (DWORD*)&lRes);
	ObjectFromLresult(lRes, IID_IHTMLDocument2, 0, (void**)&document);
}

// ö��Ѱ��Internet Explorer_Server����
BOOL CALLBACK CTiebaManagerDlg::EnumChildProc(HWND hwnd, LPARAM lParam)
{
	TCHAR buf[30];
	GetClassName(hwnd, buf, _countof(buf));
	if (_tcscmp(buf, _T("Internet Explorer_Server")) == 0)
	{
		*(HWND*)lParam = hwnd;
		return FALSE;
	}
	else
		return TRUE;
}

// дHTML��document
void CTiebaManagerDlg::WriteDocument(const CString& content, CComPtr<IHTMLDocument2>& document)
{
	SAFEARRAY *arr = SafeArrayCreateVector(VT_VARIANT, 0, 1);
	VARIANT *str;
	SafeArrayAccessData(arr, (LPVOID*)&str);
	str->vt = VT_BSTR;
	str->bstrVal = content.AllocSysString();
	SafeArrayUnaccessData(arr);
	document->write(arr);
	SafeArrayDestroy(arr);
}

// �����־
void CTiebaManagerDlg::Log(LPCTSTR content, CComPtr<IHTMLDocument2>* document)
{
	if (document == NULL)
		document = (CComPtr<IHTMLDocument2>*)&(int&)m_logDocument;

	SYSTEMTIME time;
	GetLocalTime(&time);
	CString output;
	output.Format(_T("%02d:%02d:%02d %s<br>"), time.wHour, time.wMinute, time.wSecond, content);
	WriteDocument(output, *document);

	// �������׶�
	CComPtr<IHTMLElement> body;
	(*document)->get_body(&body);
	// ȡID
	static LPOLESTR scrollHeightName = OLESTR("scrollHeight"), scrollTopName = OLESTR("scrollTop");
	static DISPID scrollHeightID = -1, scrollTopID = -1;
	if (scrollHeightID == -1)
		body->GetIDsOfNames(IID_NULL, &scrollHeightName, 1, LOCALE_SYSTEM_DEFAULT, &scrollHeightID);
	if (scrollTopID == -1)
		body->GetIDsOfNames(IID_NULL, &scrollTopName, 1, LOCALE_SYSTEM_DEFAULT, &scrollTopID);
	// body.scrollTop = body.scrollHeight
	DISPPARAMS params = {};
	_variant_t scrollHeight;
	body->Invoke(scrollHeightID, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYGET, &params,
		scrollHeight.GetAddress(), NULL, NULL);
	params.cArgs = 1;
	params.rgvarg = &scrollHeight;
	body->Invoke(scrollTopID, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYPUT, &params, NULL, NULL, NULL);
}

// ��־�����������
void CTiebaManagerDlg::BeforeNavigate2Explorer1(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers, BOOL* Cancel)
{
	CString url = _bstr_t(URL->bstrVal);
	if (url == _T("about:blank"))
		return;
	*Cancel = TRUE;

	CString prefix = url.Left(3);
	if (prefix == _T("DT:")) // ɾ����
	{
		CString code = DeleteThread(url.Right(url.GetLength() - 3));
		if (code == _T("0"))
			Log(_T("<font color=green>ɾ���ɹ���</font>"));
		else
			Log(_T("<font color=red>ɾ��ʧ�ܣ�</font>"));
		return;
	}
	else if (prefix == _T("DP:")) // ɾ����
	{
		CStringArray args;
		SplitString(args, url.Right(url.GetLength() - 3), _T(","));
		CString code = DeletePost(args[0], args[1]);
		if (code == _T("0"))
			Log(_T("<font color=green>ɾ���ɹ���</font>"));
		else
			Log(_T("<font color=red>ɾ��ʧ�ܣ�</font>"));
		return;
	}
	else if (prefix == _T("DL:")) // ɾ¥��¥
	{
		CStringArray args;
		SplitString(args, url.Right(url.GetLength() - 3), _T(","));
		CString code = DeleteLZL(args[0], args[1]);
		if (code == _T("0"))
			Log(_T("<font color=green>ɾ���ɹ���</font>"));
		else
			Log(_T("<font color=red>ɾ��ʧ�ܣ�</font>"));
		return;
	}
	else if (prefix == _T("BD:")) // ��ID
	{
		CStringArray args;
		SplitString(args, url.Right(url.GetLength() - 3), _T(","));
		CString code = BanID(args[1], args[0]);
		if (code == _T("0"))
			Log(_T("<font color=green>����ɹ���</font>"));
		else
			Log(_T("<font color=red>���ʧ�ܣ�</font>"));
		return;
	}

	ShellExecute(NULL, _T("open"), url, NULL, NULL, SW_NORMAL);
}

// �����־
void CTiebaManagerDlg::OnStnClickedStatic6()
{
	m_logDocument->open(NULL, variant_t(), variant_t(), variant_t(), NULL);
	WriteDocument(LOG_FRAME, m_logDocument);
	GetSystemTime(&m_logStartTime);
}

// ������־
void CTiebaManagerDlg::OnStnClickedStatic7()
{
	m_saveLogStatic.EnableWindow(FALSE);
	AfxBeginThread(SaveLogThread, this);
}

// ������־�߳�
UINT AFX_CDECL CTiebaManagerDlg::SaveLogThread(LPVOID _thiz)
{
	CTiebaManagerDlg* thiz = (CTiebaManagerDlg*)_thiz;
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
		CoInitializeEx(NULL, COINIT_MULTITHREADED);

		// ȡ��־HTML
		CComPtr<IHTMLDocument2> document;
		thiz->GetLogDocument(document);
		CComDispatchDriver documentDisp(document);
		// document.documentElement.outerHTML
		_variant_t res;
		documentDisp.GetPropertyByName(OLESTR("documentElement"), res.GetAddress());
		CComDispatchDriver documentElementDisp((IDispatch*)res);
		documentElementDisp.GetPropertyByName(OLESTR("outerHTML"), res.GetAddress());
		CString strHtml = (LPCTSTR)(_bstr_t)res;

		// ��һ��ȡ��ҳHTML������ĩβ���ĸ����룿
		/*CComPtr<IPersistStreamInit> psi;
		document->QueryInterface(&psi);
		HGLOBAL html = GlobalAlloc(GMEM_MOVEABLE, 5 * 1024 * 1024);
		IStream *stream;
		CreateStreamOnHGlobal(html, TRUE, &stream);
		psi->Save(stream, FALSE);
		CString strHtml = (LPCTSTR)GlobalLock(html);
		strHtml += _T("</body></html>");
		GlobalUnlock(html);
		stream->Release();*/

		// ����
		CString path;
		path.Format(_T("%s\\%d-%02d-%02d %02d��%02d��%02d.html"), folder, thiz->m_logStartTime.wYear, 
			thiz->m_logStartTime.wMonth, thiz->m_logStartTime.wDay, thiz->m_logStartTime.wHour, 
			thiz->m_logStartTime.wMinute, thiz->m_logStartTime.wSecond);
		WriteString(strHtml, path);

		CoUninitialize();
	}

	thiz->m_saveLogStatic.EnableWindow(TRUE);
	return 0;
}
#pragma endregion

#pragma region ����
// �Զ������߳�
UINT AFX_CDECL CTiebaManagerDlg::AutoUpdateThread(LPVOID _thiz)
{
	CTiebaManagerDlg* thiz = (CTiebaManagerDlg*)_thiz;
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	CheckUpdateResult res = CheckUpdate();
	CoUninitialize();
	switch (res)
	{
	case UPDATE_FAILED_TO_GET_FILE_ID:
		thiz->m_stateStatic.SetWindowText(_T("������ʧ�ܣ���ȡ�ļ�IDʧ�ܣ����������ֶ�������"));
		break;
	case UPDATE_FAILED_TO_GET_LINK:
		if (AfxMessageBox(_T("��ȡ���ص�ַʧ�ܣ��ֶ����£�"), MB_ICONQUESTION | MB_YESNO) == IDYES)
			ShellExecute(NULL, _T("open"), _T("http://pan.baidu.com/s/1hq86os8#dir/path=%2F%E6%88%91%E7%9A\
%84%E5%88%86%E4%BA%AB%2F%E7%99%BE%E5%BA%A6%E8%B4%B4%E5%90%A7%E7%9B%B8%E5%85%B3"), NULL, NULL, SW_NORMAL);
	case UPDATE_NO_UPDATE:
	case UPDATE_HAS_UPDATE:
		thiz->m_stateStatic.SetWindowText(_T("������"));
		break;
	}

	return 0;
}

// �鿴����
void CTiebaManagerDlg::OnBnClickedButton7()
{
	if (m_explorerDlg == NULL)
	{
		m_explorerDlg = new CExplorerDlg();
		m_explorerDlg->Create(IDD_EXPLORER_DIALOG, GetDesktopWindow());
	}
}

// ��̨
void CTiebaManagerDlg::OnBnClickedButton4()
{
	ShellExecute(NULL, _T("open"), _T("http://tieba.baidu.com/bawu2/platform/listPostLog?word=") + g_forumName, NULL, NULL, SW_NORMAL);
}

// ����
void CTiebaManagerDlg::OnBnClickedButton5()
{
	if (m_settingDlg == NULL)
	{
		m_settingDlg = new CSettingDlg();
		m_settingDlg->Create(IDD_SETTING_DIALOG, this);
	}
}
#pragma endregion

// ɨ�� /////////////////////////////////////////////////////////////////////////////////

// ȷ��
void CTiebaManagerDlg::OnBnClickedButton1()
{
	m_forumNameEdit.GetWindowText(g_forumName);
	if (g_forumName == _T(""))
	{
		AfxMessageBox(_T("��������������"), MB_ICONERROR);
		return;
	}

	m_forumNameEdit.EnableWindow(FALSE);
	m_confirmButton.EnableWindow(FALSE);
	m_stateStatic.SetWindowText(_T("��֤������"));
	CString src, src2, tmp, userName;
	std::wcmatch res;


	src = HTTPGet(_T("http://tieba.baidu.com/f?ie=utf-8&kw=") + EncodeURI(g_forumName));
	if (src == NET_TIMEOUT)
	{
		AfxMessageBox(_T("���ӳ�ʱ..."), MB_ICONERROR);
		goto error;
	}

	// �ɼ�������Ϣ
	tmp = GetStringBetween(src, _T("PageData.forum"), _T("}"));
	tmp.Replace(_T("\r\n"), _T(""));
	if (!std::regex_search((LPCTSTR)tmp, res, FORUM_ID_NAME_REG))
	{
		WriteString(src, _T("forum.txt"));
		AfxMessageBox(_T("���ɲ����ڣ�(Ҳ�����Ƕ������...)"), MB_ICONERROR);
		goto error;
	}

	// ȡ����ID
	g_forumID = res[3].str().c_str();

	// ȡ������
	g_forumName = JSUnescape(res[7].str().c_str());
	g_encodedForumName = EncodeURI(g_forumName);

	// ȡ�û���
	if (std::regex_search((LPCTSTR)(tmp = GetStringBetween(src, _T("PageData.user"), _T("}"))), res, USER_NAME_REG)
		|| std::regex_search((LPCTSTR)(tmp = GetStringBetween(src, _T("PageData"), _T("}"))), res, USER_NAME_REG))
		userName = JSUnescape(res[3].str().c_str());
	if (userName == _T(""))
	{
		WriteString(src, _T("forum.txt"));
		AfxMessageBox(_T("��������-�˺Ź����¼�ٶ��˺�"), MB_ICONERROR);
		goto error;
	}
	SetWindowText(_T("���ɹ����� - ") + userName);

	// ��֤�û�Ȩ��
	// �ɽӿ�
	//src2 = HTTPGet(_T("http://tieba.baidu.com/f/bawu/admin_group?kw=") + EncodeURI_GBK(g_forumName), FALSE);
	src2 = HTTPGet(_T("http://tieba.baidu.com/bawu2/platform/listBawuTeamInfo?word=") + g_encodedForumName + _T("&ie=utf-8"), FALSE);
	if (src2 == NET_TIMEOUT)
	{
		AfxMessageBox(_T("���ӳ�ʱ..."), MB_ICONERROR);
		goto error;
	}
	int pos1 = src2.Find(_T(">����<span"));
	int pos2 = src2.Find(_T(">") + userName + _T("<"));
	int pos3 = src2.Find(_T(">ͼƬС��<span"));
	if (/*pos2 == -1 || */pos2 <= pos1 || (pos3 != -1 && pos2 >= pos3))
	{
		WriteString(src2 + _T("\r\n") + g_cookie, _T("admin.txt"));
		AfxMessageBox(_T("�����ǰ�����С������"), MB_ICONERROR);
		SetWindowText(_T("���ɹ�����"));
		goto error;
	}

	// ���������û�
	BOOL inWhiteList = FALSE;
	for (const CString& i : g_whiteList)
		if (i == userName)
		{
			inWhiteList = TRUE;
			break;
		}
	if (!inWhiteList)
		g_whiteList.push_back(userName);

	// ȡtbs(�����)
	g_tbs = GetStringBetween(src, _TBS_LEFT, _TBS_RIGHT);
	if (g_tbs == _T("") && std::regex_search((LPCTSTR)(tmp = GetStringBetween(src, _T("PageData"), _T("}"))), res, TBS_REG))
		g_tbs = JSUnescape(res[3].str().c_str());
	if (g_tbs == _T(""))
	{
		WriteString(src, _T("forum.txt"));
		AfxMessageBox(_T("��ȡ�����ʧ�ܣ�"), MB_ICONERROR);
		SetWindowText(_T("���ɹ�����"));
		goto error;
	}


	m_stateStatic.SetWindowText(_T("������"));
	m_startButton.EnableWindow(TRUE);
	m_pageEdit.EnableWindow(TRUE);
	m_explorerButton.EnableWindow(TRUE);
	m_backStageButton.EnableWindow(TRUE);
	WritePrivateProfileString(_T("Setting"), _T("ForumName"), g_forumName, USER_PROFILE_PATH);
	Log(_T("<font color=green>ȷ�ϼ�����ɣ�</font>") + g_forumName + _T("<font color=green> �ɣ�ʹ���˺ţ�</font>" + userName));
	return;

error:
	m_stateStatic.SetWindowText(_T("������"));
	m_forumNameEdit.EnableWindow(TRUE);
	m_confirmButton.EnableWindow(TRUE);
}

// ��ʼ
void CTiebaManagerDlg::OnBnClickedButton2()
{
	if (g_keywords.empty() && g_images.empty() && g_blackList.empty())
	{
		AfxMessageBox(_T("��������һ��Υ�����ݻ������û���"), MB_ICONERROR);
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

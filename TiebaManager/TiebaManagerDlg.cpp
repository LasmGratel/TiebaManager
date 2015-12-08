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

#include "Setting.h"
#include "StringHelper.h"
#include "NetworkHelper.h"
#include "MiscHelper.h"
#include "Update.h"

#include "TiebaVariable.h"
#include "TiebaCollect.h"
#include "TiebaScan.h"
#include "TiebaOperate.h"

#include "ScanImage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 常量
static const UINT WM_TASKBARCREATED = RegisterWindowMessage(_T("TaskbarCreated"));
static const UINT WM_TRAY = WM_APP + 1;

#pragma region
static const TCHAR LOG_FRAME[] = _T("<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=gb2312\" /><title>日志")
								 _T("</title><style type=\"text/css\"><!--body {border:1px solid #000000;overflow:auto;margin-left:3px")
								 _T(";margin-top:3px;margin-right:3px;margin-bottom:3px;font-family:\"宋体\",Verdana;font-size:9pt;lin")
								 _T("e-height:12px}body,td,th{color:#000000}a:link{text-decoration:none}a:hover{text-decoration:underl")
								 _T("ine}a:visited{text-decoration:none}--></style></head><body>");
#pragma endregion

WNDPROC CTiebaManagerDlg::s_oldExplorerWndProc;


// 构造函数
CTiebaManagerDlg::CTiebaManagerDlg(CWnd* pParent /*=NULL*/)
	: CNormalDlg(CTiebaManagerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_clearLogStatic.m_normalColor = m_saveLogStatic.m_normalColor = RGB(128, 128, 128);
	m_clearLogStatic.m_hoverColor = m_saveLogStatic.m_hoverColor = RGB(192, 192, 192);

	m_explorerDlg = NULL;
	m_settingDlg = NULL;

	// 初始化托盘图标数据
	m_nfData.cbSize = sizeof(NOTIFYICONDATA);
	m_nfData.hIcon = AfxGetApp()->LoadIcon(IDI_TRAY);	
	_tcscpy_s(m_nfData.szTip, _T("贴吧管理器"));
	m_nfData.uCallbackMessage = WM_TRAY;
	m_nfData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;

	// 日志开始时间
	GetLocalTime(&m_logStartTime);
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


// CTiebaManagerDlg 消息处理程序


// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTiebaManagerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CNormalDlg::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTiebaManagerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
#pragma endregion

// 初始化
BOOL CTiebaManagerDlg::OnInitDialog()
{
	CNormalDlg::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

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

	// 初始化托盘窗口句柄
	m_nfData.hWnd = m_hWnd;

	// 初始化日志
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

	// 读取设置
	TCHAR buffer[260];
	// 账号
	GetPrivateProfileString(_T("Setting"), _T("UserName"), _T("[NULL]"), buffer, _countof(buffer), ALL_PROFILE_PATH);
	SetCurrentUser(buffer);
	
	// 自动更新
	g_autoUpdate = GetPrivateProfileInt(_T("Setting"), _T("AutoUpdate"), 1, ALL_PROFILE_PATH) != 0;
	if (g_autoUpdate)
		AfxBeginThread(AutoUpdateThread, this);

	// 初次运行先看关于
	if (GetPrivateProfileInt(_T("Setting"), _T("FirstRun"), 1, ALL_PROFILE_PATH) != 0)
	{
		WritePrivateProfileString(_T("Setting"), _T("FirstRun"), _T("0"), ALL_PROFILE_PATH);
		m_settingDlg = new CSettingDlg();
		m_settingDlg->Create(IDD_SETTING_DIALOG, this);
		m_settingDlg->m_tab.SetCurSel(SETTING_DLG_PAGE_COUNT - 1);
		LRESULT tmp;
		m_settingDlg->OnTcnSelchangeTab1(NULL, &tmp);
	}


	// 每24小时清除已封名单、开始循环封
	SetTimer(0, 24 * 60 * 60 * 1000, [](HWND, UINT, UINT_PTR, DWORD)
		{
			g_bannedUser.clear();
			AfxBeginThread(LoopBanThread, (CTiebaManagerDlg*)AfxGetApp()->m_pMainWnd);
		});

	// 每30分钟清除图片缓存
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


	// 测试
	/*vector<ThreadInfo> threads;
	GetThreads(_T("一个极其隐秘只有xfgryujk知道的地方"), _T("0"), threads);
	g_forumID = _T("309740");
	vector<PostInfo> posts, lzls;
	GetPosts(_T("3033489261"), _T(""), _T("1"), posts, lzls);*/


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 保存储存在窗口的数据
void CTiebaManagerDlg::OnClose()
{
	if (g_autoSaveLog)
		SaveLog(_T("Log"));

	CNormalDlg::OnClose();
}

// 保存其他数据、释放
void CTiebaManagerDlg::OnDestroy()
{
	CNormalDlg::OnDestroy();

	SaveCurrentUserProfile();
	WritePrivateProfileString(_T("Setting"), _T("UserName"), g_currentUser, ALL_PROFILE_PATH);

	g_stopScanFlag = TRUE; // 实际上线程不会返回（返回前就崩溃了？）

	g_images.clear(); // 不知道为什么不加这个Release版关闭后会崩溃...
}

#pragma region UI
// 窗口 /////////////////////////////////////////////////////////////////////////////////

// 屏蔽日志右键菜单
LRESULT CALLBACK CTiebaManagerDlg::ExplorerWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONUP)
		return 0;
	return CallWindowProc(s_oldExplorerWndProc, hwnd, uMsg, wParam, lParam);
}

// 限制最小尺寸
void CTiebaManagerDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = 705;
	lpMMI->ptMinTrackSize.y = 293;

	CNormalDlg::OnGetMinMaxInfo(lpMMI);
}

// 托盘 /////////////////////////////////////////////////////////////////////////////////

// 最小化
void CTiebaManagerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (nID == SC_MINIMIZE) // 最小化时隐藏窗口
	{
		ShowWindow(SW_HIDE);
		Shell_NotifyIcon(NIM_ADD, &m_nfData);
		return;
	}

	CNormalDlg::OnSysCommand(nID, lParam);
}

// 托盘消息
LRESULT CTiebaManagerDlg::OnTray(WPARAM wParam, LPARAM lParam)
{
	if (lParam == WM_LBUTTONDOWN) // 单击显示窗口
	{
		Shell_NotifyIcon(NIM_DELETE, &m_nfData);
		ShowWindow(SW_SHOW);
	}

	return 0;
}

// 任务栏重建
LRESULT CTiebaManagerDlg::OnTaskBarCreated(WPARAM wParam, LPARAM lParam)
{
	if (!IsWindowVisible())
		Shell_NotifyIcon(NIM_ADD, &m_nfData);
	return 0;
}
#pragma endregion

#pragma region 日志
// 取日志document，为了线程安全不要用m_logExplorer.get_Document()
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

// 枚举寻找Internet Explorer_Server窗口
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

// 写HTML到document
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

// 输出日志
void CTiebaManagerDlg::Log(LPCTSTR content, CComPtr<IHTMLDocument2>* document)
{
	if (document == NULL)
		document = (CComPtr<IHTMLDocument2>*)&(int&)m_logDocument;

	SYSTEMTIME time;
	GetLocalTime(&time);
	CString output;
	output.Format(_T("%02d:%02d:%02d %s<br>"), time.wHour, time.wMinute, time.wSecond, content);
	WriteDocument(output, *document);

	// 滚动到底端
	CComPtr<IHTMLElement> body;
	(*document)->get_body(&body);
	// 取ID
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

// 日志浏览器将导航
void CTiebaManagerDlg::BeforeNavigate2Explorer1(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers, BOOL* Cancel)
{
	CString url = _bstr_t(URL->bstrVal);
	if (url == _T("about:blank"))
		return;
	*Cancel = TRUE;

	CString prefix = url.Left(3);
	if (prefix == _T("dt:")) // 删主题
	{
		CString code = DeleteThread(url.Right(url.GetLength() - 3));
		if (code == _T("0"))
			Log(_T("<font color=green>删除成功！</font>"));
		else
			Log(_T("<font color=red>删除失败！</font>"));
	}
	else if (prefix == _T("dp:")) // 删帖子
	{
		CStringArray args;
		SplitString(args, url.Right(url.GetLength() - 3), _T(","));
		CString code = DeletePost(args[0], args[1]);
		if (code == _T("0"))
			Log(_T("<font color=green>删除成功！</font>"));
		else
			Log(_T("<font color=red>删除失败！</font>"));
	}
	else if (prefix == _T("dl:")) // 删楼中楼
	{
		CStringArray args;
		SplitString(args, url.Right(url.GetLength() - 3), _T(","));
		CString code = DeleteLZL(args[0], args[1]);
		if (code == _T("0"))
			Log(_T("<font color=green>删除成功！</font>"));
		else
			Log(_T("<font color=red>删除失败！</font>"));
	}
	else if (prefix == _T("bd:")) // 封ID
	{
		CStringArray args;
		SplitString(args, url.Right(url.GetLength() - 3), _T(","));
		CString code = BanID(args[1], args[0]);
		if (code == _T("0"))
			Log(_T("<font color=green>封禁成功！</font>"));
		else
			Log(_T("<font color=red>封禁失败！</font>"));
	}
	else if (prefix == _T("df:")) // 拉黑
	{
		CString code = Defriend(url.Right(url.GetLength() - 3));
		if (code == _T("0"))
			Log(_T("<font color=green>拉黑成功！</font>"));
		else
			Log(_T("<font color=red>拉黑失败！</font>"));
	}
	else
		ShellExecute(NULL, _T("open"), url, NULL, NULL, SW_NORMAL);
}

// 清空日志
void CTiebaManagerDlg::OnStnClickedStatic6()
{
	m_logDocument->open(NULL, variant_t(), variant_t(), variant_t(), NULL);
	WriteDocument(LOG_FRAME, m_logDocument);
	GetSystemTime(&m_logStartTime);
}

// 保存日志
void CTiebaManagerDlg::OnStnClickedStatic7()
{
	m_saveLogStatic.EnableWindow(FALSE);
	AfxBeginThread(SaveLogThread, this);
}

// 保存日志线程
UINT AFX_CDECL CTiebaManagerDlg::SaveLogThread(LPVOID _thiz)
{
	CTiebaManagerDlg* thiz = (CTiebaManagerDlg*)_thiz;
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(bi));
	bi.lpszTitle = _T("要保存的目录：");
	bi.ulFlags = BIF_STATUSTEXT;
	LPITEMIDLIST pidlSel = SHBrowseForFolder(&bi);

	if (pidlSel != NULL)
	{
		CString folder;
		SHGetPathFromIDList(pidlSel, folder.GetBuffer(MAX_PATH));
		folder.ReleaseBuffer();
		CoInitializeEx(NULL, COINIT_MULTITHREADED);

		thiz->SaveLog(folder);

		CoUninitialize();
	}

	thiz->m_saveLogStatic.EnableWindow(TRUE);
	return 0;
}

// 保存日志
void CTiebaManagerDlg::SaveLog(LPCTSTR folder)
{
	// 取日志HTML
	CComPtr<IHTMLDocument2> document;
	GetLogDocument(document);
	CComDispatchDriver documentDisp(document);
	// document.documentElement.outerHTML
	_variant_t res;
	documentDisp.GetPropertyByName(OLESTR("documentElement"), res.GetAddress());
	CComDispatchDriver documentElementDisp((IDispatch*)res);
	documentElementDisp.GetPropertyByName(OLESTR("outerHTML"), res.GetAddress());
	CString strHtml = (LPCTSTR)(_bstr_t)res;

	// 另一种取网页HTML方法，末尾有四个乱码？
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

	// 保存
	if (!PathFileExists(folder))
		CreateDirectory(folder, NULL);
	CString path;
	path.Format(_T("%s\\%d-%02d-%02d %02d：%02d：%02d.html"), folder, m_logStartTime.wYear, m_logStartTime.wMonth, 
		m_logStartTime.wDay, m_logStartTime.wHour, m_logStartTime.wMinute, m_logStartTime.wSecond);
	WriteString(strHtml, path);
}
#pragma endregion

#pragma region 杂项
// 自动更新线程
UINT AFX_CDECL CTiebaManagerDlg::AutoUpdateThread(LPVOID _thiz)
{
	CTiebaManagerDlg* thiz = (CTiebaManagerDlg*)_thiz;
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	CheckUpdateResult res = CheckUpdate();
	CoUninitialize();
	switch (res)
	{
	case UPDATE_FAILED_TO_GET_FILE_ID:
		thiz->m_stateStatic.SetWindowText(_T("检查更新失败：获取文件ID失败，在设置里手动检查更新"));
		break;
	case UPDATE_FAILED_TO_GET_LINK:
		if (AfxMessageBox(_T("获取下载地址失败，手动更新？"), MB_ICONQUESTION | MB_YESNO) == IDYES)
		{
			ShellExecute(NULL, _T("open"), _T("http://pan.baidu.com/s/1hq86os8#dir/path=%2F%E6%88%91%E7%9A%84%E5%88")
										   _T("%86%E4%BA%AB%2F%E7%99%BE%E5%BA%A6%E8%B4%B4%E5%90%A7%E7%9B%B8%E5%85%B3"),
						 NULL, NULL, SW_NORMAL);
		}
	case UPDATE_NO_UPDATE:
	case UPDATE_HAS_UPDATE:
		thiz->m_stateStatic.SetWindowText(_T("待机中"));
		break;
	}

	return 0;
}

// 循环封线程
UINT AFX_CDECL CTiebaManagerDlg::LoopBanThread(LPVOID _thiz)
{
	CTiebaManagerDlg* thiz = (CTiebaManagerDlg*)_thiz;

	// 一天内循环封过不再封
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

	// 头部
	char header[2];
	gzread(f, header, sizeof(header));
	if (header[0] != 'T' || header[1] != 'B')
	{
		gzclose(f);
		return 0;
	}

	// 读取名单、设置
	int size;
	gzread(f, &size, sizeof(int)); // 长度
	vector<CString> name(size), pid(size);
	for (int i = 0; i < size; i++)
	{
		ReadText(f, name[i]);
		ReadText(f, pid[i]);
	}
	BOOL log = FALSE, enable;
	gzread(f, &log, sizeof(BOOL)); // 输出日志
	if (gzread(f, &enable, sizeof(BOOL)) != sizeof(BOOL)) // 开启
		enable = TRUE;
	gzclose(f);
	if (!enable)
		return 0;

	// 更新时间
	if (file.m_hFile != NULL)
	{
		file.SeekToBegin();
		file.Write(&time, sizeof(time));
		file.Close();
	}

	BOOL updatePID = FALSE;
	// 循环封
	thiz->m_stateStatic.SetWindowText(_T("循环封禁中"));
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	CComPtr<IHTMLDocument2> document;
	thiz->GetLogDocument(document);
	CComPtr<IHTMLDocument2>* pDocument = (CComPtr<IHTMLDocument2>*)&(int&)document;
	for (int i = 0; i < size; i++)
	{
		CString code;
		if (pid[i] != _T("")) // 尝试用PID封禁
			code = BanID(name[i], pid[i]);
		if (pid[i] == _T("") || code != _T("0")) // 尝试不用PID封禁（用户必须为本吧会员）
		{
			code = BanID(name[i]);
			if (code != _T("0")) // 尝试获取新的PID并用PID封禁
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
				content.Format(_T("<font color=red>封禁 </font>%s<font color=red> 失败！错误代码：%s(%s)</font><a href=")
							   _T("\"bd:%s,%s\">重试</a>"), name[i], code, GetTiebaErrorText(code), pid[i], name[i]);
				thiz->Log(content, pDocument);
			}
			else
				thiz->Log(_T("<font color=red>封禁 </font>") + name[i], pDocument);
		}

		if (code == _T("0") && i < size - 1)
			Sleep(3000);
	}
	CoUninitialize();

	// 更新PID
	if (updatePID)
	{
		f = gzopen_w(CURRENT_USER_PATH + _T("\\options2.tb"), "wb");
		if (f == NULL)
		{
			thiz->m_stateStatic.SetWindowText(_T("待机中"));
			return 0;
		}

		// 头部
		gzwrite(f, "TB", 2);

		// 循环封
		int size;
		gzwrite(f, &(size = (int)name.size()), sizeof(int)); // 长度
		for (int i = 0; i < size; i++)
		{
			WriteText(f, name[i]);
			WriteText(f, pid[i]);
		}

		gzwrite(f, &log, sizeof(BOOL));		// 输出日志
		gzwrite(f, &enable, sizeof(BOOL));	// 开启

		gzclose(f);
	}

	thiz->m_stateStatic.SetWindowText(_T("待机中"));
	return 0;
}

// 查看帖子
void CTiebaManagerDlg::OnBnClickedButton7()
{
	if (m_explorerDlg == NULL)
	{
		m_explorerDlg = new CExplorerDlg();
		m_explorerDlg->Create(IDD_EXPLORER_DIALOG, GetDesktopWindow());
	}
}

// 超级功能
void CTiebaManagerDlg::OnBnClickedButton4()
{
	if (m_superFunctionDlg == NULL)
	{
		m_superFunctionDlg = new CSuperFunctionDlg();
		m_superFunctionDlg->Create(IDD_SETTING_DIALOG, this);
	}
}

// 设置
void CTiebaManagerDlg::OnBnClickedButton5()
{
	if (m_settingDlg == NULL)
	{
		m_settingDlg = new CSettingDlg();
		m_settingDlg->Create(IDD_SETTING_DIALOG, this);
	}
}
#pragma endregion

// 扫描 /////////////////////////////////////////////////////////////////////////////////

// 确定
void CTiebaManagerDlg::OnBnClickedButton1()
{
	m_forumNameEdit.GetWindowText(g_forumName);
	if (g_forumName == _T(""))
	{
		AfxMessageBox(_T("请输入贴吧名！"), MB_ICONERROR);
		return;
	}

	m_forumNameEdit.EnableWindow(FALSE);
	m_confirmButton.EnableWindow(FALSE);
	m_stateStatic.SetWindowText(_T("验证贴吧中"));
	CString src, src2, tmp, userName;
	std::wcmatch res;


	src = HTTPGet(_T("http://tieba.baidu.com/f?ie=utf-8&kw=") + EncodeURI(g_forumName));
	if (src == NET_TIMEOUT_TEXT)
	{
		AfxMessageBox(_T("连接超时..."), MB_ICONERROR);
		goto error;
	}

	// 采集贴吧信息
	tmp = GetStringBetween(src, _T("PageData.forum"), _T("}"));
	tmp.Replace(_T("\r\n"), _T(""));
	if (!std::regex_search((LPCTSTR)tmp, res, FORUM_ID_NAME_REG))
	{
		WriteString(src, _T("forum.txt"));
		AfxMessageBox(_T("贴吧不存在！(也可能是度娘抽了...)"), MB_ICONERROR);
		goto error;
	}

	// 取贴吧ID
	g_forumID = res[3].str().c_str();

	// 取贴吧名
	g_forumName = JSUnescape(res[7].str().c_str());
	g_encodedForumName = EncodeURI(g_forumName);

	// 取用户名
	if (std::regex_search((LPCTSTR)(tmp = GetStringBetween(src, _T("PageData.user"), _T("}"))), res, USER_NAME_REG)
		|| std::regex_search((LPCTSTR)(tmp = GetStringBetween(src, _T("PageData"), _T("}"))), res, USER_NAME_REG))
		userName = JSUnescape(res[3].str().c_str());
	if (userName == _T(""))
	{
		WriteString(src, _T("forum.txt"));
		AfxMessageBox(_T("请在设置-账号管理登录百度账号"), MB_ICONERROR);
		goto error;
	}
	SetWindowText(_T("贴吧管理器 - ") + userName);

	// 验证用户权限
	// 旧接口
	//src2 = HTTPGet(_T("http://tieba.baidu.com/f/bawu/admin_group?kw=") + EncodeURI_GBK(g_forumName), FALSE);
	src2 = HTTPGet(_T("http://tieba.baidu.com/bawu2/platform/listBawuTeamInfo?word=") + g_encodedForumName + _T("&ie=utf-8"), FALSE);
	if (src2 == NET_TIMEOUT_TEXT)
	{
		AfxMessageBox(_T("连接超时..."), MB_ICONERROR);
		goto error;
	}
	int pos1 = src2.Find(_T("吧主<span"));
	int pos2 = src2.Find(_T(">") + userName + _T("<"));
	int pos3 = src2.Find(_T(">图片小编<span"));
	if (/*pos2 == -1 || */pos2 <= pos1 || (pos3 != -1 && pos2 >= pos3))
	{
		WriteString(src2, _T("admin.txt"));
		AfxMessageBox(_T("您不是吧主或小吧主！"), MB_ICONERROR);
		SetWindowText(_T("贴吧管理器"));
		goto error;
	}

	// 加入信任用户
	g_whiteList.insert(userName);

	// 取tbs(口令号)
	g_tbs = GetStringBetween(src, _TBS_LEFT, _TBS_RIGHT);
	if (g_tbs == _T("") && std::regex_search((LPCTSTR)(tmp = GetStringBetween(src, _T("PageData"), _T("}"))), res, TBS_REG))
		g_tbs = JSUnescape(res[3].str().c_str());
	if (g_tbs == _T(""))
	{
		WriteString(src, _T("forum.txt"));
		AfxMessageBox(_T("获取口令号失败！"), MB_ICONERROR);
		SetWindowText(_T("贴吧管理器"));
		goto error;
	}


	m_stateStatic.SetWindowText(_T("待机中"));
	m_startButton.EnableWindow(TRUE);
	m_pageEdit.EnableWindow(TRUE);
	m_explorerButton.EnableWindow(TRUE);
	m_superFunctionButton.EnableWindow(TRUE);
	WritePrivateProfileString(_T("Setting"), _T("ForumName"), g_forumName, USER_PROFILE_PATH);
	Log(_T("<font color=green>确认监控贴吧：</font>") + g_forumName + _T("<font color=green> 吧，使用账号：</font>" + userName));
	// 开始循环封
	AfxBeginThread(LoopBanThread, this);
	return;

error:
	m_stateStatic.SetWindowText(_T("待机中"));
	m_forumNameEdit.EnableWindow(TRUE);
	m_confirmButton.EnableWindow(TRUE);
}

// 开始
void CTiebaManagerDlg::OnBnClickedButton2()
{
	if (g_keywords.empty() && g_images.empty() && g_blackList.empty())
	{
		AfxMessageBox(_T("至少设置一个违规内容或屏蔽用户！"), MB_ICONERROR);
		OnBnClickedButton5();
		return;
	}
	CString tmp;
	m_pageEdit.GetWindowText(tmp);
	if (_ttoi(tmp) < 1)
		m_pageEdit.SetWindowText(_T("1"));
	g_scanThread = AfxBeginThread(ScanThread, this);
}

// 停止
void CTiebaManagerDlg::OnBnClickedButton3()
{
	g_stopScanFlag = TRUE;
}

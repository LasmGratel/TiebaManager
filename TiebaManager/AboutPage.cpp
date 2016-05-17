﻿// AboutPage.cpp : 实现文件
//

#include "stdafx.h"
#include "AboutPage.h"

#include "Update.h"


// CAboutPage 对话框

IMPLEMENT_DYNAMIC(CAboutPage, CNormalDlg)

CAboutPage::CAboutPage(CWnd* pParent /*=NULL*/)
	: CNormalDlg(CAboutPage::IDD, pParent)
{

}

#pragma region MFC
CAboutPage::~CAboutPage()
{
}

void CAboutPage::DoDataExchange(CDataExchange* pDX)
{
	CNormalDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_edit);
	DDX_Control(pDX, IDC_STATIC1, m_checkUpdateStatic);
	DDX_Control(pDX, IDC_CHECK1, m_autoCheckUpdateCheck);
	DDX_Control(pDX, IDC_STATIC2, m_authorStatic);
}


BEGIN_MESSAGE_MAP(CAboutPage, CNormalDlg)
	ON_STN_CLICKED(IDC_STATIC1, &CAboutPage::OnStnClickedStatic1)
	ON_STN_CLICKED(IDC_STATIC2, &CAboutPage::OnStnClickedStatic2)
END_MESSAGE_MAP()
#pragma endregion

// CAboutPage 消息处理程序

// 初始化
BOOL CAboutPage::OnInitDialog()
{
	CNormalDlg::OnInitDialog();

	m_resize.AddControl(&m_edit, RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, this);
	m_resize.AddControl(&m_checkUpdateStatic, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_edit);
	m_resize.AddControl(&m_autoCheckUpdateCheck, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_edit);
	m_resize.AddControl(&m_authorStatic, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, &m_edit);

	m_edit.SetWindowText(_T("软件介绍帖http://tieba.baidu.com/p/3915111330\r\n")
						 _T("\r\n")
						 _T("html代码只转换了\"'&<>还有空格，其他以网页源码为准，如换行符为<br> (不注意的话会不匹配或匹配太多)\r\n")
						 _T("\r\n")
						 _T("我的邮箱和支付宝是xfgryujk@126.com，喜欢此软件可以捐助哦\r\n")
						 _T("\r\n")
						 _T("此程序通过GPL协议开源，你可以在https://github.com/xfgryujk/TiebaManager得到源码，欢迎完善程序\r\n")
						 _T("\r\n")
						 _T("对更新日志感兴趣的话看这里https://github.com/xfgryujk/TiebaManager/commits/master\r\n")
						 _T("\r\n")
						 _T("我只是提供方便吧务管理贴吧的工具，被用来做拆吧等恶意行为造成的损失本人不负责"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

// 检查更新
void CAboutPage::OnStnClickedStatic1()
{
	switch (CheckUpdate())
	{
	case UPDATE_NO_UPDATE:
		AfxMessageBox(_T("已经是最新版本"));
		break;
	case UPDATE_FAILED_TO_GET_FILE_INFO:
		if (AfxMessageBox(_T("获取文件信息失败，手动更新？"), MB_ICONQUESTION | MB_YESNO) == IDYES)
			ShellExecute(NULL, _T("open"), UPDATE_URL, NULL, NULL, SW_NORMAL);
		break;
	}
}

// 关于作者
void CAboutPage::OnStnClickedStatic2()
{
	ShellExecute(NULL, _T("open"), _T("http://tieba.baidu.com/i/37897560"), NULL, NULL, SW_NORMAL);
}

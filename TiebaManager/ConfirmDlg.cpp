﻿// ConfirmDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ConfirmDlg.h"
#include "ImageViewDlg.h"
#include "ScanImage.h"


// CConfirmDlg 对话框

IMPLEMENT_DYNAMIC(CConfirmDlg, CDialog)

CConfirmDlg::CConfirmDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConfirmDlg::IDD, pParent),
	m_resize(this)
{
	m_operation = NULL;
}

CConfirmDlg::CConfirmDlg(const Operation* operation, CWnd* pParent)
	: CDialog(CConfirmDlg::IDD, pParent),
	m_resize(this)
{
	m_operation = operation;
}

#pragma region MFC
CConfirmDlg::~CConfirmDlg()
{
}

void CConfirmDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_contentEdit);
	DDX_Control(pDX, IDC_STATIC1, m_static);
	DDX_Control(pDX, IDOK, m_yesButton);
	DDX_Control(pDX, IDCANCEL, m_noButton);
	DDX_Control(pDX, IDC_BUTTON1, m_explorerButton);
}


BEGIN_MESSAGE_MAP(CConfirmDlg, CDialog)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_BN_CLICKED(IDC_BUTTON1, &CConfirmDlg::OnBnClickedButton1)
END_MESSAGE_MAP()
#pragma endregion

// CConfirmDlg 消息处理程序

#pragma region UI
// 限制最小尺寸
void CConfirmDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	/*lpMMI->ptMinTrackSize.x = 455;
	lpMMI->ptMinTrackSize.y = 362;*/

	CDialog::OnGetMinMaxInfo(lpMMI);
}

// 改变尺寸
void CConfirmDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	m_resize.Resize();
}
#pragma endregion

// 初始化
BOOL CConfirmDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_resize.AddControl(&m_contentEdit, RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, this);
	m_resize.AddControl(&m_static, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_contentEdit);
	m_resize.AddControl(&m_explorerButton, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, &m_contentEdit);
	m_resize.AddControl(&m_yesButton, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, &m_contentEdit);
	m_resize.AddControl(&m_noButton, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, &m_contentEdit);

	if (m_operation != NULL)
	{
		SetWindowText(m_operation->title);
		m_contentEdit.SetWindowText(m_operation->msg + _T("\r\n\r\n作者：") + m_operation->author);
		m_contentEdit.SetSel(m_operation->pos, m_operation->pos + m_operation->length);

		if (m_operation->object != Operation::TBOBJ_LZL)
		{
			unique_ptr<vector<CString> > img(new vector<CString>());
			if (m_operation->object == Operation::TBOBJ_THREAD)
				GetThreadImage(m_operation->msg).GetImage(*img);
			else //if (m_operation->object == TBOBJ_POST)
				GetPostImage(m_operation->msg, m_operation->authorPortrait).GetImage(*img);
			if (!img->empty())
			{
				m_imageViewDlg = new CImageViewDlg(&m_imageViewDlg, this);
				m_imageViewDlg->Create(m_imageViewDlg->IDD, this);
				m_imageViewDlg->SetImages(std::move(img));
			}
		}
	}
	MessageBeep(MB_ICONQUESTION);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

// 浏览器
void CConfirmDlg::OnBnClickedButton1()
{
	if (m_operation == NULL)
		return;

	CString url;
	if (m_operation->object == Operation::TBOBJ_THREAD) // 主题
		url = _T("http://tieba.baidu.com/p/") + m_operation->tid;
	else if (m_operation->object == Operation::TBOBJ_POST) // 帖子
		url.Format(_T("http://tieba.baidu.com/p/%s?pid=%s#%s"), (LPCTSTR)m_operation->tid, (LPCTSTR)m_operation->pid, (LPCTSTR)m_operation->pid);
	else /*if (op.object == TBOBJ_POST)*/ // 楼中楼
		url.Format(_T("http://tieba.baidu.com/p/%s?pid=%s&cid=%s#%s"), (LPCTSTR)m_operation->tid, (LPCTSTR)m_operation->pid, (LPCTSTR)m_operation->pid, (LPCTSTR)m_operation->pid);

	ShellExecute(NULL, _T("open"), url, NULL, NULL, SW_NORMAL);
}

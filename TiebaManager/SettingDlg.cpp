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
#include "SettingDlg.h"
#include "TiebaManagerDlg.h"
#include <TBMConfig.h>
#include "TiebaVariable.h"
#include "ScanImage.h"
#include "MiscHelper.h"


// CSettingDlg �Ի���

IMPLEMENT_DYNAMIC(CSettingDlg, CNormalDlg)

// ���캯��
CSettingDlg::CSettingDlg(CWnd* pParent /*=NULL*/)
	: CNormalDlg(CSettingDlg::IDD, pParent),
	m_pagesResize(&m_tab), 
	m_whiteListPage(_T("�û�����")), 
	m_trustedThreadPage(_T("����ID��"))
{
	// ��ʼ��m_pages
	int i = 0;
	m_pages[i++] = &m_scanPage;
	m_pages[i++] = &m_operatePage;
	m_pages[i++] = &m_keywordsPage;
	m_pages[i++] = &m_imagePage;
	m_pages[i++] = &m_blackListPage;
	m_pages[i++] = &m_whiteListPage;
	m_pages[i++] = &m_whiteContentPage;
	m_pages[i++] = &m_trustedThreadPage;
	m_pages[i++] = &m_optionsPage;
	m_pages[i++] = &m_usersPage;
	m_pages[i++] = &m_aboutPage;
}

#pragma region MFC
CSettingDlg::~CSettingDlg()
{
}

void CSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CNormalDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tab);
	DDX_Control(pDX, IDOK, m_okButton);
	DDX_Control(pDX, IDCANCEL, m_cancelButton);
}


BEGIN_MESSAGE_MAP(CSettingDlg, CNormalDlg)
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CSettingDlg::OnTcnSelchangeTab1)
	ON_WM_CLOSE()
END_MESSAGE_MAP()
#pragma endregion

// CSettingDlg ��Ϣ�������

// ��ʼ��
BOOL CSettingDlg::OnInitDialog()
{
	CNormalDlg::OnInitDialog();

	HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(hIcon, FALSE);			// ����Сͼ��

	// ��ʼ��m_tab
	int i = 0;
	m_tab.InsertItem(i++, _T("ɨ��"));
	m_tab.InsertItem(i++, _T("����"));
	m_tab.InsertItem(i++, _T("Υ������"));
	m_tab.InsertItem(i++, _T("Υ��ͼƬ"));
	m_tab.InsertItem(i++, _T("�����û�"));
	m_tab.InsertItem(i++, _T("�����û�"));
	m_tab.InsertItem(i++, _T("��������"));
	m_tab.InsertItem(i++, _T("��������"));
	m_tab.InsertItem(i++, _T("����"));
	m_tab.InsertItem(i++, _T("�˺Ź���"));
	m_tab.InsertItem(i++, _T("����&&����"));

	// ��ʼ����ҳ
#define CREATE_PAGE(page) page.Create(page.IDD, &m_tab)
	CREATE_PAGE(m_scanPage);
	CREATE_PAGE(m_operatePage);
	CREATE_PAGE(m_keywordsPage);
	CREATE_PAGE(m_imagePage);
	CREATE_PAGE(m_blackListPage);
	CREATE_PAGE(m_whiteListPage);
	m_whiteListPage.m_static.SetWindowText(_T("�������ı������ı�ʱƥ��(������)"));
	CREATE_PAGE(m_whiteContentPage);
	CREATE_PAGE(m_trustedThreadPage);
	m_trustedThreadPage.m_static.SetWindowText(_T("��ӵ����ⲻ��ɨ�裬����ID����ַ��\"p/\"�����������"));
	CREATE_PAGE(m_optionsPage);
	CREATE_PAGE(m_usersPage);
	CREATE_PAGE(m_aboutPage);

	CRect rect;
	m_tab.GetClientRect(&rect);
	rect.left += 1; rect.right -= 3; rect.top += 23; rect.bottom -= 2;
	m_pages[0]->SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_SHOWWINDOW);
	for (i = 1; i < _countof(m_pages); i++)
		m_pages[i]->SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_HIDEWINDOW);

	m_resize.AddControl(&m_tab, RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, this);
	m_resize.AddControl(&m_okButton, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, &m_tab);
	m_resize.AddControl(&m_cancelButton, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, &m_tab);
	for (i = 0; i < _countof(m_pages); i++)
		m_pagesResize.AddControl(m_pages[i], RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_RIGHT, &m_tab, RT_KEEP_DIST_TO_BOTTOM, &m_tab);

	// ��ʾ����
	ShowPlan(g_plan);
	m_clearScanCache = FALSE; // ��m_scanPage.m_scanPageCountEdit.SetWindowText���ʼ��

	m_optionsPage.m_currentOptionStatic.SetWindowText(_T("��ǰ������") + g_userConfig.m_plan); // ��ǰ����
	// ����
	CFileFind fileFind;
	BOOL flag = fileFind.FindFile(OPTIONS_PATH + _T("*.xml"));
	while (flag)
	{
		flag = fileFind.FindNextFile();
		m_optionsPage.m_list.AddString(fileFind.GetFileTitle());
	}

	m_usersPage.m_currentUserStatic.SetWindowText(_T("��ǰ�˺ţ�") + g_globalConfig.m_currentUser); // ��ǰ�˺�
	// �˺�
	m_usersPage.m_list.AddString(_T("[NULL]"));
	flag = fileFind.FindFile(USERS_PATH + _T("*"));
	while (flag)
	{
		flag = fileFind.FindNextFile();
		if (fileFind.IsDirectory() && !fileFind.IsDots() 
			&& (PathFileExists(fileFind.GetFilePath() + _T("\\ck.xml"))))
		{
			CString name = fileFind.GetFileName();
			if (name != _T("[NULL]"))
				m_usersPage.m_list.AddString(name);
		}
	}

	m_aboutPage.m_autoCheckUpdateCheck.SetCheck(g_globalConfig.m_autoUpdate); // �Զ�����

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}

#pragma region UI
// ���� /////////////////////////////////////////////////////////////////////////////////

// ȡ��
void CSettingDlg::OnCancel()
{
	DestroyWindow();
}

// ��ʾ�Ƿ񱣴�
void CSettingDlg::OnClose()
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
void CSettingDlg::PostNcDestroy()
{
	CNormalDlg::PostNcDestroy();

	((CTiebaManagerDlg*)AfxGetApp()->m_pMainWnd)->m_settingDlg = NULL;
	delete this;
}

// ������С�ߴ�
void CSettingDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	/*lpMMI->ptMinTrackSize.x = 666;
	lpMMI->ptMinTrackSize.y = 576;*/

	CNormalDlg::OnGetMinMaxInfo(lpMMI);
}

// �ı�ߴ�
void CSettingDlg::OnSize(UINT nType, int cx, int cy)
{
	CNormalDlg::OnSize(nType, cx, cy);
	m_pagesResize.Resize();
}

// �л���ǩ
void CSettingDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	int index = m_tab.GetCurSel();
	for (int i = 0; i < _countof(m_pages); i++)
		m_pages[i]->ShowWindow(i == index ? SW_SHOW : SW_HIDE);
}
#pragma endregion

// ��ʾ��ǰ����
void CSettingDlg::ShowPlan(const CPlan& plan)
{
	CString tmp;
	tmp.Format(_T("%d"), *plan.m_scanInterval);
	m_scanPage.m_scanIntervalEdit.SetWindowText(tmp);				// ɨ����
	m_operatePage.m_banIDCheck.SetCheck(plan.m_banID);				// ��ID
	m_operatePage.OnBnClickedCheck1();
	m_operatePage.m_banDurationCombo.SetCurSel(plan.m_banDuration == 1 ? 0 : (plan.m_banDuration == 3 ? 1 : 2)); // ���ʱ��
	tmp.Format(_T("%d"), *plan.m_banTrigCount);
	m_operatePage.m_banTrigCountEdit.SetWindowText(tmp);			// ���Υ�����
	m_scanPage.m_onlyScanTitleCheck.SetCheck(plan.m_onlyScanTitle);	// ֻɨ�����
	tmp.Format(_T("%g"), *plan.m_deleteInterval);
	m_operatePage.m_deleteIntervalEdit.SetWindowText(tmp);			// ɾ�����
	m_operatePage.m_confirmCheck.SetCheck(plan.m_confirm);			// ����ǰ��ʾ
	m_operatePage.m_wapBanInterfaceCheck.SetCheck(plan.m_wapBanInterface);	// ���ʹ��WAP�ӿ�
	m_operatePage.m_autoLoopBanCheck.SetCheck(plan.m_autoLoopBan);	// �Զ�ѭ����
	tmp.Format(_T("%d"), *plan.m_scanPageCount);
	m_scanPage.m_scanPageCountEdit.SetWindowText(tmp);				// ɨ�����ҳ��
	m_scanPage.m_briefLogCheck.SetCheck(plan.m_briefLog);			// ֻ���ɾ�����
	m_operatePage.m_deleteCheck.SetCheck(plan.m_delete);			// ɾ��
	tmp.Format(_T("%d"), *plan.m_threadCount);
	m_scanPage.m_threadCountEdit.SetWindowText(tmp);				// �߳���
	m_operatePage.m_banReasonEdit.SetWindowText(*plan.m_banReason);	// ���ԭ��
	m_imagePage.m_dirEdit.SetWindowText(*plan.m_imageDir);			// Υ��ͼƬĿ¼
	tmp.Format(_T("%g"), *plan.m_SSIMThreshold);
	m_imagePage.m_thresholdEdit.SetWindowText(tmp);					// ��ֵ
	m_operatePage.m_defriendCheck.SetCheck(plan.m_defriend);		// ����
	m_operatePage.OnBnClickedCheck3();
	tmp.Format(_T("%d"), *plan.m_defriendTrigCount);
	m_operatePage.m_defriendTrigCountEdit.SetWindowText(tmp);		// ����Υ�����
	m_scanPage.m_autoSaveLogCheck.SetCheck(plan.m_autoSaveLog);		// �Զ�������־
	tmp.Format(_T("%d"), *plan.m_illegalLevel);
	m_scanPage.m_illegalLevelEdit.SetWindowText(tmp);				// Υ��ȼ�

	// Υ������
	m_keywordsPage.ShowList(plan.m_keywords);

	// �����û�
	m_blackListPage.ShowList(plan.m_blackList);

	// �����û�
	m_whiteListPage.ShowList(plan.m_whiteList);

	// ��������
	m_whiteContentPage.ShowList(plan.m_whiteContent);

	// ��������
	m_trustedThreadPage.ShowList(plan.m_trustedThread);
}

// Ӧ�öԻ����е�����
void CSettingDlg::ApplyPlanInDlg(CPlan& plan)
{
	CString strBuf;
	int intBuf;
	plan.OnChange();

	m_scanPage.m_scanIntervalEdit.GetWindowText(strBuf);
	*plan.m_scanInterval = _ttoi(strBuf);								// ɨ����
	*plan.m_banID = m_operatePage.m_banIDCheck.GetCheck();				// ��ID
	intBuf = m_operatePage.m_banDurationCombo.GetCurSel();
	*plan.m_banDuration = intBuf == 0 ? 1 : (intBuf == 1 ? 3 : 10);		// ���ʱ��
	m_operatePage.m_banTrigCountEdit.GetWindowText(strBuf);
	*plan.m_banTrigCount = _ttoi(strBuf);								// ���Υ�����
	*plan.m_onlyScanTitle = m_scanPage.m_onlyScanTitleCheck.GetCheck(); // ֻɨ�����
	m_operatePage.m_deleteIntervalEdit.GetWindowText(strBuf);
	*plan.m_deleteInterval = (float)_ttof(strBuf);						// ɾ�����
	*plan.m_confirm = m_operatePage.m_confirmCheck.GetCheck();			// ����ǰ��ʾ
	*plan.m_wapBanInterface = m_operatePage.m_wapBanInterfaceCheck.GetCheck();	// ���ʹ��WAP�ӿ�
	*plan.m_autoLoopBan = m_operatePage.m_autoLoopBanCheck.GetCheck();	// �Զ�ѭ����
	m_scanPage.m_scanPageCountEdit.GetWindowText(strBuf);
	*plan.m_scanPageCount = _ttoi(strBuf);								// ɨ�����ҳ��
	*plan.m_briefLog = m_scanPage.m_briefLogCheck.GetCheck();			// ֻ���ɾ�����
	*plan.m_delete = m_operatePage.m_deleteCheck.GetCheck();			// ɾ��
	m_scanPage.m_threadCountEdit.GetWindowText(strBuf);
	*plan.m_threadCount = _ttoi(strBuf);								// �߳���
	m_operatePage.m_banReasonEdit.GetWindowText(strBuf);
	*plan.m_banReason = strBuf;											// ���ԭ��
	m_imagePage.m_dirEdit.GetWindowText(plan.m_imageDir);				// Υ��ͼƬĿ¼
	m_imagePage.m_thresholdEdit.GetWindowText(strBuf);
	*plan.m_SSIMThreshold = _ttof(strBuf);								// ��ֵ
	*plan.m_defriend = m_operatePage.m_defriendCheck.GetCheck();		// ����
	m_operatePage.m_defriendTrigCountEdit.GetWindowText(strBuf);
	*plan.m_defriendTrigCount = _ttoi(strBuf);							// ����Υ�����
	*plan.m_autoSaveLog = m_scanPage.m_autoSaveLogCheck.GetCheck();		// �Զ�������־
	m_scanPage.m_illegalLevelEdit.GetWindowText(strBuf);
	*plan.m_illegalLevel = _ttoi(strBuf);								// Υ��ȼ�

	// Υ������
	m_keywordsPage.ApplyList(plan.m_keywords);

	// �����û�
	m_blackListPage.ApplyList(plan.m_blackList);

	// �����û�
	m_whiteListPage.ApplyList(plan.m_whiteList);

	// ��������
	m_whiteContentPage.ApplyList(plan.m_whiteContent);

	// ��������
	m_trustedThreadPage.ApplyList(plan.m_trustedThread);

	// Υ��ͼƬ
	BOOL updateImage = &plan == &g_plan && plan.m_updateImage;
	if (updateImage)
	{
		g_leagalImage.clear();
		g_illegalImage.clear();
	}

	plan.PostChange();

	if (updateImage)
	{
		strBuf.Format(_T("������%d��ͼƬ"), plan.m_images.size());
		AfxMessageBox(strBuf, MB_ICONINFORMATION);
	}

	if (&plan == &g_plan && m_clearScanCache)
	{
		if (!plan.m_briefLog)
			((CTiebaManagerDlg*)AfxGetApp()->m_pMainWnd)->m_log.Log(_T("<font color=green>�����ʷ�ظ�</font>"));
		g_userCache.m_reply->clear();
	}
}

// ��ʾ�ļ��е�����
void CSettingDlg::ShowPlanInFile(const CString& path)
{
	CPlan tmp;
	tmp.Load(path);
	g_plan.m_updateImage = TRUE;
	ShowPlan(tmp);
}

// �ѶԻ����е�����д���ļ�
void CSettingDlg::SavePlanInDlg(const CString& path)
{
	CPlan tmp;
	ApplyPlanInDlg(tmp);
	tmp.Save(path);
}

// ȷ��
void CSettingDlg::OnOK()
{
	*g_globalConfig.m_autoUpdate = m_aboutPage.m_autoCheckUpdateCheck.GetCheck();
	g_globalConfig.Save(GLOBAL_CONFIG_PATH);

	CString tmp;
	m_optionsPage.m_currentOptionStatic.GetWindowText(tmp);
	*g_userConfig.m_plan = tmp.Right(tmp.GetLength() - 5); // "��ǰ������"
	g_userConfig.Save(USER_PROFILE_PATH);

	CreateDir(OPTIONS_PATH);
	SavePlanInDlg(OPTIONS_PATH + g_userConfig.m_plan + _T(".xml"));
	ApplyPlanInDlg(g_plan);

	DestroyWindow();
}

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
#include "Setting.h"
#include "TiebaVariable.h"
#include "ScanImage.h"
#include "MiscHelper.h"


// CSettingDlg �Ի���

IMPLEMENT_DYNAMIC(CSettingDlg, CNormalDlg)

// ���캯��
CSettingDlg::CSettingDlg(CWnd* pParent /*=NULL*/)
	: CNormalDlg(CSettingDlg::IDD, pParent),
	m_pagesResize(&m_tab)
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
	m_whiteListPage.m_contentStatic.SetWindowText(_T("�û�����"));
	m_whiteListPage.m_static.SetWindowText(_T("�������ı������ı�ʱƥ��(������)"));
	CREATE_PAGE(m_whiteContentPage);
	CREATE_PAGE(m_trustedThreadPage);
	m_trustedThreadPage.m_contentStatic.SetWindowText(_T("����ID��"));
	m_trustedThreadPage.m_edit.ModifyStyle(NULL, ES_NUMBER);
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
	ShowCurrentOptions();
	m_clearScanCache = FALSE; // ��m_scanPage.m_scanPageCountEdit.SetWindowText���ʼ��

	m_optionsPage.m_currentOptionStatic.SetWindowText(_T("��ǰ������") + g_currentOption); // ��ǰ����
	// ����
	CFileFind fileFind;
	BOOL flag = fileFind.FindFile(OPTIONS_PATH + _T("*.tb"));
	while (flag)
	{
		flag = fileFind.FindNextFile();
		m_optionsPage.m_list.AddString(fileFind.GetFileTitle());
	}

	m_usersPage.m_currentUserStatic.SetWindowText(_T("��ǰ�˺ţ�") + g_currentUser); // ��ǰ�˺�
	// �˺�
	m_usersPage.m_list.AddString(_T("[NULL]"));
	flag = fileFind.FindFile(USERS_PATH + _T("*"));
	while (flag)
	{
		flag = fileFind.FindNextFile();
		if (fileFind.IsDirectory() && !fileFind.IsDots() && PathFileExists(fileFind.GetFilePath() + _T("\\ck.tb")))
		{
			CString name = fileFind.GetFileName();
			if (name != _T("[NULL]"))
				m_usersPage.m_list.AddString(name);
		}
	}

	m_aboutPage.m_autoCheckUpdateCheck.SetCheck(g_autoUpdate); // �Զ�����

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
	lpMMI->ptMinTrackSize.x = 666;
	lpMMI->ptMinTrackSize.y = 576;

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
void CSettingDlg::ShowCurrentOptions()
{
	CString tmp;
	tmp.Format(_T("%d"), g_scanInterval);
	m_scanPage.m_scanIntervalEdit.SetWindowText(tmp);			// ɨ����
	m_operatePage.m_banIDCheck.SetCheck(g_banID);				// ��ID
	m_operatePage.OnBnClickedCheck1();
	m_operatePage.m_banDurationCombo.SetCurSel(g_banDuration == 1 ? 0 : (g_banDuration == 3 ? 1 : 2)); // ���ʱ��
	tmp.Format(_T("%d"), g_banTrigCount);
	m_operatePage.m_banTrigCountEdit.SetWindowText(tmp);		// ���Υ�����
	m_scanPage.m_onlyScanTitleCheck.SetCheck(g_onlyScanTitle);	// ֻɨ�����
	tmp.Format(_T("%g"), g_deleteInterval);
	m_operatePage.m_deleteIntervalEdit.SetWindowText(tmp);		// ɾ�����
	m_operatePage.m_confirmCheck.SetCheck(g_confirm);			// ����ǰ��ʾ
	tmp.Format(_T("%d"), g_scanPageCount);
	m_scanPage.m_scanPageCountEdit.SetWindowText(tmp);			// ɨ�����ҳ��
	m_scanPage.m_briefLogCheck.SetCheck(g_briefLog);			// ֻ���ɾ�����
	m_operatePage.m_deleteCheck.SetCheck(g_delete);				// ɾ��
	tmp.Format(_T("%d"), g_threadCount);
	m_scanPage.m_threadCountEdit.SetWindowText(tmp);			// �߳���
	m_operatePage.m_banReasonEdit.SetWindowText(g_banReason);	// ���ԭ��
	m_imagePage.m_dirEdit.SetWindowText(g_imageDir);			// Υ��ͼƬĿ¼
	tmp.Format(_T("%lf"), g_SSIMThreshold);
	m_imagePage.m_thresholdEdit.SetWindowText(tmp);				// ��ֵ
	m_operatePage.m_defriendCheck.SetCheck(g_defriend);			// ����
	m_operatePage.OnBnClickedCheck3();
	tmp.Format(_T("%d"), g_defriendTrigCount);
	m_operatePage.m_defriendTrigCountEdit.SetWindowText(tmp);	// ����Υ�����
	m_scanPage.m_autoSaveLogCheck.SetCheck(g_autoSaveLog);		// �Զ�������־

	// Υ������
	m_keywordsPage.m_list.ResetContent();
	for (const RegexText& i : g_keywords)
		m_keywordsPage.m_list.AddString((i.isRegex ? IS_REGEX_PREFIX : NOT_REGEX_PREFIX) + i.text);

	// �����û�
	m_blackListPage.m_list.ResetContent();
	for (const RegexText& i : g_blackList)
		m_blackListPage.m_list.AddString((i.isRegex ? IS_REGEX_PREFIX : NOT_REGEX_PREFIX) + i.text);

	// �����û�
	m_whiteListPage.m_list.ResetContent();
	for (const CString& i : g_whiteList)
		m_whiteListPage.m_list.AddString(i);

	// ��������
	m_whiteContentPage.m_list.ResetContent();
	for (const RegexText& i : g_whiteContent)
		m_whiteContentPage.m_list.AddString((i.isRegex ? IS_REGEX_PREFIX : NOT_REGEX_PREFIX) + i.text);

	// ��������
	m_trustedThreadPage.m_list.ResetContent();
	for (const CString& i : g_trustedThread)
		m_trustedThreadPage.m_list.AddString(i);
}

static inline void ApplyRegexTexts(vector<RegexText>& vec, CListBox& list)
{
	int size = list.GetCount();
	vec.resize(size);
	CString tmp;
	for (int i = 0; i < size; i++)
	{
		list.GetText(i, tmp);
		vec[i].isRegex = tmp.Left(REGEX_PREFIX_LENGTH) == IS_REGEX_PREFIX;
		vec[i].text = tmp.Right(tmp.GetLength() - REGEX_PREFIX_LENGTH);
		vec[i].regexp = vec[i].isRegex ? vec[i].text : _T("");
	}
}

// Ӧ�öԻ����е�����
void CSettingDlg::ApplyOptionsInDlg()
{
	CString strBuf;
	int intBuf;
	g_optionsLock.Lock();

	m_scanPage.m_scanIntervalEdit.GetWindowText(strBuf);
	g_scanInterval = _ttoi(strBuf);								// ɨ����
	g_banID = m_operatePage.m_banIDCheck.GetCheck();			// ��ID
	intBuf = m_operatePage.m_banDurationCombo.GetCurSel();
	g_banDuration = intBuf == 0 ? 1 : (intBuf == 1 ? 3 : 10);	// ���ʱ��
	m_operatePage.m_banTrigCountEdit.GetWindowText(strBuf);
	g_banTrigCount = _ttoi(strBuf);								// ���Υ�����
	g_onlyScanTitle = m_scanPage.m_onlyScanTitleCheck.GetCheck(); // ֻɨ�����
	m_operatePage.m_deleteIntervalEdit.GetWindowText(strBuf);
	g_deleteInterval = (float)_ttof(strBuf);					// ɾ�����
	g_confirm = m_operatePage.m_confirmCheck.GetCheck();		// ����ǰ��ʾ
	m_scanPage.m_scanPageCountEdit.GetWindowText(strBuf);
	g_scanPageCount = _ttoi(strBuf);							// ɨ�����ҳ��
	g_briefLog = m_scanPage.m_briefLogCheck.GetCheck();			// ֻ���ɾ�����
	g_delete = m_operatePage.m_deleteCheck.GetCheck();			// ɾ��
	m_scanPage.m_threadCountEdit.GetWindowText(strBuf);
	g_threadCount = _ttoi(strBuf);								// �߳���
	m_operatePage.m_banReasonEdit.GetWindowText(strBuf);
	g_banReason = strBuf;										// ���ԭ��
	m_imagePage.m_dirEdit.GetWindowText(g_imageDir);			// Υ��ͼƬĿ¼
	m_imagePage.m_thresholdEdit.GetWindowText(strBuf);
	g_SSIMThreshold = _ttof(strBuf);							// ��ֵ
	g_defriend = m_operatePage.m_defriendCheck.GetCheck();		// ����
	m_operatePage.m_defriendTrigCountEdit.GetWindowText(strBuf);
	g_defriendTrigCount = _ttoi(strBuf);						// ����Υ�����
	g_autoSaveLog = m_scanPage.m_autoSaveLogCheck.GetCheck();	// �Զ�������־

	// Υ������
	ApplyRegexTexts(g_keywords, m_keywordsPage.m_list);

	// �����û�
	ApplyRegexTexts(g_blackList, m_blackListPage.m_list);

	// �����û�
	int size = m_whiteListPage.m_list.GetCount();
	g_whiteList.clear();
	for (int i = 0; i < size; i++)
	{
		m_whiteListPage.m_list.GetText(i, strBuf);
		g_whiteList.insert(strBuf);
	}

	// ��������
	ApplyRegexTexts(g_whiteContent, m_whiteContentPage.m_list);

	// ��������
	size = m_trustedThreadPage.m_list.GetCount();
	g_trustedThread.clear();
	for (int i = 0; i < size; i++)
	{
		m_trustedThreadPage.m_list.GetText(i, strBuf);
		g_trustedThread.insert(strBuf);
	}

	// Υ��ͼƬ
	if (m_imagePage.m_updateImage)
	{
		g_leagalImage.clear();
		g_illegalImage.clear();
		ReadImages(g_imageDir);
	}

	g_optionsLock.Unlock();

	if (m_clearScanCache)
	{
		if (!g_briefLog)
			((CTiebaManagerDlg*)AfxGetApp()->m_pMainWnd)->m_log.Log(_T("<font color=green>�����ʷ�ظ�</font>"));
		g_userCache.m_reply->clear();
	}
}

static inline void ReadRegexTexts(const gzFile& f, CListBox& list)
{
	int size;
	gzread(f, &size, sizeof(int)); // ����
	list.ResetContent();
	CString strBuf;
	for (int i = 0; i < size; i++)
	{
		int isRegex;
		gzread(f, &isRegex, sizeof(int)); // ������
		ReadText(f, strBuf);
		list.AddString((isRegex != 0 ? IS_REGEX_PREFIX : NOT_REGEX_PREFIX) + strBuf);
	}
}

static inline void WriteRegexTexts(const gzFile& f, CListBox& list)
{
	int size;
	gzwrite(f, &(size = list.GetCount()), sizeof(int)); // ����
	CString strBuf;
	for (int i = 0; i < size; i++)
	{
		list.GetText(i, strBuf);
		BOOL isRegex = strBuf.Left(REGEX_PREFIX_LENGTH) == IS_REGEX_PREFIX;
		int intBuf;
		gzwrite(f, &(intBuf = isRegex ? 1 : 0), sizeof(int)); // ������
		strBuf = strBuf.Right(strBuf.GetLength() - REGEX_PREFIX_LENGTH);
		WriteText(f, strBuf);
	}
}

// ��ʾ�ļ��е�����
void CSettingDlg::ShowOptionsInFile(LPCTSTR path)
{
	CString strBuf;

	gzFile f = gzopen_w(path, "rb");
	if (f == NULL)
		goto UseDefaultOptions;

	// ͷ��
	char header[2];
	gzread(f, header, sizeof(header));
	if (header[0] != 'T' || header[1] != 'B')
	{
		gzclose(f);
		goto UseDefaultOptions;
	}

	// Υ������
	ReadRegexTexts(f, m_keywordsPage.m_list);

	// �����û�
	ReadRegexTexts(f, m_blackListPage.m_list);

	// �����û�
	int size;
	gzread(f, &size, sizeof(int)); // ����
	m_whiteListPage.m_list.ResetContent();
	int intBuf;
	for (int i = 0; i < size; i++)
	{
		ReadText(f, strBuf);
		m_whiteListPage.m_list.AddString(strBuf);
	}

	// ��������
	ReadRegexTexts(f, m_whiteContentPage.m_list);

	// Υ��ͼƬ
	m_imagePage.m_updateImage = TRUE;

	BOOL boolBuf;
	float floatBuf;
	double doubleBuf;
	gzread(f, &intBuf, sizeof(int));						// ɨ����
	strBuf.Format(_T("%d"), intBuf);
	m_scanPage.m_scanIntervalEdit.SetWindowText(strBuf);
	gzread(f, &boolBuf, sizeof(BOOL));						// ��ID
	m_operatePage.m_banIDCheck.SetCheck(boolBuf);
	m_operatePage.OnBnClickedCheck1();
	gzread(f, &intBuf, sizeof(int));						// ���ʱ��
	m_operatePage.m_banDurationCombo.SetCurSel(intBuf == 1 ? 0 : (intBuf == 3 ? 1 : 2));
	gzread(f, &boolBuf, sizeof(BOOL));						// ��IP
	gzread(f, &intBuf, sizeof(int));						// ���Υ�����
	strBuf.Format(_T("%d"), intBuf);
	m_operatePage.m_banTrigCountEdit.SetWindowText(strBuf);
	gzread(f, &boolBuf, sizeof(BOOL));						// ֻɨ�����
	m_scanPage.m_onlyScanTitleCheck.SetCheck(boolBuf);
	gzread(f, &floatBuf, sizeof(float));					// ɾ�����
	strBuf.Format(_T("%g"), floatBuf);
	m_operatePage.m_deleteIntervalEdit.SetWindowText(strBuf);
	gzread(f, &boolBuf, sizeof(BOOL));						// ����ǰ��ʾ
	m_operatePage.m_confirmCheck.SetCheck(boolBuf);
	gzread(f, &intBuf, sizeof(int));						// ɨ�����ҳ��
	strBuf.Format(_T("%d"), intBuf);
	m_scanPage.m_scanPageCountEdit.SetWindowText(strBuf);
	gzread(f, &boolBuf, sizeof(BOOL));						// ֻ���ɾ�����
	m_scanPage.m_briefLogCheck.SetCheck(boolBuf);
	if (gzread(f, &boolBuf, sizeof(BOOL)) == sizeof(BOOL))	// ɾ��
		m_operatePage.m_deleteCheck.SetCheck(boolBuf);
	else
		m_operatePage.m_deleteCheck.SetCheck(TRUE);
	if (gzread(f, &intBuf, sizeof(int)) == sizeof(int))		// �߳���
	{
		strBuf.Format(_T("%d"), intBuf);
		m_scanPage.m_threadCountEdit.SetWindowText(strBuf);
	}
	else
		m_scanPage.m_threadCountEdit.SetWindowText(_T("2"));
	ReadText(f, strBuf);									// ���ԭ��
	m_operatePage.m_banReasonEdit.SetWindowText(strBuf);
	ReadText(f, strBuf);									// Υ��ͼƬĿ¼
	m_imagePage.m_dirEdit.SetWindowText(strBuf);
	if (gzread(f, &doubleBuf, sizeof(double)) == sizeof(double))	// ��ֵ
	{
		strBuf.Format(_T("%lf"), doubleBuf);
		m_imagePage.m_thresholdEdit.SetWindowText(strBuf);
	}
	else
		m_imagePage.m_thresholdEdit.SetWindowText(_T("2.43"));

	// ��������
	m_trustedThreadPage.m_list.ResetContent();
	if (gzread(f, &size, sizeof(int)) == sizeof(int)) // ����
		for (int i = 0; i < size; i++)
		{
			ReadText(f, strBuf);
			m_trustedThreadPage.m_list.AddString(strBuf);
		}

	gzread(f, &boolBuf, sizeof(BOOL));						// ����
	m_operatePage.m_defriendCheck.SetCheck(boolBuf);
	m_operatePage.OnBnClickedCheck3();
	if (gzread(f, &intBuf, sizeof(int)) == sizeof(int))		// ����Υ�����
	{
		strBuf.Format(_T("%d"), intBuf);
		m_scanPage.m_threadCountEdit.SetWindowText(strBuf);
	}
	else
		m_scanPage.m_threadCountEdit.SetWindowText(_T("5"));
	if (gzread(f, &boolBuf, sizeof(BOOL)) == sizeof(BOOL))	// �Զ�������־
		m_scanPage.m_autoSaveLogCheck.SetCheck(boolBuf);
	else
		m_scanPage.m_autoSaveLogCheck.SetCheck(FALSE);

	gzclose(f);
	return;

UseDefaultOptions:
	m_keywordsPage.m_list.ResetContent();					// Υ������
	m_blackListPage.m_list.ResetContent();					// �����û�
	m_whiteListPage.m_list.ResetContent();					// �����û�
	m_whiteContentPage.m_list.ResetContent();				// ��������
	m_imagePage.m_updateImage = TRUE;						// Υ��ͼƬ
	m_scanPage.m_scanIntervalEdit.SetWindowText(_T("5"));	// ɨ����
	m_operatePage.m_banIDCheck.SetCheck(FALSE);				// ��ID
	m_operatePage.OnBnClickedCheck1();
	m_operatePage.m_banDurationCombo.SetCurSel(0);			// ���ʱ��
	m_operatePage.m_banTrigCountEdit.SetWindowText(_T("1"));	// ���Υ�����
	m_scanPage.m_onlyScanTitleCheck.SetCheck(FALSE);		// ֻɨ�����
	m_operatePage.m_deleteIntervalEdit.SetWindowText(_T("2"));	// ɾ�����
	m_operatePage.m_confirmCheck.SetCheck(TRUE);			// ����ǰ��ʾ
	m_scanPage.m_scanPageCountEdit.SetWindowText(_T("1"));	// ɨ�����ҳ��
	m_scanPage.m_briefLogCheck.SetCheck(FALSE);				// ֻ���ɾ�����
	m_operatePage.m_deleteCheck.SetCheck(TRUE);				// ɾ��
	m_scanPage.m_threadCountEdit.SetWindowText(_T("2"));	// �߳���
	m_operatePage.m_banReasonEdit.SetWindowText(_T(""));	// ���ԭ��
	m_imagePage.m_dirEdit.SetWindowText(_T(""));			// Υ��ͼƬĿ¼
	m_imagePage.m_thresholdEdit.SetWindowText(_T("2.43"));	// ��ֵ
	m_trustedThreadPage.m_list.ResetContent();				// ��������
	m_operatePage.m_defriendCheck.SetCheck(FALSE);			// ����
	m_operatePage.OnBnClickedCheck3();
	m_scanPage.m_threadCountEdit.SetWindowText(_T("5"));	// ����Υ�����
	m_scanPage.m_autoSaveLogCheck.SetCheck(FALSE);			// �Զ�������־
}

// �ѶԻ����е�����д���ļ�
void CSettingDlg::SaveOptionsInDlg(LPCTSTR path)
{
	gzFile f = gzopen_w(path, "wb");
	if (f == NULL)
		return;

	// ͷ��
	gzwrite(f, "TB", 2);

	// Υ������
	WriteRegexTexts(f, m_keywordsPage.m_list);

	// �����û�
	WriteRegexTexts(f, m_blackListPage.m_list);

	// �����û�
	int size;
	gzwrite(f, &(size = m_whiteListPage.m_list.GetCount()), sizeof(int)); // ����
	CString strBuf;
	for (int i = 0; i < size; i++)
	{
		m_whiteListPage.m_list.GetText(i, strBuf);
		WriteText(f, strBuf);
	}

	// ��������
	WriteRegexTexts(f, m_whiteContentPage.m_list);

	int intBuf;
	BOOL boolBuf;
	float floatBuf;
	double doubleBuf;
	m_scanPage.m_scanIntervalEdit.GetWindowText(strBuf);
	gzwrite(f, &(intBuf = _ttoi(strBuf)), sizeof(int));								// ɨ����
	gzwrite(f, &(boolBuf = m_operatePage.m_banIDCheck.GetCheck()), sizeof(BOOL));	// ��ID
	intBuf = m_operatePage.m_banDurationCombo.GetCurSel();
	intBuf = intBuf == 0 ? 1 : (intBuf == 1 ? 3 : 10);
	gzwrite(f, &intBuf, sizeof(int));												// ���ʱ��
	gzwrite(f, &(boolBuf = FALSE), sizeof(BOOL));									// ��IP
	m_operatePage.m_banTrigCountEdit.GetWindowText(strBuf);
	gzwrite(f, &(intBuf = _ttoi(strBuf)), sizeof(int));								// ���Υ�����
	gzwrite(f, &(boolBuf = m_scanPage.m_onlyScanTitleCheck.GetCheck()), sizeof(BOOL)); // ֻɨ�����
	m_operatePage.m_deleteIntervalEdit.GetWindowText(strBuf);
	gzwrite(f, &(floatBuf = (float)_ttof(strBuf)), sizeof(float));					// ɾ�����
	gzwrite(f, &(boolBuf = m_operatePage.m_confirmCheck.GetCheck()), sizeof(BOOL));	// ����ǰ��ʾ
	m_scanPage.m_scanPageCountEdit.GetWindowText(strBuf);
	gzwrite(f, &(intBuf = _ttoi(strBuf)), sizeof(int));								// ɨ�����ҳ��
	gzwrite(f, &(boolBuf = m_scanPage.m_briefLogCheck.GetCheck()), sizeof(BOOL));	// ֻ���ɾ�����
	gzwrite(f, &(boolBuf = m_operatePage.m_deleteCheck.GetCheck()), sizeof(BOOL));	// ɾ��
	m_scanPage.m_threadCountEdit.GetWindowText(strBuf);
	gzwrite(f, &(intBuf = _ttoi(strBuf)), sizeof(int));								// �߳���
	m_operatePage.m_banReasonEdit.GetWindowText(strBuf);
	WriteText(f, strBuf);															// ���ԭ��
	m_imagePage.m_dirEdit.GetWindowText(strBuf);
	WriteText(f, strBuf);															// Υ��ͼƬĿ¼
	m_imagePage.m_thresholdEdit.GetWindowText(strBuf);
	gzwrite(f, &(doubleBuf = _ttof(strBuf)), sizeof(double));						// ��ֵ

	// ��������
	gzwrite(f, &(size = m_trustedThreadPage.m_list.GetCount()), sizeof(int)); // ����
	for (int i = 0; i < size; i++)
	{
		m_trustedThreadPage.m_list.GetText(i, strBuf);
		WriteText(f, strBuf);
	}

	gzwrite(f, &(boolBuf = m_operatePage.m_defriendCheck.GetCheck()), sizeof(BOOL)); // ����
	m_operatePage.m_defriendTrigCountEdit.GetWindowText(strBuf);
	gzwrite(f, &(intBuf = _ttoi(strBuf)), sizeof(int));								// ����Υ�����
	gzwrite(f, &(boolBuf = m_scanPage.m_autoSaveLogCheck.GetCheck()), sizeof(BOOL)); // �Զ�������־
	gzclose(f);
}

// ȷ��
void CSettingDlg::OnOK()
{
	CString tmp;
	m_optionsPage.m_currentOptionStatic.GetWindowText(tmp);
	g_currentOption = tmp.Right(tmp.GetLength() - 5);
	CreateDir(OPTIONS_PATH);
	SaveOptionsInDlg(OPTIONS_PATH + g_currentOption + _T(".tb"));
	ApplyOptionsInDlg();
	WritePrivateProfileString(_T("Setting"), _T("Option"), g_currentOption, USER_PROFILE_PATH);
	WritePrivateProfileString(_T("Setting"), _T("AutoUpdate"), 
		m_aboutPage.m_autoCheckUpdateCheck.GetCheck() ? _T("1") : _T("0"), ALL_PROFILE_PATH);

	DestroyWindow();
}

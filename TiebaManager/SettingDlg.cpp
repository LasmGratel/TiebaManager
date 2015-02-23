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
#include "SettingDlg.h"
#include "afxdialogex.h"
#include "TiebaManagerDlg.h"
#include "Setting.h"
#include "Tieba.h"
#include <zlib.h>


// CSettingDlg �Ի���

IMPLEMENT_DYNAMIC(CSettingDlg, CDialog)

// ���캯��
CSettingDlg::CSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSettingDlg::IDD, pParent)
{
	// ��ʼ��m_pages
	int i = 0;
	m_pages[i++] = &m_prefPage;
	m_pages[i++] = &m_keywordsPage;
	m_pages[i++] = &m_blackListPage;
	m_pages[i++] = &m_whiteListPage;
	m_pages[i++] = &m_whiteContentPage;
	m_pages[i++] = &m_optionsPage;
	m_pages[i++] = &m_aboutPage;
}

#pragma region MFC
CSettingDlg::~CSettingDlg()
{
}

void CSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tab);
	DDX_Control(pDX, IDC_BUTTON1, m_logoutButton);
	DDX_Control(pDX, IDOK, m_okButton);
	DDX_Control(pDX, IDCANCEL, m_cancelButton);
}


BEGIN_MESSAGE_MAP(CSettingDlg, CDialog)
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CSettingDlg::OnTcnSelchangeTab1)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON1, &CSettingDlg::OnBnClickedButton1)
END_MESSAGE_MAP()
#pragma endregion

// CSettingDlg ��Ϣ�������

// ��ʼ��
BOOL CSettingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(hIcon, FALSE);			// ����Сͼ��

	// ��ʼ��m_tab
	int i = 0;
	m_tab.InsertItem(i++, _T("��ѡ��"));
	m_tab.InsertItem(i++, _T("Υ������"));
	m_tab.InsertItem(i++, _T("�����û�"));
	m_tab.InsertItem(i++, _T("�����û�"));
	m_tab.InsertItem(i++, _T("��������"));
	m_tab.InsertItem(i++, _T("����"));
	m_tab.InsertItem(i++, _T("����&&����"));

	// ��ʼ����ҳ
	m_prefPage.Create(IDD_PREF_PAGE, &m_tab);
	m_keywordsPage.Create(IDD_LIST_PAGE, &m_tab);
	m_blackListPage.Create(IDD_LIST_PAGE, &m_tab);
	m_whiteListPage.Create(IDD_LIST_PAGE, &m_tab);
	m_whiteContentPage.Create(IDD_LIST_PAGE, &m_tab);
	m_optionsPage.Create(IDD_OPTIONS_PAGE, &m_tab);
	m_aboutPage.Create(IDD_ABOUT_PAGE, &m_tab);

	CRect rect;
	m_tab.GetClientRect(&rect);
	rect.left += 1; rect.right -= 3; rect.top += 23; rect.bottom -= 2;
	m_prefPage.SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_SHOWWINDOW);
	for (i = 1; i < _countof(m_pages); i++)
		m_pages[i]->SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_HIDEWINDOW);

	// ��ʾ����
	ShowCurrentOptions();
	m_clearScanCache = FALSE; // ��m_prefPage.m_scanPageCountEdit.SetWindowText���ʼ��
	m_optionsPage.m_currentOptionStatic.SetWindowText(_T("��ǰ������") + g_currentOption); // ��ǰ����
	m_aboutPage.m_autoCheckUpdateCheck.SetCheck(g_autoUpdate); // �Զ�����
	// ����
	CFileFind fileFind;
	BOOL flag = fileFind.FindFile(OPTIONS_PATH + _T("*.tb"));
	while (flag)
	{
		flag = fileFind.FindNextFile();
		m_optionsPage.m_list.AddString(fileFind.GetFileTitle());
	}

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

	CDialog::OnClose();
}

// �ͷ�this
void CSettingDlg::PostNcDestroy()
{
	CDialog::PostNcDestroy();

	((CTiebaManagerDlg*)AfxGetApp()->m_pMainWnd)->m_settingDlg = NULL;
	delete this;
}

// ������С�ߴ�
void CSettingDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = 489;
	lpMMI->ptMinTrackSize.y = 411;

	CDialog::OnGetMinMaxInfo(lpMMI);
}

// �ı�ߴ�
void CSettingDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (m_tab.m_hWnd == NULL)
		return;

	CRect rect;
	GetClientRect(&rect); // Ĭ��473 * 373
	m_tab.SetWindowPos(NULL, 0, 0, rect.Width() - 21, rect.Height() - 58, SWP_NOMOVE | SWP_NOREDRAW);
	int y = rect.Height() - 35;
	m_logoutButton.SetWindowPos(NULL, rect.Width() - 326, y, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);
	m_okButton.SetWindowPos(NULL, rect.Width() - 200, y, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);
	m_cancelButton.SetWindowPos(NULL, rect.Width() - 105, y, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);

	m_tab.GetClientRect(&rect);
	rect.left += 1; rect.right -= 3; rect.top += 23; rect.bottom -= 2;
	for (int i = 0; i < _countof(m_pages); i++)
		m_pages[i]->SetWindowPos(NULL, 0, 0, rect.Width(), rect.Height(), SWP_NOMOVE | SWP_NOREDRAW);

	Invalidate();
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
	m_prefPage.m_scanIntervalEdit.SetWindowText(tmp);		// ɨ����
	m_prefPage.m_banIDCheck.SetCheck(g_banID);				// ��ID
	m_prefPage.m_banDurationCombo.SetCurSel(g_banDuration == 1 ? 0 : (g_banDuration == 3 ? 1 : 2)); // ���ʱ��
	//m_prefPage.m_banIPCheck.SetCheck(g_banIP);				// ��IP
	m_prefPage.OnBnClickedCheck1();
	tmp.Format(_T("%d"), g_trigCount);
	m_prefPage.m_trigCountEdit.SetWindowText(tmp);			// ���Υ�����
	m_prefPage.m_onlyScanTitleCheck.SetCheck(g_onlyScanTitle); // ֻɨ�����
	tmp.Format(_T("%g"), g_deleteInterval);
	m_prefPage.m_deleteIntervalEdit.SetWindowText(tmp);		// ɾ�����
	m_prefPage.m_confirmCheck.SetCheck(g_confirm);			// ����ǰ��ʾ
	tmp.Format(_T("%d"), g_scanPageCount);
	m_prefPage.m_scanPageCountEdit.SetWindowText(tmp);		// ɨ�����ҳ��
	m_prefPage.m_briefLogCheck.SetCheck(g_briefLog);		// ֻ���ɾ�����

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
	m_prefPage.m_scanIntervalEdit.GetWindowText(strBuf);
	g_scanInterval = _ttoi(strBuf);								// ɨ����
	g_banID = m_prefPage.m_banIDCheck.GetCheck();				// ��ID
	intBuf = m_prefPage.m_banDurationCombo.GetCurSel();
	g_banDuration = intBuf == 0 ? 1 : (intBuf == 1 ? 3 : 10);	// ���ʱ��
	//g_banIP = m_prefPage.m_banIPCheck.GetCheck();				// ��IP
	m_prefPage.m_trigCountEdit.GetWindowText(strBuf);
	g_trigCount = _ttoi(strBuf);								// ���Υ�����
	g_onlyScanTitle = m_prefPage.m_onlyScanTitleCheck.GetCheck(); // ֻɨ�����
	m_prefPage.m_deleteIntervalEdit.GetWindowText(strBuf);
	g_deleteInterval = (float)_ttof(strBuf);					// ɾ�����
	g_confirm = m_prefPage.m_confirmCheck.GetCheck();			// ����ǰ��ʾ
	m_prefPage.m_scanPageCountEdit.GetWindowText(strBuf);
	g_scanPageCount = _ttoi(strBuf);							// ɨ�����ҳ��
	g_briefLog = m_prefPage.m_briefLogCheck.GetCheck();			// ֻ���ɾ�����

	g_optionsLock.Lock();
	// Υ������
	ApplyRegexTexts(g_keywords, m_keywordsPage.m_list);

	// �����û�
	ApplyRegexTexts(g_blackList, m_blackListPage.m_list);

	// �����û�
	int size = m_whiteListPage.m_list.GetCount();
	g_whiteList.resize(size);
	for (int i = 0; i < size; i++)
	{
		m_whiteListPage.m_list.GetText(i, g_whiteList[i]);
	}

	// ��������
	ApplyRegexTexts(g_whiteContent, m_whiteContentPage.m_list);
	g_optionsLock.Unlock();

	if (m_clearScanCache)
	{
		if (!g_briefLog)
			((CTiebaManagerDlg*)AfxGetApp()->m_pMainWnd)->Log(_T("<font color=green>���ɨ���¼</font>"));
		g_reply.clear();
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
		int strLen;
		gzread(f, &strLen, sizeof(int)); // �ַ�������
		gzread(f, strBuf.GetBuffer(strLen), strLen * sizeof(TCHAR)); // �ַ���
		strBuf.ReleaseBuffer();
		list.AddString((isRegex != 0 ? IS_REGEX_PREFIX : NOT_REGEX_PREFIX) + strBuf);
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
		gzread(f, &intBuf, sizeof(int)); // �ַ�������
		gzread(f, strBuf.GetBuffer(intBuf), intBuf * sizeof(TCHAR)); // �ַ���
		strBuf.ReleaseBuffer();
		m_whiteListPage.m_list.AddString(strBuf);
	}

	// ��������
	ReadRegexTexts(f, m_whiteContentPage.m_list);

	BOOL boolBuf;
	float floatBuf;
	gzread(f, &intBuf, sizeof(int));
	strBuf.Format(_T("%d"), intBuf);
	m_prefPage.m_scanIntervalEdit.SetWindowText(strBuf);	// ɨ����
	gzread(f, &boolBuf, sizeof(BOOL));
	m_prefPage.m_banIDCheck.SetCheck(boolBuf);				// ��ID
	gzread(f, &intBuf, sizeof(int));
	m_prefPage.m_banDurationCombo.SetCurSel(intBuf == 1 ? 0 : (intBuf == 3 ? 1 : 2)); // ���ʱ��
	gzread(f, &boolBuf, sizeof(BOOL));
	m_prefPage.m_banIPCheck.SetCheck(boolBuf);				// ��IP
	gzread(f, &intBuf, sizeof(int));
	strBuf.Format(_T("%d"), intBuf);
	m_prefPage.m_trigCountEdit.SetWindowText(strBuf);		// ���Υ�����
	gzread(f, &boolBuf, sizeof(BOOL));
	m_prefPage.m_onlyScanTitleCheck.SetCheck(boolBuf);		// ֻɨ�����
	gzread(f, &floatBuf, sizeof(float));
	strBuf.Format(_T("%g"), floatBuf);
	m_prefPage.m_deleteIntervalEdit.SetWindowText(strBuf);	// ɾ�����
	gzread(f, &boolBuf, sizeof(BOOL));
	m_prefPage.m_confirmCheck.SetCheck(boolBuf);			// ����ǰ��ʾ
	gzread(f, &intBuf, sizeof(int));
	strBuf.Format(_T("%d"), intBuf);
	m_prefPage.m_scanPageCountEdit.SetWindowText(strBuf);	// ɨ�����ҳ��
	gzread(f, &boolBuf, sizeof(BOOL));
	m_prefPage.m_briefLogCheck.SetCheck(boolBuf);			// ֻ���ɾ�����

	gzclose(f);
	return;

UseDefaultOptions:
	m_keywordsPage.m_list.ResetContent();					// Υ������
	m_blackListPage.m_list.ResetContent();					// �����û�
	m_whiteListPage.m_list.ResetContent();					// �����û�
	m_whiteContentPage.m_list.ResetContent();				// ��������
	m_prefPage.m_scanIntervalEdit.SetWindowText(_T("5"));	// ɨ����
	m_prefPage.m_banIDCheck.SetCheck(FALSE);				// ��ID
	m_prefPage.m_banDurationCombo.SetCurSel(0);				// ���ʱ��
	m_prefPage.m_banIPCheck.SetCheck(FALSE);				// ��IP
	m_prefPage.m_trigCountEdit.SetWindowText(_T("1"));		// ���Υ�����
	m_prefPage.m_onlyScanTitleCheck.SetCheck(FALSE);		// ֻɨ�����
	m_prefPage.m_deleteIntervalEdit.SetWindowText(_T("2"));	// ɾ�����
	m_prefPage.m_confirmCheck.SetCheck(TRUE);				// ����ǰ��ʾ
	m_prefPage.m_scanPageCountEdit.SetWindowText(_T("1"));	// ɨ�����ҳ��
	m_prefPage.m_briefLogCheck.SetCheck(FALSE);				// ֻ���ɾ�����
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
		int len = strBuf.GetLength();
		gzwrite(f, &len, sizeof(int)); // �ַ�������
		gzwrite(f, (LPCTSTR)strBuf, len * sizeof(TCHAR)); // �ַ���
	}
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
		int len = strBuf.GetLength();
		gzwrite(f, &len, sizeof(int)); // �ַ�������
		gzwrite(f, (LPCTSTR)strBuf, len * sizeof(TCHAR)); // �ַ���
	}

	// ��������
	WriteRegexTexts(f, m_whiteContentPage.m_list);

	int intBuf;
	BOOL boolBuf;
	float floatBuf;
	m_prefPage.m_scanIntervalEdit.GetWindowText(strBuf);
	gzwrite(f, &(intBuf = _ttoi(strBuf)), sizeof(int));								// ɨ����
	gzwrite(f, &(boolBuf = m_prefPage.m_banIDCheck.GetCheck()), sizeof(BOOL));		// ��ID
	intBuf = m_prefPage.m_banDurationCombo.GetCurSel();
	intBuf = intBuf == 0 ? 1 : (intBuf == 1 ? 3 : 10);
	gzwrite(f, &intBuf, sizeof(int));												// ���ʱ��
	gzwrite(f, &(boolBuf = m_prefPage.m_banIPCheck.GetCheck()), sizeof(BOOL));		// ��IP
	m_prefPage.m_trigCountEdit.GetWindowText(strBuf);
	gzwrite(f, &(intBuf = _ttoi(strBuf)), sizeof(int));								// ���Υ�����
	gzwrite(f, &(boolBuf = m_prefPage.m_onlyScanTitleCheck.GetCheck()), sizeof(BOOL)); // ֻɨ�����
	m_prefPage.m_deleteIntervalEdit.GetWindowText(strBuf);
	gzwrite(f, &(floatBuf = (float)_ttof(strBuf)), sizeof(float));					// ɾ�����
	gzwrite(f, &(boolBuf = m_prefPage.m_confirmCheck.GetCheck()), sizeof(BOOL));	// ����ǰ��ʾ
	m_prefPage.m_scanPageCountEdit.GetWindowText(strBuf);
	gzwrite(f, &(intBuf = _ttoi(strBuf)), sizeof(int));								// ɨ�����ҳ��
	gzwrite(f, &(boolBuf = m_prefPage.m_briefLogCheck.GetCheck()), sizeof(BOOL));	// ֻ���ɾ�����

	gzclose(f);
}

// ȷ��
void CSettingDlg::OnOK()
{
	CString tmp;
	m_optionsPage.m_currentOptionStatic.GetWindowText(tmp);
	g_currentOption = tmp.Right(tmp.GetLength() - 5);
	if (!PathFileExists(OPTIONS_PATH))
		CreateDirectory(OPTIONS_PATH, NULL);
	SaveOptionsInDlg(OPTIONS_PATH + g_currentOption + _T(".tb"));
	ApplyOptionsInDlg();
	WritePrivateProfileString(_T("Routine"), _T("Option"), g_currentOption, PROFILE_PATH);
	WritePrivateProfileString(_T("Routine"), _T("AutoUpdate"), 
		m_aboutPage.m_autoCheckUpdateCheck.GetCheck() ? _T("1") : _T("0"), PROFILE_PATH);

	DestroyWindow();
}

// �˳���¼
void CSettingDlg::OnBnClickedButton1()
{
	g_cookie = _T("");
	AfxGetApp()->m_pMainWnd->DestroyWindow();
}

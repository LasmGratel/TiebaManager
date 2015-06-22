#include "stdafx.h"
#include "Setting.h"
#include <zlib.h>
#include "Tieba.h"
#include "TiebaManagerDlg.h"


// �����ļ�·��
CString		ALL_PROFILE_PATH = _T("\\options.tb");	// ��������ʱ��ʼ��
CString		USER_PROFILE_PATH;						// ȷ������ʱ��ʼ��
CString		OPTIONS_PATH = _T("Option\\");
CString		USERS_PATH = _T("\\User\\");			// ��������ʱ��ʼ��
CString		CURRENT_USER_PATH;						// ȷ������ʱ��ʼ��
CString		COOKIE_PATH;							// ȷ������ʱ��ʼ��
CString		CACHE_PATH;								// ȷ������ʱ��ʼ��

CString	g_currentUser;		// ��ǰ�˺�
BOOL	g_autoUpdate;		// �Զ�����

// ����
CString	g_currentOption;	// ��ǰ����
int		g_scanInterval;		// ɨ����
BOOL	g_banID;			// ��ID
int		g_banDuration;		// ���ʱ��
int		g_trigCount;		// ���Υ�����
BOOL	g_onlyScanTitle;	// ֻɨ�����
float	g_deleteInterval;	// ɾ�����
BOOL	g_confirm;			// ����ǰ��ʾ
int		g_scanPageCount;	// ɨ�����ҳ��
BOOL	g_briefLog;			// ֻ���ɾ�����
BOOL	g_delete;			// ɾ��
int		g_threadCount;		// �߳���
vector<RegexText>	g_keywords;		// Υ������
vector<RegexText>	g_blackList;	// �����û�
vector<CString>		g_whiteList;	// �����û�
vector<RegexText>	g_whiteContent;	// ��������

CCriticalSection g_optionsLock; // �ж�Υ���õ��ٽ���


static inline void ReadRegexTexts(const gzFile& f, vector<RegexText>& vec)
{
	int intBuf;
	gzread(f, &intBuf, sizeof(int)); // ����
	vec.resize(intBuf);
	for (RegexText& i : vec)
	{
		gzread(f, &intBuf, sizeof(int)); // ������
		i.isRegex = intBuf != 0;
		gzread(f, &intBuf, sizeof(int)); // �ַ�������
		gzread(f, i.text.GetBuffer(intBuf), intBuf * sizeof(TCHAR)); // �ַ���
		i.text.ReleaseBuffer(intBuf);
		i.regexp = i.isRegex ? i.text : _T("");
	}
}

// ������
void ReadOptions(LPCTSTR path)
{
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
	ReadRegexTexts(f, g_keywords);

	// �����û�
	ReadRegexTexts(f, g_blackList);

	// �����û�
	int intBuf;
	gzread(f, &intBuf, sizeof(int)); // ����
	g_whiteList.resize(intBuf);
	for (CString& i : g_whiteList)
	{
		gzread(f, &intBuf, sizeof(int)); // �ַ�������
		gzread(f, i.GetBuffer(intBuf), intBuf * sizeof(TCHAR)); // �ַ���
		i.ReleaseBuffer(intBuf);
	}

	// ��������
	ReadRegexTexts(f, g_whiteContent);

	gzread(f, &g_scanInterval, sizeof(int));	// ɨ����
	gzread(f, &g_banID, sizeof(BOOL));			// ��ID
	gzread(f, &g_banDuration, sizeof(int));		// ���ʱ��
	BOOL banIP;
	gzread(f, &banIP, sizeof(BOOL));			// ��IP
	gzread(f, &g_trigCount, sizeof(int));		// ���Υ�����
	gzread(f, &g_onlyScanTitle, sizeof(BOOL));	// ֻɨ�����
	gzread(f, &g_deleteInterval, sizeof(float));// ɾ�����
	gzread(f, &g_confirm, sizeof(BOOL));		// ����ǰ��ʾ
	gzread(f, &g_scanPageCount, sizeof(int));	// ɨ�����ҳ��
	gzread(f, &g_briefLog, sizeof(BOOL));		// ֻ���ɾ�����
	if (gzread(f, &g_delete, sizeof(BOOL)) != sizeof(BOOL))			// ɾ��
		g_delete = TRUE;
	if (gzread(f, &g_threadCount, sizeof(int)) != sizeof(int))		// �߳���
		g_threadCount = 2;

	gzclose(f);
	return;

UseDefaultOptions:
	g_keywords.clear();			// Υ������
	g_blackList.clear();		// �����û�
	g_whiteList.clear();		// �����û�
	g_scanInterval = 5;			// ɨ����
	g_banID = FALSE;			// ��ID
	g_banDuration = 1;			// ���ʱ��
	g_trigCount = 1;			// ���Υ�����
	g_onlyScanTitle = FALSE;	// ֻɨ�����
	g_deleteInterval = 2.0f;	// ɾ�����
	g_confirm = TRUE;			// ����ǰ��ʾ
	g_scanPageCount = 1;		// ɨ�����ҳ��
	g_briefLog = FALSE;			// ֻ���ɾ�����
	g_delete = TRUE;			// ɾ��
	g_threadCount = 2;			// �߳���
}

static inline void WriteRegexTexts(const gzFile& f, vector<RegexText>& vec)
{
	int intBuf;
	gzwrite(f, &(intBuf = vec.size()), sizeof(int)); // ����
	for (const RegexText& i : vec)
	{
		gzwrite(f, &(intBuf = i.isRegex ? 1 : 0), sizeof(int)); // ������
		int len = i.text.GetLength();
		gzwrite(f, &(intBuf = len), sizeof(int)); // �ַ�������
		gzwrite(f, (LPCTSTR)i.text, len * sizeof(TCHAR)); // �ַ���
	}
}

// д����
void WriteOptions(LPCTSTR path)
{
	gzFile f = gzopen_w(path, "wb");
	if (f == NULL)
		return;

	// ͷ��
	gzwrite(f, "TB", 2);

	int intBuf;

	// Υ������
	WriteRegexTexts(f, g_keywords);

	// �����û�
	WriteRegexTexts(f, g_blackList);

	// �����û�
	gzwrite(f, &(intBuf = g_whiteList.size()), sizeof(int)); // ����
	for (const CString& i : g_whiteList)
	{
		int len = i.GetLength();
		gzwrite(f, &len, sizeof(int)); // �ַ�������
		gzwrite(f, (LPCTSTR)i, len * sizeof(TCHAR)); // �ַ���
	}

	// ��������
	WriteRegexTexts(f, g_whiteContent);

	gzwrite(f, &g_scanInterval, sizeof(int));		// ɨ����
	gzwrite(f, &g_banID, sizeof(BOOL));				// ��ID
	gzwrite(f, &g_banDuration, sizeof(int));		// ���ʱ��
	BOOL banIP = FALSE;
	gzwrite(f, &banIP, sizeof(BOOL));				// ��IP
	gzwrite(f, &g_trigCount, sizeof(int));			// ���Υ�����
	gzwrite(f, &g_onlyScanTitle, sizeof(BOOL));		// ֻɨ�����
	gzwrite(f, &g_deleteInterval, sizeof(float));	// ɾ�����
	gzwrite(f, &g_confirm, sizeof(BOOL));			// ����ǰ��ʾ
	gzwrite(f, &g_scanPageCount, sizeof(int));		// ɨ�����ҳ��
	gzwrite(f, &g_briefLog, sizeof(BOOL));			// ֻ���ɾ�����
	gzwrite(f, &g_delete, sizeof(BOOL));			// ɾ��
	gzwrite(f, &g_threadCount, sizeof(int));		// �߳���

	gzclose(f);
}

static inline void ReadIDList(const gzFile& f, set<__int64>& IDList)
{
	int size;
	if (gzread(f, &size, sizeof(int)) == sizeof(int) && 0 < size && size < 100000) // ����
	{
		__int64 id;
		for (int i = 0; i < size; i++)
		{
			gzread(f, &id, sizeof(__int64));
			IDList.insert(id);
		}
	}
}

static inline void WriteIDList(const gzFile& f, const set<__int64>& IDList)
{
	int len = IDList.size();
	gzwrite(f, &len, sizeof(int)); // ����
	for (auto& i : IDList)
		gzwrite(f, &i, sizeof(__int64)); // ID
}

// ���浱ǰ�˺�����
void SaveCurrentUserProfile()
{
	// ����Ŀ¼
	if (!PathFileExists(USERS_PATH))
		CreateDirectory(USERS_PATH, NULL);
	if (!PathFileExists(USERS_PATH + g_currentUser))
		CreateDirectory(USERS_PATH + g_currentUser, NULL);

	// ����Cookie
	gzFile f = gzopen_w(COOKIE_PATH, "wb");
	if (f != NULL)
	{
		int len = g_cookie.GetLength();
		gzwrite(f, &len, sizeof(int)); // �ַ�������
		gzwrite(f, (LPCTSTR)g_cookie, len * sizeof(TCHAR)); // �ַ���
		gzclose(f);
	}

	// ������ʷ�ظ�������ID
	f = gzopen_w(CACHE_PATH, "wb");
	if (f != NULL)
	{
		int len = g_reply.size();
		gzwrite(f, &len, sizeof(int)); // ����
		for (auto& i : g_reply)
		{
			gzwrite(f, &i.first, sizeof(__int64)); // ����ID
			gzwrite(f, &i.second, sizeof(int)); // �ظ���
		}
		WriteIDList(f, g_initIgnoredTID);
		WriteIDList(f, g_initIgnoredPID);
		WriteIDList(f, g_initIgnoredLZLID);
		gzclose(f);
	}
}

// ���õ�ǰ�˺�
void SetCurrentUser(LPCTSTR userName)
{
	// ���浱ǰ�˺�����
	if (g_currentUser != _T(""))
		SaveCurrentUserProfile();

	// ��������·��
	g_currentUser = userName;
	CURRENT_USER_PATH = USERS_PATH + userName;
	USER_PROFILE_PATH = CURRENT_USER_PATH + _T("\\options.tb");
	COOKIE_PATH = CURRENT_USER_PATH + _T("\\ck.tb");
	CACHE_PATH = CURRENT_USER_PATH + _T("\\cache.tb");

	// ��ȡ����
	TCHAR buffer[260];
	// ����
	GetPrivateProfileString(_T("Setting"), _T("Option"), _T("Ĭ��"), g_currentOption.GetBuffer(MAX_PATH), MAX_PATH, USER_PROFILE_PATH);
	g_currentOption.ReleaseBuffer();
	ReadOptions(OPTIONS_PATH + g_currentOption + _T(".tb"));
	// ������
	GetPrivateProfileString(_T("Setting"), _T("ForumName"), _T(""), buffer, _countof(buffer), USER_PROFILE_PATH);
	((CTiebaManagerDlg*)AfxGetApp()->m_pMainWnd)->m_forumNameEdit.SetWindowText(buffer);
	// Cookie
	gzFile f = gzopen_w(COOKIE_PATH, "rb");
	if (f != NULL)
	{
		int size;
		if (gzread(f, &size, sizeof(int)) == sizeof(int) && 0 < size && size < 1024 * 1024) // �ַ�������
		{
			gzread(f, g_cookie.GetBuffer(size), size * sizeof(TCHAR)); // �ַ���
			g_cookie.ReleaseBuffer(size);
		}
		gzclose(f);
	}

	// ��ʷ�ظ�������ID
	f = gzopen_w(CACHE_PATH, "rb");
	if (f != NULL)
	{
		int size;
		if (gzread(f, &size, sizeof(int)) == sizeof(int) && 0 < size && size < 100000) // ����
		{
			__int64 tid;
			int reply;
			for (int i = 0; i < size; i++)
			{
				gzread(f, &tid, sizeof(__int64));
				gzread(f, &reply, sizeof(int));
				g_reply[tid] = reply;
			}
		}
		ReadIDList(f, g_initIgnoredTID);
		g_ignoredTID = g_initIgnoredTID;
		ReadIDList(f, g_initIgnoredPID);
		g_ignoredPID = g_initIgnoredPID;
		ReadIDList(f, g_initIgnoredLZLID);
		g_ignoredLZLID = g_initIgnoredLZLID;
		gzclose(f);
	}
}

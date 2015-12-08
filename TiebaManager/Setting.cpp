#include "stdafx.h"
#include "Setting.h"
#include "TiebaManagerDlg.h"
#include "TiebaVariable.h"
#include "ScanImage.h"


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
int		g_banTrigCount;		// ���Υ�����
BOOL	g_onlyScanTitle;	// ֻɨ�����
float	g_deleteInterval;	// ɾ�����
BOOL	g_confirm;			// ����ǰ��ʾ
int		g_scanPageCount;	// ɨ�����ҳ��
BOOL	g_briefLog;			// ֻ���ɾ�����
BOOL	g_delete;			// ɾ��
int		g_threadCount;		// �߳���
CString	g_banReason;		// ���ԭ��
CString	g_imageDir;			// Υ��ͼƬĿ¼
double	g_SSIMThreshold;	// ��ֵ
BOOL	g_defriend;			// ����
int		g_defriendTrigCount; // ����Υ�����
BOOL	g_autoSaveLog;		// �Զ�������־
vector<RegexText>	g_keywords;		// Υ������
vector<RegexText>	g_blackList;	// �����û�
set<CString>		g_whiteList;	// �����û�
vector<RegexText>	g_whiteContent;	// ��������
vector<NameImage>	g_images;		// Υ��ͼƬ
set<CString>		g_trustedThread;// ��������

CCriticalSection g_optionsLock; // �����ٽ���


static inline void ReadRegexTexts(const gzFile& f, vector<RegexText>& vec)
{
	int intBuf;
	gzread(f, &intBuf, sizeof(int)); // ����
	vec.resize(intBuf);
	for (RegexText& i : vec)
	{
		gzread(f, &intBuf, sizeof(int)); // ������
		i.isRegex = intBuf != 0;
		ReadText(f, i.text);
		i.regexp = i.isRegex ? i.text : _T("");
	}
}

static inline void WriteRegexTexts(const gzFile& f, vector<RegexText>& vec)
{
	int intBuf;
	gzwrite(f, &(intBuf = vec.size()), sizeof(int)); // ����
	for (const RegexText& i : vec)
	{
		gzwrite(f, &(intBuf = i.isRegex ? 1 : 0), sizeof(int)); // ������
		WriteText(f, i.text);
	}
}

// ������
void ReadOptions(LPCTSTR path)
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

	g_optionsLock.Lock();

	// Υ������
	ReadRegexTexts(f, g_keywords);

	// �����û�
	ReadRegexTexts(f, g_blackList);

	// �����û�
	int intBuf;
	gzread(f, &intBuf, sizeof(int)); // ����
	for (int i = 0; i < intBuf; i++)
	{
		ReadText(f, strBuf);
		g_whiteList.insert(strBuf);
	}

	// ��������
	ReadRegexTexts(f, g_whiteContent);

	gzread(f, &g_scanInterval, sizeof(int));	// ɨ����
	gzread(f, &g_banID, sizeof(BOOL));			// ��ID
	gzread(f, &g_banDuration, sizeof(int));		// ���ʱ��
	BOOL banIP;
	gzread(f, &banIP, sizeof(BOOL));			// ��IP
	gzread(f, &g_banTrigCount, sizeof(int));	// ���Υ�����
	gzread(f, &g_onlyScanTitle, sizeof(BOOL));	// ֻɨ�����
	gzread(f, &g_deleteInterval, sizeof(float));// ɾ�����
	gzread(f, &g_confirm, sizeof(BOOL));		// ����ǰ��ʾ
	gzread(f, &g_scanPageCount, sizeof(int));	// ɨ�����ҳ��
	gzread(f, &g_briefLog, sizeof(BOOL));		// ֻ���ɾ�����
	if (gzread(f, &g_delete, sizeof(BOOL)) != sizeof(BOOL))			// ɾ��
		g_delete = TRUE;
	if (gzread(f, &g_threadCount, sizeof(int)) != sizeof(int))		// �߳���
		g_threadCount = 2;
	ReadText(f, g_banReason);										// ���ԭ��
	if (!ReadText(f, g_imageDir))									// Υ��ͼƬĿ¼
		g_images.clear();
	else
		ReadImages(g_imageDir);										// Υ��ͼƬ
	if (gzread(f, &g_SSIMThreshold, sizeof(double)) != sizeof(double))	// ��ֵ
		g_SSIMThreshold = 2.43;

	// ��������
	g_trustedThread.clear();
	if (gzread(f, &intBuf, sizeof(int)) == sizeof(int)) // ����
		for (int i = 0; i < intBuf; i++)
		{
			ReadText(f, strBuf);
			g_trustedThread.insert(strBuf);
		}

	if (gzread(f, &g_defriend, sizeof(BOOL)) != sizeof(BOOL))		// ����
		g_defriend = FALSE;
	if (gzread(f, &g_defriendTrigCount, sizeof(int)) != sizeof(int)) // ����Υ�����
		g_defriendTrigCount = 5;
	if (gzread(f, &g_autoSaveLog, sizeof(BOOL)) != sizeof(BOOL))	// �Զ�������־
		g_autoSaveLog = FALSE;

	g_optionsLock.Unlock();

	gzclose(f);
	return;

UseDefaultOptions:
	g_optionsLock.Lock();
	g_keywords.clear();			// Υ������
	g_blackList.clear();		// �����û�
	g_whiteList.clear();		// �����û�
	g_whiteContent.clear();		// ��������
	g_scanInterval = 5;			// ɨ����
	g_banID = FALSE;			// ��ID
	g_banDuration = 1;			// ���ʱ��
	g_banTrigCount = 1;			// ���Υ�����
	g_onlyScanTitle = FALSE;	// ֻɨ�����
	g_deleteInterval = 2.0f;	// ɾ�����
	g_confirm = TRUE;			// ����ǰ��ʾ
	g_scanPageCount = 1;		// ɨ�����ҳ��
	g_briefLog = FALSE;			// ֻ���ɾ�����
	g_delete = TRUE;			// ɾ��
	g_threadCount = 2;			// �߳���
	g_banReason = _T("");		// ���ԭ��
	g_imageDir = _T("");		// Υ��ͼƬĿ¼
	g_images.clear();			// Υ��ͼƬ
	g_SSIMThreshold = 2.43;		// ��ֵ
	g_trustedThread.clear();	// ��������
	g_defriend = FALSE;			// ����
	g_defriendTrigCount = 5;	// ����Υ�����
	g_autoSaveLog = FALSE;		// �Զ�������־
	g_optionsLock.Unlock();
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
		WriteText(f, i);

	// ��������
	WriteRegexTexts(f, g_whiteContent);

	gzwrite(f, &g_scanInterval, sizeof(int));		// ɨ����
	gzwrite(f, &g_banID, sizeof(BOOL));				// ��ID
	gzwrite(f, &g_banDuration, sizeof(int));		// ���ʱ��
	BOOL banIP = FALSE;
	gzwrite(f, &banIP, sizeof(BOOL));				// ��IP
	gzwrite(f, &g_banTrigCount, sizeof(int));		// ���Υ�����
	gzwrite(f, &g_onlyScanTitle, sizeof(BOOL));		// ֻɨ�����
	gzwrite(f, &g_deleteInterval, sizeof(float));	// ɾ�����
	gzwrite(f, &g_confirm, sizeof(BOOL));			// ����ǰ��ʾ
	gzwrite(f, &g_scanPageCount, sizeof(int));		// ɨ�����ҳ��
	gzwrite(f, &g_briefLog, sizeof(BOOL));			// ֻ���ɾ�����
	gzwrite(f, &g_delete, sizeof(BOOL));			// ɾ��
	gzwrite(f, &g_threadCount, sizeof(int));		// �߳���
	WriteText(f, g_banReason);						// ���ԭ��
	WriteText(f, g_imageDir);						// Υ��ͼƬĿ¼
	gzwrite(f, &g_SSIMThreshold, sizeof(double));	// ��ֵ

	// ��������
	gzwrite(f, &(intBuf = g_trustedThread.size()), sizeof(int)); // ����
	for (const CString& i : g_trustedThread)
		WriteText(f, i);

	gzwrite(f, &g_defriend, sizeof(BOOL));			// ����
	gzwrite(f, &g_defriendTrigCount, sizeof(int));	// ����Υ�����
	gzwrite(f, &g_autoSaveLog, sizeof(BOOL));		// �Զ�������־

	gzclose(f);
}

static inline void ReadIDSet(const gzFile& f, set<__int64>& IDSet)
{
	IDSet.clear();
	int size;
	if (gzread(f, &size, sizeof(int)) == sizeof(int) && 0 < size && size < 100000) // ����
	{
		__int64 id;
		for (int i = 0; i < size; i++)
		{
			gzread(f, &id, sizeof(__int64));
			IDSet.insert(id);
		}
	}
}

static inline void ReadTextSet(const gzFile& f, set<CString>& TextSet)
{
	TextSet.clear();
	int size;
	if (gzread(f, &size, sizeof(int)) == sizeof(int) && 0 < size && size < 100000) // ����
	{
		CString text;
		for (int i = 0; i < size; i++)
		{
			ReadText(f, text);
			TextSet.insert(text);
		}
	}
}

static inline void WriteIDSet(const gzFile& f, const set<__int64>& IDSet)
{
	int len = IDSet.size();
	gzwrite(f, &len, sizeof(int)); // ����
	for (auto& i : IDSet)
		gzwrite(f, &i, sizeof(__int64)); // ID
}

static inline void WriteTextSet(const gzFile& f, const set<CString>& TextSet)
{
	int len = TextSet.size();
	gzwrite(f, &len, sizeof(int)); // ����
	for (auto& i : TextSet)
		WriteText(f, i); // �ı�
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
		WriteText(f, g_cookie);
		gzclose(f);
	}

	// ������ʷ�ظ�������ID��
	f = gzopen_w(CACHE_PATH, "wb");
	if (f != NULL)
	{
		int len;
		// ��ʷ�ظ�
		gzwrite(f, &(len = g_reply.size()), sizeof(int)); // ����
		for (auto& i : g_reply)
		{
			gzwrite(f, &i.first, sizeof(__int64)); // ����ID
			gzwrite(f, &i.second, sizeof(int)); // �ظ���
		}
		// ����ID
		WriteIDSet(f, g_initIgnoredTID);
		WriteIDSet(f, g_initIgnoredPID);
		WriteIDSet(f, g_initIgnoredLZLID);
		// Υ�����
		gzwrite(f, &(len = g_userTrigCount.size()), sizeof(int)); // ����
		for (auto& i : g_userTrigCount)
		{
			WriteText(f, i.first); // �û���
			gzwrite(f, &i.second, sizeof(int)); // Υ�����
		}
		// �����û�
		WriteTextSet(f, g_defriendedUser);
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
		ReadText(f, g_cookie);
		gzclose(f);
	}

	// ��ʷ�ظ�������ID��
	f = gzopen_w(CACHE_PATH, "rb");
	if (f != NULL)
	{
		int size;
		// ��ʷ�ظ�
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
		// ����ID
		ReadIDSet(f, g_initIgnoredTID);
		g_ignoredTID = g_initIgnoredTID;
		ReadIDSet(f, g_initIgnoredPID);
		g_ignoredPID = g_initIgnoredPID;
		ReadIDSet(f, g_initIgnoredLZLID);
		g_ignoredLZLID = g_initIgnoredLZLID;
		// Υ�����
		if (gzread(f, &size, sizeof(int)) == sizeof(int) && 0 < size && size < 100000) // ����
		{
			CString userName;
			int count;
			for (int i = 0; i < size; i++)
			{
				ReadText(f, userName);
				gzread(f, &count, sizeof(int));
				g_userTrigCount[userName] = count;
			}
		}
		// �����û�
		ReadTextSet(f, g_defriendedUser);
		gzclose(f);
	}
}

#include "stdafx.h"
#include "Setting.h"
#include <zlib.h>


// �����ļ�·��
CString		PROFILE_PATH = _T("\\options.tb");
CString		COOKIE_PATH = _T("ck.tb");
CString		CACHE_PATH = _T("cache.tb");

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
		i.text.ReleaseBuffer();
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
		i.ReleaseBuffer();
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

	gzclose(f);
}

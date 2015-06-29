#pragma once
#include <vector>
using std::vector;
#include <regex>
using std::wregex;
#include <zlib.h>
struct ImageFeature;



// �����ļ�·��
extern CString	ALL_PROFILE_PATH;	// ��������ʱ��ʼ��
extern CString	USER_PROFILE_PATH;	// ȷ������ʱ��ʼ��
extern CString	OPTIONS_PATH;
extern CString	USERS_PATH;			// ȷ������ʱ��ʼ��
extern CString	CURRENT_USER_PATH;	// ȷ������ʱ��ʼ��
extern CString	COOKIE_PATH;		// ȷ������ʱ��ʼ��
extern CString	CACHE_PATH;			// ȷ������ʱ��ʼ��

struct RegexText
{
	BOOL isRegex;
	CString text;
	wregex regexp;
};

extern CString	g_currentUser;		// ��ǰ�˺�
extern BOOL		g_autoUpdate;		// �Զ�����

// ����
extern CString	g_currentOption;	// ��ǰ����
extern int		g_scanInterval;		// ɨ����
extern BOOL		g_banID;			// ��ID
extern int		g_banDuration;		// ���ʱ��
extern int		g_trigCount;		// ���Υ�����
extern BOOL		g_onlyScanTitle;	// ֻɨ�����
extern float	g_deleteInterval;	// ɾ�����
extern BOOL		g_confirm;			// ����ǰ��ʾ
extern int		g_scanPageCount;	// ɨ�����ҳ��
extern BOOL		g_briefLog;			// ֻ���ɾ�����
extern BOOL		g_delete;			// ɾ��
extern int		g_threadCount;		// �߳���
extern CString	g_banReason;		// ���ԭ��
extern CString g_imageDir;			// Υ��ͼƬĿ¼
extern vector<RegexText>	g_keywords;		// Υ������
extern vector<RegexText>	g_blackList;	// �����û�
extern vector<CString>		g_whiteList;	// �����û�
extern vector<RegexText>	g_whiteContent;	// ��������
extern vector<ImageFeature>	g_imageFeatures; // ͼƬ����

extern CCriticalSection g_optionsLock; // �ж�Υ���õ��ٽ���


// ���ַ���
inline BOOL ReadText(const gzFile& f, CString& text)
{
	int size;
	if (gzread(f, &size, sizeof(int)) != sizeof(int) || size < 0 || size > 100000) // �ַ�������
	{
		text = _T("");
		return FALSE;
	}
	if (gzread(f, text.GetBuffer(size), size * sizeof(TCHAR)) != size * sizeof(TCHAR)) // �ַ���
	{
		text.ReleaseBuffer(0);
		return FALSE;
	}
	text.ReleaseBuffer(size);
	return TRUE;
}
// д�ַ���
inline void WriteText(const gzFile& f, const CString& text)
{
	int size = text.GetLength();
	gzwrite(f, &size, sizeof(int)); // �ַ�������
	gzwrite(f, (LPCTSTR)text, size * sizeof(TCHAR)); // �ַ���
}
void ReadOptions(LPCTSTR path);
void WriteOptions(LPCTSTR path);
// ���浱ǰ�˺�����
void SaveCurrentUserProfile();
// ���õ�ǰ�˺�
void SetCurrentUser(LPCTSTR userName);

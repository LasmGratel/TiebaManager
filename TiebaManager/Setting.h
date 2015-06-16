#pragma once
#include <vector>
using std::vector;
#include <regex>
using std::wregex;


// �����ļ�·��
extern CString	PROFILE_PATH;
const CString	OPTIONS_PATH = _T("Option\\");
const TCHAR		COOKIE_PATH[] = _T("ck.tb");
const TCHAR		REPLY_PATH[] = _T("reply.tb");

struct RegexText
{
	BOOL isRegex;
	CString text;
	wregex regexp;
};


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
extern vector<RegexText>	g_keywords;		// Υ������
extern vector<RegexText>	g_blackList;	// �����û�
extern vector<CString>		g_whiteList;	// �����û�
extern vector<RegexText>	g_whiteContent;	// ��������

extern CCriticalSection g_optionsLock; // �ж�Υ���õ��ٽ���


void ReadOptions(LPCTSTR path);
void WriteOptions(LPCTSTR path);

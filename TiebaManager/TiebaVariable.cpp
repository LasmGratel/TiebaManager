#include "stdafx.h"
#include "TiebaVariable.h"
#include "TiebaOperate.h"


// ���ɡ��û���Ϣ
CString g_forumName;
CString g_encodedForumName;
CString g_forumID;
CString g_cookie;
CString g_tbs;

// ɨ��
volatile BOOL g_stopScanFlag = FALSE;
CWinThread* g_scanThread = NULL;
set<__int64> g_initIgnoredTID; // ��ɾ������ID(�ֶ�����)��Ҫд���ļ�
set<__int64> g_initIgnoredPID; // ��ɾ������ID(�ֶ�����)��Ҫд���ļ�
set<__int64> g_initIgnoredLZLID; // ��ɾ��¥��¥ID(�ֶ�����)��Ҫд���ļ�
set<__int64> g_ignoredTID; // ��ɾ������ID(��ɨ����Υ��)
set<__int64> g_ignoredPID; // ��ɾ������ID(��ɨ����Υ��)
set<__int64> g_ignoredLZLID; // ��ɾ��¥��¥ID(��ɨ����Υ��)
set<__int64> g_deletedTID; // ��ɾ������ID
map<__int64, int> g_reply; // ����Ļظ�����Ҫд���ļ�
map<CString, int> g_userTrigCount; // ĳ�û�Υ�������Ҫд���ļ�
set<CString> g_bannedUser; // �ѷ���û�
set<CString> g_defriendedUser; // �����ڵ��û���Ҫд���ļ�

// ����
CWinThread* g_confirmThread = NULL;
CWinThread* g_operateThread = NULL;

#pragma once


// ���ɡ��û���Ϣ
extern CString g_forumName;
extern CString g_encodedForumName;
extern CString g_forumID;
extern CString g_cookie;
extern CString g_tbs; // ���û�ȡtbs��ַ��http://tieba.baidu.com/dc/common/tbs

// ɨ��
extern volatile BOOL g_stopScanFlag;
extern CWinThread* g_scanThread;
extern set<__int64> g_initIgnoredTID; // ��ɾ������ID(�ֶ�����)��Ҫд���ļ�
extern set<__int64> g_initIgnoredPID; // ��ɾ������ID(�ֶ�����)��Ҫд���ļ�
extern set<__int64> g_initIgnoredLZLID; // ��ɾ��¥��¥ID(�ֶ�����)��Ҫд���ļ�
extern set<__int64> g_ignoredTID; // ��ɾ������ID(��ɨ����Υ��)
extern set<__int64> g_ignoredPID; // ��ɾ������ID(��ɨ����Υ��)
extern set<__int64> g_ignoredLZLID; // ��ɾ��¥��¥ID(��ɨ����Υ��)
extern set<__int64> g_deletedTID; // ��ɾ������ID
extern map<__int64, int> g_reply; // ����Ļظ�����Ҫд���ļ�
extern map<CString, int> g_userTrigCount; // ĳ�û�Υ�������Ҫд���ļ�
extern set<CString> g_bannedUser; // �ѷ���û�
extern set<CString> g_defriendedUser; // �����ڵ��û���Ҫд���ļ�

// ����
extern CWinThread* g_confirmThread;
extern CWinThread* g_operateThread;

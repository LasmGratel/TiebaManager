#pragma once
#include "Setting.h"


// ���ɡ��û���Ϣ
extern CString g_forumName;
extern CString g_encodedForumName;
extern CString g_forumID;
extern CString g_cookie;
extern CString g_tbs; // ���û�ȡtbs��ַ��http://tieba.baidu.com/dc/common/tbs

// ɨ��
extern volatile BOOL g_stopScanFlag;
extern CWinThread* g_scanThread;
class CUserCache
{
public:
	COption<set<__int64> > m_initIgnoredTID; // ��ɾ������ID(�ֶ�����)��Ҫд���ļ�
	COption<set<__int64> > m_initIgnoredPID; // ��ɾ������ID(�ֶ�����)��Ҫд���ļ�
	COption<set<__int64> > m_initIgnoredLZLID; // ��ɾ��¥��¥ID(�ֶ�����)��Ҫд���ļ�
	set<__int64> m_ignoredTID; // ��ɾ������ID(��ɨ����Υ��)
	set<__int64> m_ignoredPID; // ��ɾ������ID(��ɨ����Υ��)
	set<__int64> m_ignoredLZLID; // ��ɾ��¥��¥ID(��ɨ����Υ��)
	set<__int64> m_deletedTID; // ��ɾ������ID
	COption<map<__int64, int> > m_reply; // ����Ļظ�����Ҫд���ļ�
	COption<map<CString, int> > m_userTrigCount; // ĳ�û�Υ�������Ҫд���ļ�
	COption<set<CString> > m_bannedUser; // �ѷ���û�
	COption<set<CString> > m_defriendedUser; // �����ڵ��û���Ҫд���ļ�

	CUserCache()
		: m_initIgnoredTID("IgnoredTID"),
		m_initIgnoredPID("IgnoredPID"),
		m_initIgnoredLZLID("IgnoredLZLID"),
		m_reply("Reply"),
		m_userTrigCount("TrigCount"),
		m_bannedUser("BannedUser"),
		m_defriendedUser("DefriendedUser")
	{}
	BOOL Load(LPCTSTR path);
	BOOL Save(LPCTSTR path) const;
};
extern CUserCache g_userCache;

// ����
extern CWinThread* g_confirmThread;
extern CWinThread* g_operateThread;

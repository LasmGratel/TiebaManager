#pragma once
#include <TBMConfig.h>


// Cookie�ļ�
class CCookieConfig : public CConfigBase
{
public:
	COption<CString> m_cookie;

	CCookieConfig() : CConfigBase("Cookie"),
		m_cookie("Cookie")
	{
		m_options.push_back(&m_cookie);
	}
};
extern CCookieConfig g_cookieConfig;

// ɨ��
extern volatile BOOL g_stopScanFlag;
extern CWinThread* g_scanThread;
// ����
class CUserCache : public CConfigBase
{
public:
	COption<set<__int64> > m_initIgnoredTID;		// ��ɾ������ID(�ֶ�����)��Ҫд���ļ�
	COption<set<__int64> > m_initIgnoredPID;		// ��ɾ������ID(�ֶ�����)��Ҫд���ļ�
	COption<set<__int64> > m_initIgnoredLZLID;		// ��ɾ��¥��¥ID(�ֶ�����)��Ҫд���ļ�
	set<__int64> m_ignoredTID;						// ��ɾ������ID(��ɨ����Υ��)
	set<__int64> m_ignoredPID;						// ��ɾ������ID(��ɨ����Υ��)
	set<__int64> m_ignoredLZLID;					// ��ɾ��¥��¥ID(��ɨ����Υ��)
	set<__int64> m_deletedTID;						// ��ɾ������ID
	COption<map<__int64, int> > m_reply;			// ����Ļظ�����Ҫд���ļ�
	COption<map<__int64, CString> > m_lastAuthor;	// ��������ظ����ߣ�Ҫд���ļ�
	COption<map<CString, int> > m_userTrigCount;	// ĳ�û�Υ�������Ҫд���ļ�
	COption<set<CString> > m_bannedUser;			// �ѷ���û���Ҫд���ļ�
	COption<set<CString> > m_defriendedUser;		// �����ڵ��û���Ҫд���ļ�

	CUserCache()
		: CConfigBase("Cache"),
		m_initIgnoredTID("IgnoredTID"),
		m_initIgnoredPID("IgnoredPID"),
		m_initIgnoredLZLID("IgnoredLZLID"),
		m_reply("Reply"),
		m_lastAuthor("LastAuthor"),
		m_userTrigCount("TrigCount"),
		m_bannedUser("BannedUser"),
		m_defriendedUser("DefriendedUser")
	{
		m_options.push_back(&m_initIgnoredTID);
		m_options.push_back(&m_initIgnoredPID);
		m_options.push_back(&m_initIgnoredLZLID);
		m_options.push_back(&m_reply);
		m_options.push_back(&m_lastAuthor);
		m_options.push_back(&m_userTrigCount);
		m_options.push_back(&m_bannedUser);
		m_options.push_back(&m_defriendedUser);
	}

	void PostChange()
	{
		m_ignoredTID = m_initIgnoredTID;
		m_ignoredPID = m_initIgnoredPID;
		m_ignoredLZLID = m_initIgnoredLZLID;
		m_deletedTID.clear();
	}
};
extern CUserCache g_userCache;

// ����
extern CWinThread* g_confirmThread;
extern CWinThread* g_operateThread;

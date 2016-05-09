#pragma once
#include <ConfigFile.h>
#include <StringHelper.h>
#include <opencv2\core\mat.hpp>
#include "Update.h"


// �����ļ�·��
extern CString	GLOBAL_CONFIG_PATH;	// ��������ʱ��ʼ��
extern CString	USER_PROFILE_PATH;	// ȷ������ʱ��ʼ��
extern CString	OPTIONS_PATH;
extern CString	USERS_PATH;			// ȷ������ʱ��ʼ��
extern CString	CURRENT_USER_PATH;	// ȷ������ʱ��ʼ��
extern CString	COOKIE_PATH;		// ȷ������ʱ��ʼ��
extern CString	CACHE_PATH;			// ȷ������ʱ��ʼ��


// ȫ������
class CGlobalConfig : public CConfigBase
{
public:
	COption<BOOL> m_firstRun;			// ��һ������
	COption<BOOL> m_firstRunAfterUpdate;// ���º��һ������
	COption<CString> m_currentUser;		// ��ǰ�˺�
	COption<BOOL> m_autoUpdate;			// �Զ�����

	CGlobalConfig()
		: CConfigBase("Global"),
		m_firstRun("FirstRun", TRUE),
		m_firstRunAfterUpdate("FirstRunAfter" + CStringA(UPDATE_CURRENT_VERSION_A), TRUE),
		m_currentUser("UserName", _T("[NULL]"), [](const CString& value)->BOOL{ return value != _T("") && PathFileExists(USERS_PATH + value + _T("\\ck.xml")); }),
		m_autoUpdate("AutoUpdate", TRUE)
	{
		m_options.push_back(&m_firstRun);
		m_options.push_back(&m_firstRunAfterUpdate);
		m_options.push_back(&m_currentUser);
		m_options.push_back(&m_autoUpdate);
	}
};
extern CGlobalConfig g_globalConfig;

// �û�����
class CUserConfig : public CConfigBase
{
public:
	COption<CString> m_plan;		// ��ǰ����
	COption<CString> m_forumName;	// ������

	CUserConfig()
		: CConfigBase("User"),
		m_plan("Plan", _T("Ĭ��")),
		m_forumName("ForumName")
	{
		m_options.push_back(&m_plan);
		m_options.push_back(&m_forumName);
	}
};
extern CUserConfig g_userConfig;

// ����
class CPlan : public CConfigBase
{
public:
	struct Keyword : RegexText
	{
		BOOL forceToConfirm;	// ǿ��ȷ��
		int trigCount;			// ��������
	};

	struct NameImage
	{
		CString name;
		cv::Mat img;
	};
	
public:
	CCriticalSection m_optionsLock; // �����ٽ���
	BOOL m_updateImage; // ��ȡ�����Υ��ͼƬ

	COption<int>		m_scanInterval;			// ɨ����
	COption<BOOL>		m_onlyScanTitle;		// ֻɨ�����
	COption<int>		m_scanPageCount;		// ɨ�����ҳ��
	COption<BOOL>		m_briefLog;				// ֻ���ɾ�����
	COption<int>		m_threadCount;			// �߳���
	COption<BOOL>		m_autoSaveLog;			// �Զ�������־
	COption<int>		m_illegalLevel;			// Υ��ȼ�
	COption<BOOL>		m_delete;				// ɾ��
	COption<BOOL>		m_banID;				// ��ID
	COption<BOOL>		m_defriend;				// ����
	COption<float>		m_deleteInterval;		// ɾ�����
	COption<int>		m_banDuration;			// ���ʱ��
	COption<CString>	m_banReason;			// ���ԭ��
	COption<int>		m_banTrigCount;			// ���Υ�����
	COption<int>		m_defriendTrigCount;	// ����Υ�����
	COption<BOOL>		m_confirm;				// ����ǰ��ʾ
	COption<BOOL>		m_wapBanInterface;		// �����WAP�ӿ�
	COption<BOOL>		m_autoLoopBan;			// �Զ�ѭ����
	COption<vector<Keyword> >	m_keywords;		// Υ������
	vector<NameImage>			m_images;		// Υ��ͼƬ
	COption<CString>	m_imageDir;				// Υ��ͼƬĿ¼
	COption<double>		m_SSIMThreshold;		// ��ֵ
	COption<vector<RegexText> >	m_blackList;	// �����û�
	COption<set<CString> >		m_whiteList;	// �����û�
	COption<vector<RegexText> >	m_whiteContent;	// ��������
	COption<set<CString> >		m_trustedThread;// ��������

	CPlan();
	void OnChange(){ m_optionsLock.Lock(); }
	void PostChange();
};
extern CPlan g_plan;
#ifdef TIEBA_MANAGER_EXPORTS
#undef HELPER_API
#define HELPER_API __declspec(dllexport)
#endif
DECLEAR_BOTH(CPlan::Keyword)
DECLEAR_BOTH(vector<CPlan::Keyword>)
#ifdef TIEBA_MANAGER_EXPORTS
#undef HELPER_API
#define HELPER_API __declspec(dllimport)
#endif


// ���浱ǰ�˺�����
void SaveCurrentUserProfile();
// ���õ�ǰ�˺�
void SetCurrentUser(const CString& userName, BOOL save);

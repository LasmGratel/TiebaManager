#pragma once
#include "TBMCommon.h"
#include <ConfigFile.h>
#include <StringHelper.h>
#include <opencv2\core\mat.hpp>


// �����ļ�·��
extern TIEBA_MANAGER_API CString GLOBAL_CONFIG_PATH;	// ��������ʱ��ʼ��
extern TIEBA_MANAGER_API CString USER_PROFILE_PATH;		// ȷ������ʱ��ʼ��
extern TIEBA_MANAGER_API CString OPTIONS_PATH;
extern TIEBA_MANAGER_API CString USERS_PATH;			// ��������ʱ��ʼ��
extern TIEBA_MANAGER_API CString CURRENT_USER_PATH;		// ȷ������ʱ��ʼ��
extern TIEBA_MANAGER_API CString COOKIE_PATH;			// ȷ������ʱ��ʼ��
extern TIEBA_MANAGER_API CString CACHE_PATH;			// ȷ������ʱ��ʼ��


// ȫ������
class TIEBA_MANAGER_API CGlobalConfig : public CConfigBase
{
public:
	COption<BOOL> m_firstRun;				// ��һ������
	COption<BOOL> m_firstRunAfterUpdate;	// ���º��һ������
	COption<CString> m_currentUser;			// ��ǰ�˺�
	COption<BOOL> m_autoUpdate;				// �Զ�����

	CGlobalConfig();
};
extern TIEBA_MANAGER_API CGlobalConfig g_globalConfig;

// �û�����
class TIEBA_MANAGER_API CUserConfig : public CConfigBase
{
public:
	COption<CString> m_plan;		// ��ǰ����
	COption<CString> m_forumName;	// ������

	CUserConfig();
};
extern TIEBA_MANAGER_API CUserConfig g_userConfig;

// ����
class TIEBA_MANAGER_API CPlan : public CConfigBase
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
extern TIEBA_MANAGER_API CPlan g_plan;

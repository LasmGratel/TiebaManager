#pragma once
#include <zlib.h>
#include "TypeHelper.h"
#include "Update.h"


// �����ļ�·��
extern CString	GLOBAL_CONFIG_PATH;	// ��������ʱ��ʼ��
extern CString	USER_PROFILE_PATH;	// ȷ������ʱ��ʼ��
extern CString	OPTIONS_PATH;
extern CString	USERS_PATH;			// ȷ������ʱ��ʼ��
extern CString	CURRENT_USER_PATH;	// ȷ������ʱ��ʼ��
extern CString	COOKIE_PATH;		// ȷ������ʱ��ʼ��
extern CString	CACHE_PATH;			// ȷ������ʱ��ʼ��


// Option��Config������ //////////////////////////////////////////////////////////////
namespace tinyxml2{ class XMLElement; }

class COptionBase
{
public:
	const CStringA m_name;

	COptionBase(const CStringA& name) : m_name(name) {}
	virtual ~COptionBase() {}

	virtual void UseDefault() = 0;
	virtual tinyxml2::XMLElement& operator << (tinyxml2::XMLElement& root) = 0;
	virtual tinyxml2::XMLElement& operator >> (tinyxml2::XMLElement& root) const = 0;
};

template <class T>
class COption : public COptionBase
{
public:
	const T m_default;
	T m_value;
	typedef BOOL(*IsValidFunc)(const T& value);
	const IsValidFunc IsValid;

	COption(const CStringA& name, IsValidFunc _isValid = [](const T&){ return TRUE; })
		: COptionBase(name), m_default(), IsValid(_isValid)
	{}
	COption(const CStringA& name, const T& _default, IsValidFunc _isValid = [](const T&){ return TRUE; })
		: COptionBase(name), m_default(_default), IsValid(_isValid)
	{}

	bool operator == (const COption&) const = delete;
	operator const T& () const{ return m_value; }
	operator T& (){ return m_value; }
	const T& operator * () const{ return m_value; }
	T& operator * (){ return m_value; }
	const T* operator -> () const{ return &m_value; }
	T* operator -> (){ return &m_value; }
	void UseDefault(){ m_value = m_default; }

	tinyxml2::XMLElement& operator << (tinyxml2::XMLElement& root);
	tinyxml2::XMLElement& operator >> (tinyxml2::XMLElement& root) const;
};

class CConfigBase
{
protected:
	vector<COptionBase*> m_options;

public:
	const LPCSTR m_name;

	CConfigBase(LPCSTR name) : m_name(name) {}
	virtual ~CConfigBase() {}

	virtual BOOL LoadOld(const CString& path) { return FALSE; }; // ������ʷ��������
	virtual BOOL Load(const CString& path);
	virtual BOOL Save(const CString& path) const;
	virtual void UseDefault();
	virtual void OnChange() {}
	virtual void PostChange() {}
};

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

	BOOL LoadOld(const CString& path)
	{
		*m_firstRun = GetPrivateProfileInt(_T("Setting"), _T("FirstRun"), TRUE, path) != FALSE;
		TCHAR buffer[260];
		GetPrivateProfileString(_T("Setting"), _T("UserName"), _T("[NULL]"), buffer, _countof(buffer), path);
		*m_currentUser = buffer;
		*m_autoUpdate = GetPrivateProfileInt(_T("Setting"), _T("AutoUpdate"), TRUE, path) != FALSE;
		return TRUE;
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

	BOOL LoadOld(const CString& path)
	{
		GetPrivateProfileString(_T("Setting"), _T("Option"), _T("Ĭ��"), m_plan->GetBuffer(MAX_PATH), MAX_PATH, path);
		m_plan->ReleaseBuffer();
		GetPrivateProfileString(_T("Setting"), _T("ForumName"), _T(""), m_forumName->GetBuffer(MAX_PATH), MAX_PATH, path);
		m_forumName->ReleaseBuffer();
		return TRUE;
	}
};
extern CUserConfig g_userConfig;

// ����
class CPlan : public CConfigBase
{
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
	COption<vector<RegexText> >	m_keywords;		// Υ������
	vector<NameImage>			m_images;		// Υ��ͼƬ
	COption<CString>	m_imageDir;				// Υ��ͼƬĿ¼
	COption<double>		m_SSIMThreshold;		// ��ֵ
	COption<vector<RegexText> >	m_blackList;	// �����û�
	COption<set<CString> >		m_whiteList;	// �����û�
	COption<vector<RegexText> >	m_whiteContent;	// ��������
	COption<set<CString> >		m_trustedThread;// ��������

	CPlan();
	BOOL LoadOld(const CString& path);
	void OnChange(){ m_optionsLock.Lock(); }
	void PostChange();
};
extern CPlan g_plan;


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

// ���浱ǰ�˺�����
void SaveCurrentUserProfile();
// ���õ�ǰ�˺�
void SetCurrentUser(const CString& userName, BOOL save);

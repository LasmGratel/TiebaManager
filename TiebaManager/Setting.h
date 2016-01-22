#pragma once
#include <zlib.h>
#include "TypeHelper.h"



// �����ļ�·��
extern CString	ALL_PROFILE_PATH;	// ��������ʱ��ʼ��
extern CString	USER_PROFILE_PATH;	// ȷ������ʱ��ʼ��
extern CString	OPTIONS_PATH;
extern CString	USERS_PATH;			// ȷ������ʱ��ʼ��
extern CString	CURRENT_USER_PATH;	// ȷ������ʱ��ʼ��
extern CString	COOKIE_PATH;		// ȷ������ʱ��ʼ��
extern CString	CACHE_PATH;			// ȷ������ʱ��ʼ��

extern CString	g_currentUser;		// ��ǰ�˺�
extern BOOL		g_autoUpdate;		// �Զ�����

// ����
extern CString	g_currentOption;	// ��ǰ����
extern int		g_scanInterval;		// ɨ����
extern BOOL		g_banID;			// ��ID
extern int		g_banDuration;		// ���ʱ��
extern int		g_banTrigCount;		// ���Υ�����
extern BOOL		g_onlyScanTitle;	// ֻɨ�����
extern float	g_deleteInterval;	// ɾ�����
extern BOOL		g_confirm;			// ����ǰ��ʾ
extern int		g_scanPageCount;	// ɨ�����ҳ��
extern BOOL		g_briefLog;			// ֻ���ɾ�����
extern BOOL		g_delete;			// ɾ��
extern int		g_threadCount;		// �߳���
extern CString	g_banReason;		// ���ԭ��
extern CString	g_imageDir;			// Υ��ͼƬĿ¼
extern double	g_SSIMThreshold;	// ��ֵ
extern BOOL		g_defriend;			// ����
extern int		g_defriendTrigCount; // ����Υ�����
extern BOOL		g_autoSaveLog;		// �Զ�������־
extern vector<RegexText>	g_keywords;		// Υ������
extern vector<RegexText>	g_blackList;	// �����û�
extern set<CString>			g_whiteList;	// �����û�
extern vector<RegexText>	g_whiteContent;	// ��������
extern vector<NameImage>	g_images;		// Υ��ͼƬ
extern set<CString>			g_trustedThread;// ��������

extern CCriticalSection g_optionsLock; // �����ٽ���


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


namespace tinyxml2
{
	class XMLElement;
}

class COptionBase
{
public:
	const LPCSTR m_name;

	COptionBase(LPCSTR name) : m_name(name) {}
	virtual ~COptionBase() {}

	virtual tinyxml2::XMLElement& operator << (tinyxml2::XMLElement& root) = 0;
	virtual tinyxml2::XMLElement& operator >> (tinyxml2::XMLElement& root) const = 0;
};

template <class T>
class COption : public COptionBase
{
public:
	const T m_default;
	T m_value;
	typedef BOOL (*IsValidFunc)(T& value);
	const IsValidFunc IsValid;
	
	COption(LPCSTR name, IsValidFunc _isValid = [](T&){ return TRUE; }) 
		: COptionBase(name), m_default(), IsValid(_isValid)
	{}
	COption(LPCSTR name, const T& _default, IsValidFunc _isValid = [](T&){ return TRUE; }) 
		: COptionBase(name), m_default(_default), IsValid(_isValid)
	{}

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

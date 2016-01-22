#include "stdafx.h"
#include "Setting.h"
#include <tinyxml2.h>
using namespace tinyxml2;
#include "TiebaManagerDlg.h"
#include "TiebaVariable.h"
#include "ScanImage.h"
#include "MiscHelper.h"


// �����ļ�·��
CString		ALL_PROFILE_PATH = _T("\\options.tb");	// ��������ʱ��ʼ��
CString		USER_PROFILE_PATH;						// ȷ������ʱ��ʼ��
CString		OPTIONS_PATH = _T("Option\\");
CString		USERS_PATH = _T("\\User\\");			// ��������ʱ��ʼ��
CString		CURRENT_USER_PATH;						// ȷ������ʱ��ʼ��
CString		COOKIE_PATH;							// ȷ������ʱ��ʼ��
CString		CACHE_PATH;								// ȷ������ʱ��ʼ��

CString	g_currentUser;		// ��ǰ�˺�
BOOL	g_autoUpdate;		// �Զ�����

// ����
CString	g_currentOption;	// ��ǰ����
int		g_scanInterval;		// ɨ����
BOOL	g_banID;			// ��ID
int		g_banDuration;		// ���ʱ��
int		g_banTrigCount;		// ���Υ�����
BOOL	g_onlyScanTitle;	// ֻɨ�����
float	g_deleteInterval;	// ɾ�����
BOOL	g_confirm;			// ����ǰ��ʾ
int		g_scanPageCount;	// ɨ�����ҳ��
BOOL	g_briefLog;			// ֻ���ɾ�����
BOOL	g_delete;			// ɾ��
int		g_threadCount;		// �߳���
CString	g_banReason;		// ���ԭ��
CString	g_imageDir;			// Υ��ͼƬĿ¼
double	g_SSIMThreshold;	// ��ֵ
BOOL	g_defriend;			// ����
int		g_defriendTrigCount; // ����Υ�����
BOOL	g_autoSaveLog;		// �Զ�������־
vector<RegexText>	g_keywords;		// Υ������
vector<RegexText>	g_blackList;	// �����û�
set<CString>		g_whiteList;	// �����û�
vector<RegexText>	g_whiteContent;	// ��������
vector<NameImage>	g_images;		// Υ��ͼƬ
set<CString>		g_trustedThread;// ��������

CCriticalSection g_optionsLock; // �����ٽ���


static inline void ReadRegexTexts(const gzFile& f, vector<RegexText>& vec)
{
	int intBuf;
	gzread(f, &intBuf, sizeof(int)); // ����
	vec.resize(intBuf);
	for (RegexText& i : vec)
	{
		gzread(f, &intBuf, sizeof(int)); // ������
		i.isRegex = intBuf != 0;
		ReadText(f, i.text);
		i.regexp = i.isRegex ? i.text : _T("");
	}
}

static inline void WriteRegexTexts(const gzFile& f, vector<RegexText>& vec)
{
	int intBuf;
	gzwrite(f, &(intBuf = vec.size()), sizeof(int)); // ����
	for (const RegexText& i : vec)
	{
		gzwrite(f, &(intBuf = i.isRegex ? 1 : 0), sizeof(int)); // ������
		WriteText(f, i.text);
	}
}

// ������
void ReadOptions(LPCTSTR path)
{
	CString strBuf;

	gzFile f = gzopen_w(path, "rb");
	if (f == NULL)
		goto UseDefaultOptions;

	// ͷ��
	char header[2];
	gzread(f, header, sizeof(header));
	if (header[0] != 'T' || header[1] != 'B')
	{
		gzclose(f);
		goto UseDefaultOptions;
	}

	g_optionsLock.Lock();

	// Υ������
	ReadRegexTexts(f, g_keywords);

	// �����û�
	ReadRegexTexts(f, g_blackList);

	// �����û�
	int intBuf;
	gzread(f, &intBuf, sizeof(int)); // ����
	for (int i = 0; i < intBuf; i++)
	{
		ReadText(f, strBuf);
		g_whiteList.insert(strBuf);
	}

	// ��������
	ReadRegexTexts(f, g_whiteContent);

	gzread(f, &g_scanInterval, sizeof(int));	// ɨ����
	gzread(f, &g_banID, sizeof(BOOL));			// ��ID
	gzread(f, &g_banDuration, sizeof(int));		// ���ʱ��
	BOOL banIP;
	gzread(f, &banIP, sizeof(BOOL));			// ��IP
	gzread(f, &g_banTrigCount, sizeof(int));	// ���Υ�����
	gzread(f, &g_onlyScanTitle, sizeof(BOOL));	// ֻɨ�����
	gzread(f, &g_deleteInterval, sizeof(float));// ɾ�����
	gzread(f, &g_confirm, sizeof(BOOL));		// ����ǰ��ʾ
	gzread(f, &g_scanPageCount, sizeof(int));	// ɨ�����ҳ��
	gzread(f, &g_briefLog, sizeof(BOOL));		// ֻ���ɾ�����
	if (gzread(f, &g_delete, sizeof(BOOL)) != sizeof(BOOL))			// ɾ��
		g_delete = TRUE;
	if (gzread(f, &g_threadCount, sizeof(int)) != sizeof(int))		// �߳���
		g_threadCount = 2;
	ReadText(f, g_banReason);										// ���ԭ��
	if (!ReadText(f, g_imageDir))									// Υ��ͼƬĿ¼
		g_images.clear();
	else
		ReadImages(g_imageDir);										// Υ��ͼƬ
	if (gzread(f, &g_SSIMThreshold, sizeof(double)) != sizeof(double))	// ��ֵ
		g_SSIMThreshold = 2.43;

	// ��������
	g_trustedThread.clear();
	if (gzread(f, &intBuf, sizeof(int)) == sizeof(int)) // ����
		for (int i = 0; i < intBuf; i++)
		{
			ReadText(f, strBuf);
			g_trustedThread.insert(strBuf);
		}

	if (gzread(f, &g_defriend, sizeof(BOOL)) != sizeof(BOOL))		// ����
		g_defriend = FALSE;
	if (gzread(f, &g_defriendTrigCount, sizeof(int)) != sizeof(int)) // ����Υ�����
		g_defriendTrigCount = 5;
	if (gzread(f, &g_autoSaveLog, sizeof(BOOL)) != sizeof(BOOL))	// �Զ�������־
		g_autoSaveLog = FALSE;

	g_optionsLock.Unlock();

	gzclose(f);
	return;

UseDefaultOptions:
	g_optionsLock.Lock();
	g_keywords.clear();			// Υ������
	g_blackList.clear();		// �����û�
	g_whiteList.clear();		// �����û�
	g_whiteContent.clear();		// ��������
	g_scanInterval = 5;			// ɨ����
	g_banID = FALSE;			// ��ID
	g_banDuration = 1;			// ���ʱ��
	g_banTrigCount = 1;			// ���Υ�����
	g_onlyScanTitle = FALSE;	// ֻɨ�����
	g_deleteInterval = 2.0f;	// ɾ�����
	g_confirm = TRUE;			// ����ǰ��ʾ
	g_scanPageCount = 1;		// ɨ�����ҳ��
	g_briefLog = FALSE;			// ֻ���ɾ�����
	g_delete = TRUE;			// ɾ��
	g_threadCount = 2;			// �߳���
	g_banReason = _T("");		// ���ԭ��
	g_imageDir = _T("");		// Υ��ͼƬĿ¼
	g_images.clear();			// Υ��ͼƬ
	g_SSIMThreshold = 2.43;		// ��ֵ
	g_trustedThread.clear();	// ��������
	g_defriend = FALSE;			// ����
	g_defriendTrigCount = 5;	// ����Υ�����
	g_autoSaveLog = FALSE;		// �Զ�������־
	g_optionsLock.Unlock();
}

// д����
void WriteOptions(LPCTSTR path)
{
	gzFile f = gzopen_w(path, "wb");
	if (f == NULL)
		return;

	// ͷ��
	gzwrite(f, "TB", 2);

	int intBuf;

	// Υ������
	WriteRegexTexts(f, g_keywords);

	// �����û�
	WriteRegexTexts(f, g_blackList);

	// �����û�
	gzwrite(f, &(intBuf = g_whiteList.size()), sizeof(int)); // ����
	for (const CString& i : g_whiteList)
		WriteText(f, i);

	// ��������
	WriteRegexTexts(f, g_whiteContent);

	gzwrite(f, &g_scanInterval, sizeof(int));		// ɨ����
	gzwrite(f, &g_banID, sizeof(BOOL));				// ��ID
	gzwrite(f, &g_banDuration, sizeof(int));		// ���ʱ��
	BOOL banIP = FALSE;
	gzwrite(f, &banIP, sizeof(BOOL));				// ��IP
	gzwrite(f, &g_banTrigCount, sizeof(int));		// ���Υ�����
	gzwrite(f, &g_onlyScanTitle, sizeof(BOOL));		// ֻɨ�����
	gzwrite(f, &g_deleteInterval, sizeof(float));	// ɾ�����
	gzwrite(f, &g_confirm, sizeof(BOOL));			// ����ǰ��ʾ
	gzwrite(f, &g_scanPageCount, sizeof(int));		// ɨ�����ҳ��
	gzwrite(f, &g_briefLog, sizeof(BOOL));			// ֻ���ɾ�����
	gzwrite(f, &g_delete, sizeof(BOOL));			// ɾ��
	gzwrite(f, &g_threadCount, sizeof(int));		// �߳���
	WriteText(f, g_banReason);						// ���ԭ��
	WriteText(f, g_imageDir);						// Υ��ͼƬĿ¼
	gzwrite(f, &g_SSIMThreshold, sizeof(double));	// ��ֵ

	// ��������
	gzwrite(f, &(intBuf = g_trustedThread.size()), sizeof(int)); // ����
	for (const CString& i : g_trustedThread)
		WriteText(f, i);

	gzwrite(f, &g_defriend, sizeof(BOOL));			// ����
	gzwrite(f, &g_defriendTrigCount, sizeof(int));	// ����Υ�����
	gzwrite(f, &g_autoSaveLog, sizeof(BOOL));		// �Զ�������־

	gzclose(f);
}

static inline void ReadIDSet(const gzFile& f, set<__int64>& IDSet)
{
	IDSet.clear();
	int size;
	if (gzread(f, &size, sizeof(int)) == sizeof(int) && 0 < size && size < 100000) // ����
	{
		__int64 id;
		for (int i = 0; i < size; i++)
		{
			gzread(f, &id, sizeof(__int64));
			IDSet.insert(id);
		}
	}
}

static inline void ReadTextSet(const gzFile& f, set<CString>& TextSet)
{
	TextSet.clear();
	int size;
	if (gzread(f, &size, sizeof(int)) == sizeof(int) && 0 < size && size < 100000) // ����
	{
		CString text;
		for (int i = 0; i < size; i++)
		{
			ReadText(f, text);
			TextSet.insert(text);
		}
	}
}

static inline void WriteIDSet(const gzFile& f, const set<__int64>& IDSet)
{
	int len = IDSet.size();
	gzwrite(f, &len, sizeof(int)); // ����
	for (auto& i : IDSet)
		gzwrite(f, &i, sizeof(__int64)); // ID
}

static inline void WriteTextSet(const gzFile& f, const set<CString>& TextSet)
{
	int len = TextSet.size();
	gzwrite(f, &len, sizeof(int)); // ����
	for (auto& i : TextSet)
		WriteText(f, i); // �ı�
}

// ���浱ǰ�˺�����
void SaveCurrentUserProfile()
{
	// ����Ŀ¼
	CreateDir(USERS_PATH + g_currentUser);

	// ����Cookie
	gzFile f = gzopen_w(COOKIE_PATH, "wb");
	if (f != NULL)
	{
		WriteText(f, g_cookie);
		gzclose(f);
	}

	// ������ʷ�ظ�������ID��
	g_userCache.Save(CACHE_PATH);
}

// ���õ�ǰ�˺�
void SetCurrentUser(LPCTSTR userName)
{
	// ���浱ǰ�˺�����
	if (g_currentUser != _T(""))
		SaveCurrentUserProfile();

	// ��������·��
	g_currentUser = userName;
	CURRENT_USER_PATH = USERS_PATH + userName;
	USER_PROFILE_PATH = CURRENT_USER_PATH + _T("\\options.tb");
	COOKIE_PATH = CURRENT_USER_PATH + _T("\\ck.tb");
	CACHE_PATH = CURRENT_USER_PATH + _T("\\cache.xml");

	// ��ȡ����
	TCHAR buffer[260];
	// ����
	GetPrivateProfileString(_T("Setting"), _T("Option"), _T("Ĭ��"), g_currentOption.GetBuffer(MAX_PATH), MAX_PATH, USER_PROFILE_PATH);
	g_currentOption.ReleaseBuffer();
	ReadOptions(OPTIONS_PATH + g_currentOption + _T(".tb"));
	// ������
	GetPrivateProfileString(_T("Setting"), _T("ForumName"), _T(""), buffer, _countof(buffer), USER_PROFILE_PATH);
	((CTiebaManagerDlg*)AfxGetApp()->m_pMainWnd)->m_forumNameEdit.SetWindowText(buffer);
	// Cookie
	gzFile f = gzopen_w(COOKIE_PATH, "rb");
	if (f != NULL)
	{
		ReadText(f, g_cookie);
		gzclose(f);
	}

	// ��ʷ�ظ�������ID��
	g_userCache.Load(CACHE_PATH);
}

#pragma region COption
// COptionʵ�� ///////////////////////////////////////////////////////////////////////////

// ����������

XMLElement& COption<int>::operator << (XMLElement& root)
{
	XMLElement* optionNode = root.FirstChildElement(m_name);
	if (optionNode == NULL)
	{
		UseDefault();
		return root;
	}

	LPCSTR value = optionNode->FirstChild()->ToText()->Value();
	m_value = atoi(value);
	if (!IsValid(m_value))
		UseDefault();
	return root;
}

XMLElement& COption<__int64>::operator << (XMLElement& root)
{
	XMLElement* optionNode = root.FirstChildElement(m_name);
	if (optionNode == NULL)
	{
		UseDefault();
		return root;
	}

	LPCSTR value = optionNode->FirstChild()->ToText()->Value();
	m_value = _atoi64(value);
	if (!IsValid(m_value))
		UseDefault();
	return root;
}

XMLElement& COption<CString>::operator << (XMLElement& root)
{
	XMLElement* optionNode = root.FirstChildElement(m_name);
	if (optionNode == NULL)
	{
		UseDefault();
		return root;
	}

	LPCSTR value = optionNode->FirstChild()->ToText()->Value();
	m_value = value;
	if (!IsValid(m_value))
		UseDefault();
	return root;
}

// ��set

XMLElement& COption<set<__int64> >::operator << (XMLElement& root)
{
	XMLElement* optionNode = root.FirstChildElement(m_name);
	if (optionNode == NULL)
	{
		UseDefault();
		return root;
	}

	m_value.clear();
	COption<__int64> value("value");
	for (XMLElement* item = optionNode->FirstChildElement("item"); item != NULL; item = item->NextSiblingElement("item"))
	{
		value << *item;
		m_value.insert(value);
	}
	if (!IsValid(m_value))
		UseDefault();
	return root;
}

XMLElement& COption<set<CString> >::operator << (XMLElement& root)
{
	XMLElement* optionNode = root.FirstChildElement(m_name);
	if (optionNode == NULL)
	{
		UseDefault();
		return root;
	}

	m_value.clear();
	COption<CString> value("value");
	for (XMLElement* item = optionNode->FirstChildElement("item"); item != NULL; item = item->NextSiblingElement("item"))
	{
		value << *item;
		m_value.insert(value);
	}
	if (!IsValid(m_value))
		UseDefault();
	return root;
}

// ��map

XMLElement& COption<map<__int64, int> >::operator << (XMLElement& root)
{
	XMLElement* optionNode = root.FirstChildElement(m_name);
	if (optionNode == NULL)
	{
		UseDefault();
		return root;
	}

	m_value.clear();
	COption<__int64> key("key");
	COption<int> value("value");
	for (XMLElement* item = optionNode->FirstChildElement("item"); item != NULL; item = item->NextSiblingElement("item"))
	{
		key << *item;
		value << *item;
		m_value[key] = value;
	}
	if (!IsValid(m_value))
		UseDefault();
	return root;
}

XMLElement& COption<map<CString, int> >::operator << (XMLElement& root)
{
	XMLElement* optionNode = root.FirstChildElement(m_name);
	if (optionNode == NULL)
	{
		UseDefault();
		return root;
	}

	m_value.clear();
	COption<CString> key("key");
	COption<int> value("value");
	for (XMLElement* item = optionNode->FirstChildElement("item"); item != NULL; item = item->NextSiblingElement("item"))
	{
		key << *item;
		value << *item;
		m_value[key] = value;
	}
	if (!IsValid(m_value))
		UseDefault();
	return root;
}

// д��������

XMLElement& COption<int>::operator >> (XMLElement& root) const
{
	tinyxml2::XMLDocument* doc = root.GetDocument();
	XMLElement* optionNode = doc->NewElement(m_name);
	root.LinkEndChild(optionNode);

	char buffer[15];
	_itoa_s(m_value, buffer, _countof(buffer), 10);
	optionNode->LinkEndChild(doc->NewText(buffer));
	return root;
}

XMLElement& COption<__int64>::operator >> (XMLElement& root) const
{
	tinyxml2::XMLDocument* doc = root.GetDocument();
	XMLElement* optionNode = doc->NewElement(m_name);
	root.LinkEndChild(optionNode);

	char buffer[25];
	_i64toa_s(m_value, buffer, _countof(buffer), 10);
	optionNode->LinkEndChild(doc->NewText(buffer));
	return root;
}

XMLElement& COption<CString>::operator >> (XMLElement& root) const
{
	tinyxml2::XMLDocument* doc = root.GetDocument();
	XMLElement* optionNode = doc->NewElement(m_name);
	root.LinkEndChild(optionNode);

	optionNode->LinkEndChild(doc->NewText(CStringA(m_value)));
	return root;
}

// дset

XMLElement& COption<set<__int64> >::operator >> (XMLElement& root) const
{
	tinyxml2::XMLDocument* doc = root.GetDocument();
	XMLElement* optionNode = doc->NewElement(m_name);
	root.LinkEndChild(optionNode);

	COption<__int64> value("value");
	for (const __int64& i : m_value)
	{
		XMLElement* item = doc->NewElement("item");
		optionNode->LinkEndChild(item);
		*value = i;
		value >> *item;
	}
	return root;
}

XMLElement& COption<set<CString> >::operator >> (XMLElement& root) const
{
	tinyxml2::XMLDocument* doc = root.GetDocument();
	XMLElement* optionNode = doc->NewElement(m_name);
	root.LinkEndChild(optionNode);

	COption<CString> value("value");
	for (const CString& i : m_value)
	{
		XMLElement* item = doc->NewElement("item");
		optionNode->LinkEndChild(item);
		*value = i;
		value >> *item;
	}
	return root;
}

// дmap

XMLElement& COption<map<__int64, int> >::operator >> (XMLElement& root) const
{
	tinyxml2::XMLDocument* doc = root.GetDocument();
	XMLElement* optionNode = doc->NewElement(m_name);
	root.LinkEndChild(optionNode);

	COption<__int64> key("key");
	COption<int> value("value");
	for (const auto& i : m_value)
	{
		XMLElement* item = doc->NewElement("item");
		optionNode->LinkEndChild(item);
		*key = i.first;
		key >> *item;
		*value = i.second;
		value >> *item;
	}
	return root;
}

XMLElement& COption<map<CString, int> >::operator >> (XMLElement& root) const
{
	tinyxml2::XMLDocument* doc = root.GetDocument();
	XMLElement* optionNode = doc->NewElement(m_name);
	root.LinkEndChild(optionNode);

	COption<CString> key("key");
	COption<int> value("value");
	for (const auto& i : m_value)
	{
		XMLElement* item = doc->NewElement("item");
		optionNode->LinkEndChild(item);
		*key = i.first;
		key >> *item;
		*value = i.second;
		value >> *item;
	}
	return root;
}
#pragma endregion

// ���ö�дʵ�� ///////////////////////////////////////////////////////////////////////////

BOOL CUserCache::LoadOld(const CString& path)
{
	gzFile f = gzopen_w(path, "rb");
	if (f != NULL)
	{
		int size;
		// ��ʷ�ظ�
		if (gzread(f, &size, sizeof(int)) == sizeof(int) && 0 < size && size < 100000) // ����
		{
			__int64 tid;
			int reply;
			for (int i = 0; i < size; i++)
			{
				gzread(f, &tid, sizeof(__int64));
				gzread(f, &reply, sizeof(int));
				g_userCache.m_reply.m_value[tid] = reply;
			}
		}
		// ����ID
		ReadIDSet(f, g_userCache.m_initIgnoredTID);
		g_userCache.m_ignoredTID = g_userCache.m_initIgnoredTID;
		ReadIDSet(f, g_userCache.m_initIgnoredPID);
		g_userCache.m_ignoredPID = g_userCache.m_initIgnoredPID;
		ReadIDSet(f, g_userCache.m_initIgnoredLZLID);
		g_userCache.m_ignoredLZLID = g_userCache.m_initIgnoredLZLID;
		// Υ�����
		if (gzread(f, &size, sizeof(int)) == sizeof(int) && 0 < size && size < 100000) // ����
		{
			CString userName;
			int count;
			for (int i = 0; i < size; i++)
			{
				ReadText(f, userName);
				gzread(f, &count, sizeof(int));
				g_userCache.m_userTrigCount.m_value[userName] = count;
			}
		}
		// �����û�
		ReadTextSet(f, g_userCache.m_defriendedUser);
		gzclose(f);
	}
	return TRUE;
}

BOOL CUserCache::Load(const CString& path)
{
	FILE* f = NULL;
	if (_tfopen_s(&f, path, _T("rb")) != 0)
	{
		CString oldPath = path.Left(path.GetLength() - 3) + _T("tb");
		LoadOld(oldPath);
		Save(path);
		DeleteFile(oldPath);
		return TRUE;
	}

	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(f) != XML_NO_ERROR)
	{
		fclose(f);
		return FALSE;
	}
	fclose(f);

	XMLElement* root = doc.FirstChildElement("Cache");
	if (root == NULL)
		return FALSE;

	m_initIgnoredTID << *root;
	m_initIgnoredPID << *root;
	m_initIgnoredLZLID << *root;
	m_reply << *root;
	m_userTrigCount << *root;
	m_bannedUser << *root;
	m_defriendedUser << *root;

	m_ignoredTID = m_initIgnoredTID;
	m_ignoredPID = m_initIgnoredPID;
	m_ignoredLZLID = m_initIgnoredLZLID;
	//m_deletedTID.clear();

	return TRUE;
}

BOOL CUserCache::Save(const CString& path) const
{
	FILE* f = NULL;
	if (_tfopen_s(&f, path, _T("wb")) != 0)
		return FALSE;

	tinyxml2::XMLDocument doc;
	doc.LinkEndChild(doc.NewDeclaration("xml version=\"1.0\" encoding=\"GBK\""));
	tinyxml2::XMLElement* root = doc.NewElement("Cache");
	doc.LinkEndChild(root);

	m_initIgnoredTID >> *root;
	m_initIgnoredPID >> *root;
	m_initIgnoredLZLID >> *root;
	m_reply >> *root;
	m_userTrigCount >> *root;
	m_bannedUser >> *root;
	m_defriendedUser >> *root;

	BOOL res = doc.SaveFile(f) == XML_NO_ERROR;
	fclose(f);
	return res;
}

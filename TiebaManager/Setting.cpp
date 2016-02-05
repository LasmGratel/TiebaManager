#include "stdafx.h"
#include "Setting.h"
#include <tinyxml2.h>
using namespace tinyxml2;
#include "TiebaManagerDlg.h"
#include "TiebaVariable.h"
#include "ScanImage.h"
#include "StringHelper.h"
#include "MiscHelper.h"
#include "BlackListPage.h"


// �����ļ�·��
CString		GLOBAL_CONFIG_PATH = _T("\\options.xml");	// ��������ʱ��ʼ��
CString		USER_PROFILE_PATH;							// ȷ������ʱ��ʼ��
CString		OPTIONS_PATH = _T("Option\\");
CString		USERS_PATH = _T("\\User\\");				// ��������ʱ��ʼ��
CString		CURRENT_USER_PATH;							// ȷ������ʱ��ʼ��
CString		COOKIE_PATH;								// ȷ������ʱ��ʼ��
CString		CACHE_PATH;									// ȷ������ʱ��ʼ��

CGlobalConfig g_globalConfig;
CUserConfig g_userConfig;

// ����
CPlan g_plan;


// ���浱ǰ�˺�����
void SaveCurrentUserProfile()
{
	// ����Ŀ¼
	CreateDir(USERS_PATH + g_globalConfig.m_currentUser);

	// ��ǰ�˺�����
	g_userConfig.Save(USER_PROFILE_PATH);

	// Cookie
	g_userTiebaInfo.Save(COOKIE_PATH);

	// ��ʷ�ظ�������ID��
	g_userCache.Save(CACHE_PATH);
}

// ���õ�ǰ�˺�
void SetCurrentUser(const CString& userName, BOOL save)
{
	// ���浱ǰ�˺�����
	if (save)
		SaveCurrentUserProfile();

	// ��������·��
	*g_globalConfig.m_currentUser = userName;
	CURRENT_USER_PATH = USERS_PATH + userName;
	USER_PROFILE_PATH = CURRENT_USER_PATH + _T("\\options.xml");
	COOKIE_PATH = CURRENT_USER_PATH + _T("\\ck.xml");
	CACHE_PATH = CURRENT_USER_PATH + _T("\\cache.xml");

	// ��ȡ����
	g_userConfig.Load(USER_PROFILE_PATH);
	// ����
	g_plan.Load(OPTIONS_PATH + g_userConfig.m_plan + _T(".xml"));
	ReadImages(g_plan.m_imageDir);
	// ������
	((CTiebaManagerDlg*)AfxGetApp()->m_pMainWnd)->m_forumNameEdit.SetWindowText(*g_userConfig.m_forumName);

	// Cookie
	g_userTiebaInfo.Load(COOKIE_PATH);

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
UseDefault:
		UseDefault();
		return root;
	}
	XMLNode* content = optionNode->FirstChild();
	if (content == NULL)
		goto UseDefault;
	LPCSTR value = content->ToText()->Value();

	m_value = atoi(value);
	if (!IsValid(m_value))
		UseDefault();
	return root;
}

XMLElement& COption<float>::operator << (XMLElement& root)
{
	XMLElement* optionNode = root.FirstChildElement(m_name);
	if (optionNode == NULL)
	{
	UseDefault:
		UseDefault();
		return root;
	}
	XMLNode* content = optionNode->FirstChild();
	if (content == NULL)
		goto UseDefault;
	LPCSTR value = content->ToText()->Value();

	m_value = (float)atof(value);
	if (!IsValid(m_value))
		UseDefault();
	return root;
}

XMLElement& COption<double>::operator << (XMLElement& root)
{
	XMLElement* optionNode = root.FirstChildElement(m_name);
	if (optionNode == NULL)
	{
	UseDefault:
		UseDefault();
		return root;
	}
	XMLNode* content = optionNode->FirstChild();
	if (content == NULL)
		goto UseDefault;
	LPCSTR value = content->ToText()->Value();

	m_value = atof(value);
	if (!IsValid(m_value))
		UseDefault();
	return root;
}

XMLElement& COption<__int64>::operator << (XMLElement& root)
{
	XMLElement* optionNode = root.FirstChildElement(m_name);
	if (optionNode == NULL)
	{
UseDefault:
		UseDefault();
		return root;
	}
	XMLNode* content = optionNode->FirstChild();
	if (content == NULL)
		goto UseDefault;
	LPCSTR value = content->ToText()->Value();

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
UseDefault:
		UseDefault();
		return root;
	}
	XMLNode* content = optionNode->FirstChild();
	if (content == NULL)
		goto UseDefault;
	LPCSTR value = content->ToText()->Value();

	m_value = value;
	if (!IsValid(m_value))
		UseDefault();
	return root;
}

XMLElement& COption<RegexText>::operator << (XMLElement& root)
{
	XMLElement* optionNode = root.FirstChildElement(m_name);
	if (optionNode == NULL)
	{
		UseDefault();
		return root;
	}

	COption<BOOL> isRegex("IsRegex");
	COption<CString> text("Text");
	isRegex << *optionNode;
	text << *optionNode;
	
	m_value.Set(isRegex, text);

	if (!IsValid(m_value))
		UseDefault();
	return root;
}

// ��vector

XMLElement& COption<vector<CString> >::operator << (XMLElement& root)
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
		m_value.push_back(value);
	}
	if (!IsValid(m_value))
		UseDefault();
	return root;
}

XMLElement& COption<vector<RegexText> >::operator << (XMLElement& root)
{
	XMLElement* optionNode = root.FirstChildElement(m_name);
	if (optionNode == NULL)
	{
		UseDefault();
		return root;
	}

	m_value.clear();
	COption<RegexText> value("value");
	for (XMLElement* item = optionNode->FirstChildElement("item"); item != NULL; item = item->NextSiblingElement("item"))
	{
		value << *item;
		m_value.push_back(value);
	}
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

XMLElement& COption<float>::operator >> (XMLElement& root) const
{
	tinyxml2::XMLDocument* doc = root.GetDocument();
	XMLElement* optionNode = doc->NewElement(m_name);
	root.LinkEndChild(optionNode);

	char buffer[25];
	sprintf_s(buffer, "%f", m_value);
	optionNode->LinkEndChild(doc->NewText(buffer));
	return root;
}

XMLElement& COption<double>::operator >> (XMLElement& root) const
{
	tinyxml2::XMLDocument* doc = root.GetDocument();
	XMLElement* optionNode = doc->NewElement(m_name);
	root.LinkEndChild(optionNode);

	char buffer[50];
	sprintf_s(buffer, "%f", m_value);
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

XMLElement& COption<RegexText>::operator >> (XMLElement& root) const
{
	tinyxml2::XMLDocument* doc = root.GetDocument();
	XMLElement* optionNode = doc->NewElement(m_name);
	root.LinkEndChild(optionNode);

	COption<BOOL> isRegex("IsRegex");
	*isRegex = m_value.isRegex;
	isRegex >> *optionNode;
	COption<CString> text("Text");
	*text = m_value.text;
	text >> *optionNode;
	return root;
}

// дvector

XMLElement& COption<vector<CString> >::operator >> (XMLElement& root) const
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

XMLElement& COption<vector<RegexText> >::operator >> (XMLElement& root) const
{
	tinyxml2::XMLDocument* doc = root.GetDocument();
	XMLElement* optionNode = doc->NewElement(m_name);
	root.LinkEndChild(optionNode);

	COption<RegexText> value("value");
	for (const RegexText& i : m_value)
	{
		XMLElement* item = doc->NewElement("item");
		optionNode->LinkEndChild(item);
		*value = i;
		value >> *item;
	}
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

BOOL CConfigBase::Load(const CString& path)
{
	FILE* f = NULL;
	if (_tfopen_s(&f, path, _T("rb")) != 0)
	{
		OnChange();
		CString oldPath = path.Left(path.GetLength() - 3) + _T("tb");
		BOOL res = LoadOld(oldPath);
		Save(path);
		DeleteFile(oldPath);
		PostChange();
		if (!res)
			UseDefault();
		return res;
	}

	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(f) != XML_NO_ERROR)
	{
		fclose(f);
		UseDefault();
		return FALSE;
	}
	fclose(f);

	XMLElement* root = doc.FirstChildElement(m_name);
	if (root == NULL)
	{
		UseDefault();
		return FALSE;
	}

	OnChange();
	for (COptionBase* i : m_options)
		*i << *root;
	PostChange();
	return TRUE;
}

BOOL CConfigBase::Save(const CString& path) const
{
	FILE* f = NULL;
	if (_tfopen_s(&f, path, _T("wb")) != 0)
		return FALSE;

	tinyxml2::XMLDocument doc;
	doc.LinkEndChild(doc.NewDeclaration("xml version=\"1.0\" encoding=\"GBK\""));
	tinyxml2::XMLElement* root = doc.NewElement(m_name);
	doc.LinkEndChild(root);

	for (const COptionBase* i : m_options)
		*i >> *root;

	BOOL res = doc.SaveFile(f) == XML_NO_ERROR;
	fclose(f);
	return res;
}

void CConfigBase::UseDefault()
{
	OnChange();
	for (COptionBase* i : m_options)
		i->UseDefault();
	PostChange();
}

// ������ʵ��

CPlan::CPlan()
	: CConfigBase("Plan"),
	m_scanInterval		("ScanInterval",		5,		[](const int& value)->BOOL{ return 0 <= value && value <= 600; }),
	m_onlyScanTitle		("OnlyScanTitle",		FALSE),
	m_scanPageCount		("ScanPageCount",		1,		[](const int& value)->BOOL{ return 1 <= value; }),
	m_briefLog			("BriefLog",			FALSE),
	m_threadCount		("ThreadCount",			2,		[](const int& value)->BOOL{ return 1 <= value && value <= 16; }),
	m_autoSaveLog		("AutoSaveLog",			FALSE),
	m_delete			("Delete",				TRUE),
	m_banID				("BanID",				FALSE),
	m_defriend			("Defriend",			FALSE),
	m_deleteInterval	("DeleteInterval",		2.0f,	[](const float& value)->BOOL{ return 0.0f <= value && value <= 60.0f; }),
	m_banDuration		("BanDuration",			1,		[](const int& value)->BOOL{ return value == 1 || value == 3 || value == 10; }),
	m_banReason			("BanReason",			_T("")),
	m_banTrigCount		("BanTrigCount",		1,		[](const int& value)->BOOL{ return 1 <= value; }),
	m_defriendTrigCount	("DefriendTrigCount",	5,		[](const int& value)->BOOL{ return 1 <= value; }),
	m_confirm			("Confirm",				TRUE),
	m_keywords			("IllegalContent", [](const vector<RegexText>& value)->BOOL
											{
												for (const RegexText& i : value)
													if (StringIncludes(MATCH_TOO_MUCH_CONTENT_TEST1, i) 
														&& StringIncludes(MATCH_TOO_MUCH_CONTENT_TEST2, i))
														return FALSE;
												return TRUE;
											}),
	m_imageDir			("IllegalImageDir",		_T("")),
	m_SSIMThreshold		("SSIMThreshold",		2.43f,	[](const double& value)->BOOL{ return 1.0f <= value && value <= 3.0f; }),
	m_blackList			("BlackList", [](const vector<RegexText>& value)->BOOL
										{
											for (const RegexText& i : value)
												if (StringIncludes(MATCH_TOO_MUCH_USERNAME_TEST1, i) 
													&& StringIncludes(MATCH_TOO_MUCH_USERNAME_TEST2, i))
													return FALSE;
											return TRUE;
										}),
	m_whiteList			("WhiteList"),
	m_whiteContent		("TrustedContent"),
	m_trustedThread		("TrustedThread")
{
	m_updateImage = TRUE;
	m_options.push_back(&m_scanInterval);
	m_options.push_back(&m_onlyScanTitle);
	m_options.push_back(&m_scanPageCount);
	m_options.push_back(&m_briefLog);
	m_options.push_back(&m_threadCount);
	m_options.push_back(&m_autoSaveLog);
	m_options.push_back(&m_delete);
	m_options.push_back(&m_banID);
	m_options.push_back(&m_defriend);
	m_options.push_back(&m_deleteInterval);
	m_options.push_back(&m_banDuration);
	m_options.push_back(&m_banReason);
	m_options.push_back(&m_banTrigCount);
	m_options.push_back(&m_defriendTrigCount);
	m_options.push_back(&m_confirm);
	m_options.push_back(&m_keywords);
	m_options.push_back(&m_imageDir);
	m_options.push_back(&m_SSIMThreshold);
	m_options.push_back(&m_blackList);
	m_options.push_back(&m_whiteList);
	m_options.push_back(&m_whiteContent);
	m_options.push_back(&m_trustedThread);
}

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

BOOL CPlan::LoadOld(const CString& path)
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

	m_optionsLock.Lock();

	// Υ������
	ReadRegexTexts(f, m_keywords);

	// �����û�
	ReadRegexTexts(f, m_blackList);

	// �����û�
	int intBuf;
	gzread(f, &intBuf, sizeof(int)); // ����
	for (int i = 0; i < intBuf; i++)
	{
		ReadText(f, strBuf);
		m_whiteList->insert(strBuf);
	}

	// ��������
	ReadRegexTexts(f, m_whiteContent);

	gzread(f, &*m_scanInterval, sizeof(int));	// ɨ����
	gzread(f, &*m_banID, sizeof(BOOL));			// ��ID
	gzread(f, &*m_banDuration, sizeof(int));	// ���ʱ��
	BOOL banIP;
	gzread(f, &banIP, sizeof(BOOL));			// ��IP
	gzread(f, &*m_banTrigCount, sizeof(int));	// ���Υ�����
	gzread(f, &*m_onlyScanTitle, sizeof(BOOL));	// ֻɨ�����
	gzread(f, &*m_deleteInterval, sizeof(float));// ɾ�����
	gzread(f, &*m_confirm, sizeof(BOOL));		// ����ǰ��ʾ
	gzread(f, &*m_scanPageCount, sizeof(int));	// ɨ�����ҳ��
	gzread(f, &*m_briefLog, sizeof(BOOL));		// ֻ���ɾ�����
	if (gzread(f, &*m_delete, sizeof(BOOL)) != sizeof(BOOL))			// ɾ��
		*m_delete = TRUE;
	if (gzread(f, &*m_threadCount, sizeof(int)) != sizeof(int))			// �߳���
		*m_threadCount = 2;
	ReadText(f, m_banReason);											// ���ԭ��
	if (!ReadText(f, m_imageDir))										// Υ��ͼƬĿ¼
		m_images.clear();												// Υ��ͼƬ
	if (gzread(f, &*m_SSIMThreshold, sizeof(double)) != sizeof(double))	// ��ֵ
		*m_SSIMThreshold = 2.43;

	// ��������
	m_trustedThread->clear();
	if (gzread(f, &intBuf, sizeof(int)) == sizeof(int)) // ����
	for (int i = 0; i < intBuf; i++)
	{
		ReadText(f, strBuf);
		m_trustedThread->insert(strBuf);
	}

	if (gzread(f, &*m_defriend, sizeof(BOOL)) != sizeof(BOOL))		// ����
		*m_defriend = FALSE;
	if (gzread(f, &*m_defriendTrigCount, sizeof(int)) != sizeof(int)) // ����Υ�����
		*m_defriendTrigCount = 5;
	if (gzread(f, &*m_autoSaveLog, sizeof(BOOL)) != sizeof(BOOL))	// �Զ�������־
		*m_autoSaveLog = FALSE;

	m_optionsLock.Unlock();

	gzclose(f);
	return TRUE;

UseDefaultOptions:
	m_optionsLock.Lock();
	m_keywords->clear();		// Υ������
	m_blackList->clear();		// �����û�
	m_whiteList->clear();		// �����û�
	m_whiteContent->clear();	// ��������
	*m_scanInterval = 5;		// ɨ����
	*m_banID = FALSE;			// ��ID
	*m_banDuration = 1;			// ���ʱ��
	*m_banTrigCount = 1;		// ���Υ�����
	*m_onlyScanTitle = FALSE;	// ֻɨ�����
	*m_deleteInterval = 2.0f;	// ɾ�����
	*m_confirm = TRUE;			// ����ǰ��ʾ
	*m_scanPageCount = 1;		// ɨ�����ҳ��
	*m_briefLog = FALSE;		// ֻ���ɾ�����
	*m_delete = TRUE;			// ɾ��
	*m_threadCount = 2;			// �߳���
	*m_banReason = _T("");		// ���ԭ��
	*m_imageDir = _T("");		// Υ��ͼƬĿ¼
	m_images.clear();			// Υ��ͼƬ
	*m_SSIMThreshold = 2.43f;	// ��ֵ
	m_trustedThread->clear();	// ��������
	*m_defriend = FALSE;		// ����
	*m_defriendTrigCount = 5;	// ����Υ�����
	*m_autoSaveLog = FALSE;		// �Զ�������־
	m_optionsLock.Unlock();
	return TRUE;
}

void CPlan::PostChange()
{
	if (m_updateImage)
	{
		m_updateImage = FALSE;
		ReadImages(m_imageDir); 
	}
	m_optionsLock.Unlock();
}

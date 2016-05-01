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

	m_value = GBK2W(value);
	if (!IsValid(m_value))
		UseDefault();
	return root;
}

// ���Զ�������

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

XMLElement& COption<CPlan::Keyword>::operator << (XMLElement& root)
{
	XMLElement* optionNode = root.FirstChildElement(m_name);
	if (optionNode == NULL)
	{
		UseDefault();
		return root;
	}

	COption<BOOL> isRegex("IsRegex");
	COption<CString> text("Text");
	COption<BOOL> forceToConfirm("ForceToConfirm");
	COption<int> trigCount("TrigCount");
	isRegex << *optionNode;
	text << *optionNode;
	forceToConfirm << *optionNode;
	trigCount << *optionNode;

	m_value.Set(isRegex, text);
	m_value.forceToConfirm = forceToConfirm;
	m_value.trigCount = trigCount;

	if (!IsValid(m_value))
		UseDefault();
	return root;
}

// ��vector

#define DEFINE_READ_VECTOR(T) \
XMLElement& COption<vector<T> >::operator << (XMLElement& root)\
{\
	XMLElement* optionNode = root.FirstChildElement(m_name);\
	if (optionNode == NULL)\
	{\
		UseDefault();\
		return root;\
	}\
	\
	m_value.clear();\
	COption<T> value("value");\
	for (XMLElement* item = optionNode->FirstChildElement("item"); item != NULL; item = item->NextSiblingElement("item"))\
	{\
		value << *item;\
		m_value.push_back(value);\
	}\
	if (!IsValid(m_value))\
		UseDefault();\
	return root;\
}

DEFINE_READ_VECTOR(CString)
DEFINE_READ_VECTOR(RegexText)
DEFINE_READ_VECTOR(CPlan::Keyword)

// ��set

#define DEFINE_READ_SET(T) \
XMLElement& COption<set<T> >::operator << (XMLElement& root)\
{\
	XMLElement* optionNode = root.FirstChildElement(m_name);\
	if (optionNode == NULL)\
	{\
		UseDefault();\
		return root;\
	}\
	\
	m_value.clear();\
	COption<T> value("value");\
	for (XMLElement* item = optionNode->FirstChildElement("item"); item != NULL; item = item->NextSiblingElement("item"))\
	{\
		value << *item;\
		m_value.insert(value);\
	}\
	if (!IsValid(m_value))\
		UseDefault();\
	return root;\
}

DEFINE_READ_SET(__int64)
DEFINE_READ_SET(CString)

// ��map

#define DEFINE_READ_MAP(T1, T2) \
XMLElement& COption<map<T1, T2> >::operator << (XMLElement& root)\
{\
	XMLElement* optionNode = root.FirstChildElement(m_name);\
	if (optionNode == NULL)\
	{\
		UseDefault();\
		return root;\
	}\
	\
	m_value.clear();\
	COption<T1> key("key");\
	COption<T2> value("value");\
	for (XMLElement* item = optionNode->FirstChildElement("item"); item != NULL; item = item->NextSiblingElement("item"))\
	{\
		key << *item;\
		value << *item;\
		m_value[key] = value;\
	}\
	if (!IsValid(m_value))\
		UseDefault();\
	return root;\
}

DEFINE_READ_MAP(__int64, int)
DEFINE_READ_MAP(CString, int)
DEFINE_READ_MAP(__int64, CString)


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

	optionNode->LinkEndChild(doc->NewText(W2GBK(m_value)));
	return root;
}

// д�Զ�������

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

XMLElement& COption<CPlan::Keyword>::operator >> (XMLElement& root) const
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
	COption<BOOL> forceToConfirm("ForceToConfirm");
	*forceToConfirm = m_value.forceToConfirm;
	forceToConfirm >> *optionNode;
	COption<int> trigCount("TrigCount");
	*trigCount = m_value.trigCount;
	trigCount >> *optionNode;
	return root;
}

// д����

#define DEFINE_WRITE_CONTAINER(T, CONTAINER) \
XMLElement& COption<CONTAINER<T> >::operator >> (XMLElement& root) const\
{\
	tinyxml2::XMLDocument* doc = root.GetDocument();\
	XMLElement* optionNode = doc->NewElement(m_name);\
	root.LinkEndChild(optionNode);\
	\
	COption<T> value("value");\
	for (const T& i : m_value)\
	{\
		XMLElement* item = doc->NewElement("item");\
		optionNode->LinkEndChild(item);\
		*value = i;\
		value >> *item;\
	}\
	return root;\
}

// дvector

#define DEFINE_WRITE_VECTOR(T) DEFINE_WRITE_CONTAINER(T, vector)

DEFINE_WRITE_VECTOR(CString)
DEFINE_WRITE_VECTOR(RegexText)
DEFINE_WRITE_VECTOR(CPlan::Keyword)

// дset

#define DEFINE_WRITE_SET(T) DEFINE_WRITE_CONTAINER(T, set)

DEFINE_WRITE_SET(__int64)
DEFINE_WRITE_SET(CString)

// дmap

#define DEFINE_WRITE_MAP(T1, T2) \
XMLElement& COption<map<T1, T2> >::operator >> (XMLElement& root) const\
{\
	tinyxml2::XMLDocument* doc = root.GetDocument();\
	XMLElement* optionNode = doc->NewElement(m_name);\
	root.LinkEndChild(optionNode);\
	\
	COption<T1> key("key");\
	COption<T2> value("value");\
	for (const auto& i : m_value)\
	{\
		XMLElement* item = doc->NewElement("item");\
		optionNode->LinkEndChild(item);\
		*key = i.first;\
		key >> *item;\
		*value = i.second;\
		value >> *item;\
	}\
	return root;\
}

DEFINE_WRITE_MAP(__int64, int)
DEFINE_WRITE_MAP(CString, int)
DEFINE_WRITE_MAP(__int64, CString)

#pragma endregion

// ���ö�дʵ�� ///////////////////////////////////////////////////////////////////////////

BOOL CConfigBase::Load(const CString& path)
{
	FILE* f = NULL;
	if (_tfopen_s(&f, path, _T("rb")) != 0 || f == NULL)
	{
		UseDefault();
		return FALSE;
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
	if (_tfopen_s(&f, path, _T("wb")) != 0 || f == NULL)
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
	m_illegalLevel		("IllegalLevel",		0,		[](const int& value)->BOOL{ return 0 <= value && value <= 6; }),
	m_delete			("Delete",				TRUE),
	m_banID				("BanID",				FALSE),
	m_defriend			("Defriend",			FALSE),
	m_deleteInterval	("DeleteInterval",		2.5f,	[](const float& value)->BOOL{ return 0.0f <= value && value <= 60.0f; }),
	m_banDuration		("BanDuration",			1,		[](const int& value)->BOOL{ return value == 1 || value == 3 || value == 10; }),
	m_banReason			("BanReason",			_T("")),
	m_banTrigCount		("BanTrigCount",		1,		[](const int& value)->BOOL{ return 1 <= value; }),
	m_defriendTrigCount	("DefriendTrigCount",	5,		[](const int& value)->BOOL{ return 1 <= value; }),
	m_confirm			("Confirm",				TRUE),
	m_wapBanInterface	("WapBanInterface",		FALSE),
	m_autoLoopBan		("AutoLoopBan",			FALSE),
	m_keywords			("IllegalContent", [](const vector<Keyword>& value)->BOOL
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
	m_options.push_back(&m_illegalLevel);
	m_options.push_back(&m_delete);
	m_options.push_back(&m_banID);
	m_options.push_back(&m_defriend);
	m_options.push_back(&m_deleteInterval);
	m_options.push_back(&m_banDuration);
	m_options.push_back(&m_banReason);
	m_options.push_back(&m_banTrigCount);
	m_options.push_back(&m_defriendTrigCount);
	m_options.push_back(&m_confirm);
	m_options.push_back(&m_wapBanInterface);
	m_options.push_back(&m_autoLoopBan);
	m_options.push_back(&m_keywords);
	m_options.push_back(&m_imageDir);
	m_options.push_back(&m_SSIMThreshold);
	m_options.push_back(&m_blackList);
	m_options.push_back(&m_whiteList);
	m_options.push_back(&m_whiteContent);
	m_options.push_back(&m_trustedThread);
};

void CPlan::PostChange()
{
	if (m_updateImage)
	{
		m_updateImage = FALSE;
		ReadImages(m_imageDir); 
	}
	m_optionsLock.Unlock();
}

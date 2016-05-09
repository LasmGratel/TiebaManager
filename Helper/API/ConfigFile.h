#pragma once
#include "APICommon.h"
#include <tinyxml2.h>


class COptionBase
{
public:
	const CStringA m_name;

	COptionBase(const CStringA& name) : m_name(name) {}
	virtual ~COptionBase() {}

	virtual void UseDefault() = 0;
	virtual void Read(const tinyxml2::XMLElement& root) = 0;
	virtual void Write(tinyxml2::XMLElement& root) const = 0;
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

	void Read(const tinyxml2::XMLElement& root){ *this << root; }
	void Write(tinyxml2::XMLElement& root) const{ *this >> root; }
};

// ��Option
#define DECLEAR_READ(T) HELPER_API const tinyxml2::XMLElement& operator << (COption<T >& thiz, const tinyxml2::XMLElement& root)
// дOption
#define DECLEAR_WRITE(T) HELPER_API tinyxml2::XMLElement& operator >> (const COption<T >& thiz, tinyxml2::XMLElement& root)

// ��vector
#define DEFINE_READ_VECTOR(T) \
DECLEAR_READ(vector<T>)\
{\
	const tinyxml2::XMLElement* optionNode = root.FirstChildElement(thiz.m_name); \
	if (optionNode == NULL)\
	{\
		thiz.UseDefault(); \
		return root; \
	}\
	\
	thiz.m_value.clear(); \
	COption<T> value("value"); \
	for (const tinyxml2::XMLElement* item = optionNode->FirstChildElement("item"); item != NULL; item = item->NextSiblingElement("item"))\
	{\
		value << *item; \
		thiz.m_value.push_back(value); \
	}\
	if (!thiz.IsValid(thiz.m_value))\
		thiz.UseDefault(); \
	return root; \
}

// ��set
#define DEFINE_READ_SET(T) \
DECLEAR_READ(set<T>)\
{\
	const tinyxml2::XMLElement* optionNode = root.FirstChildElement(thiz.m_name); \
	if (optionNode == NULL)\
	{\
		thiz.UseDefault(); \
		return root; \
	}\
	\
	thiz.m_value.clear(); \
	COption<T> value("value"); \
	for (const tinyxml2::XMLElement* item = optionNode->FirstChildElement("item"); item != NULL; item = item->NextSiblingElement("item"))\
	{\
		value << *item; \
		thiz.m_value.insert(value); \
	}\
	if (!thiz.IsValid(thiz.m_value))\
		thiz.UseDefault(); \
	return root; \
}

// ��map
#define COMMA ,

#define DEFINE_READ_MAP(T1, T2) \
DECLEAR_READ(map<T1 COMMA T2>)\
{\
	const tinyxml2::XMLElement* optionNode = root.FirstChildElement(thiz.m_name); \
	if (optionNode == NULL)\
	{\
		thiz.UseDefault(); \
		return root; \
	}\
	\
	thiz.m_value.clear(); \
	COption<T1> key("key"); \
	COption<T2> value("value"); \
	for (const tinyxml2::XMLElement* item = optionNode->FirstChildElement("item"); item != NULL; item = item->NextSiblingElement("item"))\
	{\
		key << *item; \
		value << *item; \
		thiz.m_value[key] = value; \
	}\
	if (!thiz.IsValid(thiz.m_value))\
		thiz.UseDefault(); \
	return root; \
}

// д����
#define DEFINE_WRITE_CONTAINER(T, CONTAINER) \
DECLEAR_WRITE(CONTAINER<T>)\
{\
	tinyxml2::XMLDocument* doc = root.GetDocument(); \
	tinyxml2::XMLElement* optionNode = doc->NewElement(thiz.m_name); \
	root.LinkEndChild(optionNode); \
	\
	COption<T> value("value"); \
	for (const T& i : thiz.m_value)\
	{\
		tinyxml2::XMLElement* item = doc->NewElement("item"); \
		optionNode->LinkEndChild(item); \
		*value = i; \
		value >> *item; \
	}\
	return root; \
}

// дvector
#define DEFINE_WRITE_VECTOR(T) DEFINE_WRITE_CONTAINER(T, vector)

// дset
#define DEFINE_WRITE_SET(T) DEFINE_WRITE_CONTAINER(T, set)

// дmap
#define DEFINE_WRITE_MAP(T1, T2) \
DECLEAR_WRITE(map<T1 COMMA T2>)\
{\
	tinyxml2::XMLDocument* doc = root.GetDocument(); \
	tinyxml2::XMLElement* optionNode = doc->NewElement(thiz.m_name); \
	root.LinkEndChild(optionNode); \
	\
	COption<T1> key("key"); \
	COption<T2> value("value"); \
	for (const auto& i : thiz.m_value)\
		{\
		tinyxml2::XMLElement* item = doc->NewElement("item"); \
		optionNode->LinkEndChild(item); \
		*key = i.first; \
		key >> *item; \
		*value = i.second; \
		value >> *item; \
	}\
	return root; \
}

#define DECLEAR_BOTH(T) DECLEAR_READ(T); DECLEAR_WRITE(T);
DECLEAR_BOTH(int)
DECLEAR_BOTH(float)
DECLEAR_BOTH(double)
DECLEAR_BOTH(__int64)
DECLEAR_BOTH(CString)
DECLEAR_BOTH(vector<CString>)
DECLEAR_BOTH(set<__int64>)
DECLEAR_BOTH(set<CString>)
DECLEAR_BOTH(map<__int64 COMMA int>)
DECLEAR_BOTH(map<CString COMMA int>)
DECLEAR_BOTH(map<__int64 COMMA CString>)


class CConfigBase
{
protected:
	vector<COptionBase*> m_options;

public:
	const LPCSTR m_name;

	CConfigBase(LPCSTR name) : m_name(name) {}
	virtual ~CConfigBase() {}

	HELPER_API virtual BOOL Load(const CString& path);
	HELPER_API virtual BOOL Save(const CString& path) const;
	HELPER_API virtual void UseDefault();
	virtual void OnChange() {}
	virtual void PostChange() {}
};

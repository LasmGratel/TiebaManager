#pragma once
#include "HelperCommon.h"
#include <regex>


struct RegexText
{
	BOOL isRegex;
	CString text; // Ϊ�˱�����������regexpҪ��Set������ֵ����
	std::wregex regexp;

	void Set(BOOL _isRegex, const CString& _text)
	{
		isRegex = _isRegex;
		text = _text;
		regexp = isRegex ? text : _T("");
	}
};


// �ָ��ַ���
HELPER_API void SplitString(CStringArray& dst, const CString& src, const CString& slipt);

// �ַ�������
HELPER_API BOOL StringIncludes(const CString& str, const CString& content);
// �ַ�������
HELPER_API BOOL StringIncludes(const CString& str, const CString& content, BOOL isRegex);
// �ַ�������
HELPER_API BOOL StringIncludes(const CString& str, const RegexText& content, int* _pos = NULL, int* length = NULL);
// �ַ���ƥ��
HELPER_API BOOL StringMatchs(const CString& str, const CString& content, BOOL isRegex);
// �ַ���ƥ��
HELPER_API BOOL StringMatchs(const CString& str, const RegexText& content);

// ȡ�ַ���֮����ַ���
HELPER_API CString GetStringBetween(const CString& src, const CString& left, const CString& right, int startPos = 0);
// ȡ�ַ���֮����ַ������������ҵ��ַ���
HELPER_API CString GetStringBetween2(const CString& src, const CString& left, const CString& right, int startPos = 0);
// ȡ�ַ���֮ǰ���ַ���
HELPER_API CString GetStringBefore(const CString& src, const CString& right, int startPos = 0);

// д�ַ������ļ�
HELPER_API BOOL WriteString(const CString& src, const CString& path);

// UnicodeתGBK
HELPER_API CStringA W2GBK(const CStringW& src);
// GBKתUnicode
HELPER_API CStringW GBK2W(const CStringA& src);
// URL����
HELPER_API CString EncodeURI(const CString& src);
// URL���� GBK��
HELPER_API CString EncodeURI_GBK(const CString& src);
// HTMLת��
HELPER_API CString HTMLEscape(const CString& src);
// HTML��ת��
HELPER_API CString HTMLUnescape(const CString& src);
// JS��ת�壬������Ӧ����ת��src���˫����
HELPER_API CString JSUnescape(const CString& src);

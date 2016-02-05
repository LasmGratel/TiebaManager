#pragma once
#include <opencv2\core\mat.hpp>


struct RegexText
{
	BOOL isRegex;
	CString text; // Ϊ�˱�����������regexpҪ��Set������ֵ����
	wregex regexp;

	void Set(BOOL _isRegex, const CString& _text)
	{
		isRegex = _isRegex;
		text = _text;
		regexp = isRegex ? text : _T("");
	}
};

struct NameImage
{
	CString name;
	cv::Mat img;
};

#pragma once
#include "HelperCommon.h"


// ��������Ϣ���ӳ�
HELPER_API void Delay(DWORD time);
// ������Ϣ
HELPER_API void DoEvents();
// ����Ŀ¼
HELPER_API BOOL CreateDir(const CString& path);
// ��ʼ��COM��
HELPER_API BOOL CoInitializeHelper();

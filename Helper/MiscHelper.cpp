#include "stdafx.h"
#include <MiscHelper.h>


// ��������Ϣ���ӳ�
HELPER_API void Delay(DWORD time)
{
#pragma warning(suppress: 28159)
	DWORD startTime = GetTickCount();
#pragma warning(suppress: 28159)
	while (GetTickCount() - startTime < time)
	{
		DoEvents();
		Sleep(1); // ��ֹռ��CPU
	}
}

// ������Ϣ
HELPER_API void DoEvents()
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		DispatchMessage(&msg);
		TranslateMessage(&msg);
	}
}

// ����Ŀ¼
HELPER_API BOOL CreateDir(const CString& path)
{
	if (PathFileExists(path))
		return TRUE;
	int pos = path.ReverseFind(_T('\\'));
	if (pos != -1)
	{
		CString parent = path.Left(pos);
		if (!CreateDir(parent))
			return FALSE;
	}
	return CreateDirectory(path, NULL);
}

// ��ʼ��COM��
HELPER_API BOOL CoInitializeHelper()
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		CString tmp;
		tmp.Format(_T("CoInitializeExʧ�ܣ�\r\n�������0x%08X"), hr);
		AfxMessageBox(tmp, MB_ICONERROR);
		return FALSE;
	}
	return TRUE;
}

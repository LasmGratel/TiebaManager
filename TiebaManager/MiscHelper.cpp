#include "stdafx.h"
#include "MiscHelper.h"
#include <Dbghelp.h>

// ��������Ϣ���ӳ�
void Delay(DWORD time)
{
	DWORD startTime = GetTickCount();
	while (GetTickCount() - startTime < time)
	{
		DoEvents();
		Sleep(1); // ��ֹռ��CPU
	}
}

// ������Ϣ
void DoEvents()
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		DispatchMessage(&msg);
		TranslateMessage(&msg);
	}
}

// �쳣����
LONG WINAPI ExceptionHandler(_EXCEPTION_POINTERS* ExceptionInfo)
{
	CFile file;
	if (file.Open(_T("exception.dmp"), CFile::modeCreate | CFile::modeWrite))
	{
		MINIDUMP_EXCEPTION_INFORMATION einfo;
		einfo.ThreadId = GetCurrentThreadId();
		einfo.ExceptionPointers = ExceptionInfo;
		einfo.ClientPointers = FALSE;
		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), file, MiniDumpWithIndirectlyReferencedMemory,
			&einfo, NULL, NULL);
	}
	AfxMessageBox(_T("��������ˣ����exception.dmp�ļ�����xfgryujk@126.com��������"), MB_ICONERROR);
	return EXCEPTION_CONTINUE_SEARCH;
}

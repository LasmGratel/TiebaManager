#include "stdafx.h"
#include "Update.h"
#import <msscript.ocx> no_namespace
#include "StringHelper.h"
#include "NetworkHelper.h"
#include "TiebaManagerDlg.h"


// ��ǰ�汾���ڣ�ÿ�θ��º��޸�
const char UPDATE_CURRENT_VERSION_A[] = "16-04-27";
const CString UPDATE_CURRENT_VERSION(UPDATE_CURRENT_VERSION_A);
// ������־
const TCHAR UPDATE_LOG[] = _T("1. �����������Դ�����������ٶ�����");

// �ַ���תʱ���
static time_t Str2Time(const CString& str)
{
	CComPtr<IScriptControl> script;
	if (FAILED(script.CoCreateInstance(__uuidof(ScriptControl))))
		return 0;
	script->put_Language(_bstr_t(_T("JScript")));
	_variant_t result;
	if (FAILED(script->raw_Eval(_bstr_t(_T("Date.parse(\"") + str + _T("\")")), result.GetAddress())))
		return 0;
	return (__int64)(result.dblVal / 1000);
}

// ������
CheckUpdateResult CheckUpdate()
{
	// ȡ�ϴ�ʱ��
	CString src = HTTPGet(UPDATE_INFO_URL, FALSE);
	// �ַ���
	CString time = GetStringBetween(src, _T(R"("Last-Modified":")"), _T("\""));
	if (time == _T(""))
		return UPDATE_FAILED_TO_GET_FILE_INFO;
	// ʱ���
	time_t tTime = Str2Time(time);
	if (tTime == 0)
		return UPDATE_FAILED_TO_GET_FILE_INFO;
	tm tmTime;
	localtime_s(&tmTime, &tTime);
	// �ַ���
	time.Format(_T("%02d-%02d-%02d"), tmTime.tm_year % 100, tmTime.tm_mon + 1, tmTime.tm_mday);
	if (time == UPDATE_CURRENT_VERSION)
		return UPDATE_NO_UPDATE;

	if (AfxMessageBox(_T("���°汾") + time + _T("���Ƿ���£�"), MB_ICONQUESTION | MB_YESNO) == IDNO)
		return UPDATE_HAS_UPDATE;

	ShellExecute(NULL, _T("open"), UPDATE_URL, NULL, NULL, SW_NORMAL);
	AfxMessageBox(_T("���غ��ѹ�����Ǿɰ汾�ļ����ɱ�����ǰ����"), MB_ICONINFORMATION);
	return UPDATE_HAS_UPDATE;
}

// �Զ������߳�
UINT AFX_CDECL AutoUpdateThread(LPVOID _dlg)
{
	CTiebaManagerDlg* dlg = (CTiebaManagerDlg*)_dlg;
	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED)))
	{
		AfxMessageBox(_T("CoInitializeExʧ�ܣ�"), MB_ICONERROR);
		return 0;
	}
	CheckUpdateResult res = CheckUpdate();
	CoUninitialize();
	switch (res)
	{
	case UPDATE_FAILED_TO_GET_FILE_INFO:
		dlg->m_stateStatic.SetWindowText(_T("������ʧ�ܣ���ȡ�ļ���Ϣʧ�ܣ����������ֶ�������"));
		break;
	case UPDATE_NO_UPDATE:
	case UPDATE_HAS_UPDATE:
		dlg->m_stateStatic.SetWindowText(_T("������"));
		break;
	}

	return 0;
}

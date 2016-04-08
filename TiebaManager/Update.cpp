#include "stdafx.h"
#include "Update.h"
#include "StringHelper.h"
#include "NetworkHelper.h"
#include "TiebaManagerDlg.h"


// ��ǰ�汾���ڣ�ÿ�θ��º��޸�
const char UPDATE_CURRENT_VERSION_A[] = "16-04-08";
const CString UPDATE_CURRENT_VERSION(UPDATE_CURRENT_VERSION_A);
// ������־
const TCHAR UPDATE_LOG[] = _T("1. ֧�ֵ��뵼��txt�ļ�\r\n")
						   _T("2. �޸��ͷֱ�����UI���һ��հ�\r\n")
						   _T("3. ȷ�ϲ���ʱ���������������\r\n")
						   _T("4. ֧��ɨ��¥��¥������");

// ������
CheckUpdateResult CheckUpdate()
{
	static const wregex CHECK_UPDATE_REG(_T("\"fs_id\":(\\d+),\"path\":\"\\\\/\\\\u6211\\\\u7684\\\\u5206\\\\u4eab\\\\/\\\\u767e")
										 _T("\\\\u5ea6\\\\u8d34\\\\u5427\\\\u76f8\\\\u5173\\\\/\\\\u8d34\\\\u5427\\\\u7ba1\\\\u7")
										 _T("406\\\\u5668.zip\",\"server_filename\":\"\\\\u8d34\\\\u5427\\\\u7ba1\\\\u7406\\\\u5")
										 _T("668.zip\".*?\"server_ctime\":(\\d+),"));
	CString src = HTTPGet(_T("http://pan.baidu.com/share/list?channel=chunlei&clienttype=0&web=1&num=100&page=1&dir=%2F%E6%88%91")
						  _T("%E7%9A%84%E5%88%86%E4%BA%AB%2F%E7%99%BE%E5%BA%A6%E8%B4%B4%E5%90%A7%E7%9B%B8%E5%85%B3&uk=436464474&")
						  _T("shareid=497149087"), FALSE);
	std::wcmatch res;
	if (!std::regex_search((LPCTSTR)src, res, CHECK_UPDATE_REG))
		return UPDATE_FAILED_TO_GET_FILE_ID;

	// �ļ�ID
	CString fs_id = res[1].str().c_str();

	// �ϴ�ʱ���
	CString server_ctime = res[2].str().c_str();
	time_t tServer_ctime = _ttoi64(server_ctime);
	tm tmServer_ctime;
	localtime_s(&tmServer_ctime, &tServer_ctime);
	// �ϴ�ʱ��
	CString time;
	time.Format(_T("%02d-%02d-%02d"), tmServer_ctime.tm_year % 100, tmServer_ctime.tm_mon + 1, tmServer_ctime.tm_mday);
	if (time == UPDATE_CURRENT_VERSION)
		return UPDATE_NO_UPDATE;

	if (AfxMessageBox(_T("���°汾") + time + _T("���Ƿ���£�"), MB_ICONQUESTION | MB_YESNO) == IDNO)
		return UPDATE_HAS_UPDATE;

	static const TCHAR TIME_STAMP_LEFT[] = _T("yunData.TIMESTAMP = \"");
	static const TCHAR TIME_STAMP_RIGHT[] = _T("\"");
	static const TCHAR SIGN_LEFT[] = _T("yunData.SIGN = \"");
	static const TCHAR SIGN_RIGHT[] = _T("\"");
	static const TCHAR BDSTOKEN_LEFT[] = _T("yunData.MYBDSTOKEN = \"");
	static const TCHAR BDSTOKEN_RIGHT[] = _T("\"");
	static const TCHAR DLINK_LEFT[] = _T("\"dlink\":\"");
	static const TCHAR DLINK_RIGHT[] = _T("\"");
	src = HTTPGet(UPDATE_URL, FALSE);
	CString timeStamp = GetStringBetween(src, TIME_STAMP_LEFT, TIME_STAMP_RIGHT);
	CString sign = GetStringBetween(src, SIGN_LEFT, SIGN_RIGHT);
	CString bdstoken = GetStringBetween(src, BDSTOKEN_LEFT, BDSTOKEN_RIGHT);
	CString url;
	url.Format(_T("http://pan.baidu.com/api/sharedownload?sign=%s&timestamp=%s&bdstoken=%s&channel=chunlei")
			   _T("&clienttype=0&web=1&app_id=250528"), sign, timeStamp, bdstoken);
	src = HTTPPost(url, _T("encrypt=0&product=share&uk=436464474&primaryid=497149087&fid_list=%5B") + fs_id + _T("%5D"), FALSE);

	// �ļ���������
	CString dlink = GetStringBetween(src, DLINK_LEFT, DLINK_RIGHT);
	if (dlink == _T(""))
		return UPDATE_FAILED_TO_GET_LINK;

	dlink.Replace(_T("\\/"), _T("/"));
	ShellExecute(NULL, _T("open"), dlink, NULL, NULL, SW_NORMAL);
	AfxMessageBox(_T("���غ��ѹ�����Ǿɰ汾�ļ����ɱ�����ǰ����"), MB_ICONINFORMATION);
	return UPDATE_HAS_UPDATE;
}

// �Զ������߳�
UINT AFX_CDECL AutoUpdateThread(LPVOID _dlg)
{
	CTiebaManagerDlg* dlg = (CTiebaManagerDlg*)_dlg;
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	CheckUpdateResult res = CheckUpdate();
	CoUninitialize();
	switch (res)
	{
	case UPDATE_FAILED_TO_GET_FILE_ID:
		dlg->m_stateStatic.SetWindowText(_T("������ʧ�ܣ���ȡ�ļ�IDʧ�ܣ����������ֶ�������"));
		break;
	case UPDATE_FAILED_TO_GET_LINK:
		if (AfxMessageBox(_T("��ȡ���ص�ַʧ�ܣ��ֶ����£�"), MB_ICONQUESTION | MB_YESNO) == IDYES)
			ShellExecute(NULL, _T("open"), UPDATE_FULL_URL, NULL, NULL, SW_NORMAL);
	case UPDATE_NO_UPDATE:
	case UPDATE_HAS_UPDATE:
		dlg->m_stateStatic.SetWindowText(_T("������"));
		break;
	}

	return 0;
}

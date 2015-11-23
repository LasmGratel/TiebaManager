#pragma once
#include <set>
using std::set;
#include <map>
using std::map;
class CTiebaManagerDlg;
struct IHTMLDocument2;


extern volatile BOOL g_stopScanFlag;
extern CWinThread* g_scanThread;
extern set<__int64> g_initIgnoredTID; // ��ɾ������ID(�ֶ�����)��Ҫд���ļ�
extern set<__int64> g_initIgnoredPID; // ��ɾ������ID(�ֶ�����)��Ҫд���ļ�
extern set<__int64> g_initIgnoredLZLID; // ��ɾ��¥��¥ID(�ֶ�����)��Ҫд���ļ�
extern set<__int64> g_ignoredTID; // ��ɾ������ID(��ɨ����Υ��)
extern set<__int64> g_ignoredPID; // ��ɾ������ID(��ɨ����Υ��)
extern set<__int64> g_ignoredLZLID; // ��ɾ��¥��¥ID(��ɨ����Υ��)
extern set<__int64> g_deletedTID; // ��ɾ������ID
extern map<__int64, int> g_reply; // ����Ļظ�����Ҫд���ļ�
extern map<CString, int> g_userTrigCount; // ĳ�û�Υ�������Ҫд���ļ�
extern set<CString> g_bannedUser; // �ѷ���û�
extern set<CString> g_defriendedUser; // �����ڵ��û���Ҫд���ļ�


BOOL CheckIllegal(LPCTSTR content, LPCTSTR author, CString& msg, int& pos, int& length);
UINT AFX_CDECL ScanThread(LPVOID mainDlg);
UINT AFX_CDECL ScanPostThread(LPVOID threadID);
BOOL ScanPostPage(const CString& tid, int page, const CString& title, BOOL hasHistoryReply,
	int ScanedCount, const CString& src, int threadID, CTiebaManagerDlg* dlg, CComPtr<IHTMLDocument2>* pDocument);

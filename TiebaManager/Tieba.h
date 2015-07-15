#pragma once
#include "TiebaManagerDlg.h"
#include <regex>
using std::wregex;
#include <vector>
using std::vector;
#include <set>
using std::set;
#include <map>
using std::map;


// ���ɡ��û���Ϣ
extern CString g_forumName;
extern CString g_encodedForumName;
extern CString g_forumID;
extern CString g_cookie;
extern CString g_tbs; // ���û�ȡtbs��ַ��http://tieba.baidu.com/dc/common/tbs


// �ɼ������õĳ���
#pragma region �û���Ϣ
// 3��fid��7��������
const wregex FORUM_ID_NAME_REG(_T("id('|\")?:\\s*('|\")?(\\d+)('|\")?,\
.*?name('|\")?:\\s*('|\")(.*?)('|\")"));
// 3���û���
const wregex USER_NAME_REG(_T("name('|\")?:\\s*('|\")(.*?)('|\")"));
const TCHAR _TBS_LEFT[] = _T("PageData.tbs = \"");
const TCHAR _TBS_RIGHT[] = _T("\"");
#pragma endregion


struct ThreadInfo // ������Ϣ
{
	CString tid;		// ����ID
	CString reply;		// �ظ���
	CString title;		// ����
	CString preview;	// Ԥ��
	CString author;		// ����
};

struct PostInfo // ������Ϣ
{
	CString pid;		// ����ID
	CString floor;		// ¥��
	CString author;		// ����
	CString content;	// ����
};

enum TBObject { TBOBJ_THREAD, TBOBJ_POST, TBOBJ_LZL };


// �ɼ� /////////////////////////////////////////////////////////////////////////////////
BOOL GetThreads(LPCTSTR forumName, LPCTSTR ignoreThread, vector<ThreadInfo>& threads);
enum GetPostsResult { GET_POSTS_SUCCESS, GET_POSTS_STOP, GET_POSTS_TIMEOUT, GET_POSTS_DELETED };
GetPostsResult GetPosts(const CString& tid, const CString& _src, const CString& page, vector<PostInfo>& posts, vector<PostInfo>& lzls);
void GetLzls(const CString& tid, const CString& page, vector<PostInfo>& posts, vector<PostInfo>& lzls);

// ɨ�� /////////////////////////////////////////////////////////////////////////////////
extern volatile BOOL g_stopScanFlag;
extern CWinThread* g_scanThread;
extern set<__int64> g_initIgnoredTID; // ��ɾ������ID(�ֶ�����)��Ҫд���ļ�
extern set<__int64> g_initIgnoredPID; // ��ɾ������ID(�ֶ�����)��Ҫд���ļ�
extern set<__int64> g_initIgnoredLZLID; // ��ɾ��¥��¥ID(�ֶ�����)��Ҫд���ļ�
extern set<__int64> g_ignoredTID; // ��ɾ������ID(��ɨ����Υ��)
extern set<__int64> g_ignoredPID; // ��ɾ������ID(��ɨ����Υ��)
extern set<__int64> g_ignoredLZLID; // ��ɾ��¥��¥ID(��ɨ����Υ��)
extern set<__int64> g_deletedTID; // ��ɾ������ID
extern map<__int64, int> g_reply; // ����Ļظ���
extern map<CString, int> g_IDTrigCount; // ĳIDΥ��������ѷ�Ϊ-1
BOOL CheckIllegal(LPCTSTR content, LPCTSTR author, CString& msg);
UINT AFX_CDECL ScanThread(LPVOID mainDlg);
UINT AFX_CDECL ScanPostThread(LPVOID threadID);
BOOL ScanPostPage(const CString& tid, int page, const CString& title, BOOL hasHistoryReply,
	int ScanedCount, const CString& src, int threadID, CTiebaManagerDlg* dlg, CComPtr<IHTMLDocument2>* pDocument);

// ���� /////////////////////////////////////////////////////////////////////////////////
struct Operation
{
	CString msg;		// ��ʾ��Ϣ
	TBObject object;	// ��������
	CString tid;		// ����ID
	CString title;		// �������
	CString floor;		// ¥��
	CString pid;		// ����ID
	CString author;		// ��������
};
extern CWinThread* g_operateThread;
void AddOperation(const CString& msg, TBObject object, const CString& tid, const CString& title, 
	const CString& floor, const CString& pid, const CString& author);
UINT AFX_CDECL OperateThread(LPVOID mainDlg);
CString BanID(LPCTSTR userName, LPCTSTR pid);
CString DeleteThread(const CString& tid);
CString DeletePost(LPCTSTR tid, LPCTSTR pid);
CString DeleteLZL(LPCTSTR tid, LPCTSTR lzlid);
CString GetTiebaErrorText(const CString& errorCode);

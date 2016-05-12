#pragma once
#include "TiebaAPICommon.h"


// �ɼ������õĳ���
#pragma region �û���Ϣ
// 3��fid��7��������
const wregex FORUM_ID_NAME_REG(_T("id('|\")?:\\s*('|\")?(\\d+)('|\")?,")
							   _T(".*?name('|\")?:\\s*('|\")(.*?)('|\")"));
// 3���û���
const wregex USER_NAME_REG(_T("name('|\")?:\\s*('|\")(.*?)('|\")"));
const TCHAR _TBS_LEFT[] = _T("PageData.tbs = \"");
const TCHAR _TBS_RIGHT[] = _T("\"");
// 3��tbs
const wregex TBS_REG(_T("tbs('|\")?:\\s*('|\")(.*?)('|\")"));
#pragma endregion


// ������Ϣ
struct ThreadInfo
{
	CString tid;		// ����ID
	CString reply;		// �ظ���
	CString title;		// ����
	CString preview;	// Ԥ��
	CString author;		// ����
	CString authorID;	// ����ID
	CString lastAuthor; // ���ظ�
};

// ������Ϣ
struct PostInfo
{
	CString pid;			// ����ID
	CString floor;			// ¥��
	CString author;			// ����
	CString authorID;		// ����ID
	CString authorLevel;	// ���ߵȼ�
	CString authorPortrait;	// ����ͷ���ϣ
	CString content;		// ����
};


TIEBA_API_API BOOL GetThreads(LPCTSTR forumName, LPCTSTR ignoreThread, vector<ThreadInfo>& threads);
enum GetPostsResult { GET_POSTS_SUCCESS, GET_POSTS_TIMEOUT, GET_POSTS_DELETED };
TIEBA_API_API GetPostsResult GetPosts(const CString& tid, const CString& _src, const CString& page, vector<PostInfo>& posts);
TIEBA_API_API void GetLzls(const CString& fid, const CString& tid, const CString& page, const vector<PostInfo>& posts, vector<PostInfo>& lzls);

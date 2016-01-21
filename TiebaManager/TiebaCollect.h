#pragma once


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


struct ThreadInfo // ������Ϣ
{
	CString tid;		// ����ID
	CString reply;		// �ظ���
	CString title;		// ����
	CString preview;	// Ԥ��
	CString author;		// ����
	CString authorID;	// ����ID
};

struct PostInfo // ������Ϣ
{
	CString pid;		// ����ID
	CString floor;		// ¥��
	CString author;		// ����
	CString authorID;	// ����ID
	CString authorPortrait;	// ����ͷ���ϣ
	CString content;	// ����
};


BOOL GetThreads(LPCTSTR forumName, LPCTSTR ignoreThread, vector<ThreadInfo>& threads);
enum GetPostsResult { GET_POSTS_SUCCESS, GET_POSTS_STOP, GET_POSTS_TIMEOUT, GET_POSTS_DELETED };
GetPostsResult GetPosts(const CString& tid, const CString& _src, const CString& page, vector<PostInfo>& posts, vector<PostInfo>& lzls);
void GetLzls(const CString& tid, const CString& page, vector<PostInfo>& posts, vector<PostInfo>& lzls);

CString GetPIDFromUser(const CString& userName);

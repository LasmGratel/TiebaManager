/*
Copyright (C) 2015  xfgryujk
http://tieba.baidu.com/f?kw=%D2%BB%B8%F6%BC%AB%C6%E4%D2%FE%C3%D8%D6%BB%D3%D0xfgryujk%D6%AA%B5%C0%B5%C4%B5%D8%B7%BD

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "stdafx.h"
#include "TiebaManagerDlg.h"
#include "Tieba.h"
#include <queue>
using std::queue;
#include "Global.h"
#include "Setting.h"
#include "ConfirmDlg.h"
#include <Mmsystem.h>
#include "ScanImage.h"


// ���ɡ��û���Ϣ
CString g_forumName;
CString g_encodedForumName;
CString g_forumID;
CString g_cookie;
CString g_tbs;


// �ɼ������õĳ���
// ������ʽ̫�����Բ���
#pragma region �����б�
// ���ջ���
const TCHAR TOPIC_LEFT[] = _T("<ul id=\"thread_topic");
const TCHAR TOPIC_RIGHT[] = _T("</ul>");
const TCHAR TOPIC_TID_LEFT[] = _T("href=\"http://tieba.baidu.com/p/");
const TCHAR TOPIC_TID_RIGHT[] = _T("\"");
const TCHAR TOPIC_REPLY_LEFT[] = _T("title=\"");
const TCHAR TOPIC_REPLY_RIGHT[] = _T("���ظ�\"");
const TCHAR TOPIC_TITLE_LEFT[] = _T("title=\"");
const TCHAR TOPIC_TITLE_RIGHT[] = _T("\"");
const TCHAR TOPIC_PREVIEW_LEFT[] = _T("class=\"threadlist_abs\">");
const TCHAR TOPIC_PREVIEW_RIGHT[] = _T("</div>");

// ��ͨ����
const TCHAR THREAD_SPLIT[] = _T("data-field='{&quot;author_name&quot;:&quot;");
const TCHAR THREAD_END[] = _T("<div id=\"frs_list_pager\"");
const TCHAR THREAD_TID_LEFT[] = _T("&quot;id&quot;:");
const TCHAR THREAD_TID_RIGHT[] = _T(",");
const TCHAR THREAD_REPLY_LEFT[] = _T("&quot;reply_num&quot;:");
const TCHAR THREAD_REPLY_RIGHT[] = _T(",");
const TCHAR THREAD_TITLE_LEFT[] = _T("class=\"j_th_tit\">");
const TCHAR THREAD_TITLE_RIGHT[] = _T("</a>");
const TCHAR THREAD_PREVIEW_LEFT[] = _T("threadlist_abs_onlyline\">");
const TCHAR THREAD_PREVIEW_RIGHT[] = _T("</div>");
const TCHAR THREAD_MEDIA_LEFT[] = _T("<ul class=\"threadlist_media");
const TCHAR THREAD_MEDIA_RIGHT[] = _T("</ul>");
const TCHAR THREAD_AUTHOR_LEFT[] = _T("&quot;author_name&quot;:&quot;");
const TCHAR THREAD_AUTHOR_RIGHT[] = _T("&quot;");
#pragma endregion
#pragma region �����б�
const TCHAR PAGE_COUNT_LEFT[] = _T(",\"total_page\":");
const TCHAR PAGE_COUNT_RIGHT[] = _T("}");

const TCHAR POST_SPLIT[] = _T("data-field='{&quot;author&quot;:");
const TCHAR POST_PID_LEFT[] = _T("&quot;post_id&quot;:");
const TCHAR POST_PID_RIGHT[] = _T(",");
const TCHAR POST_FLOOR_LEFT[] = _T("&quot;post_no&quot;:");
const TCHAR POST_FLOOR_RIGHT[] = _T(",");
const TCHAR POST_AUTHOR_LEFT[] = _T("&quot;user_name&quot;:&quot;");
const TCHAR POST_AUTHOR_RIGHT[] = _T("&quot;");
const TCHAR POST_CONTENT_LEFT[] = _T("<cc>");
const TCHAR POST_CONTENT_RIGHT[] = _T("</cc>");
const TCHAR POST_SIGN_LEFT[] = _T("<img class=\"j_user_sign\"");
const TCHAR POST_SIGN_RIGHT[] = _T("/>");
#pragma endregion
#pragma region ¥��¥�б�
const wregex LZL_FLOOR_REG(_T("\"(\\d+)\":.*?\"comment_info\":\\[(.*?)during_time\":\\d+\\}\\]"));

const TCHAR LZL_SPLIT[] = _T("{\"thread_id\"");
const TCHAR LZL_PID_LEFT[] = _T("\"comment_id\":\"");
const TCHAR LZL_PID_RIGHT[] = _T("\"");
const TCHAR LZL_AUTHOR_LEFT[] = _T("\"username\":\"");
const TCHAR LZL_AUTHOR_RIGHT[] = _T("\"");
const TCHAR LZL_CONTENT_LEFT[] = _T("\"content\":\"");
const TCHAR LZL_CONTENT_RIGHT[] = _T("\",\"");
#pragma endregion


volatile BOOL g_stopScanFlag = FALSE;
CWinThread* g_scanThread = NULL;
set<__int64> g_initIgnoredTID; // ��ɾ������ID(�ֶ�����)��Ҫд���ļ�
set<__int64> g_initIgnoredPID; // ��ɾ������ID(�ֶ�����)��Ҫд���ļ�
set<__int64> g_initIgnoredLZLID; // ��ɾ��¥��¥ID(�ֶ�����)��Ҫд���ļ�
set<__int64> g_ignoredTID; // ��ɾ������ID(��ɨ����Υ��)
set<__int64> g_ignoredPID; // ��ɾ������ID(��ɨ����Υ��)
set<__int64> g_ignoredLZLID; // ��ɾ��¥��¥ID(��ɨ����Υ��)
set<__int64> g_deletedTID; // ��ɾ������ID
map<__int64, int> g_reply; // ����Ļظ���
map<CString, int> g_IDTrigCount; // ĳIDΥ��������ѷ�Ϊ-1

static vector<ThreadInfo> g_threads; // ��ǰɨ��������б�
static int g_threadIndex; // �¸�Ҫɨ�����������
static CCriticalSection g_threadIndexLock;

static queue<Operation> g_operationQueue; // ��������
static CCriticalSection g_operationQueueLock;
CWinThread* g_operateThread = NULL;


// �ɼ� /////////////////////////////////////////////////////////////////////////////////
// ȡ�����б�
BOOL GetThreads(LPCTSTR forumName, LPCTSTR ignoreThread, vector<ThreadInfo>& threads)
{
	CString src = HTTPGet(_T("http://tieba.baidu.com/f?ie=UTF-8&kw=") + EncodeURI(forumName) 
		+ _T("&tp=0&pn=") + ignoreThread + _T("&apage=1"), FALSE, &g_stopScanFlag);

	CStringArray rawThreads;
	SplitString(rawThreads, src, THREAD_SPLIT);
	if (rawThreads.GetSize() < 2)
	{
		if (src != NET_STOP_TEXT && src != NET_TIMEOUT_TEXT)
			WriteString(src, _T("forum.txt"));
		return FALSE;
	}

	int iThreads;
	// ��������
	CString topic = GetStringBetween(rawThreads[0], TOPIC_LEFT, TOPIC_RIGHT);
	if (topic != _T(""))
	{
		threads.resize(rawThreads.GetSize());
		int pos = topic.Find(TOPIC_TID_LEFT);
		threads[0].tid = GetStringBetween(topic, TOPIC_TID_LEFT, TOPIC_TID_RIGHT, pos);
		threads[0].reply = GetStringBetween(topic, TOPIC_REPLY_LEFT, TOPIC_REPLY_RIGHT);
		threads[0].title = GetStringBetween(topic, TOPIC_TITLE_LEFT, TOPIC_TITLE_RIGHT, pos);
		threads[0].preview = GetStringBetween(topic, TOPIC_PREVIEW_LEFT, TOPIC_PREVIEW_RIGHT, pos);
		threads[0].author = _T("");

		iThreads = 1;
	}
	else
	{
		threads.resize(rawThreads.GetSize() - 1);
		iThreads = 0;
	}

	// ��ͨ����
	rawThreads[rawThreads.GetSize() - 1] = GetStringBefore(rawThreads[rawThreads.GetSize() - 1], THREAD_END);
	for (int iRawThreads = 1; iRawThreads < rawThreads.GetSize(); iRawThreads++, iThreads++)
	{
		threads[iThreads].tid = GetStringBetween(rawThreads[iRawThreads], THREAD_TID_LEFT, THREAD_TID_RIGHT);
		threads[iThreads].reply = GetStringBetween(rawThreads[iRawThreads], THREAD_REPLY_LEFT, THREAD_REPLY_RIGHT);
		threads[iThreads].title = HTMLUnescape(GetStringBetween(rawThreads[iRawThreads], THREAD_TITLE_LEFT, THREAD_TITLE_RIGHT));
		threads[iThreads].preview = HTMLUnescape(GetStringBetween(rawThreads[iRawThreads], THREAD_PREVIEW_LEFT, THREAD_PREVIEW_RIGHT))
			+ _T("\r\n") + GetStringBetween2(rawThreads[iRawThreads], THREAD_MEDIA_LEFT, THREAD_MEDIA_RIGHT);
		threads[iThreads].author = JSUnescape(GetStringBefore(rawThreads[iRawThreads], THREAD_AUTHOR_RIGHT));

		//OutputDebugString(_T("\n"));
		//OutputDebugString(rawThreads[iRawThreads]);
		//OutputDebugString(_T("\n----------------------------------"));
	}

	return TRUE;
}

// ȡ�����б�
GetPostsResult GetPosts(const CString& tid, const CString& _src, const CString& page, vector<PostInfo>& posts, vector<PostInfo>& lzls)
{
	CString src = _src != _T("") ? _src : HTTPGet(_T("http://tieba.baidu.com/p/") + tid + _T("?pn=") + page, FALSE, &g_stopScanFlag);
	if (src == NET_STOP_TEXT)
		return GET_POSTS_STOP;
	if (src == NET_TIMEOUT_TEXT)
		return GET_POSTS_TIMEOUT;

	CStringArray rawPosts;
	SplitString(rawPosts, src, POST_SPLIT);
	if (rawPosts.GetSize() < 2)
		return GET_POSTS_DELETED;
	
	posts.resize(rawPosts.GetSize() - 1);
	for (int iRawPosts = 1, iPosts = 0; iRawPosts < rawPosts.GetSize(); iRawPosts++, iPosts++)
	{
		posts[iPosts].pid = GetStringBetween(rawPosts[iRawPosts], POST_PID_LEFT, POST_PID_RIGHT);
		posts[iPosts].floor = GetStringBetween(rawPosts[iRawPosts], POST_FLOOR_LEFT, POST_FLOOR_RIGHT);
		posts[iPosts].author = JSUnescape(GetStringBetween(rawPosts[iRawPosts], POST_AUTHOR_LEFT, POST_AUTHOR_RIGHT));
		//posts[iPosts].content = GetStringBetween(rawPosts[iRawPosts], POST_CONTENT_LEFT, POST_CONTENT_RIGHT);
		
		int left = rawPosts[iRawPosts].Find(POST_CONTENT_LEFT) + _tcslen(POST_CONTENT_LEFT);
		left = rawPosts[iRawPosts].Find(_T(">"), left) + 1;
		// ȥ���׿ո�
		while (left < rawPosts[iRawPosts].GetLength() && rawPosts[iRawPosts][left] == _T(' '))
			left++;
		int right = rawPosts[iRawPosts].Find(POST_CONTENT_RIGHT, left + 1);
		// CString��֧�ַ�������ַ�����
		posts[iPosts].content = rawPosts[iRawPosts].Mid(left, right - left);
		LPCTSTR pos = StrRStrI(posts[iPosts].content, NULL, _T("</div>"));
		if (pos != NULL)
		{
			right = ((DWORD)pos - (DWORD)(LPCTSTR)posts[iPosts].content) / sizeof(TCHAR) - 1;
			// ȥ��β�ո�
			while (right >= 0 && posts[iPosts].content[right] == _T(' '))
				right--;
			posts[iPosts].content = posts[iPosts].content.Left(right + 1);
		}
		// ǩ����
		posts[iPosts].content += _T("\r\n") + GetStringBetween2(rawPosts[iRawPosts], POST_SIGN_LEFT, POST_SIGN_RIGHT);

		//OutputDebugString(_T("\n"));
		//OutputDebugString(rawPosts[iRawPosts]);
		//OutputDebugString(_T("\n----------------------------------"));
	}

	GetLzls(tid, page, posts, lzls);

	return GET_POSTS_SUCCESS;
}

// ȡ¥��¥�б�
void GetLzls(const CString& tid, const CString& page, vector<PostInfo>& posts, vector<PostInfo>& lzls)
{
	time_t timestamp;
	time(&timestamp);
	CString url;
	url.Format(_T("http://tieba.baidu.com/p/totalComment?t=%I64d&tid=%s&fid=%s&pn=%s&see_lz=0"), timestamp, tid, g_forumID, page);
	CString src = HTTPGet(url, FALSE, &g_stopScanFlag);

	lzls.clear();
	int iLzls = 0;
	// ����¥��
	for (std::regex_iterator<LPCTSTR> it((LPCTSTR)src, (LPCTSTR)src + src.GetLength(), LZL_FLOOR_REG), end; it != end; it++)
	{
		// ���Ҹò�¥��
		CString pid = (*it)[1].str().c_str(); // �ò�PID
		CString floor;
		for (PostInfo post : posts)
			if (post.pid == pid)
			{
				floor = post.floor;
				break;
			}
		
		// �����ò�¥��¥
		CStringArray rawLzls;
		SplitString(rawLzls, (*it)[2].str().c_str(), LZL_SPLIT);
		lzls.resize(lzls.size() + rawLzls.GetSize() - 1);
		for (int iRawLzls = 1; iRawLzls < rawLzls.GetSize(); iRawLzls++, iLzls++)
		{
			lzls[iLzls].pid = GetStringBetween(rawLzls[iRawLzls], LZL_PID_LEFT, LZL_PID_RIGHT);
			lzls[iLzls].floor = floor;
			lzls[iLzls].author = JSUnescape(GetStringBetween(rawLzls[iRawLzls], LZL_AUTHOR_LEFT, LZL_AUTHOR_RIGHT));
			lzls[iLzls].content = HTMLUnescape(JSUnescape(GetStringBetween(rawLzls[iRawLzls], LZL_CONTENT_LEFT, LZL_CONTENT_RIGHT)));
		}

		//OutputDebugString(_T("\n"));
		//OutputDebugString(floorContent);
		//OutputDebugString(_T("\n----------------------------------"));
	}
}


// ɨ�� /////////////////////////////////////////////////////////////////////////////////
// ���Υ��
BOOL CheckIllegal(LPCTSTR content, LPCTSTR author, CString& msg, int& pos, int& length)
{
	g_optionsLock.Lock();

	// �����û�
	if (g_whiteList.find(author) != g_whiteList.end())
	{
		g_optionsLock.Unlock();
		return FALSE;
	}

	// ��������
	for (const RegexText& whiteContent : g_whiteContent)
		if (StringIncludes(content, whiteContent))
		{
			g_optionsLock.Unlock();
			return FALSE;
		}
	
	// Υ������
	for (const RegexText& keyword : g_keywords)
		if (StringIncludes(content, keyword, &pos, &length))
		{
			msg = _T("<font color=red> ����Υ���� </font>") + HTMLEscape(keyword.text);
			g_optionsLock.Unlock();
			return TRUE;
		}

	// �����û�
	for (const RegexText& blackList : g_blackList)
		if (StringMatchs(author, blackList))
		{
			pos = 0;
			length = 0;
			msg = _T("<font color=red> ���������û� </font>") + HTMLEscape(blackList.text);
			g_optionsLock.Unlock();
			return TRUE;
		}

	g_optionsLock.Unlock();
	return FALSE;
}

// ɨ������ͼƬ
static inline void ScanThreadImage(CString& msg, CTiebaManagerDlg* dlg, CComPtr<IHTMLDocument2>* pDocument)
{
	for (const ThreadInfo& thread : g_threads)
	{
		if (g_stopScanFlag)
			break;
		__int64 tid = _ttoi64(thread.tid);
		if (g_ignoredTID.find(tid) == g_ignoredTID.end()
			&& CheckImageIllegal(thread.preview, thread.author, GetThreadImage, msg))
		{
			AddOperation(thread.title + _T("\r\n") + thread.preview, TBOBJ_THREAD, thread.tid,
				thread.title, _T("1"), _T(""), thread.author);
			dlg->Log(_T("<a href=\"http://tieba.baidu.com/p/") + thread.tid + _T("\">")
				+ HTMLEscape(thread.title) + _T("</a>") + msg, pDocument);
			g_ignoredTID.insert(tid);
		}
	}
}

// ��ɨ���߳�
UINT AFX_CDECL ScanThread(LPVOID mainDlg)
{
	CTiebaManagerDlg* dlg = (CTiebaManagerDlg*)mainDlg;
	g_stopScanFlag = FALSE;
	dlg->m_startButton.EnableWindow(FALSE);
	dlg->m_pageEdit.EnableWindow(FALSE);
	dlg->m_stopButton.EnableWindow(TRUE);

	// ��ʼ�����߳�
	if (!g_operationQueue.empty() && g_operateThread == NULL)
		g_operateThread = AfxBeginThread(OperateThread, mainDlg);
	
	// ��ʼ����־�ĵ�
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		CString code;
		code.Format(_T("CoInitializeEx: 0x%08X"), hr);
		WriteString(code, _T("error.txt"));
	}
	CComPtr<IHTMLDocument2> document;
	dlg->GetLogDocument(document);
	CComPtr<IHTMLDocument2>* pDocument = (CComPtr<IHTMLDocument2>*)&(int&)document;

	// ��ʼ��ҳ��
	CString sPage;
	dlg->m_pageEdit.GetWindowText(sPage);
	int iPage = _ttoi(sPage);
	CString ignoreThread; // ����ǰ��������
	ignoreThread.Format(_T("%d"), (iPage - 1) * 50);

	CString msg;
	int pos, length;
	while (!g_stopScanFlag)
	{
		DWORD startTime = GetTickCount();
		dlg->m_stateStatic.SetWindowText(_T("ɨ��������"));
		if (!g_briefLog)
			dlg->Log(_T("<font color=green>����ɨ�迪ʼ��ʹ�÷�����</font>") + g_currentOption, pDocument);
		
		// ��ȡ�����б�
		if (!GetThreads(g_forumName, ignoreThread, g_threads))
		{
			if (g_stopScanFlag)
				break;
			if (!g_briefLog)
				dlg->Log(_T("<font color=red>��ȡ�����б�ʧ�ܣ����¿�ʼ����</font>"), pDocument);
			continue;
		}

		// ɨ������
		for (const ThreadInfo& thread : g_threads)
		{
			if (g_stopScanFlag)
				break;
			__int64 tid = _ttoi64(thread.tid);
			if (g_ignoredTID.find(tid) == g_ignoredTID.end()
				&& CheckIllegal(thread.title + _T("\r\n") + thread.preview, thread.author, msg, pos, length))
			{
				AddOperation(thread.title + _T("\r\n") + thread.preview, TBOBJ_THREAD, thread.tid, 
					thread.title, _T("0"), _T(""), thread.author, pos, length);
				dlg->Log(_T("<a href=\"http://tieba.baidu.com/p/") + thread.tid + _T("\">")
					+ HTMLEscape(thread.title) + _T("</a>") + msg, pDocument);
				g_ignoredTID.insert(tid);
			}
		}
		// ɨ������ͼƬ
		BOOL scanImage = !g_images.empty();
		if (g_onlyScanTitle && scanImage)
		{
			scanImage = FALSE;
			ScanThreadImage(msg, dlg, pDocument);
		}

		// ɨ������
		if (!g_onlyScanTitle)
		{
			dlg->m_stateStatic.SetWindowText(_T("ɨ��������"));
			g_threadIndex = 0;
			// �����߳�ɨ������
			int threadCount = g_threadCount; // g_threadCount���
			CWinThread** threadObjects = new CWinThread*[threadCount];
			HANDLE* threadHandles = new HANDLE[threadCount];
			for (int i = 0; i < threadCount; i++)
			{
				dlg->m_stateList.AddString(_T("׼����"));
				threadObjects[i] = AfxBeginThread(ScanPostThread, (LPVOID)i, 0, 0, CREATE_SUSPENDED);
				threadObjects[i]->m_bAutoDelete = FALSE;
				threadHandles[i] = threadObjects[i]->m_hThread;
				threadObjects[i]->ResumeThread();
			}
			// �ȴ�ɨ������ʱɨ������ͼƬ
			if (scanImage)
				ScanThreadImage(msg, dlg, pDocument);
			WaitForMultipleObjects(threadCount, threadHandles, TRUE, INFINITE);

			// �ͷ��߳�
			for (int i = 0; i < threadCount; i++)
			{
				//CloseHandle(threadHandles[i]); // ��������Զ��ͷţ������ϻ��쳣
				delete threadObjects[i];
			}
			delete threadHandles;
			delete threadObjects;
			dlg->m_stateList.ResetContent();
		}

		dlg->m_stateStatic.SetWindowText(_T("��ʱ��"));
		if (!g_briefLog)
		{
			CString content;
			content.Format(_T("<font color=green>����ɨ���������ʱ%.3f��</font>"), (float)(GetTickCount() - startTime) / 1000.0f);
			dlg->Log(content, pDocument);
		}

		// ��ʱ
		int count = g_scanInterval * 10;
		for (int i = 0; i < count; i++)
		{
			if (g_stopScanFlag)
				break;
			Sleep(100);
		}
	}
	g_stopScanFlag = FALSE;

	if (!g_briefLog)
		dlg->Log(_T("<font color=green>ɨ�����</font>"), pDocument);
	CoUninitialize();
	dlg->m_stopButton.EnableWindow(FALSE);
	dlg->m_startButton.EnableWindow(TRUE);
	dlg->m_pageEdit.EnableWindow(TRUE);
	dlg->m_stateStatic.SetWindowText(_T("������"));

	g_scanThread = NULL;
	return 0;
}

// ɨ�������߳�
UINT AFX_CDECL ScanPostThread(LPVOID _threadID)
{
	int threadID = (int)_threadID;
	CTiebaManagerDlg* dlg = (CTiebaManagerDlg*)AfxGetApp()->m_pMainWnd;
	// ��ʼ����־�ĵ�
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	CComPtr<IHTMLDocument2> document;
	dlg->GetLogDocument(document);
	CComPtr<IHTMLDocument2>* pDocument = (CComPtr<IHTMLDocument2>*)&(int&)document;

	CString pageCount, src;
	map<__int64, int>::iterator historyReplyIt;
	g_threadIndexLock.Lock();
	while (!g_stopScanFlag && g_threadIndex < (int)g_threads.size())
	{
		ThreadInfo& thread = g_threads[g_threadIndex++];
		g_threadIndexLock.Unlock();
		if (g_deletedTID.find(_ttoi64(thread.tid)) != g_deletedTID.end()) // ��ɾ
			goto next;
		g_optionsLock.Lock();
		if (g_trustedThread.find(thread.tid) != g_trustedThread.end()) // ����
		{
			g_optionsLock.Unlock();
			goto next;
		}
		g_optionsLock.Unlock();

		__int64 tid = _ttoi64(thread.tid);
		int reply = _ttoi(thread.reply);
		historyReplyIt = g_reply.find(tid);
		BOOL hasHistoryReply = historyReplyIt != g_reply.end();
		if (hasHistoryReply && reply <= historyReplyIt->second) // ���»ظ�
		{
			historyReplyIt->second = reply;
			goto next;
		}

		// ��һҳ
		src = HTTPGet(_T("http://tieba.baidu.com/p/" + thread.tid), FALSE, &g_stopScanFlag);
		if (src == NET_STOP_TEXT)
			goto next;
		if (src == NET_TIMEOUT_TEXT)
		{
			if (!g_briefLog)
				dlg->Log(_T("<a href=\"http://tieba.baidu.com/p/") + thread.tid + _T("\">") + thread.title
				+ _T("</a> <font color=red>��ȡ�����б�ʧ��(��ʱ)����ʱ����</font>"), pDocument);
			goto next;
		}

		// ����ҳ��
		pageCount = GetStringBetween(src, PAGE_COUNT_LEFT, PAGE_COUNT_RIGHT);
		if (pageCount == _T(""))
		{
			WriteString(src, _T("thread.txt"));
			if (!g_briefLog)
				dlg->Log(_T("<a href=\"http://tieba.baidu.com/p/") + thread.tid + _T("\">") + thread.title
				+ _T("</a> <font color=red>��ȡ�����б�ʧ��(�����ѱ�ɾ)����ʱ����</font>"), pDocument);
			goto next;
		}

		// ɨ������ҳ
		int iPageCount = _ttoi(pageCount);
		BOOL res = ScanPostPage(thread.tid, 1, thread.title, hasHistoryReply, 0, src, threadID, dlg, pDocument);
		if (iPageCount > 1 && !g_stopScanFlag)
			res = ScanPostPage(thread.tid, iPageCount, thread.title, hasHistoryReply, 0, _T(""), threadID, dlg, pDocument);

		// ��¼��ʷ�ظ�
		if (res)
			g_reply[tid] = reply;

next:
		g_threadIndexLock.Lock();
	}
	g_threadIndexLock.Unlock();
	
	CoUninitialize();
	dlg->m_stateList.DeleteString(threadID);
	dlg->m_stateList.InsertString(threadID, _T("�߳̽���"));
	return 0;
}

// ɨ������ҳ
BOOL ScanPostPage(const CString& tid, int page, const CString& title, BOOL hasHistoryReply, 
	int ScanedCount, const CString& src, int threadID, CTiebaManagerDlg* dlg, CComPtr<IHTMLDocument2>* pDocument)
{
	CString sPage;
	sPage.Format(_T("%d"), page);
	dlg->m_stateList.DeleteString(threadID);
	dlg->m_stateList.InsertString(threadID, tid + _T(":") + sPage + _T(" ") + title);

	// ��ȡ�����б�
	vector<PostInfo> posts, lzls;
	GetPostsResult res = GetPosts(tid, src, sPage, posts, lzls);
	switch (res)
	{
	case GET_POSTS_TIMEOUT:
	case GET_POSTS_DELETED:
		dlg->Log(_T("<a href=\"http://tieba.baidu.com/p/") + tid + _T("\">") + title
			+ _T("</a> <font color=red>��ȡ�����б�ʧ��(") + (res == GET_POSTS_TIMEOUT ? _T("��ʱ") : 
			_T("�����ѱ�ɾ")) + _T(")����ʱ����</font>"), pDocument);
	case GET_POSTS_STOP:
		return FALSE;
	}

	CString msg;
	int pos, length;
	// ɨ������
	for (const PostInfo& post : posts)
	{
		if (g_stopScanFlag)
			return FALSE;
		__int64 pid = _ttoi64(post.pid);
		if (g_ignoredPID.find(pid) == g_ignoredPID.end() 
			&& CheckIllegal(post.content, post.author, msg, pos, length))
		{
			AddOperation(post.content, post.floor == _T("1") ? TBOBJ_THREAD : TBOBJ_POST, 
				tid, title, post.floor, post.pid, post.author, pos, length);
			dlg->Log(_T("<a href=\"http://tieba.baidu.com/p/") + tid + _T("\">") + HTMLEscape(title) + 
				_T("</a> ") + post.floor + _T("¥") + msg, pDocument);
			g_ignoredPID.insert(pid);
		}
	}

	// ɨ��¥��¥
	for (const PostInfo& lzl : lzls)
	{
		if (g_stopScanFlag)
			return FALSE;
		if (CheckIllegal(lzl.content, lzl.author, msg, pos, length))
		{
			__int64 lzlid = _ttoi64(lzl.pid);
			if (g_ignoredLZLID.find(lzlid) == g_ignoredLZLID.end())
			{
				AddOperation(lzl.content, TBOBJ_LZL, tid, title, lzl.floor, lzl.pid, lzl.author, pos, length);
				dlg->Log(_T("<a href=\"http://tieba.baidu.com/p/") + tid + _T("\">") + HTMLEscape(title) +
					_T("</a> ") + lzl.floor + _T("¥�ظ�") + msg, pDocument);
				g_ignoredLZLID.insert(lzlid);
			}
		}
	}

	// ɨ������ͼƬ
	for (const PostInfo& post : posts)
	{
		if (g_stopScanFlag)
			return FALSE;
		__int64 pid = _ttoi64(post.pid);
		if (g_ignoredPID.find(pid) == g_ignoredPID.end()
			&& CheckImageIllegal(post.content, post.author, GetPostImage, msg))
		{
			AddOperation(post.content, post.floor == _T("1") ? TBOBJ_THREAD : TBOBJ_POST,
				tid, title, post.floor, post.pid, post.author);
			dlg->Log(_T("<a href=\"http://tieba.baidu.com/p/") + tid + _T("\">") + HTMLEscape(title) +
				_T("</a> ") + post.floor + _T("¥") + msg, pDocument);
			g_ignoredPID.insert(pid);
		}
	}

	// �ݹ�ɨ����һҳ
	if (!hasHistoryReply) // �������ʷ�ظ�ǰ�漸ҳ�ܿ��ܱ�ɨ����ˣ����ݹ�
	{
		if (++ScanedCount < g_scanPageCount) // û�ﵽ���ɨ��ҳ��
		{
			if (--page < 2) // ɨ����
				return TRUE;
			return ScanPostPage(tid, page, title, FALSE, ScanedCount, _T(""), threadID, dlg, pDocument);
		}
	}
	return TRUE;
}


// ���� /////////////////////////////////////////////////////////////////////////////////
// ��Ӳ���
void AddOperation(const CString& msg, TBObject object, const CString& tid, const CString& title,
	const CString& floor, const CString& pid, const CString& author, int pos, int length)
{
	Operation tmp;
	tmp.msg = msg;
	tmp.pos = pos;
	tmp.length = length;
	tmp.object = object;
	tmp.tid = tid;
	tmp.title = title;
	tmp.floor = floor;
	tmp.pid = pid;
	tmp.author = author;
	g_operationQueueLock.Lock();
	g_operationQueue.push(tmp);
	g_operationQueueLock.Unlock();
	if (g_operateThread == NULL)
		g_operateThread = AfxBeginThread(OperateThread, AfxGetApp()->m_pMainWnd);
}

// �����߳�
UINT AFX_CDECL OperateThread(LPVOID mainDlg)
{
	CTiebaManagerDlg* dlg = (CTiebaManagerDlg*)mainDlg;

	// ��ʼ����־�ĵ�
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	CComPtr<IHTMLDocument2> document;
	dlg->GetLogDocument(document);
	CComPtr<IHTMLDocument2>* pDocument = (CComPtr<IHTMLDocument2>*)&(int&)document;

	while (!g_operationQueue.empty() && !g_stopScanFlag)
	{
		Operation op = g_operationQueue.front();
		g_operationQueueLock.Lock();
		g_operationQueue.pop();
		g_operationQueueLock.Unlock();

		// û�в���
		if (!g_delete && !g_banID)
			continue;

		// �����ѱ�ɾ
		__int64 tid = _ttoi64(op.tid);
		if (g_deletedTID.find(tid) != g_deletedTID.end())
			continue;
		
		// ȷ���Ƿ����
		if (g_confirm)
		{
			if (CConfirmDlg(&op).DoModal() == IDCANCEL)
			{
				switch (op.object)
				{
				case TBOBJ_THREAD:
					if (op.floor == _T("1"))
						goto casePost;
					g_initIgnoredTID.insert(tid);
					dlg->Log(_T("<font color=green>���� </font><a href=\"http://tieba.baidu.com/p/") + op.tid
						+ _T("\">") + HTMLEscape(op.title) + _T("</a>"), pDocument);
					break;
				case TBOBJ_POST:
casePost:			g_initIgnoredPID.insert(_ttoi64(op.pid));
					dlg->Log(_T("<font color=green>���� </font><a href=\"http://tieba.baidu.com/p/") + op.tid
						+ _T("\">") + HTMLEscape(op.title) + _T("</a> ") + op.floor + _T("¥"), pDocument);
					break;
				case TBOBJ_LZL:
					g_initIgnoredLZLID.insert(_ttoi64(op.pid));
					dlg->Log(_T("<font color=green>���� </font><a href=\"http://tieba.baidu.com/p/") + op.tid
						+ _T("\">") + HTMLEscape(op.title) + _T("</a> ") + op.floor + _T("¥�ظ�"), pDocument);
					break;
				}
				continue;
			}
		}

		// ���
		if (g_banID)
		{
			auto countIt = g_IDTrigCount.find(op.author);
			BOOL hasHistory = countIt != g_IDTrigCount.end();
			int count = hasHistory ? (countIt->second + 1) : 1;
			if (count < g_trigCount) // û�ﵽ���Υ�����
			{
				if (count != 0) // δ��
				{
					if (hasHistory)
						countIt->second = count;
					else
						g_IDTrigCount[op.author] = 1;
				}
			}
			else // �ﵽ���Υ�����
			{
				if (op.pid == _T(""))
				{
					vector<PostInfo> posts, lzls;
					GetPosts(op.tid, _T(""), _T("1"), posts, lzls);
					if (posts.size() > 0)
						op.pid = posts[0].pid;
				}
				if (op.pid == _T(""))
				{
					dlg->Log(_T("<font color=red>��� </font>") + op.author + _T("<font color=red> ʧ�ܣ�\
(��ȡ����IDʧ��)</font>"), pDocument);
				}
				else
				{
					CString code = BanID(op.author, op.pid);
					if (code != _T("0"))
					{
						CString content;
						content.Format(_T("<font color=red>��� </font>%s<font color=red> ʧ�ܣ�\
������룺%s(%s)</font><a href=\"BD:%s,%s\">����</a>"), op.author, code, GetTiebaErrorText(code), op.pid, op.author);
						dlg->Log(content, pDocument);
					}
					else
					{
						sndPlaySound(_T("���.wav"), SND_ASYNC | SND_NODEFAULT);
						if (hasHistory)
							countIt->second = -1;
						else
							g_IDTrigCount[op.author] = -1;
						dlg->Log(_T("<font color=red>��� </font>") + op.author, pDocument);
					}
				}
			}
		}

		// ɾ��
		if (!g_delete)
			continue;
		if (op.object == TBOBJ_THREAD) // ����
		{
			CString code = DeleteThread(op.tid);
			if (code != _T("0"))
			{
				CString content;
				content.Format(_T("<a href=\"http://tieba.baidu.com/p/%s\">%s</a><font color=red> ɾ��ʧ�ܣ�\
������룺%s(%s)</font><a href=\"DT:%s\">����</a>"), op.tid, HTMLEscape(op.title), code, GetTiebaErrorText(code), op.tid);
				dlg->Log(content, pDocument);
			}
			else
			{
				sndPlaySound(_T("ɾ��.wav"), SND_ASYNC | SND_NODEFAULT);
				g_deletedTID.insert(_ttoi64(op.tid));
				dlg->Log(_T("<font color=red>ɾ�� </font><a href=\"http://tieba.baidu.com/p/") + op.tid 
					+ _T("\">") + HTMLEscape(op.title) + _T("</a>"), pDocument);
				Sleep((DWORD)(g_deleteInterval * 1000));
			}
		}
		else if (op.object == TBOBJ_POST) // ����
		{
			CString code = DeletePost(op.tid, op.pid);
			if (code != _T("0"))
			{
				CString content;
				content.Format(_T("<a href=\"http://tieba.baidu.com/p/%s\">%s</a> %s¥<font color=red> ɾ��ʧ�ܣ�\
������룺%s(%s)</font><a href=\"DP:%s,%s\">����</a>"), op.tid, HTMLEscape(op.title), op.floor, code, 
					GetTiebaErrorText(code), op.tid, op.pid);
				dlg->Log(content, pDocument);
			}
			else
			{
				sndPlaySound(_T("ɾ��.wav"), SND_ASYNC | SND_NODEFAULT);
				dlg->Log(_T("<font color=red>ɾ�� </font><a href=\"http://tieba.baidu.com/p/") + op.tid
					+ _T("\">") + HTMLEscape(op.title) + _T("</a> ") + op.floor + _T("¥"), pDocument);
				Sleep((DWORD)(g_deleteInterval * 1000));
			}
		}
		else /*if (op.object == TBOBJ_POST)*/ // ¥��¥
		{
			CString code = DeleteLZL(op.tid, op.pid);
			if (code != _T("0"))
			{
				CString content;
				content.Format(_T("<a href=\"http://tieba.baidu.com/p/%s\">%s</a> %s¥�ظ�<font color=red> ɾ��ʧ�ܣ�\
������룺%s(%s)</font><a href=\"DL:%s,%s\">����</a>"), op.tid, HTMLEscape(op.title), op.floor, code,
								  GetTiebaErrorText(code), op.tid, op.pid);
				dlg->Log(content, pDocument);
			}
			else
			{
				sndPlaySound(_T("ɾ��.wav"), SND_ASYNC | SND_NODEFAULT);
				dlg->Log(_T("<font color=red>ɾ�� </font><a href=\"http://tieba.baidu.com/p/") + op.tid
					+ _T("\">") + HTMLEscape(op.title) + _T("</a> ") + op.floor + _T("¥�ظ�"), pDocument);
				Sleep((DWORD)(g_deleteInterval * 1000));
			}
		}
	}

	g_operateThread = NULL;
	CoUninitialize();
	return 0;
}

// ȡ�������
static inline CString GetOperationErrorCode(const CString& src)
{
	if (src == NET_TIMEOUT_TEXT /*|| src == NET_STOP_TEXT*/)
		return _T("-1");
	CString code = GetStringBetween(src, _T("no\":"), _T(","));
	if (code != _T("0"))
		WriteString(src, _T("operation.txt"));
	return code;
}

// ��ID�����ش������
CString BanID(LPCTSTR userName, LPCTSTR pid)
{
	CString data;
	data.Format(_T("day=%d&fid=%s&tbs=%s&ie=gbk&user_name%%5B%%5D=%s&pid%%5B%%5D=%s&reason=%s"), 
		g_banDuration, g_forumID, g_tbs, EncodeURI(userName), pid, g_banReason != _T("") ? g_banReason : _T(" "));
	CString src = HTTPPost(_T("http://tieba.baidu.com/pmc/blockid"), data);
	return GetOperationErrorCode(src);
}

// ɾ���⣬���ش������
CString DeleteThread(const CString& tid)
{
	CString src = HTTPPost(_T("http://tieba.baidu.com/f/commit/thread/delete"), _T("kw=") + g_encodedForumName
		+ _T("&fid=") + g_forumID + _T("&tid=") + tid + _T("&ie=utf-8&tbs=") + g_tbs);
	return GetOperationErrorCode(src);
}

// ɾ���ӣ����ش������
CString DeletePost(LPCTSTR tid, LPCTSTR pid)
{
	CString data;
	data.Format(_T("commit_fr=pb&ie=utf-8&tbs=%s&kw=%s&fid=%s&tid=%s&is_vipdel=0&pid=%s&is_finf=false"), 
		g_tbs, g_encodedForumName, g_forumID, tid, pid);
	CString src = HTTPPost(_T("http://tieba.baidu.com/f/commit/post/delete"), data);
	return GetOperationErrorCode(src);
}

// ɾ¥��¥�����ش������
CString DeleteLZL(LPCTSTR tid, LPCTSTR lzlid)
{
	CString data;
	data.Format(_T("ie=utf-8&tbs=%s&kw=%s&fid=%s&tid=%s&pid=%s&is_finf=1"),
		g_tbs, g_encodedForumName, g_forumID, tid, lzlid);
	CString src = HTTPPost(_T("http://tieba.baidu.com/f/commit/post/delete"), data);
	return GetOperationErrorCode(src);
}

// ȡ�����ı�
CString GetTiebaErrorText(const CString& errorCode)
{
	if (errorCode == _T("890"))
		return _T("������ɾ");
	if (errorCode == _T("-1"))
		return _T("��ʱ");
	if (errorCode == _T("78"))
		return _T("��������");
	if (errorCode == _T("4"))
		return _T("����У��ʧ��");
	if (errorCode == _T("11"))
		return _T("�������");
	if (errorCode == _T("14") || errorCode == _T("12"))
		return _T("�ѱ�ϵͳ���");
	if (errorCode == _T("308"))
		return _T("�㱻�����ʧȥȨ��");
	if (errorCode == _T("4"))
		return _T("С����ֻ�ܷ�1���С�������ܷ�IP");
	if (errorCode == _T("872"))
		return _T("��Ʒ������ɾ");
	if (errorCode == _T("871"))
		return _T("��¥����ɾ");
	return _T("δ֪����");
}

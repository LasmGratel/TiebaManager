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
#include "Tieba.h"
#include "Global.h"
#include "Setting.h"
#include <queue>
using std::queue;
#include <Mmsystem.h>


// ���ɡ��û���Ϣ
CString g_forumName;
CString g_encodedForumName;
CString g_forumID;
CString g_cookie;
CString g_tbs;

volatile BOOL g_stopScanFlag;
CWinThread* g_scanThread = NULL;
set<__int64> g_ignoredTID; // ��ɾ������ID(��ɨ��)
set<__int64> g_ignoredPID; // ��ɾ������ID(��ɨ��)
set<__int64> g_ignoredLZLID; // ��ɾ��¥��¥ID(��ɨ��)
set<__int64> g_deletedTID; // ��ɾ������ID
map<__int64, int> g_reply; // ����Ļظ���
map<CString, int> g_IDTrigCount; // ĳIDΥ��������ѷ�Ϊ-1

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
			WriteString(_T("forum.txt"), src);
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
		threads[iThreads].preview = HTMLUnescape(GetStringBetween(rawThreads[iRawThreads], THREAD_PREVIEW_LEFT, THREAD_PREVIEW_RIGHT));
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
		right = ((DWORD)StrRStrI(posts[iPosts].content, NULL, _T("</div>")) - (DWORD)(LPCTSTR)posts[iPosts].content) / sizeof(TCHAR) - 1;
		// ȥ��β�ո�
		while (right >= 0 && posts[iPosts].content[right] == _T(' '))
			right--;
		posts[iPosts].content = posts[iPosts].content.Left(right + 1);

		//OutputDebugString(_T("\n"));
		//OutputDebugString(rawPosts[iRawThreads]);
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
	PostInfo lzl;
	// ����¥��
	for (std::regex_iterator<LPCTSTR> it((LPCTSTR)src, (LPCTSTR)src + src.GetLength(), LZL_FLOOR_REG), end; it != end; it++)
	{
		// ���Ҹò�¥��
		CString pid = (*it)[1].str().c_str(); // �ò�PID
		for (PostInfo post : posts)
			if (post.pid == pid)
			{
				lzl.floor = post.floor;
				break;
			}
		
		// �����ò�¥��¥
		CString floorContent = (*it)[2].str().c_str(); // �ò�¥��¥����
		for (std::regex_iterator<LPCTSTR> it2((LPCTSTR)floorContent, (LPCTSTR)floorContent 
			+ floorContent.GetLength(), LZL_CONTENT_REG), end2; it2 != end2; it2++)
		{
			lzl.pid = (*it2)[1].str().c_str();
			lzl.author = JSUnescape((*it2)[2].str().c_str());
			CString content = (*it2)[3].str().c_str();
			if (content == _T("\",")) // ����Ϊ��ʱ�����ƥ�䵽���
				lzl.content = _T("");
			else
				lzl.content = HTMLUnescape(JSUnescape(content));
			lzls.push_back(lzl);
		}

		//OutputDebugString(_T("\n"));
		//OutputDebugString(floorContent);
		//OutputDebugString(_T("\n----------------------------------"));
	}
}


// ɨ�� /////////////////////////////////////////////////////////////////////////////////
// ���Υ��
BOOL CheckIllegal(LPCTSTR content, LPCTSTR author, CString& msg)
{
	g_optionsLock.Lock();

	// �����û�
	for (const CString& whiteList : g_whiteList)
		if (author == whiteList)
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
		if (StringIncludes(content, keyword))
		{
			msg = _T("<font color=red> ����Υ���� </font>") + HTMLEscape(keyword.text);
			g_optionsLock.Unlock();
			return TRUE;
		}

	// �����û�
	for (const RegexText& blackList : g_blackList)
		if (StringMatchs(author, blackList))
		{
			msg = _T("<font color=red> ���������û� </font>") + HTMLEscape(blackList.text);
			g_optionsLock.Unlock();
			return TRUE;
		}

	g_optionsLock.Unlock();
	return FALSE;
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
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	CComPtr<IHTMLDocument2> document;
	dlg->GetLogDocument(document);
	CComPtr<IHTMLDocument2>* pDocument = (CComPtr<IHTMLDocument2>*)&(int&)document;

	// ��ʼ��ҳ��
	CString sPage;
	dlg->m_pageEdit.GetWindowText(sPage);
	int iPage = _ttoi(sPage);
	CString ignoreThread; // ����ǰ��������
	ignoreThread.Format(_T("%d"), (iPage - 1) * 50);

	vector<ThreadInfo> threads;
	CString msg;
	while (!g_stopScanFlag)
	{
		DWORD startTime = GetTickCount();
		dlg->m_stateStatic.SetWindowText(_T("ɨ��������"));
		if (!g_briefLog)
			dlg->Log(_T("<font color=green>����ɨ�迪ʼ��ʹ�÷�����</font>") + g_currentOption, pDocument);
		
		if (!GetThreads(g_forumName, ignoreThread, threads))
		{
			if (g_stopScanFlag)
				break;
			if (!g_briefLog)
				dlg->Log(_T("<font color=red>��ȡ�����б�ʧ�ܣ����¿�ʼ����</font>"), pDocument);
			continue;
		}

		// ɨ������
		for (const ThreadInfo& thread : threads)
		{
			if (g_stopScanFlag)
				break;
			if (CheckIllegal(thread.title + _T("\r\n") + thread.preview, thread.author, msg))
			{
				__int64 tid = _ttoi64(thread.tid);
				if (g_ignoredTID.find(tid) == g_ignoredTID.end())
				{
					AddOperation(thread.title + _T("\r\n") + thread.preview, TBOBJ_THREAD, thread.tid, 
						thread.title, _T("1"), _T(""), thread.author);
					dlg->Log(_T("<a href=\"http://tieba.baidu.com/p/") + thread.tid + _T("\">")
						+ HTMLEscape(thread.title) + _T("</a>") + msg, pDocument);
					g_ignoredTID.insert(tid);
				}
			}
		}

		// ɨ������
		if (!g_onlyScanTitle)
			for (const ThreadInfo& thread : threads)
			{
				if (g_stopScanFlag)
					break;
				if (g_deletedTID.find(_ttoi64(thread.tid)) == g_deletedTID.end())
					ScanPost(thread, dlg, pDocument);
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

// ɨ������
void ScanPost(const ThreadInfo& thread, CTiebaManagerDlg* dlg, CComPtr<IHTMLDocument2>* pDocument)
{
	__int64 tid = _ttoi64(thread.tid);
	int reply = _ttoi(thread.reply);
	auto historyReplyIt = g_reply.find(tid);
	BOOL hasHistoryReply = historyReplyIt != g_reply.end();
	if (hasHistoryReply && reply <= historyReplyIt->second) // ���»ظ�
		return;

	// ��һҳ
	CString src = HTTPGet(_T("http://tieba.baidu.com/p/" + thread.tid), FALSE, &g_stopScanFlag);
	if (src == NET_STOP_TEXT)
		return;

	// ����ҳ��
	CString pageCount = GetStringBetween(src, PAGE_COUNT_LEFT, PAGE_COUNT_RIGHT);
	if (pageCount == _T(""))
	{
		WriteString(src, _T("page1.txt"));
		if (!g_briefLog)
			dlg->Log(_T("<a href=\"http://tieba.baidu.com/p/") + thread.tid + _T("\">") + thread.title 
				+ _T("</a> <font color=red>��ȡ�����б�ʧ�ܣ���ʱ����</font>"), pDocument);
		return;
	}

	// ɨ������ҳ
	int iPageCount = _ttoi(pageCount);
	BOOL res = ScanPostPage(thread.tid, 1, thread.title, hasHistoryReply, 0, src, dlg, pDocument);
	if (iPageCount > 1 && !g_stopScanFlag)
		res = ScanPostPage(thread.tid, iPageCount, thread.title, hasHistoryReply, 0, _T(""), dlg, pDocument);

	// ��¼��ʷ�ظ�
	if (res)
	{
		if (hasHistoryReply)
			historyReplyIt->second = reply;
		else
			g_reply[tid] = reply;
	}
}

// ɨ������ҳ
BOOL ScanPostPage(const CString& tid, int page, const CString& title, BOOL hasHistoryReply, 
	int ScanedCount, const CString& src, CTiebaManagerDlg* dlg, CComPtr<IHTMLDocument2>* pDocument)
{
	dlg->m_stateStatic.SetWindowText(_T("ɨ�����ӣ�") + tid + _T(":1 ") + title);

	CString sPage;
	sPage.Format(_T("%d"), page);
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
	// ɨ������
	for (const PostInfo& post : posts)
	{
		if (g_stopScanFlag)
			return FALSE;
		if (CheckIllegal(post.content, post.author, msg))
		{
			__int64 pid = _ttoi64(post.pid);
			if (g_ignoredPID.find(pid) == g_ignoredPID.end())
			{
				AddOperation(post.content.GetLength() > 700 ? post.content.Left(700) + _T("��������") : post.content,
					post.floor == _T("1") ? TBOBJ_THREAD : TBOBJ_POST, tid, title, post.floor, post.pid, post.author);
				dlg->Log(_T("<a href=\"http://tieba.baidu.com/p/") + tid + _T("\">") + HTMLEscape(title) + 
					_T("</a> ") + post.floor + _T("¥") + msg, pDocument);
				g_ignoredPID.insert(pid);
			}
		}
	}

	// ɨ��¥��¥
	for (const PostInfo& lzl : lzls)
	{
		if (g_stopScanFlag)
			return FALSE;
		if (CheckIllegal(lzl.content, lzl.author, msg))
		{
			__int64 lzlid = _ttoi64(lzl.pid);
			if (g_ignoredLZLID.find(lzlid) == g_ignoredLZLID.end())
			{
				AddOperation(lzl.content, TBOBJ_LZL, tid, title, lzl.floor, lzl.pid, lzl.author);
				dlg->Log(_T("<a href=\"http://tieba.baidu.com/p/") + tid + _T("\">") + HTMLEscape(title) +
					_T("</a> ") + lzl.floor + _T("¥�ظ�") + msg, pDocument);
				g_ignoredLZLID.insert(lzlid);
			}
		}
	}

	// �ݹ�ɨ����һҳ
	if (!hasHistoryReply) // �������ʷ�ظ�ǰ�漸ҳ�ܿ��ܱ�ɨ����ˣ����ݹ�
	{
		if (++ScanedCount < g_scanPageCount) // û�ﵽ���ɨ��ҳ��
		{
			if (--page < 2) // ɨ����
				return TRUE;
			return ScanPostPage(tid, page, title, FALSE, ScanedCount, _T(""), dlg, pDocument);
		}
	}
	return TRUE;
}


// ���� /////////////////////////////////////////////////////////////////////////////////
// ��Ӳ���
void AddOperation(const CString& msg, TBObject object, const CString& tid, const CString& title,
	const CString& floor, const CString& pid, const CString& author)
{
	Operation tmp;
	tmp.msg = msg;
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

		// �����ѱ�ɾ
		__int64 tid = _ttoi64(op.tid);
		if (g_deletedTID.find(tid) != g_deletedTID.end())
			continue;
		
		// ȷ���Ƿ����
		if (g_confirm)
		{
			if (MessageBox(NULL, op.msg + _T("\r\n���ߣ�") + op.author + _T("\r\n�Ƿ���"),
				op.title, MB_ICONQUESTION | MB_YESNO) == IDNO)
			{
				if (op.object == TBOBJ_THREAD)
					dlg->Log(_T("<font color=green>���� </font><a href=\"http://tieba.baidu.com/p/") + op.tid
						+ _T("\">") + HTMLEscape(op.title) + _T("</a>"), pDocument);
				else
					dlg->Log(_T("<font color=green>���� </font><a href=\"http://tieba.baidu.com/p/") + op.tid
						+ _T("\">") + HTMLEscape(op.title) + _T("</a> ") + op.floor 
						+ (op.object == TBOBJ_POST ? _T("¥") : _T("¥�ظ�")), pDocument);
				continue;
			}
		}

		// ���
		if (g_banID /*|| g_banIP*/)
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
				get_ip_tbs banTBS;
				if (!GetBanTBS(op.tid, op.author, banTBS))
				{
					dlg->Log(_T("<font color=red>��� </font>") + op.author + _T("<font color=red> ʧ�ܣ�\
(��ȡ��������ʧ��)</font><a href=\"BD:") + op.tid + _T(",") + op.author + _T("\">����</a>"), pDocument);
				}
				else
				{
					BOOL success = TRUE;
					//if (g_banID) // ��ID
					{
						CString code = BanID(op.author, banTBS.tbs_ban_user);
						success = code == _T("0");
						if (!success)
						{
							CString content;
							content.Format(_T("<font color=red>��� </font>%s<font color=red> ʧ�ܣ�\
������룺%s(%s)</font><a href=\"BD:%s,%s\">����</a>"), op.author, code, GetTiebaErrorText(code), op.tid, op.author);
							dlg->Log(content, pDocument);
						}
					}
					/*if (g_banIP) // ��IP
					{
						CString code = BanIP(banTBS.ip_int, banTBS.tbs_ban_ip, banTBS.ip_secure_str);
						success = code == _T("0");
						if (!success)
						{
							CString content;
							content.Format(_T("<font color=red>��� </font>%s<font color=red>IP ʧ�ܣ�\
������룺%s(%s)</font><a href=\"BP:%s,%s\">����</a>"), op.author, code, GetTiebaErrorText(code), op.tid, op.author);
							dlg->Log(content, pDocument);
						}
					}*/
					if (success)
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

// ��ȡ�����tbs
BOOL GetBanTBS(LPCTSTR tid, LPCTSTR userName, get_ip_tbs& result)
{
	CString url;
	url.Format(_T("http://tieba.baidu.com/bawu/filter/get_ip_tbs?type=get_ip_tbs&tid=%s&pid=%s\
&user_name=%s&post_id=%s&word=%s&fid=%s&can_prison_ip=true&ie=utf-8"), tid, tid, EncodeURI(userName), 
		tid, g_encodedForumName, g_forumID);
	CString src = HTTPGet(url);
	CStringArray tmp;
	SplitString(tmp, src, _T(","));
	if (tmp.GetSize() < 6)
		return FALSE;
	result.ip_int = GetStringBetween(tmp[1], _T("\"ip_int\":\""), _T("\""));
	result.tbs_ban_user = GetStringBetween(tmp[2], _T("\"tbs_ban_user\":\""), _T("\""));
	result.tbs_ban_ip = GetStringBetween(tmp[3], _T("\"tbs_ban_ip\":\""), _T("\""));
	result.ip_secure_str = GetStringBetween(tmp[5], _T("\"ip_secure_str\":\""), _T("\""));
	return TRUE;
}

// ��ID�����ش������
CString BanID(LPCTSTR userName, LPCTSTR tbs_ban_user)
{
	CString data;
	data.Format(_T("cm=filter_forum_user&user_name=%s&ban_days=%d&word=%s&fid=%s&tbs=%s&ie=utf-8"), 
		EncodeURI(userName), g_banDuration, g_encodedForumName, g_forumID, tbs_ban_user);
	CString src = HTTPPost(_T("http://tieba.baidu.com/bawu/cm"), data);
	if (src == NET_TIMEOUT_TEXT /*|| src == NET_STOP_TEXT*/)
		return _T("-1");
	return GetStringBetween(src, _T("no\":"), _T(","));
}

// ��IP�����ش������ // �˹����Ѿ�����
/*CString BanIP(LPCTSTR ip_int, LPCTSTR tbs_ban_ip, LPCTSTR ip_secure_str)
{
	CString data;
	data.Format(_T("cm=filter_forum_ip&user_ip=%s&ip_secure_str=%s&ban_days=1&word=%s&fid=%s&tbs=%s&ie=utf-8"),
		ip_int, ip_secure_str, g_encodedForumName, g_forumID, tbs_ban_ip);
	CString src = HTTPPost(_T("http://tieba.baidu.com/bawu/cm"), data);
	if (src == NET_TIMEOUT_TEXT *//*|| src == NET_STOP_TEXT*//*)
		return _T("-1");
	return GetStringBetween(src, _T("no\":"), _T(","));
}*/

// ɾ���⣬���ش������
CString DeleteThread(const CString& tid)
{
	CString src = HTTPPost(_T("http://tieba.baidu.com/f/commit/thread/delete"), _T("kw=") + g_encodedForumName
		+ _T("&fid=") + g_forumID + _T("&tid=") + tid + _T("&ie=utf-8&tbs=") + g_tbs);
	if (src == NET_TIMEOUT_TEXT /*|| src == NET_STOP_TEXT*/)
		return _T("-1");
	return GetStringBetween(src, _T("no\":"), _T(","));
}

// ɾ���ӣ����ش������
CString DeletePost(LPCTSTR tid, LPCTSTR pid)
{
	CString data;
	data.Format(_T("commit_fr=pb&ie=utf-8&tbs=%s&kw=%s&fid=%s&tid=%s&is_vipdel=0&pid=%s&is_finf=false"), 
		g_tbs, g_encodedForumName, g_forumID, tid, pid);
	CString src = HTTPPost(_T("http://tieba.baidu.com/f/commit/post/delete"), data);
	if (src == NET_TIMEOUT_TEXT /*|| src == NET_STOP_TEXT*/)
		return _T("-1");
	return GetStringBetween(src, _T("no\":"), _T(","));
}

// ɾ¥��¥�����ش������
CString DeleteLZL(LPCTSTR tid, LPCTSTR lzlid)
{
	CString data;
	data.Format(_T("ie=utf-8&tbs=%s&kw=%s&fid=%s&tid=%s&pid=%s&is_finf=1"),
		g_tbs, g_encodedForumName, g_forumID, tid, lzlid);
	CString src = HTTPPost(_T("http://tieba.baidu.com/f/commit/post/delete"), data);
	if (src == NET_TIMEOUT_TEXT /*|| src == NET_STOP_TEXT*/)
		return _T("-1");
	return GetStringBetween(src, _T("no\":"), _T(","));
}

// ȡ�����ı�
CString GetTiebaErrorText(const CString& errorCode)
{
	if (errorCode == _T("890"))
		return _T("������ɾ");
	if (errorCode == _T("-1"))
		return _T("��ʱ");
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
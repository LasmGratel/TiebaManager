﻿#include "stdafx.h"
#include "TiebaScan.h"

#include "TiebaVariable.h"
#include <TiebaClawer.h>
#include "TBMOperate.h"

#include <StringHelper.h>
#include <NetworkHelper.h>
#include <MiscHelper.h>

#include "TiebaManagerDlg.h"

#include "ScanImage.h"


static const TCHAR PAGE_COUNT_LEFT[] = _T(",\"total_page\":");
static const TCHAR PAGE_COUNT_RIGHT[] = _T("}");


static vector<ThreadInfo> g_threads; // 当前扫描的主题列表
static int g_threadIndex; // 下个要扫描的主题索引
static CCriticalSection g_threadIndexLock;


extern queue<Operation> g_confirmQueue; // 确认队列
extern queue<Operation> g_operationQueue; // 操作队列


// 检查违规
BOOL CheckIllegal(LPCTSTR content, LPCTSTR author, const CString& authorLevel, CString& msg, BOOL& forceToConfirm, int& pos, int& length)
{
	forceToConfirm = FALSE;
	g_plan.m_optionsLock.Lock();

	// 信任用户
	if (g_plan.m_whiteList->find(author) != g_plan.m_whiteList->end())
	{
		g_plan.m_optionsLock.Unlock();
		return FALSE;
	}

	// 违规等级
	if (g_plan.m_illegalLevel > 0 && authorLevel != _T("") && _ttoi(authorLevel) <= g_plan.m_illegalLevel)
	{
		pos = 0;
		length = 0;
		msg.Format(_T("<font color=red> 触发等级小于或等于 </font>%d"), *g_plan.m_illegalLevel);
		g_plan.m_optionsLock.Unlock();
		return TRUE;
	}

	// 屏蔽用户
	for (const RegexText& blackList : *g_plan.m_blackList)
		if (StringMatchs(author, blackList))
		{
			pos = 0;
			length = 0;
			msg = _T("<font color=red> 触发屏蔽用户 </font>") + HTMLEscape(blackList.text);
			g_plan.m_optionsLock.Unlock();
			return TRUE;
		}


	// 信任内容
	for (const RegexText& whiteContent : *g_plan.m_whiteContent)
		if (StringIncludes(content, whiteContent))
		{
			g_plan.m_optionsLock.Unlock();
			return FALSE;
		}

	// 违规内容
	for (CPlan::Keyword& keyword : *g_plan.m_keywords)
		if (StringIncludes(content, keyword, &pos, &length))
		{
			keyword.trigCount++;
			forceToConfirm = keyword.forceToConfirm;
			msg = _T("<font color=red> 触发违禁词 </font>") + HTMLEscape(keyword.text);
			g_plan.m_optionsLock.Unlock();
			return TRUE;
		}

	g_plan.m_optionsLock.Unlock();
	return FALSE;
}

// 扫描主题图片
static inline void ScanThreadImage(CString& msg, CTiebaManagerDlg* dlg)
{
	for (const ThreadInfo& thread : g_threads)
	{
		if (g_stopScanFlag)
			break;
		__int64 tid = _ttoi64(thread.tid);
		if (g_userCache.m_ignoredTID.find(tid) == g_userCache.m_ignoredTID.end()
			&& CheckImageIllegal(thread.author, GetThreadImage(thread), msg))
		{
			AddConfirm(FALSE, thread.title + _T("\r\n") + thread.preview, Operation::TBOBJ_THREAD, thread.tid,
				thread.title, _T("1"), _T(""), thread.author, thread.authorID);
			dlg->m_log.Log(_T("<a href=\"http://tieba.baidu.com/p/") + thread.tid + _T("\">")
				+ HTMLEscape(thread.title) + _T("</a>") + msg);
			g_userCache.m_ignoredTID.insert(tid);
		}
	}
}

// 总扫描线程
UINT AFX_CDECL ScanThread(LPVOID mainDlg)
{
	CTiebaManagerDlg* dlg = (CTiebaManagerDlg*)mainDlg;
	g_stopScanFlag = FALSE;
	dlg->m_startButton.EnableWindow(FALSE);
	dlg->m_pageEdit.EnableWindow(FALSE);
	dlg->m_stopButton.EnableWindow(TRUE);

	// 开始操作、确认线程
	if (g_operateThread == NULL && !g_operationQueue.empty())
		g_operateThread = AfxBeginThread(OperateThread, mainDlg);
	if (g_confirmThread == NULL && !g_confirmQueue.empty())
		g_confirmThread = AfxBeginThread(ConfirmThread, mainDlg);

	// 初始化
	if (!CoInitializeHelper())
		return 0;

	// 初始化页数
	CString sPage;
	dlg->m_pageEdit.GetWindowText(sPage);
	int iPage = _ttoi(sPage);
	CString ignoreThread; // 忽略前几个主题
	ignoreThread.Format(_T("%d"), (iPage - 1) * 50);

	CString msg;
	BOOL forceToConfirm;
	int pos, length;
	while (!g_stopScanFlag)
	{
#pragma warning(suppress: 28159)
		DWORD startTime = GetTickCount();
		dlg->m_stateStatic.SetWindowText(_T("扫描主题中"));
		if (!g_plan.m_briefLog)
			dlg->m_log.Log(_T("<font color=green>本轮扫描开始，使用方案：</font>") + g_userConfig.m_plan);

		// 获取主题列表
		if (!GetThreads(g_tiebaOperate->GetForumName(), ignoreThread, g_threads))
		{
			if (g_stopScanFlag)
				break;
			if (!g_plan.m_briefLog)
				dlg->m_log.Log(_T("<font color=red>获取主题列表失败，重新开始本轮</font>"));
			continue;
		}

		// 扫描主题
		for (const ThreadInfo& thread : g_threads)
		{
			if (g_stopScanFlag)
				break;
			__int64 tid = _ttoi64(thread.tid);
			if (g_userCache.m_ignoredTID.find(tid) == g_userCache.m_ignoredTID.end()
				&& CheckIllegal(thread.title + _T("\r\n") + thread.preview, thread.author, _T(""), msg, forceToConfirm, pos, length))
			{
				AddConfirm(forceToConfirm, thread.title + _T("\r\n") + thread.preview, Operation::TBOBJ_THREAD, thread.tid,
					thread.title, _T("0"), _T(""), thread.author, thread.authorID, _T(""), pos, length);
				dlg->m_log.Log(_T("<a href=\"http://tieba.baidu.com/p/") + thread.tid + _T("\">")
					+ HTMLEscape(thread.title) + _T("</a>") + msg);
				g_userCache.m_ignoredTID.insert(tid);
			}
		}
		// 扫描主题图片
		BOOL scanImage = !g_plan.m_images.empty();
		if (g_plan.m_onlyScanTitle && scanImage)
		{
			scanImage = FALSE;
			ScanThreadImage(msg, dlg);
		}

		// 扫描帖子
		if (!g_plan.m_onlyScanTitle)
		{
			dlg->m_stateStatic.SetWindowText(_T("扫描帖子中"));
			g_threadIndex = 0;
			// 创建线程扫描帖子
			int threadCount = g_plan.m_threadCount; // g_threadCount会变
			unique_ptr<unique_ptr<CWinThread>[]> threadObjects(new unique_ptr<CWinThread>[threadCount]);
			HANDLE* threadHandles(new HANDLE[threadCount]);
			for (int i = 0; i < threadCount; i++)
			{
				dlg->m_stateList.AddString(_T("准备中"));
				threadObjects[i].reset(AfxBeginThread(ScanPostThread, (LPVOID)i, 0, 0, CREATE_SUSPENDED));
				threadObjects[i]->m_bAutoDelete = FALSE;
				threadHandles[i] = threadObjects[i]->m_hThread;
				threadObjects[i]->ResumeThread();
			}
			// 等待扫描帖子时扫描主题图片
			if (scanImage)
				ScanThreadImage(msg, dlg);
			WaitForMultipleObjects(threadCount, threadHandles, TRUE, INFINITE);

			// 释放
			delete[] threadHandles;
			dlg->m_stateList.ResetContent();
		}

		dlg->m_stateStatic.SetWindowText(_T("延时中"));
		if (!g_plan.m_briefLog)
		{
			CString content;
#pragma warning(suppress: 28159)
			content.Format(_T("<font color=green>本轮扫描结束，用时%.3f秒</font>"), (float)(GetTickCount() - startTime) / 1000.0f);
			dlg->m_log.Log(content);
		}

		// 延时
		int count = g_plan.m_scanInterval * 10;
		for (int i = 0; i < count; i++)
		{
			if (g_stopScanFlag)
				break;
			Sleep(100);
		}
	}
	g_stopScanFlag = FALSE;

	if (!g_plan.m_briefLog)
		dlg->m_log.Log(_T("<font color=green>扫描结束</font>"));
	CoUninitialize();
	dlg->m_stopButton.EnableWindow(FALSE);
	dlg->m_startButton.EnableWindow(TRUE);
	dlg->m_pageEdit.EnableWindow(TRUE);
	dlg->m_stateStatic.SetWindowText(_T("待机中"));

	g_scanThread = NULL;
	return 0;
}

// 扫描帖子线程
UINT AFX_CDECL ScanPostThread(LPVOID _threadID)
{
	int threadID = (int)_threadID;
	CTiebaManagerDlg* dlg = (CTiebaManagerDlg*)AfxGetApp()->m_pMainWnd;
	// 初始化
	if (!CoInitializeHelper())
		return 0;

	CString pageCount, src;
	map<__int64, int>::iterator historyReplyIt;
	g_threadIndexLock.Lock();
	while (!g_stopScanFlag && g_threadIndex < (int)g_threads.size())
	{
		ThreadInfo& thread = g_threads[g_threadIndex++];
		g_threadIndexLock.Unlock();
		if (g_userCache.m_deletedTID.find(_ttoi64(thread.tid)) != g_userCache.m_deletedTID.end()) // 已删
			goto Next;
		g_plan.m_optionsLock.Lock();
		if (g_plan.m_trustedThread->find(thread.tid) != g_plan.m_trustedThread->end()) // 信任
		{
			g_plan.m_optionsLock.Unlock();
			goto Next;
		}
		g_plan.m_optionsLock.Unlock();

		__int64 tid = _ttoi64(thread.tid);
		int reply = _ttoi(thread.reply);
		historyReplyIt = g_userCache.m_reply->find(tid);
		BOOL hasHistoryReply = historyReplyIt != g_userCache.m_reply->end();
		if (hasHistoryReply 
			&& reply == historyReplyIt->second // 回复数减少时也扫描，防止漏掉
			&& thread.lastAuthor == (*g_userCache.m_lastAuthor)[tid]) // 判断最后回复人，防止回复数-1然后有新回复+1
		{
			// 无新回复，跳过
			historyReplyIt->second = reply;
			goto Next;
		}

		// 第一页
		src = HTTPGet(_T("http://tieba.baidu.com/p/" + thread.tid));
		if (src == NET_TIMEOUT_TEXT)
		{
			if (!g_plan.m_briefLog)
				dlg->m_log.Log(_T("<a href=\"http://tieba.baidu.com/p/") + thread.tid + _T("\">") + thread.title
				+ _T("</a> <font color=red>获取贴子列表失败(超时)，暂时跳过</font>"));
			goto Next;
		}

		// 帖子页数
		pageCount = GetStringBetween(src, PAGE_COUNT_LEFT, PAGE_COUNT_RIGHT);
		if (pageCount == _T(""))
		{
			WriteString(src, _T("thread.txt"));
			if (!g_plan.m_briefLog)
				dlg->m_log.Log(_T("<a href=\"http://tieba.baidu.com/p/") + thread.tid + _T("\">") + thread.title
				+ _T("</a> <font color=red>获取贴子列表失败(可能已被删)，暂时跳过</font>"));
			goto Next;
		}

		// 扫描帖子页
		int iPageCount = _ttoi(pageCount);
		BOOL res = ScanPostPage(thread.tid, 1, thread.title, hasHistoryReply, 0, src, threadID, dlg);
		if (iPageCount > 1 && !g_stopScanFlag)
			res = ScanPostPage(thread.tid, iPageCount, thread.title, hasHistoryReply, 0, _T(""), threadID, dlg);

		// 记录历史回复
		if (res)
		{
			(*g_userCache.m_reply)[tid] = reply;
			(*g_userCache.m_lastAuthor)[tid] = thread.lastAuthor;
		}

	Next:
		g_threadIndexLock.Lock();
	}
	g_threadIndexLock.Unlock();

	CoUninitialize();
	dlg->m_stateList.DeleteString(threadID);
	dlg->m_stateList.InsertString(threadID, _T("线程结束"));
	return 0;
}

// 扫描帖子页
BOOL ScanPostPage(const CString& tid, int page, const CString& title, BOOL hasHistoryReply,
	int ScanedCount, const CString& src, int threadID, CTiebaManagerDlg* dlg)
{
	CString sPage;
	sPage.Format(_T("%d"), page);
	dlg->m_stateList.DeleteString(threadID);
	dlg->m_stateList.InsertString(threadID, tid + _T(":") + sPage + _T(" ") + title);

	// 获取帖子列表
	vector<PostInfo> posts, lzls;
	GetPostsResult res = GetPosts(tid, src, sPage, posts);
	switch (res)
	{
	case GET_POSTS_TIMEOUT:
	case GET_POSTS_DELETED:
		dlg->m_log.Log(_T("<a href=\"http://tieba.baidu.com/p/") + tid + _T("\">") + title
			+ _T("</a> <font color=red>获取贴子列表失败(") + (res == GET_POSTS_TIMEOUT ? _T("超时") :
			_T("可能已被删")) + _T(")，暂时跳过</font>"));
		return FALSE;
	}
	GetLzls(g_tiebaOperate->GetForumID(), tid, sPage, posts, lzls);

	CString msg;
	BOOL forceToConfirm;
	int pos, length;
	// 扫描帖子
	for (const PostInfo& post : posts)
	{
		if (g_stopScanFlag)
			return FALSE;
		__int64 pid = _ttoi64(post.pid);
		if (g_userCache.m_ignoredPID.find(pid) == g_userCache.m_ignoredPID.end()
			&& CheckIllegal(post.content, post.author, post.authorLevel, msg, forceToConfirm, pos, length))
		{
			AddConfirm(forceToConfirm, post.content, post.floor == _T("1") ? Operation::TBOBJ_THREAD : Operation::TBOBJ_POST,
				tid, title, post.floor, post.pid, post.author, post.authorID, post.authorPortrait, pos, length);
			dlg->m_log.Log(_T("<a href=\"http://tieba.baidu.com/p/") + tid + _T("\">") + HTMLEscape(title) +
				_T("</a> ") + post.floor + _T("楼") + msg);
			g_userCache.m_ignoredPID.insert(pid);
		}
	}

	// 扫描楼中楼
	for (const PostInfo& lzl : lzls)
	{
		if (g_stopScanFlag)
			return FALSE;
		if (CheckIllegal(lzl.content, lzl.author, _T(""), msg, forceToConfirm, pos, length))
		{
			__int64 lzlid = _ttoi64(lzl.pid);
			if (g_userCache.m_ignoredLZLID.find(lzlid) == g_userCache.m_ignoredLZLID.end())
			{
				AddConfirm(forceToConfirm, lzl.content, Operation::TBOBJ_LZL, tid, title, lzl.floor, lzl.pid, lzl.author, 
					lzl.authorID, lzl.authorPortrait, pos, length);
				dlg->m_log.Log(_T("<a href=\"http://tieba.baidu.com/p/") + tid + _T("\">") + HTMLEscape(title) +
					_T("</a> ") + lzl.floor + _T("楼回复") + msg);
				g_userCache.m_ignoredLZLID.insert(lzlid);
			}
		}
	}

	// 扫描帖子图片
	for (const PostInfo& post : posts)
	{
		if (g_stopScanFlag)
			return FALSE;
		__int64 pid = _ttoi64(post.pid);
		if (g_userCache.m_ignoredPID.find(pid) == g_userCache.m_ignoredPID.end()
			&& CheckImageIllegal(post.author, GetPostImage(post), msg))
		{
			AddConfirm(FALSE, post.content, post.floor == _T("1") ? Operation::TBOBJ_THREAD : Operation::TBOBJ_POST,
				tid, title, post.floor, post.pid, post.author, post.authorID, post.authorPortrait);
			dlg->m_log.Log(_T("<a href=\"http://tieba.baidu.com/p/") + tid + _T("\">") + HTMLEscape(title) +
				_T("</a> ") + post.floor + _T("楼") + msg);
			g_userCache.m_ignoredPID.insert(pid);
		}
	}

	// 扫描楼中楼图片
	for (const PostInfo& lzl : lzls)
	{
		if (g_stopScanFlag)
			return FALSE;
		__int64 pid = _ttoi64(lzl.pid);
		if (g_userCache.m_ignoredLZLID.find(pid) == g_userCache.m_ignoredLZLID.end()
			&& CheckImageIllegal(lzl.author, GetPostImage(lzl), msg))
		{
			AddConfirm(FALSE, lzl.content, Operation::TBOBJ_LZL, tid, title, lzl.floor, lzl.pid, lzl.author, lzl.authorID, lzl.authorPortrait);
			dlg->m_log.Log(_T("<a href=\"http://tieba.baidu.com/p/") + tid + _T("\">") + HTMLEscape(title) +
				_T("</a> ") + lzl.floor + _T("楼回复") + msg);
			g_userCache.m_ignoredLZLID.insert(pid);
		}
	}

	// 递归扫描上一页
	if (!hasHistoryReply) // 如果有历史回复前面几页很可能被扫描过了，不递归
	{
		if (++ScanedCount < g_plan.m_scanPageCount) // 没达到最大扫描页数
		{
			if (--page < 2) // 扫描完
				return TRUE;
			return ScanPostPage(tid, page, title, FALSE, ScanedCount, _T(""), threadID, dlg);
		}
	}
	return TRUE;
}

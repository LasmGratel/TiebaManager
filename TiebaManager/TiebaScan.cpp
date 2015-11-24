#include "stdafx.h"
#include "TiebaScan.h"

#include "TiebaVariable.h"
#include "TiebaCollect.h"
#include <queue>
using std::queue;
#include "TiebaOperate.h"
#include "Setting.h"

#include "StringHelper.h"
#include "NetworkHelper.h"

#include "TiebaManagerDlg.h"

#include "ScanImage.h"


static const TCHAR PAGE_COUNT_LEFT[] = _T(",\"total_page\":");
static const TCHAR PAGE_COUNT_RIGHT[] = _T("}");


static vector<ThreadInfo> g_threads; // ��ǰɨ��������б�
static int g_threadIndex; // �¸�Ҫɨ�����������
static CCriticalSection g_threadIndexLock;


extern queue<Operation> g_operationQueue; // ��������


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
			&& CheckImageIllegal(thread.author, GetThreadImage(thread.preview), msg))
		{
			AddOperation(thread.title + _T("\r\n") + thread.preview, TBOBJ_THREAD, thread.tid,
				thread.title, _T("1"), _T(""), thread.author, thread.authorID);
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
					thread.title, _T("0"), _T(""), thread.author, thread.authorID, _T(""), pos, length);
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
				tid, title, post.floor, post.pid, post.author, post.authorID, _T(""), pos, length);
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
				AddOperation(lzl.content, TBOBJ_LZL, tid, title, lzl.floor, lzl.pid, lzl.author, lzl.authorID, _T(""), pos, length);
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
			&& CheckImageIllegal(post.author, GetPostImage(post.content, post.authorPortrait), msg))
		{
			AddOperation(post.content, post.floor == _T("1") ? TBOBJ_THREAD : TBOBJ_POST,
				tid, title, post.floor, post.pid, post.author, post.authorID);
			dlg->Log(_T("<a href=\"http://tieba.baidu.com/p/") + tid + _T("\">") + HTMLEscape(title) +
				_T("</a> ") + post.floor + _T("¥") + msg, pDocument);
			g_ignoredPID.insert(pid);
		}
	}

	// ɨ��¥��¥ͼƬ
	for (const PostInfo& lzl : lzls)
	{
		if (g_stopScanFlag)
			return FALSE;
		__int64 pid = _ttoi64(lzl.pid);
		if (g_ignoredLZLID.find(pid) == g_ignoredLZLID.end()
			&& CheckImageIllegal(lzl.author, GetPostImage(lzl.content, lzl.authorPortrait), msg))
		{
			AddOperation(lzl.content, TBOBJ_LZL, tid, title, lzl.floor, lzl.pid, lzl.author, lzl.authorID);
			dlg->Log(_T("<a href=\"http://tieba.baidu.com/p/") + tid + _T("\">") + HTMLEscape(title) +
				_T("</a> ") + lzl.floor + _T("¥�ظ�") + msg, pDocument);
			g_ignoredLZLID.insert(pid);
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

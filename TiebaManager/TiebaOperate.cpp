#include "stdafx.h"
#include "TiebaOperate.h"
#include <queue>
using std::queue;
#include "TiebaCollect.h"
#include "TiebaScan.h"
#include "StringHelper.h"
#include "NetworkHelper.h"
#include "Setting.h"
#include "TiebaManagerDlg.h"
#include "ConfirmDlg.h"
#include <Mmsystem.h>


extern volatile BOOL g_stopScanFlag;


queue<Operation> g_operationQueue; // ��������
static CCriticalSection g_operationQueueLock;
CWinThread* g_operateThread = NULL;


// ��Ӳ���
void AddOperation(const CString& msg, TBObject object, const CString& tid, const CString& title,
	const CString& floor, const CString& pid, const CString& author, const CString& authorID,
	const CString& authorPortrait, int pos, int length)
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
	tmp.authorID = authorID;
	tmp.authorPortrait = authorPortrait;
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
		if (!g_delete && !g_banID && !g_defriend)
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
				casePost : g_initIgnoredPID.insert(_ttoi64(op.pid));
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

		// ����Υ�����
		auto countIt = g_userTrigCount.find(op.author);
		BOOL hasHistory = countIt != g_userTrigCount.end();
		int count = hasHistory ? (countIt->second + 1) : 1;
		if (hasHistory)
			countIt->second = count;
		else
			g_userTrigCount[op.author] = 1;

		// ���
		if (g_banID && count >= g_banTrigCount && g_bannedUser.find(op.author) == g_bannedUser.end()) // �ﵽ���Υ�������δ��
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
				dlg->Log(_T("<font color=red>��� </font>") + op.author + _T("<font color=red> ʧ�ܣ�(��ȡ����IDʧ��)</font>")
						 , pDocument);
			}
			else
			{
				CString code = BanID(op.author, op.pid);
				if (code != _T("0"))
				{
					CString content;
					content.Format(_T("<font color=red>��� </font>%s<font color=red> ʧ�ܣ�������룺%s(%s)</font><a href=")
								   _T("\"bd:%s,%s\">����</a>"), op.author, code, GetTiebaErrorText(code), op.pid, op.author);
					dlg->Log(content, pDocument);
				}
				else
				{
					sndPlaySound(_T("���.wav"), SND_ASYNC | SND_NODEFAULT);
					g_bannedUser.insert(op.author);
					dlg->Log(_T("<font color=red>��� </font>") + op.author, pDocument);
				}
			}
		}

		// ����
		if (g_defriend && count >= g_defriendTrigCount && g_defriendedUser.find(op.author) == g_defriendedUser.end()) // �ﵽ����Υ�������δ����
		{
			CString code = Defriend(op.authorID);
			if (code != _T("0"))
			{
				CString content;
				content.Format(_T("<font color=red>���� </font>%s<font color=red> ʧ�ܣ�������룺%s(%s)</font><a href=")
							   _T("\"df:%s\">����</a>"), op.author, code, GetTiebaErrorText(code), op.authorID);
				dlg->Log(content, pDocument);
			}
			else
			{
				sndPlaySound(_T("���.wav"), SND_ASYNC | SND_NODEFAULT);
				g_defriendedUser.insert(op.author);
				dlg->Log(_T("<font color=red>���� </font>") + op.author, pDocument);
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
				content.Format(_T("<a href=\"http://tieba.baidu.com/p/%s\">%s</a><font color=red> ɾ��ʧ�ܣ�������룺%s(%s)</font><a href=")
							   _T("\"dt:%s\">����</a>"), op.tid, HTMLEscape(op.title), code, GetTiebaErrorText(code), op.tid);
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
				content.Format(_T("<a href=\"http://tieba.baidu.com/p/%s\">%s</a> %s¥<font color=red> ɾ��ʧ�ܣ�������룺%s(%s)</font>")
							   _T("<a href=\"dp:%s,%s\">����</a>"), op.tid, HTMLEscape(op.title), op.floor, code, GetTiebaErrorText(code), 
							   op.tid, op.pid);
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
				content.Format(_T("<a href=\"http://tieba.baidu.com/p/%s\">%s</a> %s¥�ظ�<font color=red> ɾ��ʧ�ܣ�������룺")
							   _T("%s(%s)</font><a href=\"dl:%s,%s\">����</a>"), op.tid, HTMLEscape(op.title), op.floor, code,
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
		return _T("-65536");
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

// ���ڣ����ش������
CString Defriend(LPCTSTR userID)
{
	CString src = HTTPPost(_T("http://tieba.baidu.com/bawu2/platform/addBlack"), _T("ie=utf-8&tbs=") + g_tbs
		+ _T("&user_id=") + userID + _T("&word=") + g_encodedForumName);
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
	if (errorCode == _T("-65536"))
		return _T("��ʱ");
	if (errorCode == _T("-1"))
		return _T("Ȩ�޲���");
	if (errorCode == _T("4"))
		return _T("����У��ʧ��");
	if (errorCode == _T("11"))
		return _T("�������");
	if (errorCode == _T("14") || errorCode == _T("12"))
		return _T("�ѱ�ϵͳ���");
	if (errorCode == _T("74"))
		return _T("�û�������(����ѭ�����õ����ӱ�ɾ����Ҫ�������)");
	if (errorCode == _T("78"))
		return _T("��������");
	if (errorCode == _T("308"))
		return _T("�㱻�����ʧȥȨ��");
	if (errorCode == _T("871"))
		return _T("��¥����ɾ");
	if (errorCode == _T("872"))
		return _T("��Ʒ������ɾ");
	if (errorCode == _T("890"))
		return _T("������ɾ");
	if (errorCode == _T("4011"))
		return _T("��Ҫ��֤��(����̫�죿)");
	return _T("δ֪����");
}

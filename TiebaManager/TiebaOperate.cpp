#include "stdafx.h"
#include "TiebaOperate.h"

#include "TiebaVariable.h"
#include "TiebaCollect.h"
#include "Setting.h"

#include "StringHelper.h"
#include "NetworkHelper.h"

#include "TiebaManagerDlg.h"
#include "ConfirmDlg.h"
#include "SuperFunctionDlg.h"

#include <Mmsystem.h>


queue<Operation> g_confirmQueue; // ȷ�϶���
CCriticalSection g_confirmQueueLock;
queue<Operation> g_operationQueue; // ��������
CCriticalSection g_operationQueueLock;

CString g_randomTid; // ĳ��tid��ȷ������ʱ��ʼ����WAP�ӿ���


// ���ȷ��
void AddConfirm(BOOL forceToConfirm, const CString& msg, TBObject object, const CString& tid, const CString& title,
	const CString& floor, const CString& pid, const CString& author, const CString& authorID,
	const CString& authorPortrait, int pos, int length)
{
	Operation tmp;
	tmp.forceToConfirm = forceToConfirm;
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
	g_confirmQueueLock.Lock();
	g_confirmQueue.push(tmp);
	if (g_confirmThread == NULL)
		g_confirmThread = AfxBeginThread(ConfirmThread, AfxGetApp()->m_pMainWnd);
	g_confirmQueueLock.Unlock();
}

// ȷ���߳�
UINT AFX_CDECL ConfirmThread(LPVOID mainDlg)
{
	CTiebaManagerDlg* dlg = (CTiebaManagerDlg*)mainDlg;

	// ��ʼ��
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	while (!g_confirmQueue.empty() && !g_stopScanFlag)
	{
		g_confirmQueueLock.Lock();
		Operation op = g_confirmQueue.front();
		g_confirmQueue.pop();
		g_confirmQueueLock.Unlock();

		// û�в���
		if (!g_plan.m_delete && !g_plan.m_banID && !g_plan.m_defriend)
			continue;

		// ȷ���Ƿ����
		if (g_plan.m_confirm || op.forceToConfirm)
		{
			if (CConfirmDlg(&op).DoModal() == IDCANCEL)
			{
				switch (op.object)
				{
				case TBOBJ_THREAD:
					if (op.floor == _T("1"))
						goto casePost;
					g_userCache.m_initIgnoredTID->insert(_ttoi64(op.tid));
					dlg->m_log.Log(_T("<font color=green>���� </font><a href=\"http://tieba.baidu.com/p/") + op.tid
						+ _T("\">") + HTMLEscape(op.title) + _T("</a>"));
					break;
				case TBOBJ_POST:
				casePost : g_userCache.m_initIgnoredPID->insert(_ttoi64(op.pid));
					dlg->m_log.Log(_T("<font color=green>���� </font><a href=\"http://tieba.baidu.com/p/") + op.tid
						+ _T("\">") + HTMLEscape(op.title) + _T("</a> ") + op.floor + _T("¥"));
					break;
				case TBOBJ_LZL:
					g_userCache.m_initIgnoredLZLID->insert(_ttoi64(op.pid));
					dlg->m_log.Log(_T("<font color=green>���� </font><a href=\"http://tieba.baidu.com/p/") + op.tid
						+ _T("\">") + HTMLEscape(op.title) + _T("</a> ") + op.floor + _T("¥�ظ�"));
					break;
				}
				continue;
			}
		}

		// �������
		AddOperation(op);
	}

	g_confirmThread = NULL;
	CoUninitialize();
	return 0;
}

// ��Ӳ���
void AddOperation(const Operation& operation)
{
	g_operationQueueLock.Lock();
	g_operationQueue.push(operation);
	if (g_operateThread == NULL)
		g_operateThread = AfxBeginThread(OperateThread, AfxGetApp()->m_pMainWnd);
	g_operationQueueLock.Unlock();
}

// �����߳�
UINT AFX_CDECL OperateThread(LPVOID mainDlg)
{
	CTiebaManagerDlg* dlg = (CTiebaManagerDlg*)mainDlg;

	// ��ʼ��
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	while (!g_operationQueue.empty() && !g_stopScanFlag)
	{
		g_operationQueueLock.Lock();
		Operation op = g_operationQueue.front();
		g_operationQueue.pop();
		g_operationQueueLock.Unlock();

		// û�в���
		if (!g_plan.m_delete && !g_plan.m_banID && !g_plan.m_defriend)
			continue;

		// ����Υ�����
		auto countIt = g_userCache.m_userTrigCount->find(op.author);
		BOOL hasHistory = countIt != g_userCache.m_userTrigCount->end();
		int count = hasHistory ? (countIt->second + 1) : 1;
		if (hasHistory)
			countIt->second = count;
		else
			(*g_userCache.m_userTrigCount)[op.author] = 1;

		// ���
		if (g_plan.m_banID && count >= g_plan.m_banTrigCount
			&& g_userCache.m_bannedUser->find(op.author) == g_userCache.m_bannedUser->end()) // �ﵽ���Υ�������δ��
		{
			// ��ʹ��WAP�ӿڻ���ʱ����Ϊ1�����ȡPID
			if ((!g_plan.m_wapBanInterface || g_plan.m_banDuration != 1) && op.pid == _T(""))
			{
				vector<PostInfo> posts, lzls;
				GetPosts(op.tid, _T(""), _T("1"), posts, lzls);
				if (posts.size() > 0)
					op.pid = posts[0].pid;
			}
			if ((!g_plan.m_wapBanInterface || g_plan.m_banDuration != 1) && op.pid == _T(""))
			{
				dlg->m_log.Log(_T("<font color=red>��� </font>") + op.author + _T("<font color=red> ʧ�ܣ�(��ȡ����IDʧ��)</font>"));
			}
			else
			{
				CString code = op.pid == _T("") ? BanIDWap(op.author) : BanID(op.author, op.pid);
				if (code != _T("0"))
				{
					CString content;
					content.Format(_T("<font color=red>��� </font>%s<font color=red> ʧ�ܣ�������룺%s(%s)</font><a href=")
								   _T("\"bd:%s,%s\">����</a>"), op.author, code, GetTiebaErrorText(code), op.author, op.pid);
					dlg->m_log.Log(content);
				}
				else
				{
					sndPlaySound(_T("���.wav"), SND_ASYNC | SND_NODEFAULT);
					g_userCache.m_bannedUser->insert(op.author);
					dlg->m_log.Log(_T("<font color=red>��� </font>") + op.author);
				}
			}

			// �Զ�ѭ����
			if (g_plan.m_autoLoopBan)
			{
				CLoopBanConfig config;
				config.Load(CURRENT_USER_PATH + _T("\\options2.xml"));
				auto it = std::find(config.m_userList->cbegin(), config.m_userList->cend(), op.author);
				if (it == config.m_userList->cend())
				{
					config.m_userList->push_back(op.author);
					config.m_pidList->push_back(_T(""));
					config.Save(CURRENT_USER_PATH + _T("\\options2.xml"));
					DeleteFile(CURRENT_USER_PATH + _T("\\LoopBanDate.xml"));
				}
			}
		}

		// ����
		if (g_plan.m_defriend && count >= g_plan.m_defriendTrigCount
			&& g_userCache.m_defriendedUser->find(op.author) == g_userCache.m_defriendedUser->end()) // �ﵽ����Υ�������δ����
		{
			CString code = Defriend(op.authorID);
			if (code != _T("0"))
			{
				CString content;
				content.Format(_T("<font color=red>���� </font>%s<font color=red> ʧ�ܣ�������룺%s(%s)</font><a href=")
							   _T("\"df:%s\">����</a>"), op.author, code, GetTiebaErrorText(code), op.authorID);
				dlg->m_log.Log(content);
			}
			else
			{
				sndPlaySound(_T("���.wav"), SND_ASYNC | SND_NODEFAULT);
				g_userCache.m_defriendedUser->insert(op.author);
				dlg->m_log.Log(_T("<font color=red>���� </font>") + op.author);
			}
		}

		// �����ѱ�ɾ����ɾ��
		__int64 tid = _ttoi64(op.tid);
		if (g_userCache.m_deletedTID.find(tid) != g_userCache.m_deletedTID.end())
			continue;

		// ɾ��
		if (!g_plan.m_delete)
			continue;
		if (op.object == TBOBJ_THREAD) // ����
		{
			CString code = DeleteThread(op.tid);
			if (code != _T("0"))
			{
				CString content;
				content.Format(_T("<a href=\"http://tieba.baidu.com/p/%s\">%s</a><font color=red> ɾ��ʧ�ܣ�������룺%s(%s)</font><a href=")
							   _T("\"dt:%s\">����</a>"), op.tid, HTMLEscape(op.title), code, GetTiebaErrorText(code), op.tid);
				dlg->m_log.Log(content);
			}
			else
			{
				sndPlaySound(_T("ɾ��.wav"), SND_ASYNC | SND_NODEFAULT);
				g_userCache.m_deletedTID.insert(_ttoi64(op.tid));
				dlg->m_log.Log(_T("<font color=red>ɾ�� </font><a href=\"http://tieba.baidu.com/p/") + op.tid
					+ _T("\">") + HTMLEscape(op.title) + _T("</a>"));
				Sleep((DWORD)(g_plan.m_deleteInterval * 1000));
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
				dlg->m_log.Log(content);
			}
			else
			{
				sndPlaySound(_T("ɾ��.wav"), SND_ASYNC | SND_NODEFAULT);
				dlg->m_log.Log(_T("<font color=red>ɾ�� </font><a href=\"http://tieba.baidu.com/p/") + op.tid
					+ _T("\">") + HTMLEscape(op.title) + _T("</a> ") + op.floor + _T("¥"));
				Sleep((DWORD)(g_plan.m_deleteInterval * 1000));
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
				dlg->m_log.Log(content);
			}
			else
			{
				sndPlaySound(_T("ɾ��.wav"), SND_ASYNC | SND_NODEFAULT);
				dlg->m_log.Log(_T("<font color=red>ɾ�� </font><a href=\"http://tieba.baidu.com/p/") + op.tid
					+ _T("\">") + HTMLEscape(op.title) + _T("</a> ") + op.floor + _T("¥�ظ�"));
				Sleep((DWORD)(g_plan.m_deleteInterval * 1000));
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
		*g_plan.m_banDuration, g_userTiebaInfo.m_forumID, g_userTiebaInfo.m_tbs, EncodeURI(userName), pid,
		*g_plan.m_banReason != _T("") ? *g_plan.m_banReason : _T("%20"));
	CString src = HTTPPost(_T("http://tieba.baidu.com/pmc/blockid"), data);
	return GetOperationErrorCode(src);
}

// ��ID�����ش�����룬����PID���û�����Ϊ���ɻ�Ա��
CString BanID(LPCTSTR userName)
{
	CString data;
	data.Format(_T("day=%d&fid=%s&tbs=%s&ie=gbk&user_name%%5B%%5D=%s&reason=%s"),
		*g_plan.m_banDuration, g_userTiebaInfo.m_forumID, g_userTiebaInfo.m_tbs, EncodeURI(userName),
		*g_plan.m_banReason != _T("") ? *g_plan.m_banReason : _T("%20"));
	CString src = HTTPPost(_T("http://tieba.baidu.com/pmc/blockid"), data);
	return GetOperationErrorCode(src);
}

// ��ID�����ش�����룬WAP�ӿڣ�����PID��ֻ�ܷ�1�죨����Ҫ��TID��
CString BanIDWap(LPCTSTR userName)
{
	CString url;
	url.Format(_T("http://tieba.baidu.com/mo/q/m?tn=bdFIL&ntn=banid&day=1&un=%s&tbs=%s")
			   _T("&word=%s&fid=%s&z=%s&$el=%%5Bobject%%20Array%%5D&reason=%s"),
		EncodeURI(userName), g_userTiebaInfo.m_tbs, g_userTiebaInfo.m_encodedForumName, 
		g_userTiebaInfo.m_forumID, g_randomTid, *g_plan.m_banReason != _T("") ? *g_plan.m_banReason : _T("%20"));
	CString src = HTTPGet(url);
	return GetOperationErrorCode(src);
}

// ���ڣ����ش������
CString Defriend(LPCTSTR userID)
{
	CString src = HTTPPost(_T("http://tieba.baidu.com/bawu2/platform/addBlack"), _T("ie=utf-8&tbs=") + g_userTiebaInfo.m_tbs
		+ _T("&user_id=") + userID + _T("&word=") + g_userTiebaInfo.m_encodedForumName);
	return GetOperationErrorCode(src);
}

// ɾ���⣬���ش������
CString DeleteThread(const CString& tid)
{
	CString src = HTTPPost(_T("http://tieba.baidu.com/f/commit/thread/delete"), _T("kw=") + g_userTiebaInfo.m_encodedForumName
		+ _T("&fid=") + g_userTiebaInfo.m_forumID + _T("&tid=") + tid + _T("&ie=utf-8&tbs=") + g_userTiebaInfo.m_tbs);
	return GetOperationErrorCode(src);
}

// ɾ���ӣ����ش������
CString DeletePost(LPCTSTR tid, LPCTSTR pid)
{
	CString data;
	data.Format(_T("commit_fr=pb&ie=utf-8&tbs=%s&kw=%s&fid=%s&tid=%s&is_vipdel=0&pid=%s&is_finf=false"),
		g_userTiebaInfo.m_tbs, g_userTiebaInfo.m_encodedForumName, g_userTiebaInfo.m_forumID, tid, pid);
	CString src = HTTPPost(_T("http://tieba.baidu.com/f/commit/post/delete"), data);
	return GetOperationErrorCode(src);
}

// ɾ¥��¥�����ش������
CString DeleteLZL(LPCTSTR tid, LPCTSTR lzlid)
{
	CString data;
	data.Format(_T("ie=utf-8&tbs=%s&kw=%s&fid=%s&tid=%s&pid=%s&is_finf=1"),
		g_userTiebaInfo.m_tbs, g_userTiebaInfo.m_encodedForumName, g_userTiebaInfo.m_forumID, tid, lzlid);
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
		return _T("�û�������(���������ѱ�ɾ���û����˳����ɻ�Ա���û������ض�̬)");
	if (errorCode == _T("77"))
		return _T("����ʧ��");
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

#include "stdafx.h"
#include "TBMOperate.h"

#include "TiebaVariable.h"
#include <TiebaClawer.h>

#include <MiscHelper.h>

#include "TiebaManagerDlg.h"
#include "ConfirmDlg.h"
#include "SuperFunctionDlg.h"

#include <Mmsystem.h>


queue<Operation> g_confirmQueue; // ȷ�϶���
CCriticalSection g_confirmQueueLock;
queue<Operation> g_operationQueue; // ��������
CCriticalSection g_operationQueueLock;

unique_ptr<CTiebaOperate> g_tiebaOperate;


// ���ȷ��
void AddConfirm(BOOL forceToConfirm, const CString& msg, Operation::TBObject object, const CString& tid, const CString& title,
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
	if (!CoInitializeHelper())
		return 0;

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
				case Operation::TBOBJ_THREAD:
					if (op.floor == _T("1"))
						goto CasePost;
					g_userCache.m_initIgnoredTID->insert(_ttoi64(op.tid));
					dlg->m_log.Log(_T("<font color=green>���� </font><a href=\"http://tieba.baidu.com/p/") + op.tid
						+ _T("\">") + HTMLEscape(op.title) + _T("</a>"));
					break;
				case Operation::TBOBJ_POST:
				CasePost:
					g_userCache.m_initIgnoredPID->insert(_ttoi64(op.pid));
					dlg->m_log.Log(_T("<font color=green>���� </font><a href=\"http://tieba.baidu.com/p/") + op.tid
						+ _T("\">") + HTMLEscape(op.title) + _T("</a> ") + op.floor + _T("¥"));
					break;
				case Operation::TBOBJ_LZL:
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
	if (!CoInitializeHelper())
		return 0;

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
			// ��ʹ�ÿͻ��˽ӿڱ����ȡPID
			if ((!g_plan.m_wapBanInterface /*|| g_plan.m_banDuration != 1*/) && op.pid == _T(""))
			{
				vector<PostInfo> posts;
				GetPosts(op.tid, _T(""), _T("1"), posts);
				if (posts.size() > 0)
					op.pid = posts[0].pid;
			}
			if ((!g_plan.m_wapBanInterface /*|| g_plan.m_banDuration != 1*/) && op.pid == _T(""))
			{
				dlg->m_log.Log(_T("<font color=red>��� </font>") + op.author + _T("<font color=red> ʧ�ܣ�(��ȡ����IDʧ��)</font>"));
			}
			else
			{
				CString code = (g_plan.m_wapBanInterface || op.pid == _T("")) ? 
					g_tiebaOperate->BanIDClient(op.author) : g_tiebaOperate->BanID(op.author, op.pid);
				if (code != _T("0"))
				{
					CString content;
					content.Format(_T("<font color=red>��� </font>%s<font color=red> ʧ�ܣ�������룺%s(%s)</font><a href=")
								   _T("\"bd:%s,%s\">����</a>"), (LPCTSTR)op.author, (LPCTSTR)code, (LPCTSTR)GetTiebaErrorText(code), (LPCTSTR)op.author, (LPCTSTR)op.pid);
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
				config.Load(CURRENT_USER_DIR_PATH + _T("options2.xml"));
				auto it = std::find(config.m_userList->cbegin(), config.m_userList->cend(), op.author);
				if (it == config.m_userList->cend())
				{
					config.m_userList->push_back(op.author);
					config.m_pidList->push_back(_T(""));
					config.Save(CURRENT_USER_DIR_PATH + _T("options2.xml"));
					DeleteFile(CURRENT_USER_DIR_PATH + _T("LoopBanDate.xml"));
				}
			}
		}

		// ����
		if (g_plan.m_defriend && count >= g_plan.m_defriendTrigCount
			&& g_userCache.m_defriendedUser->find(op.author) == g_userCache.m_defriendedUser->end()) // �ﵽ����Υ�������δ����
		{
			CString code = g_tiebaOperate->Defriend(op.authorID);
			if (code != _T("0"))
			{
				CString content;
				content.Format(_T("<font color=red>���� </font>%s<font color=red> ʧ�ܣ�������룺%s(%s)</font><a href=")
							   _T("\"df:%s\">����</a>"), (LPCTSTR)op.author, (LPCTSTR)code, (LPCTSTR)GetTiebaErrorText(code), (LPCTSTR)op.authorID);
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
		if (op.object == Operation::TBOBJ_THREAD) // ����
		{
			CString code = g_tiebaOperate->DeleteThread(op.tid);
			if (code != _T("0"))
			{
				CString content;
				content.Format(_T("<a href=\"http://tieba.baidu.com/p/%s\">%s</a><font color=red> ɾ��ʧ�ܣ�������룺%s(%s)</font><a href=")
							   _T("\"dt:%s\">����</a>"), (LPCTSTR)op.tid, (LPCTSTR)HTMLEscape(op.title), (LPCTSTR)code, (LPCTSTR)GetTiebaErrorText(code), (LPCTSTR)op.tid);
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
		else if (op.object == Operation::TBOBJ_POST) // ����
		{
			CString code = g_tiebaOperate->DeletePost(op.tid, op.pid);
			if (code != _T("0"))
			{
				CString content;
				content.Format(_T("<a href=\"http://tieba.baidu.com/p/%s\">%s</a> %s¥<font color=red> ɾ��ʧ�ܣ�������룺%s(%s)</font>")
							   _T("<a href=\"dp:%s,%s\">����</a>"), (LPCTSTR)op.tid, (LPCTSTR)HTMLEscape(op.title), (LPCTSTR)op.floor, (LPCTSTR)code, 
							   (LPCTSTR)GetTiebaErrorText(code), (LPCTSTR)op.tid, (LPCTSTR)op.pid);
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
		else /*if (op.object == Operation::TBOBJ_POST)*/ // ¥��¥
		{
			CString code = g_tiebaOperate->DeleteLZL(op.tid, op.pid);
			if (code != _T("0"))
			{
				CString content;
				content.Format(_T("<a href=\"http://tieba.baidu.com/p/%s\">%s</a> %s¥�ظ�<font color=red> ɾ��ʧ�ܣ�������룺")
							   _T("%s(%s)</font><a href=\"dl:%s,%s\">����</a>"), (LPCTSTR)op.tid, (LPCTSTR)HTMLEscape(op.title), 
							   (LPCTSTR)op.floor, (LPCTSTR)code, (LPCTSTR)GetTiebaErrorText(code), (LPCTSTR)op.tid, (LPCTSTR)op.pid);
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

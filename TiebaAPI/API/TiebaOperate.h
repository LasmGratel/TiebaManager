#pragma once
#include "TiebaAPICommon.h"


class TIEBA_API_API CTiebaOperate
{
protected:
	CString m_forumID;
	CString m_forumName;
	CString m_encodedForumName;
	CString& m_cookie;
	const CString& m_bduss;
	const CString& m_tbs;
	const int& m_banDuration;
	const CString& m_banReason;

	CString m_randomTid; // ĳ��tid��ȷ������ʱ��ʼ����WAP�ӿ���

public:
	CTiebaOperate(const CString& forumID, const CString& forumName, CString& cookie, const CString& bduss, 
		const CString& tbs, const int& banDuration, const CString& banReason, const CString& randomTid);

	// ��ID�����ش������
	CString BanID(const CString& userName, const CString& pid);
	// ��ID�����ش�����룬����PID���û�����Ϊ���ɻ�Ա��
	CString BanID(const CString& userName);
	// ��ID�����ش�����룬WAP�ӿڣ�����PID��ֻ�ܷ�1�죨����Ҫ��TID��
	CString BanIDWap(const CString& userName);
	// ��ID�����ش�����룬�ͻ��˽ӿڣ�����PID��С�ɿɷ�10��
	CString BanIDClient(const CString& userName);
	// ���ڣ����ش������
	CString Defriend(const CString& userID);
	// ɾ���⣬���ش������
	CString DeleteThread(const CString& tid);
	// ɾ���ӣ����ش������
	CString DeletePost(const CString& tid, const CString& pid);
	// ɾ¥��¥�����ش������
	CString DeleteLZL(const CString& tid, const CString& lzlid);
};

// ȡ�����ı�
TIEBA_API_API CString GetTiebaErrorText(const CString& errorCode);

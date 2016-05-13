#pragma once
#include "TiebaAPICommon.h"


class TIEBA_API_API CTiebaOperate
{
protected:
	CString m_forumID;			// ����ID��ȷ������ʱ��ʼ��
	CString m_forumName;		// ��������ȷ������ʱ��ʼ��
	CString m_encodedForumName; // UTF8 URL�������������ȷ������ʱ��ʼ��

	CString m_userName;			// �û�����ȷ������ʱ��ʼ��
	CString& m_cookie;
	CString m_bduss;			// Cookie�е�BDUSS��ȷ������ʱ��ʼ�������ڿͻ��˽ӿ�
	CString m_tbs;				// ����ţ������Ƿ�ֹCSRF�õģ�ȷ������ʱ��ʼ�������û�ȡtbs��ַ��http://tieba.baidu.com/dc/common/tbs

	const int& m_banDuration;	// ���ʱ��
	const CString& m_banReason; // ���ԭ��

	CString m_randomTid;		// ĳ��tid��ȷ������ʱ��ʼ��������WAP�ӿںͿͻ��˽ӿڷ��

public:
	CString GetForumID() { return m_forumID; }
	CString GetForumName() { return m_forumName; }
	CString GetEncodedForumName() { return m_encodedForumName; }
	CString GetUserName_() { return m_userName; }
	CString GetTBS() { return m_tbs; }

	CTiebaOperate(CString& cookie, const int& banDuration, const CString& banReason);

	enum SetTiebaResult { SET_TIEBA_OK, SET_TIEBA_TIMEOUT, SET_TIEBA_NOT_FOUND, SET_TIEBA_NOT_LOGIN, SET_TIEBA_NO_POWER, SET_TIEBA_NO_TBS };
	// ����Ҫ����������
	SetTiebaResult SetTieba(const CString& forumName);

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

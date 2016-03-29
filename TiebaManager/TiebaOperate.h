#pragma once


enum TBObject { TBOBJ_THREAD, TBOBJ_POST, TBOBJ_LZL };


struct Operation
{
	CString msg;		// ��ʾ��Ϣ
	int pos;			// ����λ��
	int length;			// ��������
	TBObject object;	// ��������
	CString tid;		// ����ID
	CString title;		// �������
	CString floor;		// ¥��
	CString pid;		// ����ID
	CString author;		// ��������
	CString authorID;	// ��������ID
	CString authorPortrait;	// ��������ͷ���ϣ
};


void AddConfirm(const CString& msg, TBObject object, const CString& tid, const CString& title,
	const CString& floor, const CString& pid, const CString& author, const CString& authorID,
	const CString& authorPortrait = _T(""), int pos = 0, int length = 0);
UINT AFX_CDECL ConfirmThread(LPVOID mainDlg);
void AddOperation(const Operation& op);
UINT AFX_CDECL OperateThread(LPVOID mainDlg);
CString BanID(LPCTSTR userName, LPCTSTR pid);
CString BanID(LPCTSTR userName);
extern CString g_randomTid; // ĳ��tid��ȷ������ʱ��ʼ����WAP�ӿ���
CString BanIDWap(LPCTSTR userName);
CString Defriend(LPCTSTR userID);
CString DeleteThread(const CString& tid);
CString DeletePost(LPCTSTR tid, LPCTSTR pid);
CString DeleteLZL(LPCTSTR tid, LPCTSTR lzlid);
CString GetTiebaErrorText(const CString& errorCode);

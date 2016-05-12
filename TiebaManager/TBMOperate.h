#pragma once
#include <TiebaOperate.h>


struct Operation
{
	enum TBObject { TBOBJ_THREAD, TBOBJ_POST, TBOBJ_LZL };

	BOOL forceToConfirm; // ǿ��ȷ��
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


void AddConfirm(BOOL forceToConfirm, const CString& msg, Operation::TBObject object, const CString& tid, const CString& title,
	const CString& floor, const CString& pid, const CString& author, const CString& authorID,
	const CString& authorPortrait = _T(""), int pos = 0, int length = 0);
UINT AFX_CDECL ConfirmThread(LPVOID mainDlg);
void AddOperation(const Operation& op);
UINT AFX_CDECL OperateThread(LPVOID mainDlg);

extern unique_ptr<CTiebaOperate> g_tiebaOperate;

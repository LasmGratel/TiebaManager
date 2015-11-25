#pragma once
#include "NormalDlg.h"
#include "afxwin.h"


// CLockThreadPage �Ի���

class CLockThreadPage : public CNormalDlg
{
	DECLARE_DYNAMIC(CLockThreadPage)

public:
	CLockThreadPage(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLockThreadPage();

// �Ի�������
	enum { IDD = IDD_LOCK_THREAD_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();

	static UINT AFX_CDECL LockThreadThread(LPVOID _thiz);


protected:
	static CLockThreadPage* m_instance;
	static volatile BOOL m_stopFlag;

public:
	CEdit m_tidEdit;
	CEdit m_pageEdit;
	CEdit m_floorEdit;
	CButton m_startButton;
	CButton m_stopButton;
};

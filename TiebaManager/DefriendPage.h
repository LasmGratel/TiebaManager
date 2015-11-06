#pragma once
#include "NormalDlg.h"
#include "afxwin.h"


// CDefriendPage �Ի���

class CDefriendPage : public CNormalDlg
{
	DECLARE_DYNAMIC(CDefriendPage)

public:
	CDefriendPage(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDefriendPage();

// �Ի�������
	enum { IDD = IDD_DEFRIEND_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();

	static UINT AFX_CDECL DefriendThread(LPVOID _thiz);


protected:
	static CDefriendPage* m_instance;
	static volatile BOOL m_stopFlag;

public:
	CEdit m_startPageEdit;
	CEdit m_endPageEdit;
	CButton m_startButton;
	CButton m_stopButton;
	CStatic m_stateStatic;
};

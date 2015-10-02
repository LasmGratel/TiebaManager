#pragma once
#include "afxwin.h"
#include "ListPage.h"
#include <vector>


// CLoopBanPage �Ի���

class CLoopBanPage : public CListPage
{
	DECLARE_DYNAMIC(CLoopBanPage)

public:
	CLoopBanPage(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLoopBanPage();

// �Ի�������
	enum { IDD = IDD_LOOP_BAN_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg virtual void OnClickedButton1();
	afx_msg virtual void OnClickedButton2();
	afx_msg virtual void OnClickedButton3();

protected:
	CString GetPID(const CString& userName);


public:
	CButton m_enableCheck;
	CButton m_logCheck;

	std::vector<CString> m_pid;
};

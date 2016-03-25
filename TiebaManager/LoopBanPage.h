#pragma once
#include "afxwin.h"
#include "NormalListPage.h"


// CLoopBanPage �Ի���

class CLoopBanPage : public CNormalListPage
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

	virtual void OnAdd(int index);


public:
	CButton m_enableCheck;
	CButton m_logCheck;
	CStatic m_static2;
	CEdit m_banIntervalEdit;
	CStatic m_static3;
};

UINT AFX_CDECL LoopBanThread(LPVOID _dlg);

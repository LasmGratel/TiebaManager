#pragma once
#include "ListPage.h"


// CWhiteListTestPage �Ի���

class CWhiteListPage : public CListPage
{
	DECLARE_DYNAMIC(CWhiteListPage)

public:
	CWhiteListPage(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CWhiteListPage();

// �Ի�������
	enum { IDD = IDD_LIST_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};

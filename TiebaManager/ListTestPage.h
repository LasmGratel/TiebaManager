#pragma once
#include "afxwin.h"
#include "ListPage.h"


// CListTestPage �Ի���

class CListTestPage : public CListPage
{
	DECLARE_DYNAMIC(CListTestPage)

public:
	CListTestPage(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CListTestPage();

// �Ի�������
	enum { IDD = IDD_LIST_TEST_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnClickedButton7();

protected:
	// ����ƥ��
	virtual BOOL TestMatch(int index) = 0;


public:
	CEdit m_testEdit;
	CButton m_testButton;
};

// �ò���ص���������������
static const TCHAR MATCH_TOO_MUCH_CONTENT_TEST1[] = _T("�����񵼺���ɾ����ѯ�������꾫���ɹ棬ˢ����ˢ��");
static const TCHAR MATCH_TOO_MUCH_CONTENT_TEST2[] = _T("���ꡤ����14�����޽��㿾�ع�");
static const TCHAR MATCH_TOO_MUCH_USERNAME_TEST1[] = _T("����ԭ�ŵ�ûJ8");
static const TCHAR MATCH_TOO_MUCH_USERNAME_TEST2[] = _T("��������");

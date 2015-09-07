#pragma once
#include "afxwin.h"
#include "NormalDlg.h"


// CRegListPage �Ի���

class CRegListPage : public CNormalDlg
{
	DECLARE_DYNAMIC(CRegListPage)

public:
	CRegListPage(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CRegListPage();

// �Ի�������
	enum { IDD = IDD_REG_LIST_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDblclkList1();
	afx_msg void OnClickedButton1();
	afx_msg void OnClickedButton2();
	afx_msg void OnClickedButton3();
	afx_msg void OnClickedButton6();

protected:
	// ����Ƿ�ƥ��̫��
	virtual BOOL CheckMatchTooMuch(const CString& text, BOOL isRegex){ return FALSE; };
	// ��ӻ��޸ĺ����
	virtual void PostChangeList(){}
	// ����ƥ��
	virtual BOOL TestMatch(const CString& test, const CString& text, BOOL isRegex) = 0;


public:
	CListBox m_list;
	CEdit m_edit;
	CButton m_regexCheck;
	CButton m_addButton;
	CButton m_deleteButton;
	CButton m_changeButton;
	CEdit m_testEdit;
	CStatic m_static;
	CButton m_testButton;
protected:
	COLORREF m_staticColor;
};

static const TCHAR IS_REGEX_PREFIX[] = _T("��|");
static const TCHAR NOT_REGEX_PREFIX[] = _T("��|");
static const int REGEX_PREFIX_LENGTH = _countof(IS_REGEX_PREFIX) - 1;

// �ò���ص���������������
static const TCHAR MATCH_TOO_MUCH_CONTENT_TEST1[] = _T("�����񵼺���ɾ����ѯ�������꾫���ɹ棬ˢ����ˢ��");
static const TCHAR MATCH_TOO_MUCH_CONTENT_TEST2[] = _T("���ꡤ����14�����޽��㿾�ع�");
static const TCHAR MATCH_TOO_MUCH_USERNAME_TEST1[] = _T("����ԭ�ŵ�ûJ8");
static const TCHAR MATCH_TOO_MUCH_USERNAME_TEST2[] = _T("��������");

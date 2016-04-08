#pragma once
#include "ListPage.h"


// CNormalListPage �Ի���

class CNormalListPage : public CListPage
{
	DECLARE_DYNAMIC(CNormalListPage)

public:
	CNormalListPage(const CString& inputTitle, CWnd* pParent = NULL);   // ��׼���캯��
	CNormalListPage(const CString& inputTitle, UINT nIDTemplate, CWnd* pParentWnd = NULL);
	virtual ~CNormalListPage();

	// �Ի�������
	enum { IDD = IDD_LIST_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	virtual BOOL SetItem(int index);
	virtual BOOL Export(const CString& path);
	virtual BOOL Import(const CString& path);
	virtual void ShowList(const vector<CString>& list);
	virtual void ShowList(const set<CString>& list);
	virtual void ApplyList(vector<CString>& list);
	virtual void ApplyList(set<CString>& list);


protected:
	const CString m_inputTitle;
};

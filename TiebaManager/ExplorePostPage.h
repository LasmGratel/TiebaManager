#pragma once
#include "ExplorerPage.h"
struct PostInfo;


// CExploreThreadPage �Ի���

class CExplorePostPage : public CExplorerPage
{
	DECLARE_DYNAMIC(CExplorePostPage)

public:
	CExplorePostPage(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CExplorePostPage();

// �Ի�������
	enum { IDD = IDD_EXPLORER_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void OnBnClickedButton1();
	afx_msg void OnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);


public:
	CString m_tid;
	vector<PostInfo> m_posts;
};

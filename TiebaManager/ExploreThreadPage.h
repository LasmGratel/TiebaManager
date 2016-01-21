#pragma once
#include "ExplorerPage.h"
struct ThreadInfo;


// CExploreThreadPage �Ի���

class CExploreThreadPage : public CExplorerPage
{
	DECLARE_DYNAMIC(CExploreThreadPage)

public:
	CExploreThreadPage(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CExploreThreadPage();

// �Ի�������
	enum { IDD = IDD_EXPLORER_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void OnBnClickedButton1();
	afx_msg void OnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);


public:
	vector<ThreadInfo> m_threads;
};

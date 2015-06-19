#pragma once
#include "ExplorerPage.h"
#include <vector>
using std::vector;
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

protected:
	vector<ThreadInfo> m_threads;
};

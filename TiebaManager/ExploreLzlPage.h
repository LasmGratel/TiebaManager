#pragma once
#include "ExplorerPage.h"
#include <vector>
using std::vector;
struct PostInfo;


// CExploreThreadPage �Ի���

class CExploreLzlPage : public CExplorerPage
{
	DECLARE_DYNAMIC(CExploreLzlPage)

public:
	CExploreLzlPage(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CExploreLzlPage();

// �Ի�������
	enum { IDD = IDD_EXPLORER_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);


public:
	vector<PostInfo> m_lzls;
};

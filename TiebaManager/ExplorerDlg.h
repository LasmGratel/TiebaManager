#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "ExploreThreadPage.h"


// CExplorerDlg �Ի���

class CExplorerDlg : public CDialog
{
	DECLARE_DYNAMIC(CExplorerDlg)

public:
	CExplorerDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CExplorerDlg();

// �Ի�������
	enum { IDD = IDD_EXPLORER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	virtual void OnCancel();
	virtual void OnOK();
public:
	afx_msg void OnClose();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void PostNcDestroy();
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnInitDialog();


public:
	CTabCtrl m_tab;
	CExploreThreadPage m_exploreThreadPage;
	CEdit m_edit;
	CButton m_deleteButton;
	CButton m_banButton;
	CButton m_explorerButton;

protected:
	CDialog* m_pages[1];
};

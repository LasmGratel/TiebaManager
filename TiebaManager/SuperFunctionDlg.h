#pragma once
#include "NormalDlg.h"
#include "LoopBanPage.h"


// CSuperFunctionDlg �Ի���

class CSuperFunctionDlg : public CNormalDlg
{
	DECLARE_DYNAMIC(CSuperFunctionDlg)

public:
	CSuperFunctionDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSuperFunctionDlg();

// �Ի�������
	enum { IDD = IDD_SETTING_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void PostNcDestroy();
	virtual void OnOK();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClose();

	void ShowCurrentOptions();
	void ApplyOptionsInDlg();


public:
	CTabCtrl m_tab;
	CLoopBanPage m_loopBanPage;
	CButton m_okButton;
	CButton m_cancelButton;

	BOOL m_clearScanCache;
protected:
	CWnd* m_pages[1];
	CResizeControl m_pagesResize;
};

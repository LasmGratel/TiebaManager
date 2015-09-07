#pragma once
#include "afxwin.h"
#include "NormalDlg.h"


// CPrefPage �Ի���

class CPrefPage : public CNormalDlg
{
	DECLARE_DYNAMIC(CPrefPage)

public:
	CPrefPage(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPrefPage();

// �Ի�������
	enum { IDD = IDD_PREF_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnEnKillfocusEdit1();
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnEnKillfocusEdit3();
	afx_msg void OnEnKillfocusEdit4();
	afx_msg void OnEnKillfocusEdit5();
	afx_msg void OnEnChangeEdit5();
	afx_msg void OnEnKillfocusEdit2();


public:
	CEdit m_scanIntervalEdit;
	CButton m_deleteCheck;
	CButton m_banIDCheck;
	CComboBox m_banDurationCombo;
	CEdit m_banReasonEdit;
	CEdit m_trigCountEdit;
	CButton m_onlyScanTitleCheck;
	CEdit m_deleteIntervalEdit;
	CButton m_confirmCheck;
	CEdit m_scanPageCountEdit;
	CButton m_briefLogCheck;
	CEdit m_threadCountEdit;
};

#pragma once
#include "afxwin.h"
#include "Tieba.h"


// CConfirmDlg �Ի���

class CConfirmDlg : public CDialog
{
	DECLARE_DYNAMIC(CConfirmDlg)

public:
	CConfirmDlg(CWnd* pParent = NULL);   // ��׼���캯��
	CConfirmDlg(Operation* operation, CWnd* pParent = NULL);   // ָ�����ݵĹ��캯��
	virtual ~CConfirmDlg();

// �Ի�������
	enum { IDD = IDD_CONFIRM_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();


public:
	CEdit m_contentEdit;
	CStatic m_static;
	CButton m_yesButton;
	CButton m_noButton;

protected:
	Operation* m_operation;
public:
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
};

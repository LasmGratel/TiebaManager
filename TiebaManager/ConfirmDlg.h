#pragma once
#include "afxwin.h"
#include "TBMOperate.h"
#include "ResizeControl.h"
class CImageViewDlg;


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
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnBnClickedButton1();


public:
	CEdit m_contentEdit;
	CStatic m_static;
	CButton m_explorerButton;
	CButton m_yesButton;
	CButton m_noButton;

	CImageViewDlg* m_imageViewDlg;

protected:
	CResizeControl m_resize;

	Operation* m_operation;
};

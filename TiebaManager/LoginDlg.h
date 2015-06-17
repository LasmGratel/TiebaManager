#pragma once
#include "afxwin.h"


// CLoginDlg �Ի���

class CLoginDlg : public CDialog
{
	DECLARE_DYNAMIC(CLoginDlg)

public:
	CLoginDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLoginDlg();

// �Ի�������
	enum { IDD = IDD_LOGIN_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnStnClickedStatic4();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);


public:
	CEdit m_userNameEdit;
	CEdit m_passwordEdit;
	CEdit m_verifyCodeEdit;
	CStatic m_verifyCodePicture;

	CString m_cookie;
	CString m_userName;
protected:
	CString m_token;
	CString m_verifyStr;
	CImage m_verifyImage;
};

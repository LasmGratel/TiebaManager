#pragma once
#include "afxwin.h"


// CInputDlg �Ի���

class CInputDlg : public CDialog
{
	DECLARE_DYNAMIC(CInputDlg)

public:
	CInputDlg(const CString& title, CString& content, BOOL* isRegex = NULL, BOOL showRegexCheck = FALSE, UINT nIDTemplate = IDD, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CInputDlg();

// �Ի�������
	enum { IDD = IDD_INPUT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();


public:
	CEdit m_edit;
	CButton m_regexCheck;

	CString& m_content;
	BOOL* m_isRegex;

protected:
	const CString m_title;
	const BOOL m_showRegexCheck;
};

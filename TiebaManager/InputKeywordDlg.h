#pragma once
#include "afxwin.h"
#include "InputDlg.h"


// CInputKeywordDlg �Ի���

class CInputKeywordDlg : public CInputDlg
{
	DECLARE_DYNAMIC(CInputKeywordDlg)

public:
	CInputKeywordDlg(const CString& title, CString& content, BOOL* isRegex = NULL, BOOL* forceToConfirm = NULL, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CInputKeywordDlg();

	// �Ի�������
	enum { IDD = IDD_INPUT_KEYWORD_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();


public:
	CButton m_forceToConfirmCheck;

	BOOL* m_forceToConfirm;
};

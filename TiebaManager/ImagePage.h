#pragma once
#include "afxwin.h"
#include "NormalDlg.h"


// CImagePage �Ի���

class CImagePage : public CNormalDlg
{
	DECLARE_DYNAMIC(CImagePage)

public:
	CImagePage(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CImagePage();

// �Ի�������
	enum { IDD = IDD_IMAGE_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnEnKillfocusEdit5();


public:
	CEdit m_dirEdit;
	CButton m_browseButton;
	CButton m_updateButton;
	CStatic m_static;
	CEdit m_thresholdEdit;
};

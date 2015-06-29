#pragma once
#include "afxwin.h"


// CImagePage �Ի���

class CImagePage : public CDialog
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
	virtual void OnCancel();
	virtual void OnOK();
public:
	afx_msg void OnClose();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();


public:
	CEdit m_dirEdit;
	CButton m_browseButton;
	CButton m_calcFeatureButton;
};

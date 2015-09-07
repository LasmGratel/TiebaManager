#pragma once


// CSuperFunctionDlg �Ի���

class CSuperFunctionDlg : public CDialog
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


public:
	CTabCtrl m_tab;
	CButton m_okButton;
	CButton m_cancelButton;
protected:
	CDialog* m_pages[1];
};

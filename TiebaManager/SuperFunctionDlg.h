#pragma once
#include "NormalDlg.h"
#include "LoopBanPage.h"
#include "DefriendPage.h"
#include "LockThreadPage.h"
#include "Setting.h"


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
	CDefriendPage m_defriendPage;
	CLockThreadPage m_lockThreadPage;
	CButton m_okButton;
	CButton m_cancelButton;

	BOOL m_clearCache;
protected:
	CWnd* m_pages[3];
	CResizeControl m_pagesResize;
};

class CLoopBanConfig : public CConfigBase
{
public:
	COption<BOOL> m_enable;					// ����
	COption<BOOL> m_log;					// �����־
	COption<float> m_banInterval;			// ������
	COption<vector<CString> > m_userList;	// �û��б�

	CLoopBanConfig()
		: CConfigBase("LoopBan"),
		m_enable("Enable", TRUE),
		m_log("Log"),
		m_banInterval("BanInterval", 0.0f, [](const float& value)->BOOL{ return 0.0f <= value && value <= 60.0f; }),
		m_userList("Name")
	{
		m_options.push_back(&m_enable);
		m_options.push_back(&m_log);
		m_options.push_back(&m_banInterval);
		m_options.push_back(&m_userList);
	}

	BOOL LoadOld(const CString& path)
	{
		gzFile f = gzopen_w(path, "rb");
		if (f == NULL)
			return FALSE;

		// ͷ��
		char header[2];
		gzread(f, header, sizeof(header));
		if (header[0] != 'T' || header[1] != 'B')
		{
			gzclose(f);
			return FALSE;
		}

		int size;
		gzread(f, &size, sizeof(int)); // ����
		m_userList->resize(size);
		CString tmp;
		for (int i = 0; i < size; i++)
		{
			ReadText(f, (*m_userList)[i]);
			ReadText(f, tmp);
		}
		*m_log = FALSE;
		gzread(f, &*m_log, sizeof(BOOL)); // �����־
		if (gzread(f, &*m_enable, sizeof(BOOL)) != sizeof(BOOL)) // ����
			*m_enable = TRUE;

		gzclose(f);
		return TRUE;
	}
};

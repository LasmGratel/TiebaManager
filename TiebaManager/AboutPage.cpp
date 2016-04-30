// AboutPage.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AboutPage.h"
#include "Update.h"


// CAboutPage �Ի���

IMPLEMENT_DYNAMIC(CAboutPage, CNormalDlg)

CAboutPage::CAboutPage(CWnd* pParent /*=NULL*/)
	: CNormalDlg(CAboutPage::IDD, pParent)
{

}

#pragma region MFC
CAboutPage::~CAboutPage()
{
}

void CAboutPage::DoDataExchange(CDataExchange* pDX)
{
	CNormalDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_edit);
	DDX_Control(pDX, IDC_STATIC1, m_checkUpdateStatic);
	DDX_Control(pDX, IDC_CHECK1, m_autoCheckUpdateCheck);
	DDX_Control(pDX, IDC_STATIC2, m_authorStatic);
}


BEGIN_MESSAGE_MAP(CAboutPage, CNormalDlg)
	ON_STN_CLICKED(IDC_STATIC1, &CAboutPage::OnStnClickedStatic1)
	ON_STN_CLICKED(IDC_STATIC2, &CAboutPage::OnStnClickedStatic2)
END_MESSAGE_MAP()
#pragma endregion

// CAboutPage ��Ϣ�������

// ��ʼ��
BOOL CAboutPage::OnInitDialog()
{
	CNormalDlg::OnInitDialog();

	m_resize.AddControl(&m_edit, RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, this);
	m_resize.AddControl(&m_checkUpdateStatic, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_edit);
	m_resize.AddControl(&m_autoCheckUpdateCheck, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_edit);
	m_resize.AddControl(&m_authorStatic, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, &m_edit);

	m_edit.SetWindowText(_T("���������http://tieba.baidu.com/p/3915111330\r\n")
						 _T("\r\n")
						 _T("html����ֻת����\"'&<>���пո���������ҳԴ��Ϊ׼���绻�з�Ϊ<br> (��ע��Ļ��᲻ƥ���ƥ��̫��)\r\n")
						 _T("\r\n")
						 _T("�ҵ������֧������xfgryujk@126.com��ϲ����������Ծ���Ŷ\r\n")
						 _T("\r\n")
						 _T("�˳���ͨ��GPLЭ�鿪Դ���������https://github.com/xfgryujk/TiebaManager�õ�Դ�룬��ӭ���Ƴ���\r\n")
						 _T("\r\n")
						 _T("�Ը�����־����Ȥ�Ļ�������https://github.com/xfgryujk/TiebaManager/commits/master\r\n")
						 _T("\r\n")
						 _T("��ֻ���ṩ�������������ɵĹ��ߣ�����������ɵȶ�����Ϊ��ɵ���ʧ���˲�����"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}

// ������
void CAboutPage::OnStnClickedStatic1()
{
	switch (CheckUpdate())
	{
	case UPDATE_NO_UPDATE:
		AfxMessageBox(_T("�Ѿ������°汾"));
		break;
	case UPDATE_FAILED_TO_GET_FILE_INFO:
		if (AfxMessageBox(_T("��ȡ�ļ���Ϣʧ�ܣ��ֶ����£�"), MB_ICONQUESTION | MB_YESNO) == IDYES)
			ShellExecute(NULL, _T("open"), UPDATE_URL, NULL, NULL, SW_NORMAL);
		break;
	}
}

// ��������
void CAboutPage::OnStnClickedStatic2()
{
	ShellExecute(NULL, _T("open"), _T("http://tieba.baidu.com/i/37897560"), NULL, NULL, SW_NORMAL);
}

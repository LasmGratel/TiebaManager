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

	m_edit.SetWindowText(_T("���������http://tieba.baidu.com/p/3915111330\r\n\
\r\n\
html����ֻת����\"'&<>���пո���������ҳԴ��Ϊ׼���绻�з�Ϊ<br> (��ע��Ļ��᲻ƥ���ƥ��̫��)\r\n\
\r\n\
�ҵ������֧������xfgryujk@126.com��ϲ����������Ծ���Ŷ\r\n\
\r\n\
�˳���ͨ��GPLЭ�鿪Դ���������https://github.com/xfgryujk/TiebaManager�õ�Դ�룬��ӭ���Ƴ���\r\n\
\r\n\
�Ը�����־����Ȥ�Ļ�������https://github.com/xfgryujk/TiebaManager/commits/master\r\n\
\r\n\
��ֻ���ṩ�������������ɵĹ��ߣ�����������ɵȶ�����Ϊ��ɵ���ʧ���˲�����"));

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
	case UPDATE_FAILED_TO_GET_FILE_ID:
		if (AfxMessageBox(_T("��ȡ�ļ�IDʧ�ܣ��ֶ����£�"), MB_ICONQUESTION | MB_YESNO) == IDYES)
			ShellExecute(NULL, _T("open"), _T("http://pan.baidu.com/s/1hq86os8#dir/path=%2F%E6%88%91%E7%9A\
%84%E5%88%86%E4%BA%AB%2F%E7%99%BE%E5%BA%A6%E8%B4%B4%E5%90%A7%E7%9B%B8%E5%85%B3"), NULL, NULL, SW_NORMAL);
		break;
	case UPDATE_FAILED_TO_GET_LINK:
		if (AfxMessageBox(_T("��ȡ���ص�ַʧ�ܣ��ֶ����£�"), MB_ICONQUESTION | MB_YESNO) == IDYES)
			ShellExecute(NULL, _T("open"), _T("http://pan.baidu.com/s/1hq86os8#dir/path=%2F%E6%88%91%E7%9A\
%84%E5%88%86%E4%BA%AB%2F%E7%99%BE%E5%BA%A6%E8%B4%B4%E5%90%A7%E7%9B%B8%E5%85%B3"), NULL, NULL, SW_NORMAL);
		break;
	}
}

// ��������
void CAboutPage::OnStnClickedStatic2()
{
	ShellExecute(NULL, _T("open"), _T("http://tieba.baidu.com/i/37897560"), NULL, NULL, SW_NORMAL);
}

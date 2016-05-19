// PluginDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PluginDlg.h"

#include "PluginManager.h"


// CPluginDlg �Ի���

IMPLEMENT_DYNAMIC(CPluginDlg, CNormalDlg)

CPluginDlg::CPluginDlg(CPluginDlg*& pThis, CPluginManager& pluginManager, CWnd* pParent /*=NULL*/) : CNormalDlg(CPluginDlg::IDD, pParent),
	m_pThis(pThis),
	m_pluginManager(pluginManager),
	m_plugins(pluginManager.GetPlugins())
{

}

#pragma region MFC
CPluginDlg::~CPluginDlg()
{
}

void CPluginDlg::DoDataExchange(CDataExchange* pDX)
{
	CNormalDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDC_BUTTON1, m_configButton);
	DDX_Control(pDX, IDC_EDIT1, m_edit);
}


BEGIN_MESSAGE_MAP(CPluginDlg, CNormalDlg)
	ON_WM_CLOSE()
	ON_LBN_SELCHANGE(IDC_LIST1, &CPluginDlg::OnLbnSelchangeList1)
	ON_BN_CLICKED(IDC_BUTTON1, &CPluginDlg::OnBnClickedButton1)
END_MESSAGE_MAP()
#pragma endregion

// CPluginDlg ��Ϣ�������

// ��ʼ��
BOOL CPluginDlg::OnInitDialog()
{
	CNormalDlg::OnInitDialog();

	HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(hIcon, FALSE);			// ����Сͼ��

	m_resize.AddControl(&m_list, RT_NULL, NULL, RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, this);
	m_resize.AddControl(&m_configButton, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list);
	m_resize.AddControl(&m_edit, RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, this);

	// ��ʾ���
	for (const auto& i : m_plugins)
		m_list.AddString(i->m_name);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}

#pragma region UI
// ���� /////////////////////////////////////////////////////////////////////////////////

// ȡ��
void CPluginDlg::OnCancel()
{
	DestroyWindow();
}

// �رմ���
void CPluginDlg::OnClose()
{
	DestroyWindow();
}

// �ͷ�this
void CPluginDlg::PostNcDestroy()
{
	CNormalDlg::PostNcDestroy();

	m_pThis = NULL;
	delete this;
}
#pragma endregion

// ѡ��
void CPluginDlg::OnLbnSelchangeList1()
{
	int index = m_list.GetCurSel();
	if (index == LB_ERR)
		return;
	m_edit.SetWindowText(m_plugins[index]->GetDescription());
}

// ����
void CPluginDlg::OnBnClickedButton1()
{
	int index = m_list.GetCurSel();
	if (index == LB_ERR)
		return;
	m_plugins[m_list.GetCurSel()]->OnConfig();
}

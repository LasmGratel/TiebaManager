// InputDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TiebaManager.h"
#include "InputDlg.h"


// CInputDlg �Ի���

IMPLEMENT_DYNAMIC(CInputDlg, CDialog)

CInputDlg::CInputDlg(const CString& title, CString& content, BOOL* isRegex, BOOL showRegexCheck, UINT nIDTemplate, CWnd* pParent /*=NULL*/)
	: CDialog(nIDTemplate, pParent), 
	m_title(title), 
	m_content(content),
	m_isRegex(isRegex),
	m_showRegexCheck(showRegexCheck)
{

}

CInputDlg::~CInputDlg()
{
}

void CInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_edit);
	DDX_Control(pDX, IDC_CHECK1, m_regexCheck);
}


BEGIN_MESSAGE_MAP(CInputDlg, CDialog)
END_MESSAGE_MAP()


// CInputDlg ��Ϣ�������

// ��ʼ��
BOOL CInputDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(m_title);

	m_edit.SetWindowText(m_content);
	if (m_isRegex != NULL)
		m_regexCheck.SetCheck(*m_isRegex);

	if (m_showRegexCheck)
		m_regexCheck.ShowWindow(SW_NORMAL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}

// ȷ��
void CInputDlg::OnOK()
{
	m_edit.GetWindowText(m_content);
	if (m_isRegex != NULL)
		*m_isRegex = m_regexCheck.GetCheck();

	CDialog::OnOK();
}

// PrefPage.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PrefPage.h"
#include "SettingDlg.h"


// CPrefPage �Ի���

IMPLEMENT_DYNAMIC(CPrefPage, CDialog)

CPrefPage::CPrefPage(CWnd* pParent /*=NULL*/)
	: CDialog(CPrefPage::IDD, pParent)
{

}

#pragma region MFC
CPrefPage::~CPrefPage()
{
}

void CPrefPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_scanIntervalEdit);
	DDX_Control(pDX, IDC_CHECK2, m_deleteCheck);
	DDX_Control(pDX, IDC_CHECK1, m_banIDCheck);
	DDX_Control(pDX, IDC_COMBO1, m_banDurationCombo);
	DDX_Control(pDX, IDC_EDIT3, m_trigCountEdit);
	DDX_Control(pDX, IDC_CHECK3, m_onlyScanTitleCheck);
	DDX_Control(pDX, IDC_EDIT4, m_deleteIntervalEdit);
	DDX_Control(pDX, IDC_CHECK4, m_confirmCheck);
	DDX_Control(pDX, IDC_EDIT5, m_scanPageCountEdit);
	DDX_Control(pDX, IDC_CHECK5, m_briefLogCheck);
	DDX_Control(pDX, IDC_EDIT2, m_threadCountEdit);
	DDX_Control(pDX, IDC_EDIT7, m_banReasonEdit);
}


BEGIN_MESSAGE_MAP(CPrefPage, CDialog)
	ON_WM_CLOSE()
	ON_EN_KILLFOCUS(IDC_EDIT1, &CPrefPage::OnEnKillfocusEdit1)
	ON_BN_CLICKED(IDC_CHECK1, &CPrefPage::OnBnClickedCheck1)
	ON_EN_KILLFOCUS(IDC_EDIT3, &CPrefPage::OnEnKillfocusEdit3)
	ON_EN_KILLFOCUS(IDC_EDIT4, &CPrefPage::OnEnKillfocusEdit4)
	ON_EN_KILLFOCUS(IDC_EDIT5, &CPrefPage::OnEnKillfocusEdit5)
	ON_EN_CHANGE(IDC_EDIT5, &CPrefPage::OnEnChangeEdit5)
	ON_EN_KILLFOCUS(IDC_EDIT2, &CPrefPage::OnEnKillfocusEdit2)
END_MESSAGE_MAP()
#pragma endregion

// CPrefPage ��Ϣ�������

#pragma region UI
// ����Esc�رմ���
void CPrefPage::OnCancel()
{
}

// ���λس��رմ���
void CPrefPage::OnOK()
{
}

// ���ٴ���
void CPrefPage::OnClose()
{
	DestroyWindow();

	CDialog::OnClose();
}
#pragma endregion

BOOL CPrefPage::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��ʼ�����ʱ��
	m_banDurationCombo.AddString(_T("1"));
	m_banDurationCombo.AddString(_T("3"));
	m_banDurationCombo.AddString(_T("10"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}

// ɨ����
void CPrefPage::OnEnKillfocusEdit1()
{
	CString tmp;
	m_scanIntervalEdit.GetWindowText(tmp);
	int scanInterval = _ttoi(tmp);
	if (scanInterval < 0 || scanInterval > 600)
		m_scanIntervalEdit.SetWindowText(_T("5"));
}

// ��ID
void CPrefPage::OnBnClickedCheck1()
{
	BOOL enable = m_banIDCheck.GetCheck();
	m_banDurationCombo.EnableWindow(enable);
	m_banReasonEdit.EnableWindow(enable);
	m_trigCountEdit.EnableWindow(enable);
}

// ���Υ�����
void CPrefPage::OnEnKillfocusEdit3()
{
	CString tmp;
	m_trigCountEdit.GetWindowText(tmp);
	int trigCount = _ttoi(tmp);
	if (trigCount < 1)
		m_trigCountEdit.SetWindowText(_T("1"));
}

// ɾ�����
void CPrefPage::OnEnKillfocusEdit4()
{
	CString tmp;
	m_deleteIntervalEdit.GetWindowText(tmp);
	float deleteInterval = (float)_ttof(tmp);
	if (deleteInterval < 0 || deleteInterval > 60)
		m_deleteIntervalEdit.SetWindowText(_T("1.5"));
}

// ɨ�����ҳ�� ʧȥ����
void CPrefPage::OnEnKillfocusEdit5()
{
	CString tmp;
	m_scanPageCountEdit.GetWindowText(tmp);
	int scanPageCount = _ttoi(tmp);
	if (scanPageCount < 1)
		m_scanPageCountEdit.SetWindowText(_T("1"));
}

// ɨ�����ҳ�� ���ݸı�
void CPrefPage::OnEnChangeEdit5()
{
	((CSettingDlg*)GetParent()->GetParent())->m_clearScanCache = TRUE;
}

// �߳���
void CPrefPage::OnEnKillfocusEdit2()
{
	CString tmp;
	m_threadCountEdit.GetWindowText(tmp);
	int threadCount = _ttoi(tmp);
	if (threadCount < 1 || threadCount > 16)
		m_threadCountEdit.SetWindowText(_T("2"));
}

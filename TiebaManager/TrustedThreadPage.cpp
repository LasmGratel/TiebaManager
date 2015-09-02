// TrustedThreadPage.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TrustedThreadPage.h"


// CTrustedThreadPage �Ի���

IMPLEMENT_DYNAMIC(CTrustedThreadPage, CDialog)

CTrustedThreadPage::CTrustedThreadPage(CWnd* pParent /*=NULL*/)
	: CDialog(CTrustedThreadPage::IDD, pParent)
{

}

#pragma region MFC
CTrustedThreadPage::~CTrustedThreadPage()
{
}

void CTrustedThreadPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDC_EDIT1, m_edit);
	DDX_Control(pDX, IDC_BUTTON1, m_addButton);
	DDX_Control(pDX, IDC_BUTTON2, m_deleteButton);
	DDX_Control(pDX, IDC_BUTTON3, m_changeButton);
	DDX_Control(pDX, IDC_STATIC1, m_static);
}


BEGIN_MESSAGE_MAP(CTrustedThreadPage, CDialog)
	ON_WM_CLOSE()
	ON_WM_CTLCOLOR()
	ON_LBN_DBLCLK(IDC_LIST1, &CTrustedThreadPage::OnLbnDblclkList1)
	ON_BN_CLICKED(IDC_BUTTON1, &CTrustedThreadPage::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CTrustedThreadPage::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CTrustedThreadPage::OnBnClickedButton3)
END_MESSAGE_MAP()
#pragma endregion

// CTrustedThreadPage ��Ϣ�������

#pragma region UI
// ����Esc�رմ���
void CTrustedThreadPage::OnCancel()
{
}

// ���λس��رմ���
void CTrustedThreadPage::OnOK()
{
}

// ���ٴ���
void CTrustedThreadPage::OnClose()
{
	DestroyWindow();

	CDialog::OnClose();
}

// �ؼ���ɫ
HBRUSH CTrustedThreadPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->m_hWnd == m_static.m_hWnd)
		pDC->SetTextColor(RGB(255, 0, 0));

	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}
#pragma endregion

// ˫���б�
void CTrustedThreadPage::OnLbnDblclkList1()
{
	CString tmp;
	m_list.GetText(m_list.GetCurSel(), tmp);
	m_edit.SetWindowText(tmp);
}

// ���
void CTrustedThreadPage::OnBnClickedButton1()
{
	CString text;
	m_edit.GetWindowText(text);
	if (text == _T(""))
	{
		AfxMessageBox(_T("���ݲ���Ϊ�գ�"), MB_ICONERROR);
		return;
	}
	int index = m_list.GetCurSel();
	index = m_list.InsertString(index + 1, text);
	m_list.SetCurSel(index);
}

// ɾ��
void CTrustedThreadPage::OnBnClickedButton2()
{
	int index = m_list.GetCurSel();
	if (index == LB_ERR)
		return;
	m_list.DeleteString(index);
	m_list.SetCurSel(index == 0 ? 0 : index - 1);
}

// �޸�
void CTrustedThreadPage::OnBnClickedButton3()
{
	CString text;
	m_edit.GetWindowText(text);
	if (text == _T(""))
	{
		AfxMessageBox(_T("���ݲ���Ϊ�գ�"), MB_ICONERROR);
		return;
	}
	int index = m_list.GetCurSel();
	if (index == LB_ERR)
		return;
	m_list.DeleteString(index);
	index = m_list.InsertString(index, text);
	m_list.SetCurSel(index);
}

// ListPage.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ListPage.h"


// CListPage �Ի���

IMPLEMENT_DYNAMIC(CListPage, CNormalDlg)

CListPage::CListPage(CWnd* pParent /*=NULL*/)
	: CNormalDlg(CListPage::IDD, pParent)
{
	m_staticColor = RGB(255, 0, 0);
}

CListPage::CListPage(UINT nIDTemplate, CWnd* pParentWnd)
	: CNormalDlg(nIDTemplate, pParentWnd)
{
	m_staticColor = RGB(255, 0, 0);
}

#pragma region MFC
CListPage::~CListPage()
{
}

void CListPage::DoDataExchange(CDataExchange* pDX)
{
	CNormalDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDC_BUTTON1, m_addButton);
	DDX_Control(pDX, IDC_BUTTON2, m_deleteButton);
	DDX_Control(pDX, IDC_BUTTON3, m_changeButton);
	DDX_Control(pDX, IDC_BUTTON4, m_exportButton);
	DDX_Control(pDX, IDC_BUTTON5, m_importButton);
	DDX_Control(pDX, IDC_BUTTON6, m_clearButton);
	DDX_Control(pDX, IDC_STATIC1, m_static);
}


BEGIN_MESSAGE_MAP(CListPage, CNormalDlg)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON1, &CListPage::OnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CListPage::OnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CListPage::OnClickedButton3)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CListPage::OnNMDblclkList1)
	ON_BN_CLICKED(IDC_BUTTON4, &CListPage::OnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CListPage::OnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CListPage::OnClickedButton6)
END_MESSAGE_MAP()
#pragma endregion

// CListPage ��Ϣ�������

#pragma region UI
// �ؼ���ɫ
HBRUSH CListPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CNormalDlg::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->m_hWnd == m_static.m_hWnd)
		pDC->SetTextColor(m_staticColor);

	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}
#pragma endregion

// ��ʼ��
BOOL CListPage::OnInitDialog()
{
	CNormalDlg::OnInitDialog();

	m_resize.AddControl(&m_list, RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, this);
	m_resize.AddControl(&m_addButton, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, this);
	m_resize.AddControl(&m_deleteButton, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, this);
	m_resize.AddControl(&m_changeButton, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, this);
	m_resize.AddControl(&m_exportButton, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, this);
	m_resize.AddControl(&m_importButton, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, this);
	m_resize.AddControl(&m_clearButton, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, this);
	m_resize.AddControl(&m_static, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, this, RT_KEEP_DIST_TO_RIGHT, this);

	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}

// ���
void CListPage::OnClickedButton1()
{
	int index = m_list.GetSelectionMark() + 1;
	index = m_list.InsertItem(index, NULL);
	if (SetItem(index))
	{
		m_list.SetSelectionMark(index);
		m_list.SetItemState(index, LVNI_FOCUSED | LVNI_SELECTED, LVNI_FOCUSED | LVNI_SELECTED);
		OnAdd(index);
	}
	else
		m_list.DeleteItem(index);
}

// ɾ��
void CListPage::OnClickedButton2()
{
	int index = m_list.GetSelectionMark();
	if (index == LB_ERR)
		return;
	m_list.DeleteItem(index);
	OnDelete(index);
	index = index > 0 ? index - 1 : index;
	m_list.SetSelectionMark(index);
	m_list.SetItemState(index, LVNI_FOCUSED | LVNI_SELECTED, LVNI_FOCUSED | LVNI_SELECTED);
}

// �޸�
void CListPage::OnClickedButton3()
{
	int index = m_list.GetSelectionMark();
	if (index == LB_ERR)
		return;
	if (SetItem(index))
		OnAdd(index);
}

// ����
void CListPage::OnClickedButton4()
{
	CFileDialog dlg(FALSE, _T("xml"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("XML�ļ� (*.xml)|*.xml|TXT�ļ� (*.txt)|*.txt|�����ļ� (*.*)|*.*||"), this);
	if (dlg.DoModal() == IDOK)
	{
		if (!Export(dlg.GetPathName()))
			AfxMessageBox(_T("����ʧ�ܣ�"), MB_ICONERROR);
	}
}

// ����txt
BOOL CListPage::Export(const CString& path)
{
	CStdioFile file;
	if (!file.Open(path, CFile::modeCreate | CFile::modeWrite))
		return FALSE;

	for (int i = 0; i < m_list.GetItemCount(); i++)
	{
		file.WriteString(m_list.GetItemText(i, 0));
		file.WriteString(_T("\n"));
	}
	return TRUE;
}

// ����
void CListPage::OnClickedButton5()
{
	CFileDialog dlg(TRUE, _T("xml"), NULL, 0,
		_T("XML�ļ� (*.xml)|*.xml|TXT�ļ� (*.txt)|*.txt|�����ļ� (*.*)|*.*||"), this);
	if (dlg.DoModal() == IDOK)
	{
		if (!Import(dlg.GetPathName()))
			AfxMessageBox(_T("��ȡʧ�ܣ�"), MB_ICONERROR);
		else
			OnAdd(-1);
	}
}

// ����txt
BOOL CListPage::Import(const CString& path)
{
	CStdioFile file;
	if (!file.Open(path, CFile::modeRead))
		return FALSE;

	m_list.DeleteAllItems();
	int i = 0;
	CString tmp;
	while (file.ReadString(tmp))
		m_list.InsertItem(i++, tmp);
	return TRUE;
}

// ˫��
void CListPage::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if (pNMItemActivate->iItem != LB_ERR)
		OnClickedButton3();
	*pResult = 0;
}

// ���
void CListPage::OnClickedButton6()
{
	m_list.DeleteAllItems();
	OnDelete(-1);
}

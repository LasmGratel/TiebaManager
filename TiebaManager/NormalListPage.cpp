// NormalListPage.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "NormalListPage.h"
#include "Setting.h"
#include "InputDlg.h"


// CNormalListPage �Ի���

IMPLEMENT_DYNAMIC(CNormalListPage, CListPage)

CNormalListPage::CNormalListPage(const CString& inputTitle, CWnd* pParent /*=NULL*/)
	: CListPage(pParent), 
	m_inputTitle(inputTitle)
{
}

CNormalListPage::CNormalListPage(const CString& inputTitle, UINT nIDTemplate, CWnd* pParentWnd /*=NULL*/)
	: CListPage(nIDTemplate, pParentWnd),
	m_inputTitle(inputTitle)
{
}

CNormalListPage::~CNormalListPage()
{
}

void CNormalListPage::DoDataExchange(CDataExchange* pDX)
{
	CListPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CNormalListPage, CListPage)
END_MESSAGE_MAP()


// CNormalListPage ��Ϣ�������


BOOL CNormalListPage::OnInitDialog()
{
	CListPage::OnInitDialog();

	m_list.ModifyStyle(0, LVS_NOCOLUMNHEADER);
	m_list.InsertColumn(0, _T(""), LVCFMT_LEFT, 500);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}

class CNormalListFile : public CConfigBase
{
public:
	COption<vector<CString> > m_list;

	CNormalListFile()
		: CConfigBase("NormalList"),
		m_list("NormalList")
	{
		m_options.push_back(&m_list);
	}
};

// ����xml
BOOL CNormalListPage::Export(const CString& path)
{
	if (path.Right(4).CompareNoCase(_T(".xml")) != 0)
		return CListPage::Export(path);

	CNormalListFile tmp;
	ApplyList(tmp.m_list);
	return tmp.Save(path);
}

// ����xml
BOOL CNormalListPage::Import(const CString& path)
{
	if (path.Right(4).CompareNoCase(_T(".xml")) != 0)
		return CListPage::Import(path);

	CNormalListFile tmp;
	if (!tmp.Load(path))
		return FALSE;
	ShowList(tmp.m_list);
	return TRUE;
}

BOOL CNormalListPage::SetItem(int index)
{
	CString content = m_list.GetItemText(index, 0);
	CInputDlg dlg(m_inputTitle, content, NULL, FALSE, CInputDlg::IDD, this);
	if (dlg.DoModal() == IDOK && content != _T(""))
	{
		m_list.SetItemText(index, 0, content);
		return TRUE;
	}
	return FALSE;
}

void CNormalListPage::ShowList(const vector<CString>& list)
{
	m_list.DeleteAllItems();
	for (UINT i = 0; i < list.size(); i++)
		m_list.InsertItem(i, list[i]);
}

void CNormalListPage::ShowList(const set<CString>& list)
{
	m_list.DeleteAllItems();
	int i = 0;
	for (const CString& item : list)
		m_list.InsertItem(i++, item);
}

void CNormalListPage::ApplyList(vector<CString>& list)
{
	int size = m_list.GetItemCount();
	list.resize(size);
	for (int i = 0; i < size; i++)
		list[i] = m_list.GetItemText(i, 0);
}

void CNormalListPage::ApplyList(set<CString>& list)
{
	list.clear();
	CString tmp;
	int size = m_list.GetItemCount();
	for (int i = 0; i < size; i++)
	{
		tmp = m_list.GetItemText(i, 0);
		list.insert(tmp);
	}
}

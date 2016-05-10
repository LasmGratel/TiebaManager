// RegListPage.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "RegListPage.h"
#include <TBMConfig.h>
#include "InputDlg.h"
#include <StringHelper.h>


// CRegListPage �Ի���

IMPLEMENT_DYNAMIC(CRegListPage, CListTestPage)

CRegListPage::CRegListPage(const CString& inputTitle, CWnd* pParent /*=NULL*/)
	: CListTestPage(pParent), 
	m_inputTitle(inputTitle)
{
}

CRegListPage::~CRegListPage()
{
}

void CRegListPage::DoDataExchange(CDataExchange* pDX)
{
	CListTestPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRegListPage, CListTestPage)
END_MESSAGE_MAP()


// CRegListPage ��Ϣ�������


BOOL CRegListPage::OnInitDialog()
{
	CListTestPage::OnInitDialog();

	int i = 0;
	m_list.InsertColumn(i++, _T("����"), LVCFMT_LEFT, 500);
	m_list.InsertColumn(i++, _T("����"), LVCFMT_LEFT, 50);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}

class CRegListFile : public CConfigBase
{
public:
	COption<vector<RegexText> > m_list;

	CRegListFile()
		: CConfigBase("RegList"),
		m_list("RegList")
	{
		m_options.push_back(&m_list);
	}
};

// ����xml
BOOL CRegListPage::Export(const CString& path)
{
	if (path.Right(4).CompareNoCase(_T(".xml")) != 0)
		return CListPage::Export(path);

	CRegListFile tmp;
	ApplyList(tmp.m_list);
	return tmp.Save(path);
}

// ����xml
BOOL CRegListPage::Import(const CString& path)
{
	if (path.Right(4).CompareNoCase(_T(".xml")) != 0)
		return CListPage::Import(path);

	CRegListFile tmp;
	if (!tmp.Load(path))
		return FALSE;
	ShowList(tmp.m_list);
	return TRUE;
}

BOOL CRegListPage::SetItem(int index)
{
	CString content = m_list.GetItemText(index, 0);
	BOOL isRegex = m_list.GetItemText(index, 1) == IS_REGEX_TEXT;
	CInputDlg dlg(m_inputTitle, content, &isRegex, TRUE, CInputDlg::IDD, this);
	if (dlg.DoModal() == IDOK && content != _T(""))
	{
		if (CheckMatchTooMuch(content, isRegex))
		{
			AfxMessageBox(_T("ƥ��̫���ˣ�"), MB_ICONERROR);
			return FALSE;
		}
		m_list.SetItemText(index, 0, content);
		m_list.SetItemText(index, 1, isRegex ? IS_REGEX_TEXT : _T(""));
		return TRUE;
	}
	return FALSE;
}

BOOL CRegListPage::TestMatch(int index)
{
	CString test;
	m_testEdit.GetWindowText(test);
	BOOL isRegex = m_list.GetItemText(index, 1) == IS_REGEX_TEXT;
	CString text = m_list.GetItemText(index, 0);
	return TestMatch(test, text, isRegex);
}

void CRegListPage::ShowList(const vector<RegexText>& list)
{
	m_list.DeleteAllItems();
	for (UINT i = 0; i < list.size(); i++)
	{
		m_list.InsertItem(i, list[i].text);
		m_list.SetItemText(i, 1, list[i].isRegex ? IS_REGEX_TEXT : _T(""));
	}
}

void CRegListPage::ApplyList(vector<RegexText>& list)
{
	int size = m_list.GetItemCount();
	list.resize(size);
	for (int i = 0; i < size; i++)
		list[i].Set(m_list.GetItemText(i, 1) == IS_REGEX_TEXT, m_list.GetItemText(i, 0));
}

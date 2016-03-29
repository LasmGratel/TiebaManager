// KeywordsPage.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "KeywordsPage.h"
#include "StringHelper.h"
#include "SettingDlg.h"
#include "InputKeywordDlg.h"


// CKeywordsPage �Ի���

IMPLEMENT_DYNAMIC(CKeywordsPage, CRegListPage)

CKeywordsPage::CKeywordsPage(CWnd* pParent /*=NULL*/)
	: CRegListPage(_T("Υ�����ݣ�"), pParent)
{

}

CKeywordsPage::~CKeywordsPage()
{
}

void CKeywordsPage::DoDataExchange(CDataExchange* pDX)
{
	CRegListPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CKeywordsPage, CRegListPage)
END_MESSAGE_MAP()


// CKeywordsPage ��Ϣ�������


BOOL CKeywordsPage::OnInitDialog()
{
	CRegListPage::OnInitDialog();

	int i = 2;
	m_list.InsertColumn(i++, _T("ǿ��ȷ��"), LVCFMT_LEFT, 80);
	m_list.InsertColumn(i++, _T("��������"), LVCFMT_LEFT, 80);

	m_static.SetWindowText(_T("֧��ͼƬ��ַ\\��Ƶ��ַ���������ı������ı�ʱƥ��\r\n")
						   _T("ע��������ʽ .* ƥ�������ı����벻Ҫ��������ʹ�ã���"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}

void CKeywordsPage::ShowList(const vector<CPlan::Keyword>& list)
{
	m_list.DeleteAllItems();
	CString tmp;
	for (UINT i = 0; i < list.size(); i++)
	{
		m_list.InsertItem(i, list[i].text);
		m_list.SetItemText(i, 1, list[i].isRegex ? IS_REGEX_TEXT : _T(""));
		m_list.SetItemText(i, 2, list[i].forceToConfirm ? IS_REGEX_TEXT : _T(""));
		tmp.Format(_T("%d"), list[i].trigCount);
		m_list.SetItemText(i, 3, tmp);
	}
}

void CKeywordsPage::ApplyList(vector<CPlan::Keyword>& list)
{
	int size = m_list.GetItemCount();
	list.resize(size);
	for (int i = 0; i < size; i++)
	{
		list[i].Set(m_list.GetItemText(i, 1) == IS_REGEX_TEXT, m_list.GetItemText(i, 0));
		list[i].forceToConfirm = m_list.GetItemText(i, 2) == IS_REGEX_TEXT;
		list[i].trigCount = _ttoi(m_list.GetItemText(i, 3));
	}
}

class CKeywordListFile : public CConfigBase
{
public:
	COption<vector<CPlan::Keyword> > m_list;

	CKeywordListFile()
		: CConfigBase("RegList"),
		m_list("RegList")
	{
		m_options.push_back(&m_list);
	}
};

// ����
void CKeywordsPage::OnClickedButton4()
{
	CFileDialog dlg(FALSE, _T("xml"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("XML�ļ� (*.xml)|*.xml|�����ļ� (*.*)|*.*||"), this);
	if (dlg.DoModal() == IDOK)
	{
		CKeywordListFile tmp;
		ApplyList(tmp.m_list);
		if (!tmp.Save(dlg.GetPathName()))
			AfxMessageBox(_T("����ʧ�ܣ�"), MB_ICONERROR);
	}
}

// ����
void CKeywordsPage::OnClickedButton5()
{
	CFileDialog dlg(TRUE, _T("xml"), NULL, 0,
		_T("XML�ļ� (*.xml)|*.xml|�����ļ� (*.*)|*.*||"), this);
	if (dlg.DoModal() == IDOK)
	{
		CKeywordListFile tmp;
		if (!tmp.Load(dlg.GetPathName()))
			AfxMessageBox(_T("��ȡʧ�ܣ�"), MB_ICONERROR);
		else
		{
			ShowList(tmp.m_list);
			OnAdd(-1);
		}
	}
}

BOOL CKeywordsPage::SetItem(int index)
{
	CString content = m_list.GetItemText(index, 0);
	BOOL isRegex = m_list.GetItemText(index, 1) == IS_REGEX_TEXT;
	BOOL forceToConfirm = m_list.GetItemText(index, 2) == IS_REGEX_TEXT;
	CInputKeywordDlg dlg(m_inputTitle, content, &isRegex, &forceToConfirm, this);
	if (dlg.DoModal() == IDOK && content != _T(""))
	{
		if (CheckMatchTooMuch(content, isRegex))
		{
			AfxMessageBox(_T("ƥ��̫���ˣ�"), MB_ICONERROR);
			return FALSE;
		}
		m_list.SetItemText(index, 0, content);
		m_list.SetItemText(index, 1, isRegex ? IS_REGEX_TEXT : _T(""));
		m_list.SetItemText(index, 2, forceToConfirm ? IS_REGEX_TEXT : _T(""));
		m_list.SetItemText(index, 3, _T("0"));
		return TRUE;
	}
	return FALSE;
}

void CKeywordsPage::OnAdd(int index)
{
	((CSettingDlg*)GetParent()->GetParent())->m_clearScanCache = TRUE;
}

BOOL CKeywordsPage::CheckMatchTooMuch(const CString& text, BOOL isRegex)
{
	return StringIncludes(MATCH_TOO_MUCH_CONTENT_TEST1, text, isRegex)
		&& StringIncludes(MATCH_TOO_MUCH_CONTENT_TEST2, text, isRegex);
}

BOOL CKeywordsPage::TestMatch(const CString& test, const CString& text, BOOL isRegex)
{
	return StringIncludes(test, text, isRegex);
}

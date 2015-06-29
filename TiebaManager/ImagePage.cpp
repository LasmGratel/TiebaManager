// ImagePage.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TiebaManager.h"
#include "ImagePage.h"
#include "afxdialogex.h"
#include "ScanImage.h"


// CImagePage �Ի���

IMPLEMENT_DYNAMIC(CImagePage, CDialog)

CImagePage::CImagePage(CWnd* pParent /*=NULL*/)
	: CDialog(CImagePage::IDD, pParent)
{

}

#pragma region MFC
CImagePage::~CImagePage()
{
}

void CImagePage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_dirEdit);
	DDX_Control(pDX, IDC_BUTTON1, m_browseButton);
	DDX_Control(pDX, IDC_BUTTON2, m_calcFeatureButton);
}


BEGIN_MESSAGE_MAP(CImagePage, CDialog)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON1, &CImagePage::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CImagePage::OnBnClickedButton2)
END_MESSAGE_MAP()
#pragma endregion

// CImagePage ��Ϣ�������

#pragma region UI
// ����Esc�رմ���
void CImagePage::OnCancel()
{
}

// ���λس��رմ���
void CImagePage::OnOK()
{
}

// ���ٴ���
void CImagePage::OnClose()
{
	DestroyWindow();

	CDialog::OnClose();
}
#pragma endregion

// ���
void CImagePage::OnBnClickedButton1()
{
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(bi));
	bi.lpszTitle = _T("Υ��ͼƬĿ¼��");
	bi.ulFlags = BIF_STATUSTEXT;
	LPITEMIDLIST pidlSel = SHBrowseForFolder(&bi);

	if (pidlSel != NULL)
	{
		CString folder;
		SHGetPathFromIDList(pidlSel, folder.GetBuffer(MAX_PATH));
		folder.ReleaseBuffer();
		m_dirEdit.SetWindowText(folder);
	}
}

// ��������ֵ
void CImagePage::OnBnClickedButton2()
{
	vector<CString> imagePath;

	CString dir;
	m_dirEdit.GetWindowText(dir);
	if (dir == _T(""))
		return;
	CFileFind fileFind;
	static const TCHAR* IMG_EXT[] = { _T("\\*.jpg"), _T("\\*.png"), _T("\\*.jpeg"), _T("\\*.bmp") };
	for (int i = 0; i < _countof(IMG_EXT); i++)
	{
		BOOL flag = fileFind.FindFile(dir + IMG_EXT[i]);
		while (flag)
		{
			flag = fileFind.FindNextFile();
			imagePath.push_back(fileFind.GetFilePath());
		}
	}

	vector<ImageFeature> feature(imagePath.size());
	for (UINT i = 0; i < imagePath.size(); i++)
	{
		LPTSTR pos = StrRChr(imagePath[i], NULL, _T('\\'));
		feature[i].name = (pos == NULL ? imagePath[i] : pos + 1);
		CalculateFeature(imagePath[i], feature[i].feature);
	}
	WriteFeatures(dir + FEATURE_PATH, feature);

	CString msg;
	msg.Format(_T("�ѱ���%d��ͼƬ"), feature.size());
	AfxMessageBox(msg);
	((CSettingDlg*)GetParent()->GetParent())->m_clearScanCache = TRUE;
}

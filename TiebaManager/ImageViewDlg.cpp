// ImageViewDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ImageViewDlg.h"
#include "ScanImage.h"
#include "Global.h"


// CImageViewDlg �Ի���

IMPLEMENT_DYNAMIC(CImageViewDlg, CDialog)

// ���캯��
CImageViewDlg::CImageViewDlg(CImageViewDlg** pThis, CWnd* pParent /*=NULL*/)
	: CDialog(CImageViewDlg::IDD, pParent)
{
	m_pThis = pThis;
	m_imageURL = NULL;
	m_curImageIndex = 0;
}

#pragma region MFC
CImageViewDlg::~CImageViewDlg()
{
	if (m_imageURL != NULL)
		delete m_imageURL;
}

void CImageViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC1, m_imageStatic);
	DDX_Control(pDX, IDC_BUTTON1, m_prevButton);
	DDX_Control(pDX, IDC_BUTTON2, m_nextButton);
	DDX_Control(pDX, IDC_BUTTON3, m_saveButton);
}


BEGIN_MESSAGE_MAP(CImageViewDlg, CDialog)
	ON_WM_CLOSE()
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_BUTTON1, &CImageViewDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CImageViewDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CImageViewDlg::OnBnClickedButton3)
END_MESSAGE_MAP()
#pragma endregion

// CImageViewDlg ��Ϣ�������

#pragma region UI
// ����Esc�رմ���
void CImageViewDlg::OnCancel()
{
}

// ���λس��رմ���
void CImageViewDlg::OnOK()
{
}

// ���ٴ���
void CImageViewDlg::OnClose()
{
	DestroyWindow();
}

// �ͷ�this
void CImageViewDlg::PostNcDestroy()
{
	CDialog::PostNcDestroy();

	*m_pThis = NULL;
	delete this;
}

// ������С�ߴ�
void CImageViewDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = 240;
	lpMMI->ptMinTrackSize.y = 240;

	CDialog::OnGetMinMaxInfo(lpMMI);
}

// �ı�ߴ�
void CImageViewDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (m_imageStatic.m_hWnd == NULL)
		return;

	CRect rect;
	GetClientRect(&rect); // Ĭ��452 * 384
	m_imageStatic.SetWindowPos(NULL, 0, 0, rect.Width() - 21, rect.Height() - 58, SWP_NOMOVE | SWP_NOREDRAW);
	int y = rect.Height() - 35;
	m_prevButton.SetWindowPos(NULL, 11, y, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);
	m_nextButton.SetWindowPos(NULL, 74, y, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);
	m_saveButton.SetWindowPos(NULL, 137, y, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);

	Invalidate();
}
#pragma endregion

// ��ʼ��
BOOL CImageViewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetClassLong(m_imageStatic.m_hWnd, GCL_HCURSOR, (LONG)LoadCursor(NULL, IDC_ARROW));

	RECT rect;
	GetParent()->GetWindowRect(&rect);
	SetWindowPos(NULL, rect.right, rect.top, 0, 0, SWP_NOSIZE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}

// ����ͼƬ
void CImageViewDlg::SetImages(vector<CString>* imageURL)
{
	if (m_imageURL != NULL)
		delete m_imageURL;
	m_imageURL = imageURL;
	SetCurImage(0);
}

// ���õ�ǰͼƬ
void CImageViewDlg::SetCurImage(int index)
{
	m_curImageIndex = index;
	if (m_curImageIndex < 0 || (UINT)m_curImageIndex >= m_imageURL->size())
	{
		if (!m_curImage.IsNull())
			m_curImage.Destroy();
		SetWindowText(_T(""));
		m_imageStatic.Invalidate();
		return;
	}

	CString imgName = GetImageName((*m_imageURL)[m_curImageIndex]);
	if (PathFileExists(IMG_CACHE_PATH + imgName))
	{
		// ��ȡͼƬ����
		if (!m_curImage.IsNull())
			m_curImage.Destroy();
		m_curImage.Load(IMG_CACHE_PATH + imgName);
	}
	else
	{
		// ����ͼƬ
		BYTE* buffer;
		ULONG size;
		if (HTTPGetRaw((*m_imageURL)[m_curImageIndex], &buffer, &size) == NET_SUCCESS)
		{
			ReadImage(buffer, size, m_curImage);

			if (!PathFileExists(IMG_CACHE_PATH))
				CreateDirectory(IMG_CACHE_PATH, NULL);
			CFile file;
			if (file.Open(IMG_CACHE_PATH + imgName, CFile::modeCreate | CFile::modeWrite))
				file.Write(buffer, size);
			delete buffer;
		}
	}

	SetWindowText(imgName);
	m_imageStatic.Invalidate();
}

// ��ͼƬ
void CImageViewDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (nIDCtl == m_imageStatic.GetDlgCtrlID())
	{
		FillRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, (HBRUSH)GetStockObject(WHITE_BRUSH));
		if (!m_curImage.IsNull())
		{
			SetStretchBltMode(lpDrawItemStruct->hDC, HALFTONE);
			float ratio1 = (float)(lpDrawItemStruct->rcItem.right - lpDrawItemStruct->rcItem.left) / m_curImage.GetWidth();
			float ratio2 = (float)(lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top) / m_curImage.GetHeight();
			float ratio = ratio1 < ratio2 ? ratio1 : ratio2;
			m_curImage.Draw(lpDrawItemStruct->hDC, 0, 0, (int)(m_curImage.GetWidth() * ratio), (int)(m_curImage.GetHeight() * ratio));
		}
		FrameRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, (HBRUSH)GetStockObject(BLACK_BRUSH));
		return;
	}

	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

// ��һ��
void CImageViewDlg::OnBnClickedButton1()
{
	int index = m_curImageIndex - 1;
	if (index < 0)
		index = m_imageURL->size() - 1;
	SetCurImage(index);
}

// ��һ��
void CImageViewDlg::OnBnClickedButton2()
{
	int index = m_curImageIndex + 1;
	if ((UINT)index >= m_imageURL->size())
		index = 0;
	SetCurImage(index);
}

// ����
void CImageViewDlg::OnBnClickedButton3()
{
	if (m_curImage.IsNull())
		return;

	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(bi));
	bi.lpszTitle = _T("����Ŀ¼��");
	bi.ulFlags = BIF_STATUSTEXT;
	LPITEMIDLIST pidlSel = SHBrowseForFolder(&bi);

	if (pidlSel != NULL)
	{
		CString folder;
		SHGetPathFromIDList(pidlSel, folder.GetBuffer(MAX_PATH));
		folder.ReleaseBuffer();
		
		CString imgName = GetImageName((*m_imageURL)[m_curImageIndex]);
		m_curImage.Save(folder + _T("\\") + imgName);
	}
}

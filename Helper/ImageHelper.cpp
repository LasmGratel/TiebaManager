#include "stdafx.h"
#include <ImageHelper.h>
#include <opencv2\imgcodecs.hpp>


static BOOL CImageToMat(const CImage& image, Mat& img)
{
	img.create(image.GetHeight(), image.GetWidth(), CV_8UC3);
	if (img.data == NULL)
		return FALSE;

	// ֧��24λ��32λͼ
	int bpp = image.GetBPP() / 8;
	if (bpp < 3)
		return FALSE;
	for (int y = 0; y < image.GetHeight(); y++)
	{
		BYTE* src = (BYTE*)image.GetPixelAddress(0, y);
		BYTE* dst = img.ptr<BYTE>(y);
		for (int x = 0; x < image.GetWidth(); x++)
		{
			dst[0] = src[0];
			dst[1] = src[1];
			dst[2] = src[2];
			src += bpp;
			dst += 3;
		}
	}
	return TRUE;
}

// ���ļ�����ͼƬ
HELPER_API BOOL ReadImage(const CString& path, Mat& img)
{
	img = cv::imread((LPCSTR)(CStringA)path);
	if (img.data != NULL)
		return TRUE;
	
	// ����OpenCV��֧�ֵĸ�ʽ(GIF)
	CImage image;
	image.Load(path);
	if (image.IsNull())
		return FALSE;

	return CImageToMat(image, img);
}

// ���ڴ����ͼƬ
HELPER_API BOOL ReadImage(const BYTE* buffer, ULONG size, CImage& img)
{
	// ������
	HGLOBAL m_hMem = GlobalAlloc(GMEM_FIXED, size);
	if (m_hMem == NULL)
		return FALSE;
	BYTE* pmem = (BYTE*)GlobalLock(m_hMem);
	if (pmem == NULL)
		return FALSE;
	memcpy(pmem, buffer, size);
	IStream* pstm = NULL;
	if (FAILED(CreateStreamOnHGlobal(m_hMem, FALSE, &pstm)) || pstm == NULL)
		return FALSE;

	// ���ص�CImage
	if (!img.IsNull())
		img.Destroy();
	img.Load(pstm);
	
	// �ͷ���
	GlobalUnlock(m_hMem);
	pstm->Release();
	return !img.IsNull();
}

// ���ڴ����ͼƬ
HELPER_API BOOL ReadImage(const BYTE* buffer, ULONG size, Mat& img)
{
	try
	{
		vector<BYTE> _imgBuffer(buffer, buffer + size);
		img = cv::imdecode(Mat(_imgBuffer), cv::IMREAD_COLOR);
	}
	catch (...)
	{
	}
	if (img.data != NULL)
		return TRUE;

	// ����OpenCV��֧�ֵĸ�ʽ(GIF)
	CImage image;
	if (!ReadImage(buffer, size, image))
		return FALSE;
	return CImageToMat(image, img);
}

// ��ͼƬ��ַȡͼƬ��
HELPER_API CString GetImageName(const CString& img)
{
	LPTSTR pos = StrRChr(img, NULL, _T('/'));
	CString imgName = (pos == NULL ? img : pos + 1);
	int right = imgName.Find(_T("?"));
	if (right != -1)
		imgName = imgName.Left(right);
	if (imgName.Right(4).CompareNoCase(_T(".jpg")) != 0)
		imgName += _T(".jpg");
	return imgName;
}

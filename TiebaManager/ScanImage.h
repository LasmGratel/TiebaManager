#pragma once
#include <zlib.h>
#include <vector>
using std::vector;
#include <opencv2\core\mat.hpp>
using cv::Mat;
#include "Tieba.h"
#include "Global.h"


const int HUE_BIN = 36, SATURATION_BIN = 32;
const CString FEATURE_PATH = _T("\\feature.tb");
const CString IMG_CACHE_PATH = _T("ImageCache\\");


struct ImageFeature
{
	CString name;
	Mat feature;
};


void ReadFeature(const gzFile& f, ImageFeature& feature);
void ReadFeatures(const TCHAR* path, vector<ImageFeature>& features);
void WriteFeature(const gzFile& f, const ImageFeature& feature);
void WriteFeatures(const TCHAR* path, const vector<ImageFeature>& features);

void CalculateFeature(const BYTE* imgBuffer, ULONG imgSize, Mat& feature);
void CalculateFeature(const CString& imgPath, Mat& feature);
void CalculateFeature(const Mat& img, Mat& feature);
double CompareFeature(const Mat& feature1, const Mat& feature2);

void GetImage(const ThreadInfo& thread, vector<CString>& img);
void GetImage(const PostInfo& post, vector<CString>& img);

template<typename InfoType>
BOOL CheckImageIllegal(InfoType content, CString& msg)
{
	if (g_imageFeatures.empty())
		return FALSE;

	g_optionsLock.Lock();
	// �����û�
	for (const CString& whiteList : g_whiteList)
		if (content.author == whiteList)
		{
			g_optionsLock.Unlock();
			return FALSE;
		}
	g_optionsLock.Unlock();

	vector<CString> imgs;
	GetImage(content, imgs);
	for (const CString& img : imgs)
	{
		LPTSTR pos = StrRChr(img, NULL, _T('/'));
		CString imgName = (pos == NULL ? img : pos + 1);

		// ȡͼƬ����ֵ
		ImageFeature feature;
		if (PathFileExists(IMG_CACHE_PATH + imgName + _T(".tb")))
		{
			// ��ȡ����ֵ����
			gzFile f = gzopen_w(IMG_CACHE_PATH + imgName + _T(".tb"), "rb");
			if (f != NULL)
			{
				ReadFeature(f, feature);
				gzclose(f);
			}
		}
		/*else if (PathFileExists(IMG_CACHE_PATH + imgName))
		{
			// ��ȡͼƬ���棬��������ֵ
			feature.name = imgName;
			CalculateFeature(IMG_CACHE_PATH + imgName, feature.feature);
			if (feature.feature.data == NULL)
				continue;

			gzFile f = gzopen_w(IMG_CACHE_PATH + imgName + _T(".tb"), "wb");
			if (f != NULL)
			{
				WriteFeature(f, feature);
				gzclose(f);
			}
		}*/
		else
		{
			// ����ͼƬ����������ֵ
			BYTE* buffer;
			ULONG size;
			if (HTTPGetRaw(img, &buffer, &size) == NET_SUCCESS)
			{
				feature.name = imgName;
				CalculateFeature(buffer, size, feature.feature);
				if (feature.feature.data == NULL)
				{
					delete buffer;
					continue;
				}

				if (!PathFileExists(IMG_CACHE_PATH))
					CreateDirectory(IMG_CACHE_PATH, NULL);
				gzFile f = gzopen_w(IMG_CACHE_PATH + imgName + _T(".tb"), "wb");
				if (f != NULL)
				{
					WriteFeature(f, feature);
					gzclose(f);
				}
				/*CFile file;
				if (file.Open(IMG_CACHE_PATH + imgName, CFile::modeCreate | CFile::modeWrite))
					file.Write(buffer, size);*/
				delete buffer;
			}
		}

		if (feature.feature.data == NULL)
			continue;
		// �жϺ�Υ��ͼƬ����ֵ���С����ֵ
		g_optionsLock.Lock();
		for (const ImageFeature i : g_imageFeatures)
			if (CompareFeature(feature.feature, i.feature) < 0.25)
			{
				msg = _T("<font color=red> ����Υ��ͼƬ </font>" + i.name);
				g_optionsLock.Unlock();
				return TRUE;
			}
		g_optionsLock.Unlock();
	}

	return FALSE;
}

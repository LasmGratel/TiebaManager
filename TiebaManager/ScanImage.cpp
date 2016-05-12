#include "stdafx.h"
#include "ScanImage.h"
#include <ImageHelper.h>
#include <TBMConfig.h>
#include <opencv2\imgproc\imgproc.hpp>
#include <NetworkHelper.h>
#include <MiscHelper.h>


set<CString> g_leagalImage; // �Ѽ�鲻Υ���ͼƬ
set<CString> g_illegalImage; // �Ѽ��Υ���ͼƬ


// 1��ͼƬ��ַ
static const wregex THREAD_IMG_REG(_T("<img .*?bpic=\"(.*?)\".*?/>"));
// 2��ͼƬ��ַ
static const wregex POST_IMG_REG(_T("<img .*?class=\"(BDE_Image|j_user_sign)\".*?src=\"(.*?)\".*?>"));


// ��Ŀ¼��ȡͼƬ��g_images
void ReadImages(const CString& dir)
{
	vector<CString> imagePath;

	if (dir == _T(""))
	{
		g_plan.m_images.clear();
		return;
	}
	CFileFind fileFind;
	static const TCHAR* IMG_EXT[] = { _T("\\*.jpg"), _T("\\*.png"), _T("\\*.gif"), _T("\\*.jpeg"), _T("\\*.bmp") };
	for (int i = 0; i < _countof(IMG_EXT); i++)
	{
		BOOL flag = fileFind.FindFile(dir + IMG_EXT[i]);
		while (flag)
		{
			flag = fileFind.FindNextFile();
			imagePath.push_back(fileFind.GetFilePath());
		}
	}

	g_plan.m_images.resize(imagePath.size());
	UINT imgCount = 0;
	for (CString& i : imagePath)
	{
		g_plan.m_images[imgCount].name = GetImageName(i);
		if (ReadImage(i, g_plan.m_images[imgCount].img))
			imgCount++;
	}
	if (imagePath.size() != imgCount)
	{
		g_plan.m_images.resize(imgCount);
		CString msg;
		msg.Format(_T("%u��ͼƬ����ʧ�ܣ�"), imagePath.size() - imgCount);
		AfxMessageBox(msg, MB_ICONINFORMATION);
	}
}


// ������Ԥ��ȡͼƬ��ַ
void GetThreadImage::GetImage(vector<CString>& img)
{
	for (std::regex_iterator<LPCTSTR> it((LPCTSTR)m_preview, (LPCTSTR)m_preview + m_preview.GetLength(), THREAD_IMG_REG), 
		end; it != end; ++it)
		img.push_back((*it)[1].str().c_str());
}

// ������ȡͼƬ��ַ
void GetPostImage::GetImage(vector<CString>& img)
{
	for (std::regex_iterator<LPCTSTR> it((LPCTSTR)m_content, (LPCTSTR)m_content + m_content.GetLength(), POST_IMG_REG), 
		end; it != end; ++it)
		img.push_back((*it)[2].str().c_str());
	if (m_portrait != _T(""))
		img.push_back(_T("http://tb.himg.baidu.com/sys/portrait/item/") + m_portrait);
}


// ���ͼƬΥ��1����������û�����ȡͼƬ��ַ
BOOL CheckImageIllegal(const CString& author, GetImagesBase& getImage, CString& msg)
{
	if (g_plan.m_images.empty())
		return FALSE;

	// �����û�
	g_plan.m_optionsLock.Lock();
	if (g_plan.m_whiteList->find(author) != g_plan.m_whiteList->end())
	{
		g_plan.m_optionsLock.Unlock();
		return FALSE;
	}
	g_plan.m_optionsLock.Unlock();

	vector<CString> imgs;
	getImage.GetImage(imgs);
	return DoCheckImageIllegal(imgs, msg);
}

// SSIM�㷨�Ƚ�ͼƬ
static double getMSSIM(const Mat& i1, const Mat& i2)
{
	static const double C1 = 6.5025, C2 = 58.5225;
	static const int d = CV_32F;
	try
	{
		Mat I1, I2;
		i1.convertTo(I1, d);           // �����ڵ��ֽ������Ͻ��м��㣬��Χ������
		i2.convertTo(I2, d);

		if (I1.rows * I1.cols < I2.rows * I2.cols)
			cv::resize(I2, I2, I1.size());
		else
			cv::resize(I1, I1, I2.size());

		Mat I2_2 = I2.mul(I2);        // I2^2
		Mat I1_2 = I1.mul(I1);        // I1^2
		Mat I1_I2 = I1.mul(I2);        // I1 * I2

		///////////////////////// �������� ///////////////////////////////

		Mat mu1, mu2;
		GaussianBlur(I1, mu1, cv::Size(11, 11), 1.5);
		I1.release();
		GaussianBlur(I2, mu2, cv::Size(11, 11), 1.5);
		I2.release();

		Mat mu1_2 = mu1.mul(mu1);
		Mat mu2_2 = mu2.mul(mu2);
		Mat mu1_mu2 = mu1.mul(mu2);
		mu1.release();
		mu2.release();

		Mat sigma1_2, sigma2_2, sigma12;

		GaussianBlur(I1_2, sigma1_2, cv::Size(11, 11), 1.5);
		I1_2.release();
		sigma1_2 -= mu1_2;

		GaussianBlur(I2_2, sigma2_2, cv::Size(11, 11), 1.5);
		I2_2.release();
		sigma2_2 -= mu2_2;

		GaussianBlur(I1_I2, sigma12, cv::Size(11, 11), 1.5);
		I1_I2.release();
		sigma12 -= mu1_mu2;

		///////////////////////////////// ��ʽ ////////////////////////////////
		Mat t1, t2, t3;

		t1 = 2 * mu1_mu2 + C1;
		t2 = 2 * sigma12 + C2;
		t3 = t1.mul(t2);              // t3 = ((2*mu1_mu2 + C1).*(2*sigma12 + C2))

		t1 = mu1_2 + mu2_2 + C1;
		t2 = sigma1_2 + sigma2_2 + C2;
		t1 = t1.mul(t2);               // t1 =((mu1_2 + mu2_2 + C1).*(sigma1_2 + sigma2_2 + C2))
		t2.release();

		Mat ssim_map;
		divide(t3, t1, ssim_map);      // ssim_map =  t3./t1;
		t1.release();
		t3.release();

		cv::Scalar mssim = mean(ssim_map); // mssim = ssim_map��ƽ��ֵ
		return mssim.val[0] + mssim.val[1] + mssim.val[2];
	}
	catch (...) // GaussianBlur����Matʱ�����׳��쳣-215��
	{
		return 0;
	}
}

// ���ͼƬΥ��2������ͼƬ���Ƚ�ͼƬ
BOOL DoCheckImageIllegal(vector<CString>& imgs, CString& msg)
{
	for (const CString& img : imgs)
	{
		CString imgName = GetImageName(img);

		// ��黺����
		if (g_leagalImage.find(imgName) != g_leagalImage.end())
			continue;
		if (g_illegalImage.find(imgName) != g_illegalImage.end())
			return TRUE;

		// ��ȡͼƬ
		Mat image;
		if (PathFileExists(IMG_CACHE_PATH + imgName))
		{
			// ��ȡͼƬ����
			ReadImage(IMG_CACHE_PATH + imgName, image);
		}
		else
		{
			// ����ͼƬ
			unique_ptr<BYTE[]> buffer;
			ULONG size;
			if (HTTPGetRaw(img, &buffer, &size) == NET_SUCCESS)
			{
				ReadImage(buffer.get(), size, image);

				CreateDir(IMG_CACHE_PATH);
				CFile file;
				if (file.Open(IMG_CACHE_PATH + imgName, CFile::modeCreate | CFile::modeWrite))
					file.Write(buffer.get(), size);
			}
		}

		if (image.data == NULL || image.cols < 30 || image.rows < 30) // �ߴ�̫С���Ƚ�
			continue;
		// �жϺ�Υ��ͼƬ�Ƚϴ�����ֵ
		g_plan.m_optionsLock.Lock();
		for (const auto& i : g_plan.m_images)
		{
			if (i.img.cols < 30 || i.img.rows < 30) // �ߴ�̫С���Ƚ�
				continue;
			double mssim = getMSSIM(image, i.img);
			if (mssim > g_plan.m_SSIMThreshold)
			{
				msg.Format(_T("<font color=red> ����Υ��ͼƬ </font>%s<font color=red> ���ƶ�%.3lf</font>"),
					(LPCTSTR)i.name, mssim);
				g_illegalImage.insert(imgName);
				g_plan.m_optionsLock.Unlock();
				return TRUE;
			}
		}
		g_leagalImage.insert(imgName);
		g_plan.m_optionsLock.Unlock();
	}

	return FALSE;
}

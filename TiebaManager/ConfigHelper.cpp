#include "stdafx.h"
#include "ConfigHelper.h"
#include <MiscHelper.h>
#include <TBMConfig.h>
#include "TiebaVariable.h"
#include "ScanImage.h"
#include "TiebaManagerDlg.h"


// ���浱ǰ�˺�����
void SaveCurrentUserConfig()
{
	// ����Ŀ¼
	CreateDir(USERS_PATH + g_globalConfig.m_currentUser);

	// ��ǰ�˺�����
	g_userConfig.Save(USER_PROFILE_PATH);

	// Cookie
	g_cookieConfig.Save(COOKIE_PATH);

	// ��ʷ�ظ�������ID��
	g_userCache.Save(CACHE_PATH);
}

// ���õ�ǰ�˺�
void SetCurrentUser(const CString& userName, BOOL save)
{
	// ���浱ǰ�˺�����
	if (save)
		SaveCurrentUserConfig();

	// ��������·��
	*g_globalConfig.m_currentUser = userName;
	CURRENT_USER_PATH = USERS_PATH + userName;
	USER_PROFILE_PATH = CURRENT_USER_PATH + _T("\\options.xml");
	COOKIE_PATH = CURRENT_USER_PATH + _T("\\ck.xml");
	CACHE_PATH = CURRENT_USER_PATH + _T("\\cache.xml");

	// ��ȡ����
	g_userConfig.Load(USER_PROFILE_PATH);
	// ����
	g_plan.Load(OPTIONS_PATH + g_userConfig.m_plan + _T(".xml"));
	ReadImages(g_plan.m_imageDir);
	// ������
	((CTiebaManagerDlg*)AfxGetApp()->m_pMainWnd)->m_forumNameEdit.SetWindowText(*g_userConfig.m_forumName);

	// Cookie
	g_cookieConfig.Load(COOKIE_PATH);

	// ��ʷ�ظ�������ID��
	g_userCache.Load(CACHE_PATH);
}

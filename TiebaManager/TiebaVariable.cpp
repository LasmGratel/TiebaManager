#include "stdafx.h"
#include "TiebaVariable.h"
#include "TiebaOperate.h"
#include "StringHelper.h"


// ���ɡ��û���Ϣ
CUserTiebaInfo g_userTiebaInfo;

// ɨ��
volatile BOOL g_stopScanFlag = FALSE;
CWinThread* g_scanThread = NULL;
CUserCache g_userCache;

// ����
CWinThread* g_confirmThread = NULL;
CWinThread* g_operateThread = NULL;


void CUserTiebaInfo::PostChange()
{
	m_bduss = GetStringBetween(m_cookie, _T("BDUSS="), _T(";"));
}

#include "stdafx.h"
#include "TiebaVariable.h"
#include "TBMOperate.h"
#include <StringHelper.h>


// Cookie�ļ�
CCookieConfig g_cookieConfig;

// ɨ��
volatile BOOL g_stopScanFlag = FALSE;
CWinThread* g_scanThread = NULL;
CUserCache g_userCache;

// ����
CWinThread* g_confirmThread = NULL;
CWinThread* g_operateThread = NULL;

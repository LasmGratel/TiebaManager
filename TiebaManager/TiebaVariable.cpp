#include "stdafx.h"
#include "TiebaVariable.h"
#include "TiebaOperate.h"


// ���ɡ��û���Ϣ
CUserTiebaInfo g_userTiebaInfo;

// ɨ��
volatile BOOL g_stopScanFlag = FALSE;
CWinThread* g_scanThread = NULL;
CUserCache g_userCache;

// ����
CWinThread* g_confirmThread = NULL;
CWinThread* g_operateThread = NULL;

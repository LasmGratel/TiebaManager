#include "stdafx.h"
#include "TiebaVariable.h"
#include "TiebaOperate.h"


// ���ɡ��û���Ϣ
CString g_forumName;
CString g_encodedForumName;
CString g_forumID;
CString g_cookie;
CString g_tbs;

// ɨ��
volatile BOOL g_stopScanFlag = FALSE;
CWinThread* g_scanThread = NULL;
CUserCache g_userCache;

// ����
CWinThread* g_confirmThread = NULL;
CWinThread* g_operateThread = NULL;


// TiebaManager.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "TiebaManager.h"
#include "TiebaManagerDlg.h"
#include "MiscHelper.h"
#include "Setting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTiebaManagerApp

BEGIN_MESSAGE_MAP(CTiebaManagerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CTiebaManagerApp ����

CTiebaManagerApp::CTiebaManagerApp()
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO:  �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CTiebaManagerApp ����

CTiebaManagerApp theApp;


// CTiebaManagerApp ��ʼ��

BOOL CTiebaManagerApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()��  ���򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();


	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO:  Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	//SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));


	// ����쳣����
	SetUnhandledExceptionFilter(ExceptionHandler);

	// ���ػ����
	setlocale(LC_CTYPE, "chs");

	// ��ʼ�������ļ�·��
	TCHAR cd[MAX_PATH];
	GetCurrentDirectory(_countof(cd), cd);
	GLOBAL_CONFIG_PATH = cd + GLOBAL_CONFIG_PATH;
	USERS_PATH = cd + USERS_PATH;

	// ����������
	CTiebaManagerDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();


	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}


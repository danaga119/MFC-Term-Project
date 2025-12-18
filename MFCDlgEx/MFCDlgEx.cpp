
#include "pch.h"
#include "framework.h"
#include "MFCDlgEx.h"
#include "MFCDlgExDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CMFCDlgExApp, CWinApp)
    ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

CMFCDlgExApp theApp;

CMFCDlgExApp::CMFCDlgExApp()
{
}

BOOL CMFCDlgExApp::InitInstance()
{
    CWinApp::InitInstance();


    
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    AfxEnableControlContainer();

    
    CMFCDlgExDlg dlg;
    m_pMainWnd = &dlg;

    INT_PTR nResponse = dlg.DoModal();
    if (nResponse == IDOK)
    {
        
    }
    else if (nResponse == IDCANCEL)
    {
       
    }
    else if (nResponse == -1)
    {

        TRACE("경고: 다이얼로그(IDD_MFCDLGEX_DIALOG) 생성 실패, 애플리케이션을 종료합니다.\n");
    }


    return FALSE;
}

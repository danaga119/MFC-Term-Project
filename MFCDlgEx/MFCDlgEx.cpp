// MFCDlgEx.cpp

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


    // 공용 컨트롤
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    AfxEnableControlContainer();

    // 메인 다이얼로그 생성
    CMFCDlgExDlg dlg;
    m_pMainWnd = &dlg;

    INT_PTR nResponse = dlg.DoModal();
    if (nResponse == IDOK)
    {
        // TODO: OK 눌렀을 때 UpdateSheetInfo
    }
    else if (nResponse == IDCANCEL)
    {
        // TODO: Cancel 눌렀을 때 UpdateSheetInfo
    }
    else if (nResponse == -1)
    {
        // 다이얼로그 생성 실패 시 Output에 경고 남기기
        TRACE("경고: 다이얼로그(IDD_MFCDLGEX_DIALOG) 생성 실패, 애플리케이션을 종료합니다.\n");
    }

    // 대화상자가 닫히면 앱 종료
    return FALSE;
}

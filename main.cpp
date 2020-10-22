// XSPlayer.cpp : 定义应用程序的入口点。
//

#include "framework.h"

#include "Core/Application.h"
#include "UI/MainFrame.h"
#include "Utils.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow) {
//    AllocConsole();
//     FILE* Journal = NULL;
//     freopen_s(&Journal, "CONOUT$", "a+", stdout);

    DuiLib::CPaintManagerUI::SetInstance(hInstance);

    XSPlayer::String strPath = XSPlayer::Utils::GetAppPath();
    DuiLib::CPaintManagerUI::SetResourcePath(strPath.c_str());
 //   DuiLib::CPaintManagerUI::SetResourceZip(_T("Data.zip"));

    XSPlayer::MainFrame mainFrmae;
    mainFrmae.Init();
    mainFrmae.Create(NULL, _T("XSPlayer"), UI_WNDSTYLE_FRAME, WS_EX_APPWINDOW, 0, 0, 800, 480);
    mainFrmae.CenterWindow();
    ShowWindow(mainFrmae, SW_SHOW);
    
    DuiLib::CPaintManagerUI::MessageLoop();

    return 0;
}

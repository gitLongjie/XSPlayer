#include "UI/MainFrame.h"
#include "UI/OfflineUITab.h"
#include "UI/OnlineUITab.h"
#include "UI/RightPannel.h"
#include "UI/MediaList.h"

#include "Decode/DecodeHandleChain.h"
#include "Render/AudioRenderChain.h"
#include "Decode/FFmpegDecodeTask.h"
#include "IO/IOHandleChain.h"
#include "IO/SqliteHelper.h"
#include "Script/MediaSource9Ku.h"
#include "Utils.h"

#include "resource.h"

namespace XSPlayer {

    MainFrame::MainFrame() : supper() {
        MediaManager::GetSingleton().RegistEvent(this);
    }

    MainFrame::~MainFrame() {
        m_trayIcon.hIcon = NULL;
        Shell_NotifyIcon(NIM_DELETE, &m_trayIcon);
        MediaManager::GetSingleton().UnregistEvent(this);
    }

    void MainFrame::InitWindow() {
        if (nullptr != m_pOfflineUITab) {
            m_pOfflineUITab->InitWindow();
        }

        if (nullptr != m_pOnlineUITab) {
            m_pOnlineUITab->InitWindow();
        }

        if (nullptr != m_pRightPannel) {
            m_pRightPannel->InitWindow();
        }

        AddTrayIcon();
        OnInitMediaManager();
    }

    void MainFrame::OnFinalMessage(HWND hWnd) {
        WindowImplBase::OnFinalMessage(hWnd);
        PostQuitMessage(0);
    }

    LRESULT MainFrame::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
        LRESULT hr = 0;

        HAND_WINDOW_MESSAGE(m_pOfflineUITab);
        HAND_WINDOW_MESSAGE(m_pOnlineUITab);
        HAND_WINDOW_MESSAGE(m_pRightPannel);

        BOOL bret = false;
        switch (uMsg) {
        case WM_SHOWTASK:
        {
            OnTrayIcon(uMsg, wParam, lParam, bret);
        }
        break;

        case WM_CHANGE_CUR_PLAY:
        {
            String curPlayTitle = MediaManager::GetSingleton().GetMediaName(static_cast<size_t>(lParam));
            m_curPlayTitle = String(_T("正在播放:")) + curPlayTitle;
            DuiLib::CLabelUI* pTitle = dynamic_cast<DuiLib::CLabelUI*>(m_PaintManager.FindControl(_T("lbCurPlay")));
            if (nullptr != pTitle) {
                pTitle->SetText(m_curPlayTitle.c_str());
            }
        }break;

        case WM_CHANGE_STOP_PLAY:
        {
            m_curPlayTitle = String(_T("正在播放:"));
            DuiLib::CLabelUI* pTitle = dynamic_cast<DuiLib::CLabelUI*>(m_PaintManager.FindControl(_T("lbCurPlay")));
            if (nullptr != pTitle) {
                pTitle->SetText(m_curPlayTitle.c_str());
            }
        }break;

        default:
            break;
        }
        
        LRESULT lRes = 0;
        if (m_PaintManager.MessageHandler(uMsg, wParam, lParam, lRes)) return lRes;
        return __super::HandleMessage(uMsg, wParam, lParam);
    }

    void MainFrame::Notify(DuiLib::TNotifyUI& msg) {
        if (_tcsicmp(msg.sType, kClick) == 0)
        {
            if (_tcsicmp(msg.pSender->GetName(), kCloseButtonControlName) == 0)
            {
                OnExit(msg);
            }
            else if (_tcsicmp(msg.pSender->GetName(), _T("btn_test")) == 0) {
                DuiLib::CSliderUI* pSliderUI = static_cast<DuiLib::CSliderUI*>(m_PaintManager.FindControl(_T("player_slider")));
                if (nullptr == pSliderUI) {
                    return;
                }

                //         pSliderUI->SetMinValue(0);
                //         pSliderUI->SetMaxValue(nDuration);
                pSliderUI->SetValue(10);
            }
        }
        else if (_tcsicmp(msg.sType, kSelectChanged) == 0) {
            OnSelectChanged(msg);
        }
    }

    DuiLib::CControlUI* MainFrame::CreateControl(LPCTSTR pstrClass) {
        if (0 == _tcsicmp(pstrClass, kOfflineUI)) {
            m_pOfflineUITab = new OfflineUITab(&m_PaintManager);
            DuiLib::INotifyUI* pOfflineUINotify = dynamic_cast<DuiLib::INotifyUI*>(m_pOfflineUITab);
            if (nullptr != pOfflineUINotify) {
                m_PaintManager.AddNotifier(pOfflineUINotify);
            }
            return m_pOfflineUITab;
        }
        else if (0 == _tcsicmp(pstrClass, kOnlineUI)) {
            m_pOnlineUITab = new OnlineUITab(&m_PaintManager);
            DuiLib::INotifyUI* pUINotify = dynamic_cast<DuiLib::INotifyUI*>(m_pOnlineUITab);
            if (nullptr != m_pOnlineUITab) {
                m_PaintManager.AddNotifier(m_pOnlineUITab);
            }
            return m_pOnlineUITab;
        }
        else if (0 == _tcsicmp(pstrClass, kRightPannelUI)) {
            m_pRightPannel = new RightPannel;
            DuiLib::INotifyUI* pRightNotify = dynamic_cast<DuiLib::INotifyUI*>(m_pRightPannel);
            if (nullptr != pRightNotify) {
                m_PaintManager.AddNotifier(pRightNotify);
            }
            return m_pRightPannel;
        }
        
        return nullptr;
    }

    bool MainFrame::OnNotify(const EventPtr& pEvent) {
        auto pControlEvent = std::dynamic_pointer_cast<ControlEvent>(pEvent);
        if (nullptr == pControlEvent) {
            return false;
        }

        if (ControlEvent::EControl::EC_PLAY == pControlEvent->GetEC()) {
            Media* pMedia = pControlEvent->GetMedia();
            if (nullptr == pMedia) {
                return false;
            }

            PostMessage(WM_CHANGE_CUR_PLAY, 0, pMedia->GetMediaId());
            return true;
        }
        

        return false;
    }

    void MainFrame::Init(void) {
        AudioRenderChainPtr pAudioRender = std::make_shared<AudioRenderChain>(nullptr, ThreadType::TT_UNKNOWN);
        DecodeHandleChainPtr pDecodeHandle = std::make_shared<DecodeHandleChain>(pAudioRender, ThreadType::TT_DECODE);
        pAudioRender->SetLastChain(pDecodeHandle);
        IOHandleChainPtr pIOHandle = std::make_shared<IOHandleChain>(pDecodeHandle, ThreadType::TT_IO);
        pDecodeHandle->SetLastChain(pIOHandle);
        MediaManager::GetSingleton().AddHandleChain(pIOHandle);

        MediaManager::GetSingleton().SetInfoCallback([this](const MediaInfo& info) {
            if (1 == info.nType) {
                PostMessage(WM_RIGHTPANNEL_INIT_DURATION, info.nDuration, 0);
            }
            else if (2 == info.nType) {
                PostMessage(WM_RIGHTPANNEL_UPDATE_DURATION, info.nDuration, 0);
            }
        });
    }

    UINT MainFrame::GetClassStyle() const {
        return CS_DBLCLKS;
    }

    DuiLib::UILIB_RESOURCETYPE MainFrame::GetResourceType() const {
        return DuiLib::UILIB_FILE;
    }

    DuiLib::CDuiString MainFrame::GetSkinFolder() {
        return _T("Data/skin/chinesestyle");
    }

    DuiLib::CDuiString MainFrame::GetSkinFile() {
        DuiLib::CDuiString skinFile = GetSkinFolder() + _T("/ui.xml");
        return skinFile;
    }

    LPCTSTR MainFrame::GetWindowClassName(void) const {
        return _T("XSPlayer");
    }

    void MainFrame::OnExit(DuiLib::TNotifyUI& msg) {
        MediaManager::GetSingleton().Stop();
        m_trayIcon.hIcon = NULL;
        Shell_NotifyIcon(NIM_DELETE, &m_trayIcon);        
        ::PostQuitMessage(0);
 //       ShowWindow(SW_HIDE);
    }

    void MainFrame::OnSelectChanged(DuiLib::TNotifyUI& msg) {
        DuiLib::CDuiString name = msg.pSender->GetName();
        DuiLib::CTabLayoutUI* pControl = static_cast<DuiLib::CTabLayoutUI*>(m_PaintManager.FindControl(_T("switch")));
        if (name == _T("offline"))
            pControl->SelectItem(0);
        else if (name == _T("online"))
            pControl->SelectItem(1);
    }

    void MainFrame::AddTrayIcon() {
        memset(&m_trayIcon, 0, sizeof(NOTIFYICONDATA));
        m_trayIcon.cbSize = sizeof(NOTIFYICONDATA);
        String iconPath = Utils::GetAppPath();
        iconPath.append(_T("/Data/xsplayer.ico"));
        HICON hIcon = (HICON)::LoadImage(NULL, iconPath.c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_SHARED);
        DWORD dw = GetLastError();
        ::SendMessage(*this, STM_SETICON, IMAGE_ICON, (LPARAM)(UINT)hIcon);

        SetIcon(IDI_ICON1);

        m_trayIcon.hIcon = hIcon;
        m_trayIcon.hWnd = m_hWnd;
        lstrcpy(m_trayIcon.szTip, _T("XSPlayer"));
        m_trayIcon.uCallbackMessage = WM_SHOWTASK;
        m_trayIcon.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
        Shell_NotifyIcon(NIM_ADD, &m_trayIcon);
        ShowWindow(SW_HIDE);
    }

    LRESULT MainFrame::OnTrayIcon(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
        //如果在图标中单击左键则还原
        if (lParam == WM_LBUTTONDOWN) {
            //显示主窗口
            ShowWindow(SW_SHOWNORMAL);
        }
        //如果在图标中单击右键则弹出声明式菜单
        if (lParam == WM_RBUTTONDOWN) {
            //获取鼠标坐标
            POINT pt; GetCursorPos(&pt);
            //右击后点别地可以清除“右击出来的菜单”
            SetForegroundWindow(m_hWnd);
            //托盘菜单    win32程序使用的是HMENU，如果是MFC程序可以使用CMenu
            HMENU hMenu;
            //生成托盘菜单
            hMenu = CreatePopupMenu();
            //添加菜单,关键在于设置的一个标识符  WM_ONCLOSE 点击后会用到
            AppendMenu(hMenu, MF_STRING, WM_ONCLOSE, _T("Exit"));
            //弹出菜单,并把用户所选菜单项的标识符返回
            int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD, pt.x, pt.y, NULL, m_hWnd, NULL);
            //如果标识符是WM_ONCLOSE则关闭
            if (cmd == WM_ONCLOSE) {
                m_trayIcon.hIcon = NULL;
                Shell_NotifyIcon(NIM_DELETE, &m_trayIcon);
                //退出程序
                ::PostQuitMessage(0);
            }
        }
        bHandled = true;
        return 0;
    }

    void MainFrame::OnInitMediaManager(void) {
        SqliteHelperFactory sqlitFactory;
        MediaManager::GetSingleton().AddMediaSource(&sqlitFactory, m_pOfflineUITab);
        MediaSource9KuFactory kuFactory;
        MediaManager::GetSingleton().AddMediaSource(&kuFactory, m_pOnlineUITab);
    }

}
#include "UI/RightPannel.h"

#include <string>

#include "Core/MediaManager.h"
#include "UI/BuildCallback.h"
#include "UI/LrcPannel.h"

namespace XSPlayer {

    RightPannel::RightPannel() : XSControlUI() {

        m_lastPlayTime = std::chrono::high_resolution_clock::now();
    }

    RightPannel::~RightPannel() {
        
    }

    void RightPannel::Notify(DuiLib::TNotifyUI& msg) {
        if (_tcsicmp(msg.sType, kClick) == 0) {
            DuiLib::CDuiString strName = msg.pSender->GetName();
            if (0 == _tcsicmp(strName, kbtnMediaPlay)) {
                OnBtnPlay();
            }
            else if (0 == _tcsicmp(strName, _T("btnMediaNext"))) {
                OnBtnNext();
            }
            else if (0 == _tcsicmp(strName, _T("btnMediaLast"))) {
                OnBtnLast();
            }
            else if (0 == _tcsicmp(strName, _T("btnMediaStop"))) {
                OnBtnStop();
            }
        }
        if (0 == _tcsicmp(msg.sType, kSliderValueChanged)) {
            if (msg.pSender == m_pSliderPlayUI) {
                OnSeekPostion();
            }
        }
    }

    LRESULT RightPannel::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg)
        {
        case WM_RIGHTPANNEL_INIT_DURATION: 
            InitPlaySolider(wParam, lParam);
            break;

        case WM_RIGHTPANNEL_UPDATE_DURATION:
            UpdatePlayPos(wParam, lParam);
            break;

        case WM_OFFLINE_PLAY:
        {
            m_mediaSoruce = MediaSourceType::MST_LOCAL;
            m_playStatus = PlayStatus::PS_Stop;
            OnBtnPlay();
        }
            break;

        case WM_ONLINE_PLAY:
        {
            m_mediaSoruce = MediaSourceType::MST_9KU_SERVER;
            m_playStatus = PlayStatus::PS_Stop;
            OnBtnPlay();
        }
        break;

        case WM_MEDIA_LRC_LOADED:
        {
            char* szLrc =reinterpret_cast<char*>(lParam);
            String lrc(szLrc);
            delete[] szLrc;

            OnDisplayLrc(lrc);
        }
        break;

        default:
            break;
        }

        return 0;
    }

    void RightPannel::DoInit(void) {
        DuiLib::CDialogBuilder dailogBuilder;
        BuildCallback callback(nullptr);
        DuiLib::CContainerUI* pContainerUI = dynamic_cast<DuiLib::CContainerUI*>(dailogBuilder.Create(_T("Data/skin/chinesestyle/right_pannel.xml"), (UINT)0, &callback));
        if (nullptr != pContainerUI) {
            Add(pContainerUI);
        }
        else {
            RemoveAll();
        }
    }

    bool RightPannel::OnPlayMedia(void) {
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> tm = now - m_lastPlayTime;	// 毫秒
        if (tm.count() < 200) {
            return false;
        }
        m_lastPlayTime = now;

        bool bPlaySuccess = false;
        if (MediaSourceType::MST_LOCAL == m_mediaSoruce) {
            bPlaySuccess = OnPlayLocalMedia();
        }
        else {
            bPlaySuccess = OnPlayServerMedia();
        }

        return bPlaySuccess;
    }

    bool RightPannel::OnPlayMedida(DuiLib::CControlUI* pControl) {
        if (nullptr == pControl) {
            return false;
        }

        String userData(pControl->GetUserData().GetData());
        size_t mediaID = -1;

#if defined(UNICODE) || defined(_UNICODE)
        mediaID = std::stoul(userData);
        pListElementUI->SetUserData(std::to_wstring(mediaID).c_str());
#else
        mediaID = std::stoul(userData);
#endif

        MediaManager::GetSingleton().Stop();
        return MediaManager::GetSingleton().PlayMedia(mediaID);

    }

    bool RightPannel::OnPlayLocalMedia(void) {
        if (MediaSourceType::MST_LOCAL != m_mediaSoruce) {
            return false;
        }
        
        DuiLib::CListUI* pList = static_cast<DuiLib::CListUI*>(m_pManager->FindControl(kOfflineList));
        if (nullptr == pList) {
            return false;
        }

        int index = pList->GetCurSel();
        DuiLib::CControlUI* pControl = pList->GetItemAt(index);
        if (!OnPlayMedida(pControl)) {
            return false;
        }
        return true;
    }
   
    bool RightPannel::OnPlayServerMedia(void) {
        if (MediaSourceType::MST_9KU_SERVER != m_mediaSoruce) {
            return false;
        }

        DuiLib::CListUI* pList = static_cast<DuiLib::CListUI*>(m_pManager->FindControl(kOnlineList));
        if (nullptr == pList) {
            return false;
        }

        int index = pList->GetCurSel();
        DuiLib::CControlUI* pControl = pList->GetItemAt(index);
        if (!OnPlayMedida(pControl)) {
            return false;
        }
        return true;
    }

    bool RightPannel::PushToList(const std::vector<String>&& filePath) {
        if (filePath.empty()) {
            return false;
        }

        DuiLib::CListUI* pList = static_cast<DuiLib::CListUI*>(FindSubControl(_T("domainlist")));
        if (nullptr == pList) {
            return false;
        }

        for (const auto& item : filePath) {
            DuiLib::CListTextElementUI* pListItem = new DuiLib::CListTextElementUI;
            PostMessage(m_pManager->GetPaintWindow(), WM_OFFLINE_ADD_LISTITEM, 0, (LPARAM)pListItem);
        }

        return true;

    }

    void RightPannel::OnAddListItem(UINT uMsg, WPARAM wParam, LPARAM lParam)  {
        DuiLib::CListUI* pList = static_cast<DuiLib::CListUI*>(FindSubControl(_T("domainlist")));
        if (nullptr == pList) {
            return;
        }
        DuiLib::CListTextElementUI* pListElement = (DuiLib::CListTextElementUI*)(lParam);
        pList->Add(pListElement);
    }

    void RightPannel::OnBtnPlay(void) {
        if (m_playStatus == PlayStatus::PS_Stop) {
            if (!OnPlayMedia()) {
                return;
            }

            m_playStatus = PlayStatus::PS_Playing;
            ChangePlayImage(true);
        }
        else if (PlayStatus::PS_Playing == m_playStatus) {
            MediaManager::GetSingleton().Pause();
            m_playStatus = PlayStatus::PS_Puase;
            ChangePlayImage(false);
        }
        else if (PlayStatus::PS_Puase == m_playStatus) {
            m_playStatus = PlayStatus::PS_Playing;
            MediaManager::GetSingleton().Continue();
            ChangePlayImage(true);
        }
    }

    void RightPannel::InitPlaySolider(WPARAM wParam, LPARAM lParam) {
        m_nDuration = int(wParam);
       
        m_pSliderPlayUI->SetMaxValue(m_nDuration);
        m_pSliderPlayUI->SetValue(0);

        int totolHour = m_nDuration / 3600;
        int tatolMiSec = m_nDuration % 3600;
        int totolMimite = tatolMiSec / 60;
        int totalSec = tatolMiSec % 60;

#ifdef _UNICODE
        wchar_t sz[256] = { 0 };
#else
        char sz[256] = { 0 };
#endif
        wsprintf(sz, _T("%02d:%02d:%02d"), totolHour, totolMimite, totalSec);
        m_pLabelTotoleTime->SetText(sz);

        UpdateTimeText();
    }

    void RightPannel::UpdatePlayPos(WPARAM wParam, LPARAM lParam) {
        int nDuration = int(wParam);
       
        if (m_bSeek) {
            return;
        }
        m_pSliderPlayUI->SetValue(nDuration);
        m_nCurrentDuration = nDuration;
        UpdateTimeText();

        OnUpdateLrc(nDuration);

        if (m_nCurrentDuration >= m_nDuration) {
            OnBtnNext();
        }
    }

    void RightPannel::UpdateTimeText(void) {
        if (nullptr == m_pLabelPlayTime) {
            return;
        }

        int playHour = m_nCurrentDuration / 3600;
        int playMiSec = m_nCurrentDuration % 3600;
        int playMimite = playMiSec / 60;
        int playSec = playMiSec % 60;

#ifdef _UNICODE
        wchar_t sz[256] = { 0 };
#else
        char sz[256] = { 0 };
#endif
        
        wsprintf(sz, _T("%02d:%02d:%02d"), playHour, playMimite, playSec);
        m_pLabelPlayTime->SetText(sz);
    }

    void RightPannel::OnBtnNext() {
        if (MediaSourceType::MST_LOCAL == m_mediaSoruce) {
            SendMessage(m_pManager->GetPaintWindow(), WM_OFFLINE_NEXT, 0, reinterpret_cast<LPARAM>(&m_mediaSoruce));
        }
        else {
            SendMessage(m_pManager->GetPaintWindow(), WM_ONLINE_NEXT, 0, reinterpret_cast<LPARAM>(&m_mediaSoruce));
        }
        
    }

    void RightPannel::InitWindow(void) {
        __super::InitWindow();
       // SetTimer(m_pManager->GetPaintWindow(), IDT_TIMER1_1SECONDE, 1000, NULL);
        m_pSliderPlayUI = static_cast<DuiLib::CSliderUI*>(m_pManager->FindControl(_T("player_slider")));
        m_pLabelPlayTime = dynamic_cast<DuiLib::CLabelUI*>(m_pManager->FindControl(_T("player_time")));
        m_pLabelTotoleTime =  dynamic_cast<DuiLib::CLabelUI*>(m_pManager->FindControl(_T("player_totol_time")));
        m_pBtnPlay = dynamic_cast<DuiLib::CButtonUI*>(m_pManager->FindControl(_T("btnMediaPlay")));

        m_pLrcPannel = dynamic_cast<LrcPannel*>(m_pManager->FindControl(_T("lrcPannel")));

//        m_pLabelPlayTime->SetText(_T("00:00:00"));
//        m_pLabelTotoleTime->SetText(_T("00:00:00"));

        ChangePlayImage(false);
    }

    void RightPannel::OnBtnLast() {
        SendMessage(m_pManager->GetPaintWindow(), WM_CHANGE_LAST_PLAY, 0, reinterpret_cast<LPARAM>(&m_mediaSoruce));
    }

    void RightPannel::OnBtnStop() {
        MediaManager::GetSingleton().Stop();
        m_playStatus = PlayStatus::PS_Stop;
        ChangePlayImage(false);
        SendMessage(m_pManager->GetPaintWindow(), WM_CHANGE_STOP_PLAY, 0, 0);
        m_pLabelPlayTime->SetText(_T("00:00:00"));
        m_pLabelTotoleTime->SetText(_T("00:00:00"));
        m_pSliderPlayUI->SetValue(0);
    }

    void RightPannel::OnSeekPostion() {
        if (nullptr == m_pSliderPlayUI) {
            return;
        }
        m_bSeek = true;
        int value = m_pSliderPlayUI->GetValue();
        MediaManager::GetSingleton().Seek(value);
        m_bSeek = false;
    }

    void RightPannel::ChangePlayImage(bool bPlay) {
        if (bPlay) {
            m_pBtnPlay->SetNormalImage(_T("file='Data/skin/image/control_button/pause.png'"));
            m_pBtnPlay->SetHotImage(_T("file='Data/skin/image/control_button/pause_hot.png'"));
            m_pBtnPlay->SetPushedImage(_T("file='Data/skin/image/control_button/pause_hot.png'"));
        }
        else {
            m_pBtnPlay->SetNormalImage(_T("file='Data/skin/image/control_button/play.png'"));
            m_pBtnPlay->SetHotImage(_T("file='Data/skin/image/control_button/play_hot.png'"));
            m_pBtnPlay->SetPushedImage(_T("file='Data/skin/image/control_button/play_hot.png'"));
        }
    }

    void RightPannel::OnDisplayLrc(const String& lrc) {
        if (nullptr == m_pLrcPannel) {
            return;
        }

        m_pLrcPannel->UpdateContent(lrc);
    }

    void RightPannel::OnUpdateLrc(size_t len) {
        if (nullptr == m_pLrcPannel) {
            return;
        }

        m_pLrcPannel->UpdateLrc(len);
    }

}
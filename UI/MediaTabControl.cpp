#include "UI/MediaTabControl.h"

#include "UI/BuildCallback.h"

#include <string>
#include <functional>
#include <algorithm>

#include<Shlobj.h>

#include "Core/MediaManager.h"

#include "UI/Menu.h"
#include "UI/MediaList.h"
#include "UI/ListItem.h"
#include "UI/BuildCallback.h"
#include "UI/OfflineUITab.h"

namespace XSPlayer {

    const TCHAR* const kNormalImage = _T("normalimage");
    const TCHAR* const kHotImage = _T("hotimage");
    const TCHAR* const kPushedImage = _T("pushedimage");
    const TCHAR* const kGroup = _T("group");

    DuiLib::CDialogBuilder MediaTabControl::m_optionUIBuilder;

    MediaTabControl::MediaTabControl(void) : XSControlUI() {
    }

    MediaTabControl::~MediaTabControl() {
        
    }

    void MediaTabControl::DoInit() {
        DuiLib::CDialogBuilder dailogBuilder;
        BuildCallback callback(m_pManager);
        DuiLib::CContainerUI* pContainerUI = dynamic_cast<DuiLib::CContainerUI*>(dailogBuilder.Create(_T("Data/skin/chinesestyle/media_tab_control.xml"), (UINT)0, &callback));
        if (nullptr != pContainerUI) {
            Add(pContainerUI);
        }
        else {
            RemoveAll();
        }
    }

    void MediaTabControl::InitWindow(void) {
        if (nullptr == m_pManager) {
            return;
        }

        __super::InitWindow();

        m_pTabHeader = dynamic_cast<DuiLib::CVerticalLayoutUI*>(m_pManager->FindControl("tab_header"));
        m_pMediaList = dynamic_cast<OfflineUITab*>(m_pManager->FindControl("media_list"));
        if (nullptr != m_pMediaList) {
            m_pMediaList->InitWindow();
        }
       // AddTab(_T("本地媒体"), kMediaSourceLocal);
    }

    void MediaTabControl::UnInitWindow(void) {
        int count = m_pTabHeader->GetCount();
        for (int index = 0; index < count; ++index) {
            DuiLib::CControlUI* pOption = m_pTabHeader->GetItemAt(index);
            
            auto pMediaList = reinterpret_cast<MediaList*>(pOption->GetTag());
            if (nullptr == pMediaList) {
                continue;
            }

            pMediaList->UnInit();
        }
    }

    void MediaTabControl::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) {
        String path = DuiLib::CPaintManagerUI::GetResourcePath();
        if (0 == _tcsicmp(pstrName, kNormalImage)) {
            m_normalImage = pstrValue;
        }
        else if (0 == _tcsicmp(pstrName, kHotImage)) {
            m_hotImage = pstrValue;
        }
        else if (0 == _tcsicmp(pstrName, kPushedImage)) {
            m_pushedImage = pstrValue;
        }
        else if (0 == _tcsicmp(pstrName, kGroup)) {
            m_group = pstrValue;
        }

        __super::SetAttribute(pstrName, pstrValue);
    }

    void MediaTabControl::Notify(DuiLib::TNotifyUI& msg) {
        if (NotifyOption(msg)) {
            return;
        }
// 
//         if (0 == _tcsicmp(msg.sType, "itemactivate")) {
//             const String strID = msg.pSender->GetUserData();
//             size_t mediaID = -1;
// 
// #if defined(UNICODE) || defined(_UNICODE)
//             mediaID = std::stoul(userData);
//             pListElementUI->SetUserData(std::to_wstring(mediaID).c_str());
// #else
//             mediaID = std::stoul(strID);
// #endif
//             OnPlay(mediaID);
//         }
    }

    LRESULT MediaTabControl::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
        case WM_ADD_LISTITEM:
           return OnAddMedia(wParam, lParam);
            break;

        case WM_ADD_MEDIA_TYEP_ITEM:
            return OnAddMediaTypeTab(wParam, lParam);
            break;

        case WM_PLAY_MEDIA_NEXT:
            return OnPlayNext();

        case WM_PLAY_MEDIA_LAST:
            return OnPlayLast();

        default:
            break;
        }
        return 0;
    }

    bool MediaTabControl::AddTab(const String& txt, const String& mediaSource) {
        if (nullptr == m_pTabHeader) {
            return false;
        }

        auto pOption = CreateTabUI();
        if (nullptr == pOption) {
            return false;
        }

        pOption->SetText(txt.c_str());
        pOption->SetNormalImage(m_normalImage.c_str());
        pOption->SetHotImage(m_hotImage.c_str());
        pOption->SetPushedImage(m_pushedImage.c_str());
        pOption->SetSelectedHotImage(m_pushedImage.c_str());
        pOption->SetSelectedImage(m_pushedImage.c_str());
        pOption->SetGroup(m_group.c_str());

        pOption->SetUserData(mediaSource.c_str());
        MediaManager::GetSingleton().AddMediaContainter(mediaSource);

        return m_pTabHeader->Add(pOption);
    }

    DuiLib::COptionUI* MediaTabControl::CreateTabUI(void) {
        return new DuiLib::COptionUI;
    }

    bool MediaTabControl::NotifyOption(DuiLib::TNotifyUI& msg) {
        auto pOptionUI = dynamic_cast<DuiLib::COptionUI*>(msg.pSender);
        if (nullptr == pOptionUI) {
            return false;
        }

        if (nullptr == m_pMediaList) {
            return false;
        }
        m_pMediaList->RemoveAllMedia();

        m_curMediaSource = pOptionUI->GetUserData();
        const auto pMediaContainer = MediaManager::GetSingleton().GetMediaContainer(m_curMediaSource);
        if (nullptr == pMediaContainer) {
            return false;
        }

        size_t count = pMediaContainer->GetChildernCount();
        for (size_t index = 0; index < count; ++index) {
            //PostMessage(m_pManager->GetPaintWindow(), )
           m_pMediaList->AddMedia(pMediaContainer->GetChild(index));
        }

        return true;
    }

    bool MediaTabControl::OnAddMediaTypeTab(WPARAM wParam, LPARAM lParam) {
        const String* szText = reinterpret_cast<const String*>(wParam);
        const String* szSource = reinterpret_cast<const String*>(lParam);
        bool flag = AddTab(*szText, *szSource);
        delete szText;
        delete szSource;
        return flag;
    }

    bool MediaTabControl::OnAddMedia(WPARAM wParam, LPARAM lParam) {
        const String* szMediaSource = (reinterpret_cast<String*>(wParam));
        Media* pMedia = reinterpret_cast<Media*>(lParam);
        if (nullptr == pMedia) {
            delete szMediaSource;
            return false;
        }

        bool success = MediaManager::GetSingleton().AddMedia(pMedia, *szMediaSource);
        if (m_curMediaSource == *szMediaSource) {
            OnAddToMediaList(pMedia);
        }
        
        delete szMediaSource;
        return success;
    }

    bool MediaTabControl::OnAddToMediaList(Media* pMedia) {
        if (nullptr == m_pMediaList) {
            return false;
        }

        m_pMediaList->AddMedia(pMedia);
        return true;
    }

    void MediaTabControl::OnPlay(unsigned int id) {
        const size_t playId = MediaManager::GetSingleton().GetCurrentPlay();
        if (playId == id) {
            return;
        }

        MediaManager::GetSingleton().Stop();
        MediaManager::GetSingleton().PlayMedia(id);
    }

    bool MediaTabControl::OnPlayNext() {
        if (nullptr == m_pMediaList) {
            return false;
        }


        return m_pMediaList->PlayNext();
    }

    bool MediaTabControl::OnPlayLast() {
        if (nullptr == m_pMediaList) {
            return false;
        }


        return m_pMediaList->PlayLast();
    }

}
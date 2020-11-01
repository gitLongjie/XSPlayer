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
        m_pTabBody = dynamic_cast<DuiLib::CTabLayoutUI*>(m_pManager->FindControl("tab_body"));

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

        if (0 == _tcsicmp(msg.sType, "itemactivate")) {
            auto* pItem = dynamic_cast<MediaListItem*>(msg.pSender);
            if (nullptr == pItem) {
                return;
            }

            size_t mediaId = pItem->GetMediaId();
            OnPlay(mediaId);
        }
    }

    LRESULT MediaTabControl::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
        case WM_ADD_LISTITEM:
            OnAddList(wParam, lParam);
            break;

        case WM_ADD_MEDIA_TYEP_ITEM:
            OnAddMediaTypeTab(wParam, lParam);
            break;

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

        DuiLib::CControlUI* pControlUI = CreateTabBody(mediaSource);
        if (nullptr == pControlUI) {
            delete pOption;
            return false;
        }
        
        pOption->SetTag(reinterpret_cast<UINT_PTR>(pControlUI));

        return m_pTabHeader->Add(pOption);
    }

    DuiLib::COptionUI* MediaTabControl::CreateTabUI(void) {
        return new DuiLib::COptionUI;
    }

    DuiLib::CControlUI* MediaTabControl::CreateTabBody(const String& mediaSource) {
        if (nullptr == m_pTabBody) {
            return nullptr;
        }

        MediaList* pMediaList = new MediaList(mediaSource);
        if (nullptr == pMediaList) {
            return nullptr;
        }

        m_pTabBody->Add(pMediaList);
        pMediaList->SetVisible(true);
        return pMediaList;
    }

    bool MediaTabControl::NotifyOption(DuiLib::TNotifyUI& msg) {
        auto pOptionUI = dynamic_cast<DuiLib::COptionUI*>(msg.pSender);
        if (nullptr == pOptionUI) {
            return false;
        }

        if (nullptr == m_pTabHeader) {
            return false;
        }

        bool flag = false;
        int count = m_pTabHeader->GetCount();
        for (int index = 0; index < count; ++index) {
            DuiLib::CControlUI* pOption = m_pTabHeader->GetItemAt(index);
            if (msg.pSender == pOption) {
                flag = true;
              //  OnNotifyOption(pOptionUI);
                m_pTabBody->SelectItem(index);
                break;
            }
// 
//             auto pControlUI = reinterpret_cast<DuiLib::CControlUI*>(pOption->GetTag());
//             if (nullptr == pControlUI) {
//                 continue;
//             }
// 
//             pControlUI->SetVisible(false);
        }
        return flag;
    }

    bool MediaTabControl::OnNotifyOption(DuiLib::COptionUI* pOptionUI) {
        if (nullptr == pOptionUI) {
            return false;
        }

        auto pControlUI = reinterpret_cast<DuiLib::CControlUI*>(pOptionUI->GetTag());
        if (nullptr == pControlUI) {
            return false;
        }

        pControlUI->SetVisible(true);
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

    bool MediaTabControl::OnAddList(WPARAM wParam, LPARAM lParam) {
        const String* szMediaSource = (reinterpret_cast<String*>(wParam));
        Media* pMedia = reinterpret_cast<Media*>(lParam);
        if (nullptr == pMedia) {
            return false;
        }

        MediaList* pMediaList = nullptr;
        int count = m_pTabHeader->GetCount();
        for (int index = 0; index < count; ++index) {
            DuiLib::CControlUI* pOption = m_pTabHeader->GetItemAt(index);
            if (nullptr == pOption) {
                continue;
            }

            auto pList = reinterpret_cast<MediaList*>(pOption->GetTag());
            if (nullptr == pList) {
                continue;
            }

            if (pList->Compare(*szMediaSource)) {
                pMediaList = pList;
                break;
            }
        }

        if (nullptr == pMediaList) {
            delete szMediaSource;
            return false;
        }

        bool flag = pMediaList->Add(*szMediaSource, pMedia);
        delete szMediaSource;
        return flag;
    }

    void MediaTabControl::OnPlay(unsigned int id) {
        const size_t playId = MediaManager::GetSingleton().GetCurrentPlay();
        if (playId == id) {
            return;
        }

        MediaManager::GetSingleton().Stop();
        MediaManager::GetSingleton().PlayMedia(id);
    }

}